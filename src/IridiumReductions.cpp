#include "Iridium/IridiumReductions.h"
#include "generated/IridiumTypes.h"

std::shared_ptr<EnvWriteSEXP> reduceJSDecl(std::shared_ptr<JSExplicitBindingDeclarationSEXP> explicitBinding)
{
  explicitBinding->unsetJSLET();
  explicitBinding->unsetJSCONST();
  explicitBinding->unsetJSVAR();
  explicitBinding->tag = "EnvWrite";
  return EnvWriteSEXP::generateFrom(explicitBinding);
}

std::shared_ptr<JSExplicitBindingDeclarationSEXP> reduceJSFunDecl(std::shared_ptr<JSFuncDeclSEXP> funcDecl)
{
  funcDecl->tag = "JSExplicitBindingDeclaration";
  // IRISEXP LValTarget, IRISEXP RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT
  return std::make_shared<JSExplicitBindingDeclarationSEXP>(
    funcDecl->getLValTarget(),
    funcDecl->getRVal(),
    false,
    false,
    true,
    false,
    true,
    false
  );
  // return JSExplicitBindingDeclarationSEXP::generateFrom(funcDecl);
}
