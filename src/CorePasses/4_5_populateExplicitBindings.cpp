#include "Iridium/CorePasses/4_5_populateExplicitBindings.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/IridiumReductions.h"



void populateExplicitBindings(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  bool USE_TOP_LEVEL_LOCALS = getenv("IRI_CS_BRREACHED") ? false : true;
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
    std::unordered_map<double, std::vector<std::tuple<std::string, EnvBindingSEXPKindFlag, bool, bool>>> explicitBindings;

    auto bbs = std::dynamic_pointer_cast<ListSEXP>(container->getBB());
    assert(bbs && "Expected ListSEXP");
    for (auto &_bb : bbs->args)
    {
      auto bb = std::dynamic_pointer_cast<BBSEXP>(_bb);
      assert(bb && "Expected bb to be a BBSEXP");

      auto localScope = bb->getScopeIDX();
      auto varHoistingScope = findVARHoistingScope(localScope, iridiumBuildContext);

      // Iterate over STMT
      for (int i = 0; i < bb->args.size(); i++)
      {
        auto stmt = bb->args.at(i);

        bool doInit = true;
        if (auto nonInitDeclaration = std::dynamic_pointer_cast<JSExplicitBindingDeclarationNSEXP>(stmt))
        {
          nonInitDeclaration->tag = "JSExplicitBindingDeclaration";
          stmt = JSExplicitBindingDeclarationSEXP::generateFrom(nonInitDeclaration);
          doInit = false;
        }

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
            scopeToHoistTo = varHoistingScope;
            flag = EnvBindingSEXPKindFlag::JSVAR;
          }
          else
            throw std::runtime_error("Explicit binding kind failed");

          auto binding = std::dynamic_pointer_cast<ResolveEnvBindingSEXP>(jsExplicitBindingDeclarationStmt->getLValTarget());
          assert(binding && "Expected binding to be ResolveEnvBindingSEXP");

          if (scopeToHoistTo == topLevelScopeIdx && !isModule)
          {
            if (USE_TOP_LEVEL_LOCALS)
            {
              explicitBindings[scopeToHoistTo].push_back(std::make_tuple(binding->getNAME(), flag, doInit, binding->hasASW()));
            }
            else
            {
              // Top Level Global Declaration for script mode
              sloppyDeclarations.push_back(std::make_pair(binding->getNAME(), flag));
            }
          }
          else
          {
            // Create a binding in the current bindings frame at the relevant scope
            explicitBindings[scopeToHoistTo].push_back(std::make_tuple(binding->getNAME(), flag, doInit, binding->hasASW()));
          }

          if (!jsExplicitBindingDeclarationStmt->hasRVal())
          {
            if (jsExplicitBindingDeclarationStmt->hasJSVAR() || doInit == false)
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

        for (std::tuple<std::string, EnvBindingSEXPKindFlag, bool, bool> &b : e.second)
        {
          auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(container->getBindings());
          assert(bindingsSEXP && "Expected bindingsSEXP");

          auto &[bindingName, kindFlag, doInit, isASW] = b;
          // std::string bindingName = b.first;

          IRISEXP lval = std::make_shared<ResolveEnvBindingSEXP>(bindingName, false);

          IRISEXP rval;
          if (kindFlag == EnvBindingSEXPKindFlag::JSVAR)
          {
            rval = std::make_shared<EnvReadSEXP>(std::make_shared<GlobalBindingSEXP>("undefined"), false);
          }
          else
          {
            rval = std::make_shared<JSNUBDSEXP>();
          }

          if (!hasBindingReference(bindingsSEXP, containerBC->scopeIdx, bindingName, kindFlag, localScope, parentScope))
          {
            if (startBB->hasTopLevel() && USE_TOP_LEVEL_LOCALS == false)
            {
              auto localBinding = std::make_shared<EnvBindingSEXP>(bindingName, isASW, false, false, kindFlag == EnvBindingSEXPKindFlag::JSLET, kindFlag == EnvBindingSEXPKindFlag::JSCONST, kindFlag == EnvBindingSEXPKindFlag::JSVAR, containerBC->scopeIdx, -1, localScope, parentScope, -1);
              auto remoteBinding = std::make_shared<RemoteEnvBindingSEXP>(localBinding, false, -1);
              
              addToListSEXP(bindingsSEXP->getRemoteBindings(), remoteBinding);

              if (doInit)
              {
                envWrites.push_back(
                  std::make_shared<EnvWriteSEXP>(lval, rval, false, false, true, false)
                );
              }
            }
            else
            {
              auto localBinding = std::make_shared<EnvBindingSEXP>(bindingName, isASW, false, false, kindFlag == EnvBindingSEXPKindFlag::JSLET, kindFlag == EnvBindingSEXPKindFlag::JSCONST, kindFlag == EnvBindingSEXPKindFlag::JSVAR, containerBC->scopeIdx, -1, localScope, parentScope, -1);
              addToListSEXP(bindingsSEXP->getLocalBindings(), localBinding);

              if(doInit)
              {
                envWrites.push_back(
                  std::make_shared<EnvWriteSEXP>(lval, rval, false, false, true, false)
                );
              }

            }
          }
        }

        prependArgs(startBB->args, envWrites);
      }
    }
  }
}
