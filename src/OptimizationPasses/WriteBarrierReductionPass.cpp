#include "Iridium/OptimizationPasses/WriteBarrierReductionPass.h"

FileView *WriteBarrierReductionPass::currFileView = nullptr;

std::string WriteBarrierReductionPass::name() const
{
    return "WriteBarrierReduction";
}

bool WriteBarrierReductionPass::predicateLambdaSEXP(const IRISEXP &ele)
{
    if (std::dynamic_pointer_cast<LambdaSEXP>(ele))
        return true;
    return false;
}

bool WriteBarrierReductionPass::markCapturedBindings(IRISEXP curr, const TDZA &inData)
{
    bool changed = false;

    if (auto lambdaSEXP = std::dynamic_pointer_cast<LambdaSEXP>(curr))
    {
        // Assume updateSafelyCapturedBindingsSet returns whether anything was added
        changed |= WriteBarrierReductionPass::currFileView->updateSafelyCapturedBindingsSet(lambdaSEXP->getStartBBIDX(), inData);
    }

    for (auto e : curr->args)
        changed |= markCapturedBindings(e, inData);

    return changed;
}

bool WriteBarrierReductionPass::patchExpr(IRISEXP curr, IRISEXP binding, const TDZLattice &latticeVal)
{
    bool changed = false;

    if (auto envWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(curr))
    {
        if (latticeVal.kind == TDZLattice::SAFE &&
            !envWriteSEXP->getSAFE() &&
            envWriteSEXP->getLValTarget() == binding)
        {
            if (binding->hasFlag("JSCONST") &&
                !envWriteSEXP->hasTHISINIT())
            {
                envWriteSEXP->setTHROWERR();
            }

            envWriteSEXP->setSAFE(true);
            changed = true;
        }

        if (auto remEnvBindingWrite =
                std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(
                    envWriteSEXP->getLValTarget()))
        {
            if (!envWriteSEXP->getSAFE() &&
                WriteBarrierReductionPass::currFileView->safelyCapturedBindings.count(remEnvBindingWrite) > 0)
            {
                if (resolveRemoteBinding(remEnvBindingWrite)->hasFlag("JSCONST") &&
                    !envWriteSEXP->hasTHISINIT())
                {
                    envWriteSEXP->setTHROWERR();
                }

                WriteBarrierReductionPass::currFileView->safelyCaptured++;
                envWriteSEXP->setSAFE(true);
                changed = true;
            }
        }
    }

    if (auto envReadSEXP = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
    {
        if (auto o = std::dynamic_pointer_cast<EnvBindingSEXP>(envReadSEXP->getObj()))
        {
            if (latticeVal.kind == TDZLattice::SAFE && o == binding)
            {
                if (!envReadSEXP->hasSAFE())
                {
                    envReadSEXP->setSAFE();
                    changed = true;
                }
            }
        }

        if (auto o = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(envReadSEXP->getObj()))
        {
            if (!envReadSEXP->hasSAFE() &&
                WriteBarrierReductionPass::currFileView->safelyCapturedBindings.count(o) > 0)
            {
                WriteBarrierReductionPass::currFileView->safelyCaptured++;
                envReadSEXP->setSAFE();
                changed = true;
            }
        }
    }

    for (auto e : curr->args)
        changed |= WriteBarrierReductionPass::patchExpr(e, binding, latticeVal);

    return changed;
}

bool WriteBarrierReductionPass::Transform(std::shared_ptr<BBSEXP> &bb, const TDZA &inData)
{
    bool changed = false;

    inData.iter(bb, [&](size_t idx, const TDZA &val)
                {
        if (val.dfv.store.empty())
            return;

        for (auto &[binding, latticeVal] : val.dfv.store)
        {
          changed |= WriteBarrierReductionPass::patchExpr(bb->args.at(idx), binding, latticeVal);
        }

        if (WriteBarrierReductionPass::currFileView &&
            hasNode(bb->args.at(idx), WriteBarrierReductionPass::predicateLambdaSEXP))
        {
          changed |= WriteBarrierReductionPass::markCapturedBindings(bb->args.at(idx), val);
        } });

    return changed;
}

bool WriteBarrierReductionPass::run(BBContainerView &bb,
                                    FileView &fileView,
                                    AnalysisManager &AM)
{
    bool changed = false;

    currFileView = &fileView;

    const auto &tdza = AM.getTDZA(bb);

    for (const auto &[vertex, state] : tdza)
    {
        if (state.dfv.store.empty())
        {
            continue;
        }

        auto currBB = bb.cfgManager.cfg[vertex];
        changed |= Transform(currBB, state);
    }

    currFileView = nullptr;

    return changed;
}
