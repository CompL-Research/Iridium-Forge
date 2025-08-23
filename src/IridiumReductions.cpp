#include "Iridium/IridiumReductions.h"
#include "generated/IridiumTypes.h"

// std::shared_ptr<NOPSEXP> makeNOPSEXP()
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "NOP";
//   return std::make_shared<NOPSEXP>(sexp);
// }

// std::shared_ptr<ListSEXP> makeListSEXP()
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "List";
//   return std::make_shared<ListSEXP>(sexp);
// }

// std::shared_ptr<BindingsSEXP> makeBindingsSEXP(double parentScope)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "Bindings";
//   auto res = std::make_shared<BindingsSEXP>(sexp);

//   res->args.resize(3);
  
//   auto localBindings = makeListSEXP();
//   localBindings->setTYPE("EnvBinding");
//   res->setLocalBindings(localBindings);

//   auto remoteBindings = makeListSEXP();
//   remoteBindings->setTYPE("RemoteEnvBinding");
//   res->setRemoteBindings(remoteBindings);

//   auto lambdas = makeListSEXP();
//   lambdas->setTYPE("PoolBinding");
//   res->setLambdas(lambdas);

//   res->setParentScope(parentScope);
//   return res;
// }


// std::shared_ptr<BBContainerSEXP> makeBBContainerSEXP(double scopeIdx, double parentScope)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "BBContainer";
//   auto res = std::make_shared<BBContainerSEXP>(sexp);

//   res->args.resize(2);

//   res->setBindings(makeBindingsSEXP(parentScope));
//   auto bbContainer = makeListSEXP();
//   bbContainer->setTYPE("BB");
//   res->setBB(bbContainer);

//   res->setScopeIDX(scopeIdx);

//   return res;
// }

// std::shared_ptr<EnvBindingSEXP> makeEnvBindingSEXP(double refIdx, double scopeIdx, std::string name, EnvBindingSEXPKindFlag kindFlag, double scope, double parentScope)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "EnvBinding";
//   auto res = std::make_shared<EnvBindingSEXP>(sexp);

//   res->setREFIDX(refIdx);
//   res->setIDX(scopeIdx);
//   res->setNAME(name);
//   if (kindFlag == EnvBindingSEXPKindFlag::JSARG) res->setJSARG();
//   else if (kindFlag == EnvBindingSEXPKindFlag::JSRESTARG) res->setJSRESTARG();
//   else if (kindFlag == EnvBindingSEXPKindFlag::JSLET) res->setJSLET();
//   else if (kindFlag == EnvBindingSEXPKindFlag::JSCONST) res->setJSCONST();
//   else if (kindFlag == EnvBindingSEXPKindFlag::JSVAR) res->setJSVAR();

//   res->setScope(scope);
//   res->setParentScope(parentScope);
//   res->setNEXT(-1);
//   return res;
// }

// std::shared_ptr<RemoteEnvBindingSEXP> makeRemoteEnvBindingSEXP(std::shared_ptr<EnvBindingSEXP> obj, double refIdx)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "RemoteEnvBinding";
//   auto res = std::make_shared<RemoteEnvBindingSEXP>(sexp);
//   res->args.resize(1);
//   res->setParentReference(obj);
//   res->setREFIDX(refIdx);
//   return res;
// }

// std::shared_ptr<GlobalBindingSEXP> makeGlobalBindingSEXP(const std::string & name)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "GlobalBinding";
//   auto res = std::make_shared<GlobalBindingSEXP>(sexp);
//   res->setNAME(name);
//   return res;
// }

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
  return JSExplicitBindingDeclarationSEXP::generateFrom(funcDecl);
}

// std::shared_ptr<EnvWriteSEXP> makeEnvWrite(IRISEXP lval, IRISEXP rval, bool safe, bool thisInit)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "EnvWrite";
//   auto res = std::make_shared<EnvWriteSEXP>(sexp);

//   res->args.resize(2);

//   res->setLValTarget(lval);
//   res->setRVal(rval);
//   res->setSAFE(safe);
//   res->setTHISINIT(thisInit);

//   return res;
// }

// std::shared_ptr<ResolveEnvBindingSEXP> makeResolveEnvBindingSEXP(std::string name)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "ResolveEnvBinding";
//   auto res = std::make_shared<ResolveEnvBindingSEXP>(sexp);
//   res->setNAME(name);
//   return res;
// }

// std::shared_ptr<EnvReadSEXP> makeEnvReadSEXP(std::string name)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "EnvRead";
//   auto res = std::make_shared<EnvReadSEXP>(sexp);

//   res->args.resize(1);

//   res->setObj(makeResolveEnvBindingSEXP(name));
//   return res;
// }

// std::shared_ptr<JSNUBDSEXP> makeJSNUBDSEXP()
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "JSNUBD";
//   auto res = std::make_shared<JSNUBDSEXP>(sexp);

//   return res;
// }

// std::shared_ptr<JSSloppyDeclSEXP> makeJSSloppyDeclSEXP(std::string name, EnvBindingSEXPKindFlag kindFlag)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "JSSloppyDecl";
//   auto res = std::make_shared<JSSloppyDeclSEXP>(sexp);

//   res->setNAME(name);
//   if (kindFlag == EnvBindingSEXPKindFlag::JSLET) res->setJSLET();
//   else if (kindFlag == EnvBindingSEXPKindFlag::JSCONST) res->setJSCONST();
//   else if (kindFlag == EnvBindingSEXPKindFlag::JSVAR) res->setJSVAR();
//   else throw std::runtime_error("Invalid JSSloppyDeclSEXP kind flag");

//   return res;
// }

// std::shared_ptr<JSImplicitBindingDeclarationSEXP> makeJSImplicitBindingDeclarationSEXP(IRISEXP store, IRISEXP args, std::string bindingName, EnvBindingSEXPKindFlag kindFlag, double opid)
// {
//   auto sexp = std::make_shared<IridiumSEXP>();
//   sexp->tag = "JSImplicitBindingDeclaration";
//   auto res = std::make_shared<JSImplicitBindingDeclarationSEXP>(sexp);

//   res->args.resize(2);

//   res->setStore(store);
//   res->setArgs(args);

//   res->setNAME(bindingName);

//   if (kindFlag == EnvBindingSEXPKindFlag::JSLET) res->setJSLET();
//   else if (kindFlag == EnvBindingSEXPKindFlag::JSCONST) res->setJSCONST();
//   else if (kindFlag == EnvBindingSEXPKindFlag::JSVAR) res->setJSVAR();
//   else throw std::runtime_error("Invalid kindFlag passed while creating JSImplicitBindingDeclarationSEXP");

//   res->setOPID(opid);

//   res->setSAFE(true);
//   res->setTHISINIT(false);
//   res->setSKIPINIT();

//   return res;
// }