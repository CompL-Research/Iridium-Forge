#include "Iridium/Passes/5_initializeStackFrame.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"

void initializeStackFrame(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  for (auto &bbc : fileSexp->args)
  {
    auto container = std::dynamic_pointer_cast<BBContainerSEXP>(bbc);
    if (!container)
      continue;

    assert(iridiumBuildContext.find(container->getScopeIDX()) != iridiumBuildContext.end());
    auto containerBC = iridiumBuildContext[container->getScopeIDX()];

    auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(container->getBindings());
    assert(bindingsSEXP && "Expected bindingsSEXP");

    // Initialize remote bindings, this only holds valid for top level module references...
    {
      int i = 0;
      for (auto &b : bindingsSEXP->getRemoteBindings()->args)
      {
        auto remoteBindingSEXP = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b);
        assert(remoteBindingSEXP && "Expected remoteBindingSEXP");
        auto bindingSEXP = std::dynamic_pointer_cast<EnvBindingSEXP>(remoteBindingSEXP->getParentReference());
        assert(bindingSEXP && "Expected bindingSEXP");
        int refIdx = i++;
        remoteBindingSEXP->setREFIDX(refIdx);
        bindingSEXP->setREFIDX(refIdx);
      }
    }

    // Initialize local bindings
    {
      std::unordered_map<double, std::vector<std::shared_ptr<EnvBindingSEXP>>> mapping;
      for (auto &b : bindingsSEXP->getLocalBindings()->args)
      {
        auto bindingSEXP = std::dynamic_pointer_cast<EnvBindingSEXP>(b);
        assert(bindingSEXP && "Expected bindingSEXP");

        mapping[bindingSEXP->getScope()].push_back(bindingSEXP);
      }

      // Sort and flatten the stack frame

      //
      // 1 :  [A{REFIDX: 0, NEXT: -1}, B{REFIDX: 1, NEXT: 0}...]
      // 3 :  [C{REFIDX: 0, NEXT: -1}, D{REFIDX: 1, NEXT: 0}...]
      // 0 :  [E{REFIDX: 0, NEXT: -1}, F{REFIDX: 1, NEXT: 0}...]
      //

      //
      // [E{REFIDX: 0, NEXT: -1}, F{REFIDX: 1, NEXT: 0}... A{REFIDX: 0, NEXT: -1}, B{REFIDX: 1, NEXT: 0}... C{REFIDX: 0, NEXT: -1}, D{REFIDX: 1, NEXT: 0}...]
      //

      std::vector<std::shared_ptr<EnvBindingSEXP>> flattened;
      std::unordered_map<double, int> lastRefIdxPerScope;

      std::vector<std::pair<double, std::vector<std::shared_ptr<EnvBindingSEXP>>>> sorted(mapping.begin(), mapping.end());

      std::sort(sorted.begin(), sorted.end(),
                [](auto &a, auto &b)
                { return a.first < b.first; });

      int i = 0;
      for (auto &entry : sorted)
      {
        auto scope = entry.first;
        auto &vec = entry.second;
        bool first = true;
        for (auto &bindingSEXP : vec)
        {
          int refIdx = i++;
          bindingSEXP->setREFIDX(static_cast<int>(refIdx));
          bindingSEXP->setNEXT(first ? -1 : static_cast<int>(refIdx - 1));
          flattened.push_back(bindingSEXP);
          first = false;
          lastRefIdxPerScope[scope] = static_cast<int>(refIdx);
        }
      }

      std::function<int(double)> patchToParentIDX = [&](double scopeIdx) -> double
      {
        if (scopeIdx == -1)
          return -1;
        if (lastRefIdxPerScope.find(scopeIdx) != lastRefIdxPerScope.end())
          return lastRefIdxPerScope[scopeIdx];
        return patchToParentIDX(iridiumBuildContext[scopeIdx]->parent);
      };

      for (auto &e : flattened)
      {
        if (e->getNEXT() == -1)
        {
          e->setNEXT(patchToParentIDX(e->getParentScope()));
        }
      }

      std::vector<IRISEXP> converted;
      converted.reserve(flattened.size());

      for (auto &e : flattened)
      {
        converted.push_back(e); // If IRISEXP can be constructed from shared_ptr<EnvBindingSEXP>
      }

      bindingsSEXP->getLocalBindings()->args = std::move(converted);
    }
  }
}