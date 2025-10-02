#include "Iridium/CorePasses/4_5_populateExplicitBindings.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/IridiumReductions.h"

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
            // explicitBindings[scopeToHoistTo].push_back(std::make_pair(binding->getNAME(), flag));
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
              bb->args.at(i) = std::make_shared<NOPSEXP>();
            }
            else
            {
              // This statement has been reduced to a simple EnvWriteSEXP
              jsExplicitBindingDeclarationStmt->args.resize(2);
              jsExplicitBindingDeclarationStmt->setRVal(std::make_shared<EnvReadSEXP>(std::make_shared<GlobalBindingSEXP>("undefined"), false));
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

        envWrites.push_back(
          std::make_shared<JSSloppyDeclSEXP>(e.first, e.second == EnvBindingSEXPKindFlag::JSLET, e.second == EnvBindingSEXPKindFlag::JSCONST, e.second == EnvBindingSEXPKindFlag::JSVAR)
        );
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

          IRISEXP lval = std::make_shared<ResolveEnvBindingSEXP>(bindingName, false);

          IRISEXP rval;
          if (b.second == EnvBindingSEXPKindFlag::JSVAR)
          {
            rval = std::make_shared<EnvReadSEXP>(std::make_shared<GlobalBindingSEXP>("undefined"), false);
          }
          else
          {
            rval = std::make_shared<JSNUBDSEXP>();
          }

          if (!hasBindingReference(bindingsSEXP, containerBC->scopeIdx, bindingName, b.second, localScope, parentScope))
          {
            if (startBB->hasTopLevel())
            { // the place where it will be hoisted to, is it the top level container?
              // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
              auto localBinding = std::make_shared<EnvBindingSEXP>(bindingName, false, false, false, b.second == EnvBindingSEXPKindFlag::JSLET, b.second == EnvBindingSEXPKindFlag::JSCONST, b.second == EnvBindingSEXPKindFlag::JSVAR, containerBC->scopeIdx, -1, localScope, parentScope, -1);

              // auto localBinding = makeEnvBindingSEXP(-1, containerBC->scopeIdx, bindingName, b.second, localScope, parentScope);
              auto remoteBinding = std::make_shared<RemoteEnvBindingSEXP>(localBinding, false, -1);
              
              addToListSEXP(bindingsSEXP->getRemoteBindings(), remoteBinding);

              envWrites.push_back(
                // IRISEXP LValTarget, IRISEXP RVal, bool SLOPPY, bool THROWERR, bool SAFE, bool THISINIT
                std::make_shared<EnvWriteSEXP>(lval, rval, false, false, true, false)
              );
            }
            else
            {
              auto localBinding = std::make_shared<EnvBindingSEXP>(bindingName, false, false, false, b.second == EnvBindingSEXPKindFlag::JSLET, b.second == EnvBindingSEXPKindFlag::JSCONST, b.second == EnvBindingSEXPKindFlag::JSVAR, containerBC->scopeIdx, -1, localScope, parentScope, -1);
              addToListSEXP(bindingsSEXP->getLocalBindings(), localBinding);

              envWrites.push_back(
                // IRISEXP LValTarget, IRISEXP RVal, bool SLOPPY, bool THROWERR, bool SAFE, bool THISINIT
                std::make_shared<EnvWriteSEXP>(lval, rval, false, false, true, false)
              );
            }
          }
        }

        prependArgs(startBB->args, envWrites);
      }
    }
  }
}
