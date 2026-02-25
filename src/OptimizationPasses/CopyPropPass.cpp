#include "Iridium/OptimizationPasses/CopyPropPass.h"

std::string CopyPropPass::name() const
{
    return "CopyProp";
}

std::shared_ptr<EnvBindingSEXP> CopyPropPass::resolve(const std::shared_ptr<EnvBindingSEXP> &start, const CopyPropInfo &val)
{
    std::set<std::shared_ptr<EnvBindingSEXP>> alreadyVisited;
    auto curr = start;

    while (true)
    {
        // cycle detected → bail out, return original
        if (alreadyVisited.count(curr) > 0)
            return start;

        alreadyVisited.insert(curr);

        // no further mapping → stop
        auto it = val.dfv.find(curr);
        if (it == val.dfv.end())
            return curr;

        curr = it->second;
    }
}

bool CopyPropPass::patchExpr(IRISEXP curr, const CopyPropInfo &val)
{
    bool changed = false;

    if (auto envReadSEXP = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
    {
        if (auto o = std::dynamic_pointer_cast<EnvBindingSEXP>(envReadSEXP->getObj()))
        {
            if (val.dfv.count(o) > 0)
            {
                auto res = CopyPropPass::resolve(o, val);
                if (res != o)
                {
                    envReadSEXP->setObj(res);
                    changed = true;
                }
            }
        }
    }

    for (auto &e : curr->args)
        changed |= CopyPropPass::patchExpr(e, val);

    return changed;
}

bool CopyPropPass::Transform(std::shared_ptr<BBSEXP> &bb, const CopyPropInfo &inData)
{
    bool changed = false;

    inData.iter(bb, [&](size_t idx, const CopyPropInfo &val)
                {
        // Skip if no copies available (big speed win)
        if (val.dfv.empty())
            return;

        changed |= CopyPropPass::patchExpr(bb->args.at(idx), val); });

    return changed;
}

bool CopyPropPass::run(BBContainerView &bb,
                       FileView & /*fileView*/,
                       AnalysisManager &AM)
{
    // if (std::getenv("NO_COPYPROP"))
    //   return false;

    bool changed = false;

    const auto &captured = AM.getCapturedBindings(bb);
    CopyPropInfo::blacklist = captured;

    const auto &copyInfo = AM.getCopyProp(bb);

    for (const auto &[vertex, state] : copyInfo)
    {
        if (state.dfv.empty())
        {
            continue;
        }

        auto currBB = bb.cfgManager.cfg[vertex];
        changed |= Transform(currBB, state);
    }

    return changed;
}
