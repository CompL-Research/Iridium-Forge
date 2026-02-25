#include "Iridium/OptimizationPasses/DCEPass.h"

std::string DCEPass::name() const
{
    return "DCE";
}

bool DCEPass::Transform(std::shared_ptr<BBSEXP> &bb, const Liveness &inData)
{
    bool changed = false;

    inData.iter(bb, [&](size_t idx, const Liveness &val)
                {
        auto &stmt = bb->args.at(idx);

        auto newStmt = patchExpr(stmt, val);

        if (newStmt != stmt)
        {
            stmt = newStmt;
            changed = true;
        } });

    if (changed)
    {
        filterNOPs(bb);
    }

    return changed;
}

bool DCEPass::maybeSideEffect(IRISEXP rVAL)
{
    if (
        auto bindingRead = std::dynamic_pointer_cast<EnvReadSEXP>(rVAL))
    {
        if (auto b = std::dynamic_pointer_cast<GlobalBindingSEXP>(bindingRead->getObj()))
            return false;
        if (auto b = std::dynamic_pointer_cast<EnvBindingSEXP>(bindingRead->getObj()))
        {
            if (b->hasJSARG() || b->hasJSRESTARG())
                return false;
        }
        return !bindingRead->hasSAFE();
    }
    if (
        std::dynamic_pointer_cast<BooleanSEXP>(rVAL) ||
        std::dynamic_pointer_cast<LambdaSEXP>(rVAL) ||
        std::dynamic_pointer_cast<NullSEXP>(rVAL) ||
        std::dynamic_pointer_cast<NumberSEXP>(rVAL) ||
        std::dynamic_pointer_cast<RegExpSEXP>(rVAL) ||
        std::dynamic_pointer_cast<StringSEXP>(rVAL) ||
        std::dynamic_pointer_cast<BitIntSEXP>(rVAL) ||
        std::dynamic_pointer_cast<JSNUBDSEXP>(rVAL) ||
        std::dynamic_pointer_cast<JSObjectSEXP>(rVAL) ||
        std::dynamic_pointer_cast<JSPrivateSEXP>(rVAL))
        return false;
    return true;
}

IRISEXP DCEPass::patchExpr(IRISEXP curr, const Liveness &val)
{
    // TODO
    // If noSideEffect(RVAL): StackReject[1](RVAL) => NOP
    //

    if (auto implicitBindingDecl = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(curr))
    {
        if (auto LVAL = std::dynamic_pointer_cast<EnvBindingSEXP>(implicitBindingDecl->getStore()))
        {
            if (val.dfv.count(LVAL) == 0
                // && implicitBindingDecl->getOPID() != 10
            )
            {
                return std::make_shared<NOPSEXP>();
            }
        }
    }

    if (auto envWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(curr))
    {

        // // LVAL = (ILVAL = RVAL) => LVAL = (RVAL)
        // if (auto innerEnvWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(envWriteSEXP->getRVal()))
        // {
        //   if (innerEnvWriteSEXP->getSAFE())
        //   {
        //     if (auto LVAL = std::dynamic_pointer_cast<EnvBindingSEXP>(innerEnvWriteSEXP->getLValTarget()))
        //     {
        //       if (val.dfv.count(LVAL) == 0)
        //       {
        //         // std::cout << "KILLING (inner): ";
        //         // curr->prettyPrint(std::cout);
        //         // std::cout << std::endl;

        //         envWriteSEXP->setRVal(innerEnvWriteSEXP->getRVal());

        //         // std::cout << "AFTER KILLING (inner): ";
        //         // curr->prettyPrint(std::cout);
        //         // std::cout << std::endl;
        //       }
        //     }
        //   }
        // }

        if (!envWriteSEXP->getSAFE())
            return curr;

        // LVAL = [RVAL] => StackReject[1]([RVAL])
        if (auto LVAL = std::dynamic_pointer_cast<EnvBindingSEXP>(envWriteSEXP->getLValTarget()))
        {
            if (val.dfv.count(LVAL) == 0)
            {
                // std::cout << "KILLING (outer): ";
                // curr->prettyPrint(std::cout);
                // std::cout << std::endl;

                // If writing to a const location, we cannot remove it as dead code
                if (auto innerEnvWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(envWriteSEXP->getRVal()))
                {
                    if (innerEnvWriteSEXP->hasTHROWERR())
                        return std::make_shared<ThrowSEXP>(std::make_shared<StringSEXP>("Write to const (inner Write) @ " + LVAL->getNAME()));
                    return envWriteSEXP->getRVal();
                }

                if (envWriteSEXP->hasTHROWERR())
                    return std::make_shared<ThrowSEXP>(std::make_shared<StringSEXP>(LVAL->getNAME()));

                if (!maybeSideEffect(envWriteSEXP->getRVal()))
                {
                    return std::make_shared<NOPSEXP>();
                }

                auto res = std::make_shared<StackRejectSEXP>(1);
                res->args.push_back(envWriteSEXP->getRVal());

                // std::cout << "AFTER KILLING (outer): ";
                // res->prettyPrint(std::cout);
                // std::cout << std::endl;

                return res;
            }
        }
    }

    if (auto stackReject = std::dynamic_pointer_cast<StackRejectSEXP>(curr))
    {
        if (auto envRead = std::dynamic_pointer_cast<EnvReadSEXP>(stackReject->args.at(0)))
        {
            if (envRead->hasSAFE())
            {
                return std::make_shared<NOPSEXP>();
            }
        }
    }

    return curr;
}

bool DCEPass::Transform(std::shared_ptr<BBSEXP> &bb, const Liveness &inData)
{
    bool changed = false;

    inData.iter(bb, [&](size_t idx, const Liveness &val)
                {
        auto &stmt = bb->args.at(idx);

        auto newStmt = patchExpr(stmt, val);

        if (newStmt != stmt)
        {
            stmt = newStmt;
            changed = true;
        } });

    if (changed)
    {
        filterNOPs(bb);
    }

    return changed;
}

bool DCEPass::run(BBContainerView &bb, FileView & /*fileView*/, AnalysisManager &AM)
{
    bool changed = false;

    const auto &captured = AM.getCapturedBindings(bb);
    Liveness::blacklist = captured;

    const auto &liveness = AM.getLiveness(bb);

    for (const auto &[vertex, state] : liveness)
    {
        if (state.dfv.empty())
            continue;

        auto currBB = bb.cfgManager.cfg[vertex];
        changed |= DCEPass::Transform(currBB, state);
    }

    return changed;
}
