// Generated: 2025-08-23 18:45:45
#pragma once
#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
#include "generated/IridiumTypes.h"
class ParseIridiumTypes {
public:
  static IRISEXP specialize(IRISEXP obj) {
    auto & tag = obj->tag;
    if (tag == "File") return FileSEXP::generateFrom(obj);
    if (tag == "ResolveEnvBinding") return ResolveEnvBindingSEXP::generateFrom(obj);
    if (tag == "List") return ListSEXP::generateFrom(obj);
    if (tag == "JSImplicitBindingDeclaration") return JSImplicitBindingDeclarationSEXP::generateFrom(obj);
    if (tag == "EnvRead") return EnvReadSEXP::generateFrom(obj);
    if (tag == "IfJump") return IfJumpSEXP::generateFrom(obj);
    if (tag == "String") return StringSEXP::generateFrom(obj);
    if (tag == "FieldRead") return FieldReadSEXP::generateFrom(obj);
    if (tag == "JSExplicitBindingDeclaration") return JSExplicitBindingDeclarationSEXP::generateFrom(obj);
    if (tag == "CallSite") return CallSiteSEXP::generateFrom(obj);
    if (tag == "ReturnAsync") return ReturnAsyncSEXP::generateFrom(obj);
    if (tag == "BB") return BBSEXP::generateFrom(obj);
    if (tag == "Return") return ReturnSEXP::generateFrom(obj);
    if (tag == "IfElseJump") return IfElseJumpSEXP::generateFrom(obj);
    if (tag == "Goto") return GotoSEXP::generateFrom(obj);
    if (tag == "JSFuncDecl") return JSFuncDeclSEXP::generateFrom(obj);
    if (tag == "Lambda") return LambdaSEXP::generateFrom(obj);
    if (tag == "NOP") return NOPSEXP::generateFrom(obj);
    if (tag == "BBContainer") return BBContainerSEXP::generateFrom(obj);
    if (tag == "Bindings") return BindingsSEXP::generateFrom(obj);
    if (tag == "StarExport") return StarExportSEXP::generateFrom(obj);
    if (tag == "StaticImport") return StaticImportSEXP::generateFrom(obj);
    if (tag == "LocalStaticExport") return LocalStaticExportSEXP::generateFrom(obj);
    if (tag == "NamedReexport") return NamedReexportSEXP::generateFrom(obj);
    if (tag == "ModuleRequest") return ModuleRequestSEXP::generateFrom(obj);
    if (tag == "EnvBinding") return EnvBindingSEXP::generateFrom(obj);
    if (tag == "RemoteEnvBinding") return RemoteEnvBindingSEXP::generateFrom(obj);
    if (tag == "GlobalBinding") return GlobalBindingSEXP::generateFrom(obj);
    if (tag == "EnvWrite") return EnvWriteSEXP::generateFrom(obj);
    if (tag == "JSNUBD") return JSNUBDSEXP::generateFrom(obj);
    if (tag == "JSSloppyDecl") return JSSloppyDeclSEXP::generateFrom(obj);
    if (tag == "Number") return NumberSEXP::generateFrom(obj);
    if (tag == "JSClass") return JSClassSEXP::generateFrom(obj);
    if (tag == "JSCheckConstructor") return JSCheckConstructorSEXP::generateFrom(obj);
    if (tag == "StackReject") return StackRejectSEXP::generateFrom(obj);
    throw std::runtime_error("ParseIridiumTypes::specialize unhandled Tag: " + tag);
  }
};