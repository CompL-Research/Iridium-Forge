#include "Iridium/Passes/4_5_populateExplicitBindings.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"

void populateExplicitBindings(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  bool isModule = false;
  double topLevelScopeIdx = 0;

  // Iterate over BBContainerSEXP
  for (auto &bbc : fileSexp->args)
  {
    auto container = std::dynamic_pointer_cast<BBContainerSEXP>(bbc);
    if (!container)
      continue;
    assert(iridiumBuildContext.find(container->getScopeIDX()) != iridiumBuildContext.end());
    auto containerBC = iridiumBuildContext[container->getScopeIDX()];

    bool isTopLevel = container->hasTopLevel();

    if (isTopLevel)
    {
      isModule = containerBC->isModule;
      topLevelScopeIdx = containerBC->scopeIdx;
    }

    std::vector<std::pair<std::string, EnvBindingSEXPKindFlag>> sloppyDeclarations;
    std::unordered_map<double, std::vector<std::pair<std::string, EnvBindingSEXPKindFlag>>> explicitBindings;

    auto bbs = std::dynamic_pointer_cast<ListSEXP>(container->getBB());
    assert(bbs && "Expected ListSEXP");
    for (auto &_bb : bbs->args)
    {
      auto bb = std::dynamic_pointer_cast<BBSEXP>(_bb);
      assert(bb && "Expected bb to be a BBSEXP");

      auto localScope = bb->getScopeIDX();
      auto parentClosureScope = findParentClosureScope(localScope, iridiumBuildContext);

      // Iterate over STMT
      for (int i = 0; i < bb->args.size(); i++)
      {
        auto stmt = bb->args.at(i);

        //
        // Explicit Binding Declaration
        //
        if (auto jsExplicitBindingDeclarationStmt = std::dynamic_pointer_cast<JSExplicitBindingDeclarationSEXP>(stmt))
        {
          double scopeToHoistTo;
          EnvBindingSEXPKindFlag flag = EnvBindingSEXPKindFlag::JSVAR;
          if (jsExplicitBindingDeclarationStmt->hasJSLET())
          {
            scopeToHoistTo = localScope;
            flag = EnvBindingSEXPKindFlag::JSLET;
          }
          else if (jsExplicitBindingDeclarationStmt->hasJSCONST())
          {
            scopeToHoistTo = localScope;
            flag = EnvBindingSEXPKindFlag::JSCONST;
          }
          else if (jsExplicitBindingDeclarationStmt->hasJSVAR())
          {
            scopeToHoistTo = parentClosureScope;
            flag = EnvBindingSEXPKindFlag::JSVAR;
          }
          else
            throw std::runtime_error("Explicit binding kind failed");

          auto binding = std::dynamic_pointer_cast<ResolveEnvBindingSEXP>(jsExplicitBindingDeclarationStmt->getLValTarget());
          assert(binding && "Expected binding to be ResolveEnvBindingSEXP");

          if (scopeToHoistTo == topLevelScopeIdx && !isModule)
          {
            // Top Level Global Declaration for script mode
            sloppyDeclarations.push_back(std::make_pair(binding->getNAME(), flag));
          }
          else
          {
            // Create a binding in the current bindings frame at the relevant scope
            explicitBindings[scopeToHoistTo].push_back(std::make_pair(binding->getNAME(), flag));
          }

          if (!jsExplicitBindingDeclarationStmt->hasRVal())
          {
            if (jsExplicitBindingDeclarationStmt->hasJSVAR())
            {
              // This statement is no longer needed
              bb->args.at(i) = makeNOPSEXP();
            }
            else
            {
              // This statement has been reduced to a simple EnvWriteSEXP
              jsExplicitBindingDeclarationStmt->setRVal(makeGlobalBindingSEXP("undefined"));
              bb->args.at(i) = reduceJSDecl(jsExplicitBindingDeclarationStmt);
            }
          }
          else
          {
            bb->args.at(i) = reduceJSDecl(jsExplicitBindingDeclarationStmt);
          }
        }
      }
    }
    {
      // Handle Sloppy Declarations
      std::shared_ptr<BBSEXP> &startBB = containerBC->BB[0];
      std::vector<IRISEXP> envWrites;
      envWrites.reserve(sloppyDeclarations.size() * 2);
      for (auto &e : sloppyDeclarations)
      {

        envWrites.push_back(makeJSSloppyDeclSEXP(e.first, e.second));
        // TODO: Is this needed???

        // IRISEXP lval = makeResolveEnvBindingSEXP(e.first);
        // IRISEXP rval;
        // if (e.second == EnvBindingSEXPKindFlag::JSVAR)
        // {
        //   rval = makeEnvReadSEXP("undefined");
        // }
        // else
        // {
        //   rval = makeJSNUBDSEXP();
        // }
        // envWrites.push_back(makeEnvWrite(lval, rval, true, false));
      }
      // prepend envWrites to startBB->args
      prependArgs(startBB->args, envWrites);
    }

    {
      // Handle Explicit Declarations
      for (auto &e : explicitBindings)
      {
        auto &bindingContext = iridiumBuildContext[e.first];
        double localScope = bindingContext->scopeIdx;
        double parentScope = bindingContext->parent;

        std::shared_ptr<BBSEXP> &startBB = bindingContext->BB[0];
        std::vector<IRISEXP> envWrites;

        envWrites.reserve(e.second.size());

        for (auto &b : e.second)
        {
          auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(container->getBindings());
          assert(bindingsSEXP && "Expected bindingsSEXP");
          std::string bindingName = b.first;

          IRISEXP lval = makeResolveEnvBindingSEXP(bindingName);

          IRISEXP rval;
          if (b.second == EnvBindingSEXPKindFlag::JSVAR)
          {
            rval = makeEnvReadSEXP("undefined");
          }
          else
          {
            rval = makeJSNUBDSEXP();
          }

          if (!hasBindingReference(bindingsSEXP, containerBC->scopeIdx, bindingName, b.second, localScope, parentScope))
          {
            if (startBB->hasTopLevel())
            { // the place where it will be hoisted to, is it the top level container?
              auto localBinding = makeEnvBindingSEXP(-1, containerBC->scopeIdx, bindingName, b.second, localScope, parentScope);
              auto res = makeRemoteEnvBindingSEXP(localBinding, -1);
              addToListSEXP(bindingsSEXP->getRemoteBindings(), res);

              envWrites.push_back(makeEnvWrite(lval, rval, true, false));
            }
            else
            {
              auto res = makeEnvBindingSEXP(-1, containerBC->scopeIdx, bindingName, b.second, localScope, parentScope);
              addToListSEXP(bindingsSEXP->getLocalBindings(), res);

              envWrites.push_back(makeEnvWrite(lval, rval, true, false));
            }
          }
        }

        prependArgs(startBB->args, envWrites);
      }
    }
  }
}
