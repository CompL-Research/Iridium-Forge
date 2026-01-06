#include "Iridium/CorePasses/20_releaseEscapingBindingsFromScope.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

#define PRINT_ESCAPE_INFO 0
#define PRINT_STACK_TO_HEAP_MOVEMENT 0

void debugScopeWiseCapturedBindings(const std::unordered_map<double, std::set<std::shared_ptr<EnvBindingSEXP>>>& scopeWiseCapturedBindings) {
  for (const auto& pair : scopeWiseCapturedBindings) {
    std::cout << "Scope: " << pair.first << std::endl;
    std::cout << "  Captured Bindings:" << std::endl;
    for (const auto& bindingPtr : pair.second) {
      std::cout << "    - [" << bindingPtr->getREFIDX() << "] " << bindingPtr->getNAME() << std::endl;
    }
  }
}


void releaseEscapingBindingsFromScope(IRISEXP file, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{

  // Create a list of captured bindings at each scope
  std::unordered_map<double, std::set<std::shared_ptr<EnvBindingSEXP>>> scopeWiseCapturedBindings;

  { // Populate scopeWiseCapturedBindings map
    auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(file);
    assert(fileSEXP && "Expected FileSEXP");
    for (auto bbCont : fileSEXP->args)
    {
      auto bbContainerSEXP = std::dynamic_pointer_cast<BBContainerSEXP>(bbCont);
      if (!bbContainerSEXP)
        continue;
      
      auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(bbContainerSEXP->getBindings());
      assert(bindingsSEXP && "Expected BindingsSEXP");

      for (auto b : bindingsSEXP->getRemoteBindings()->args)
      {
        auto rbSEXP = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b);
        if (!rbSEXP)
          throw std::runtime_error("Expected RemoteEnvBindingSEXP");
        auto bSEXP = resolveRemoteBinding(rbSEXP);
        scopeWiseCapturedBindings[bSEXP->getScope()].insert(bSEXP);
      }
    }
  }

  #if PRINT_ESCAPE_INFO == 1
  debugScopeWiseCapturedBindings(scopeWiseCapturedBindings);
  #endif

  std::function<bool(double, double, std::vector<double> &)> isEscapeFlow = [&](double currentScope, double targetScope, std::vector<double> & scopeList) {
    // If currentScope is parent of currentScope, then return true
    if (currentScope == -1) return false;
    if (currentScope == targetScope) return true;
    // Push all scopes, except the target scope...
    scopeList.push_back(currentScope);
    assert(iridiumBuildContext.count(currentScope) > 0 && "build context missing for scope");
    return isEscapeFlow(iridiumBuildContext[currentScope]->parent, targetScope, scopeList);
  };

  std::function<void(double, std::vector<double> &)> getThrowTargetScope = [&](double currentScope, std::vector<double> & scopeList) {
    // Not closing top level, assuming it follows return semantics...
    if(currentScope == -1) return;

    scopeList.push_back(currentScope);
    assert(iridiumBuildContext.count(currentScope) > 0 && "build context missing for scope");
    auto currCTX = iridiumBuildContext[currentScope];
    auto startBB = currCTX->BB[0];
    if (startBB->hasFlag("TryBB") || startBB->hasClosureBoundary()) { // Matched TryBB | ClosureBoundary
      return;
    }
    getThrowTargetScope(currCTX->parent, scopeList);
  };

  std::unordered_map<double, std::shared_ptr<BBSEXP>> bbObjCache;

  auto getBBSEXPFromBBIDX = [&](double bbIDX) {
    if (bbObjCache.count(bbIDX) > 0) return bbObjCache[bbIDX];
    for (auto e : iridiumBuildContext) {
      for (auto bb : e.second->BB) {
        if (bb->getIDX() == bbIDX) {
          bbObjCache[bbIDX] = bb;
          return bb;
        }
      }
    }
    throw std::runtime_error("Failed to find BBObject using bbIDX");
  };

  auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(file);
  assert(fileSEXP && "Expected FileSEXP");
  for (auto bbCont : fileSEXP->args)
  {
    auto bbContainerSEXP = std::dynamic_pointer_cast<BBContainerSEXP>(bbCont);
    if (!bbContainerSEXP)
      continue;

    for (auto bb : bbContainerSEXP->getBB()->args) {
      auto bbSEXP = std::dynamic_pointer_cast<BBSEXP>(bb);
      assert(bbSEXP && "Expected BB");

      auto& args = bbSEXP->args;
      for (auto it = args.begin(); it != args.end(); ++it) {
        auto stmt = *it;
        if (auto gotoSEXP = std::dynamic_pointer_cast<GotoSEXP>(stmt)) {
          bool isContinueCTX = gotoSEXP->hasFlag("CONTINUE_TARGET");
          double currScope = bbSEXP->getScopeIDX();
          double targetScope = getBBSEXPFromBBIDX(gotoSEXP->getIDX())->getScopeIDX();

          if (isContinueCTX) {
            // act like a frame closed, use break target as targetScope
            targetScope = getBBSEXPFromBBIDX(gotoSEXP->getFlagDouble("CONTINUE_TARGET"))->getScopeIDX();
            gotoSEXP->removeFlag("CONTINUE_TARGET");
          }

          std::vector<double> scopeList;
          bool needsEscapeHandling = isEscapeFlow(currScope, targetScope, scopeList);
          if (isContinueCTX) assert(needsEscapeHandling);
          
          if (!needsEscapeHandling) continue;

          #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
          std::cout << "Escape Handler case: " << currScope << " -> " << targetScope << std::endl;
          std::cout << " -- ";
          for (auto sss : scopeList) std::cout << sss << " ";
          std::cout << std::endl;
          #endif

          auto stackToHeapNode = std::make_shared<StackToHeapSEXP>();
 
          #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
          std::cout << " --[Move to heap] ";
          #endif
          for (auto & s : scopeList) {
            for (auto b : scopeWiseCapturedBindings[s]) {
              stackToHeapNode->args.push_back(b);
              #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
              std::cout << "[" << b->getREFIDX() << "] " << b->getNAME() << " ";
              #endif
            }
          }
          #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
          std::cout << std::endl;
          #endif
  
          if (stackToHeapNode->args.size() > 0) {
            // Insert stackToHeapNode just before the current stmt in the vector...
            it = args.insert(it, stackToHeapNode);

            // Advance 'it' to point back to the original 'goto' statement 
            // so the next loop increment moves us to the statement after 'goto'
            ++it;
          }
        }

        if (auto gotoSEXP = std::dynamic_pointer_cast<ThrowSEXP>(stmt)) {
          double currScope = bbSEXP->getScopeIDX();
          std::vector<double> scopeList;
          getThrowTargetScope(currScope, scopeList);

          #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
          std::cout << "[Throw] Escape Handler case: " << currScope << std::endl;
          std::cout << " -- ";
          for (auto sss : scopeList) std::cout << sss << " ";
          std::cout << std::endl;
          #endif
          
          auto stackToHeapNode = std::make_shared<StackToHeapSEXP>();

          #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
          std::cout << " --[Move to heap] ";
          #endif
          for (auto & s : scopeList) {
            for (auto b : scopeWiseCapturedBindings[s]) {
              stackToHeapNode->args.push_back(b);
              #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
              std::cout << "[" << b->getREFIDX() << "] " << b->getNAME() << " ";
              #endif
            }
          }
          #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
          std::cout << std::endl;
          #endif
  
          if (stackToHeapNode->args.size() > 0) {
            // Insert stackToHeapNode just before the current stmt in the vector...
            it = args.insert(it, stackToHeapNode);

            // Advance 'it' to point back to the original 'goto' statement 
            // so the next loop increment moves us to the statement after 'goto'
            ++it;
          }
        }

        if (auto loopInitPrelude = std::dynamic_pointer_cast<LoopInitPreludeEndSEXP>(stmt)) {
          auto stackToHeapNode = std::make_shared<StackToHeapSEXP>();

          #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
          std::cout << " --[Move to heap] ";
          #endif
          for (auto b : scopeWiseCapturedBindings[bbSEXP->getScopeIDX()]) {
            stackToHeapNode->args.push_back(b);
            #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
            std::cout << "[" << b->getREFIDX() << "] " << b->getNAME() << " ";
            #endif
          }
          #if PRINT_STACK_TO_HEAP_MOVEMENT == 1
          std::cout << std::endl;
          #endif

          if (stackToHeapNode->args.size() > 0) {
            *it = stackToHeapNode;
          } else {
            *it = std::make_shared<NOPSEXP>();
          }
        }
      }
    }
  }
}