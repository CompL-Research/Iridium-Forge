#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
#include "Iridium/IridiumTypes.h"
#include "Iridium/IridiumBuildContext.h"

double findParentClosureScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " + std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  auto &startBB = buildContext->BB.at(0);
  if (startBB->hasClosureBoundary() || startBB->hasTopLevel())
    return startingScope;
  return findParentClosureScope(buildContext->parent, iridiumBuildContext);
}

double getLexicalScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " + std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  return buildContext->parent;
}

void setClosureFlags(double flag, std::shared_ptr<BBContainerSEXP> bbContainer)
{
  bbContainer->setContainerFlagID(flag);
  switch ((int)flag)
  {
  case 0:
    throw std::runtime_error("Invalid closure flag");
  case 1:
    break;
  case 2:
    bbContainer->setPROTO();
    bbContainer->setNEW();
    break; // flags.push("PROTO", "NEW");
  case 3:
    bbContainer->setPROTO();
    bbContainer->setNEW();
    bbContainer->setSCALL();
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    bbContainer->setDERIVED();
    break; // flags.push("PROTO", "NEW", "SCALL", "SOBJ", "HOME", "DERIVED");
  case 4:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 5:
    bbContainer->setHOME();
    break; // flags.push("HOME");
  case 6:
    break;
  case 7:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 8:
    bbContainer->setHOME();
    break; // flags.push("HOME");
  case 9:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 10:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME")
  case 11:
    break;
  case 12:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME");
  default:
    throw std::runtime_error("expected a valid closure flag");
  }
}

void addToListSEXP(IRISEXP list, IRISEXP elementToAdd)
{
  auto listSEXP = std::dynamic_pointer_cast<ListSEXP>(list);
  assert(listSEXP && "Tried to add to a non-list");

  if (listSEXP->hasTYPE())
    assert(listSEXP->getTYPE() == elementToAdd->tag);

  listSEXP->args.push_back(elementToAdd);
}

bool sameKindFlag(std::shared_ptr<EnvBindingSEXP> binding, EnvBindingSEXPKindFlag kindFlag)
{
  if (kindFlag == EnvBindingSEXPKindFlag::JSLET && binding->hasJSLET())
    return true;
  if (kindFlag == EnvBindingSEXPKindFlag::JSCONST && binding->hasJSCONST())
    return true;
  if (kindFlag == EnvBindingSEXPKindFlag::JSVAR && binding->hasJSVAR())
    return true;
  if (kindFlag == EnvBindingSEXPKindFlag::JSARG && binding->hasJSARG())
    return true;
  if (kindFlag == EnvBindingSEXPKindFlag::JSRESTARG && binding->hasJSRESTARG())
    return true;
  return false;
}

std::shared_ptr<EnvBindingSEXP> resolveRemoteBinding(std::shared_ptr<RemoteEnvBindingSEXP> rbin)
{
  auto containedBinding = rbin->getParentReference();
  if (auto bin = std::dynamic_pointer_cast<EnvBindingSEXP>(containedBinding))
  {
    return bin;
  }
  else if (auto bin = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(containedBinding))
  {
    return resolveRemoteBinding(bin);
  }
  else
    throw std::runtime_error("resolveRemoteBinding failed");
}

bool hasBindingReference(std::shared_ptr<BindingsSEXP> bindingsSEXP, double idx, std::string name, EnvBindingSEXPKindFlag kindFlag, double localScope, double parentScope)
{
  auto localBindings = bindingsSEXP->getLocalBindings();
  auto remoteBindings = bindingsSEXP->getRemoteBindings();
  for (auto &b : localBindings->args)
  {
    if (auto bin = std::dynamic_pointer_cast<EnvBindingSEXP>(b))
    {
      if (
          bin->getIDX() == idx && bin->getNAME() == name && sameKindFlag(bin, kindFlag) && bin->getScope() == localScope && bin->getParentScope() == parentScope)
        return true;
    }
    else
      throw std::runtime_error("localBindings is non EnvBinding SEXP");
  }

  for (auto &b : remoteBindings->args)
  {
    if (auto rbin = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b))
    {
      auto bin = resolveRemoteBinding(rbin);
      if (
          bin->getIDX() == idx && bin->getNAME() == name && sameKindFlag(bin, kindFlag) && bin->getScope() == localScope && bin->getParentScope() == parentScope)
        return true;
    }
    else
      throw std::runtime_error("remotebindings is non RemoteEnvBinding SEXP");
  }

  return false;
}

template <typename T, typename... Args>
IRISEXP make_iriexp(Args&&... args) {
    return std::static_pointer_cast<IridiumSEXP>(
        std::make_shared<T>(std::forward<Args>(args)...)
    );
}

IRISEXP specializeSEXP(IRISEXP obj) 
{
  auto & tag = obj->tag;
  if (tag == "File")
  {
    return make_iriexp<FileSEXP>(obj);
  }
  if (tag == "ResolveEnvBinding")
  {
    return make_iriexp<ResolveEnvBindingSEXP>(obj);
  }
  if (tag == "List")
  {
    return make_iriexp<ListSEXP>(obj);
  }
  if (tag == "JSImplicitBindingDeclaration")
  {
    return make_iriexp<JSImplicitBindingDeclarationSEXP>(obj);
  }
  if (tag == "EnvRead")
  {
    return make_iriexp<EnvReadSEXP>(obj);
  }
  if (tag == "IfJump")
  {
    return make_iriexp<IfJumpSEXP>(obj);
  }

  if (tag == "String")
  {
    return make_iriexp<StringSEXP>(obj);
  }

  if (tag == "FieldRead")
  {
    return make_iriexp<FieldReadSEXP>(obj);
  }

  if (tag == "JSExplicitBindingDeclaration")
  {
    return make_iriexp<JSExplicitBindingDeclarationSEXP>(obj);
  }

  if (tag == "CallSite")
  {
    return make_iriexp<CallSiteSEXP>(obj);
  }

  if (tag == "ReturnAsync")
  {
    return make_iriexp<ReturnAsyncSEXP>(obj);
  }

  if (tag == "BB")
  {
    return make_iriexp<BBSEXP>(obj);
  }

  if (tag == "Return")
  {
    return make_iriexp<ReturnSEXP>(obj);
  }

  if (tag == "IfElseJump")
  {
    return make_iriexp<IfElseJumpSEXP>(obj);
  }

  if (tag == "Goto")
  {
    return make_iriexp<GotoSEXP>(obj);
  }

  if (tag == "JSFuncDecl")
  {
    return make_iriexp<JSFuncDeclSEXP>(obj);
  }

  if (tag == "Lambda")
  {
    return make_iriexp<LambdaSEXP>(obj);
  }

  if (tag == "NOP")
  {
    return make_iriexp<NOPSEXP>(obj);
  }

  if (tag == "BBContainer")
  {
    return make_iriexp<BBContainerSEXP>(obj);
  }

  if (tag == "Bindings")
  {
    return make_iriexp<BindingsSEXP>(obj);
  }

  if (tag == "StarExport")
  {
    return make_iriexp<StarExportSEXP>(obj);
  }

  if (tag == "StaticImport")
  {
    return make_iriexp<StaticImportSEXP>(obj);
  }

  if (tag == "LocalStaticExport")
  {
    return make_iriexp<LocalStaticExportSEXP>(obj);
  }

  if (tag == "NamedReexport")
  {
    return make_iriexp<NamedReexportSEXP>(obj);
  }

  if (tag == "ModuleRequest")
  {
    return make_iriexp<ModuleRequestSEXP>(obj);
  }

  if (tag == "EnvBinding")
  {
    return make_iriexp<EnvBindingSEXP>(obj);
  }

  if (tag == "RemoteEnvBinding")
  {
    return make_iriexp<RemoteEnvBindingSEXP>(obj);
  }

  if (tag == "GlobalBinding")
  {
    return make_iriexp<GlobalBindingSEXP>(obj);
  }

  if (tag == "EnvWrite")
  {
    return make_iriexp<EnvWriteSEXP>(obj);
  }

  if (tag == "JSNUBD")
  {
    return make_iriexp<JSNUBDSEXP>(obj);
  }

  if (tag == "JSSloppyDecl")
  {
    return make_iriexp<JSSloppyDeclSEXP>(obj);
  }

  if (tag == "Number")
  {
    return make_iriexp<NumberSEXP>(obj);
  }

  throw std::runtime_error("Unhandled Iridium Tag: " + tag);
}

BBSEXPFLAGS getBBFlag(std::shared_ptr<BBSEXP> b) 
{
  if (b->hasTopLevel()) return BBSEXPFLAGS::TopLevel;
  if (b->hasClosureBoundary()) return BBSEXPFLAGS::ClosureBoundary;
  if (b->hasLexical()) return BBSEXPFLAGS::Lexical;
  throw std::runtime_error("Failed to get a valid flag from a BBSEXP");
}

void setBBFlag(std::shared_ptr<BBSEXP> b, BBSEXPFLAGS flagToSet) 
{
  b->unsetTopLevel();
  b->unsetClosureBoundary();
  b->unsetLexical();
  if (flagToSet == BBSEXPFLAGS::TopLevel) return b->setTopLevel();
  if (flagToSet == BBSEXPFLAGS::ClosureBoundary) return b->setClosureBoundary();
  if (flagToSet == BBSEXPFLAGS::Lexical) return b->setLexical();
  throw std::runtime_error("Impossible case reached setBBFlag");
}

