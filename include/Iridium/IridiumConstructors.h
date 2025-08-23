#pragma once
#include "Iridium/Globals.h"

class NOPSEXP;
class ListSEXP;
class GlobalBindingSEXP;
class EnvWriteSEXP;
class JSExplicitBindingDeclarationSEXP;
class JSFuncDeclSEXP;
class ResolveEnvBindingSEXP;
class EnvReadSEXP;
class JSNUBDSEXP;
class JSSloppyDeclSEXP;
class JSImplicitBindingDeclarationSEXP;

std::shared_ptr<NOPSEXP> makeNOPSEXP();
std::shared_ptr<ListSEXP> makeListSEXP();
std::shared_ptr<BindingsSEXP> makeBindingsSEXP(double parentScope);
std::shared_ptr<BBContainerSEXP> makeBBContainerSEXP(double scopeIdx, double parentScope);
std::shared_ptr<EnvBindingSEXP> makeEnvBindingSEXP(double refIdx, double idx, std::string b, EnvBindingSEXPKindFlag kindFlag, double scope, double parentScope);
std::shared_ptr<RemoteEnvBindingSEXP> makeRemoteEnvBindingSEXP(std::shared_ptr<EnvBindingSEXP>, double refIdx);

std::shared_ptr<GlobalBindingSEXP> makeGlobalBindingSEXP(const std::string & name);
std::shared_ptr<EnvWriteSEXP> reduceJSDecl(std::shared_ptr<JSExplicitBindingDeclarationSEXP> explicitBinding);
std::shared_ptr<JSExplicitBindingDeclarationSEXP> reduceJSFunDecl(std::shared_ptr<JSFuncDeclSEXP> funcDecl);
std::shared_ptr<EnvWriteSEXP> makeEnvWrite(IRISEXP lval, IRISEXP rval, bool safe, bool thisInit);
std::shared_ptr<ResolveEnvBindingSEXP> makeResolveEnvBindingSEXP(std::string name);
std::shared_ptr<EnvReadSEXP> makeEnvReadSEXP(std::string name);
std::shared_ptr<JSNUBDSEXP> makeJSNUBDSEXP();
std::shared_ptr<JSSloppyDeclSEXP> makeJSSloppyDeclSEXP(std::string name, EnvBindingSEXPKindFlag kindFlag);
std::shared_ptr<JSImplicitBindingDeclarationSEXP> makeJSImplicitBindingDeclarationSEXP(IRISEXP store, IRISEXP args, std::string bindingName, EnvBindingSEXPKindFlag kindFlag, double opid);