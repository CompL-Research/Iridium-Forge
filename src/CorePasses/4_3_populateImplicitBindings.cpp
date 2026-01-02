#include "Iridium/CorePasses/4_3_populateImplicitBindings.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void populateImplicitBindings(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  // Iterate over BBContainerSEXP
  for (auto &bbc : fileSexp->args)
  {
    auto container = std::dynamic_pointer_cast<BBContainerSEXP>(bbc);
    if (!container)
      continue;
    assert(iridiumBuildContext.find(container->getScopeIDX()) != iridiumBuildContext.end());
    auto containerBC = iridiumBuildContext[container->getScopeIDX()];

    auto bbs = std::dynamic_pointer_cast<ListSEXP>(container->getBB());
    assert(bbs && "Expected ListSEXP");
    for (auto &_bb : bbs->args)
    {
      auto bb = std::dynamic_pointer_cast<BBSEXP>(_bb);
      assert(bb && "Expected bb to be a BBSEXP");

      auto localScope = bb->getScopeIDX();
      auto parentScope = iridiumBuildContext[localScope]->parent;

      // Iterate over STMT
      for (int i = 0; i < bb->args.size(); i++)
      {
        auto stmt = bb->args.at(i);

        //
        // Implicit Binding Declaration
        //
        if (auto jsImplicitBindingDeclarationStmt = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(stmt))
        {
          std::string bindingName = jsImplicitBindingDeclarationStmt->getNAME();
          EnvBindingSEXPKindFlag kind;

          if (jsImplicitBindingDeclarationStmt->hasJSLET())
            kind = EnvBindingSEXPKindFlag::JSLET;
          else if (jsImplicitBindingDeclarationStmt->hasJSCONST())
            kind = EnvBindingSEXPKindFlag::JSCONST;
          else if (jsImplicitBindingDeclarationStmt->hasJSVAR())
            kind = EnvBindingSEXPKindFlag::JSVAR;
          else
            throw std::runtime_error("Invalid kind for an implicit binding");

          // Due to VARBoundary, this is not longer the case
          // assert(localScope == containerBC->scopeIdx);

          // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
          auto bindingSEXP = std::make_shared<EnvBindingSEXP>(bindingName, false, false, false, jsImplicitBindingDeclarationStmt->hasJSLET(), jsImplicitBindingDeclarationStmt->hasJSCONST(), jsImplicitBindingDeclarationStmt->hasJSVAR(), containerBC->scopeIdx, -1, localScope, parentScope, -1);

          auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(container->getBindings());
          assert(bindingsSEXP && "Expected bindingsSEXP");
          addToListSEXP(bindingsSEXP->getLocalBindings(), bindingSEXP);
        }
      }
    }
  }
}