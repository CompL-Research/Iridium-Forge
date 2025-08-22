#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"

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

std::shared_ptr<NOPSEXP> makeNOPSEXP()
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "NOP";
  return std::make_shared<NOPSEXP>(sexp);
}

std::shared_ptr<ListSEXP> makeListSEXP()
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "List";
  return std::make_shared<ListSEXP>(sexp);
}

std::shared_ptr<BindingsSEXP> makeBindingsSEXP(double parentScope)
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "Bindings";
  auto res = std::make_shared<BindingsSEXP>(sexp);

  res->args.resize(3);
  
  auto localBindings = makeListSEXP();
  localBindings->setTYPE("EnvBinding");
  res->setLocalBindings(localBindings);

  auto remoteBindings = makeListSEXP();
  remoteBindings->setTYPE("RemoteEnvBinding");
  res->setRemoteBindings(remoteBindings);

  auto lambdas = makeListSEXP();
  lambdas->setTYPE("PoolBinding");
  res->setLambdas(lambdas);

  res->setParentScope(parentScope);
  return res;
}


std::shared_ptr<BBContainerSEXP> makeBBContainerSEXP(double scopeIdx, double parentScope)
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "BBContainer";
  auto res = std::make_shared<BBContainerSEXP>(sexp);

  res->args.resize(2);

  res->setBindings(makeBindingsSEXP(parentScope));
  auto bbContainer = makeListSEXP();
  bbContainer->setTYPE("BB");
  res->setBB(bbContainer);

  res->setScopeIDX(scopeIdx);

  return res;
}

std::shared_ptr<EnvBindingSEXP> makeEnvBindingSEXP(double refIdx, double scopeIdx, std::string name, EnvBindingSEXPKindFlag kindFlag, double scope, double parentScope)
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "EnvBinding";
  auto res = std::make_shared<EnvBindingSEXP>(sexp);

  res->setREFIDX(refIdx);
  res->setIDX(scopeIdx);
  res->setNAME(name);
  if (kindFlag == EnvBindingSEXPKindFlag::JSARG) res->setJSARG();
  else if (kindFlag == EnvBindingSEXPKindFlag::JSRESTARG) res->setJSRESTARG();
  else if (kindFlag == EnvBindingSEXPKindFlag::JSLET) res->setJSLET();
  else if (kindFlag == EnvBindingSEXPKindFlag::JSCONST) res->setJSCONST();
  else if (kindFlag == EnvBindingSEXPKindFlag::JSVAR) res->setJSVAR();

  res->setScope(scope);
  res->setParentScope(parentScope);
  res->setNEXT(-1);
  return res;
}

std::shared_ptr<RemoteEnvBindingSEXP> makeRemoteEnvBindingSEXP(std::shared_ptr<EnvBindingSEXP> obj, double refIdx)
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "RemoteEnvBinding";
  auto res = std::make_shared<RemoteEnvBindingSEXP>(sexp);
  res->args.resize(1);
  res->setParentReference(obj);
  res->setREFIDX(refIdx);
  return res;
}