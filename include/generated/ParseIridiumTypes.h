// Generated: 2025-08-25 23:39:31
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
    if (tag == "ResolvePrivateEnvBinding") return ResolvePrivateEnvBindingSEXP::generateFrom(obj);
    if (tag == "PVTEnvRead") return PVTEnvReadSEXP::generateFrom(obj);
    if (tag == "JSPrivate") return JSPrivateSEXP::generateFrom(obj);
    if (tag == "JSPrivateFieldWrite") return JSPrivateFieldWriteSEXP::generateFrom(obj);
    if (tag == "JSADDBRAND") return JSADDBRANDSEXP::generateFrom(obj);
    if (tag == "JSPrivateFieldRead") return JSPrivateFieldReadSEXP::generateFrom(obj);
    if (tag == "PoolBinding") return PoolBindingSEXP::generateFrom(obj);
    if (tag == "ResolveContinueTarget") return ResolveContinueTargetSEXP::generateFrom(obj);
    if (tag == "ResolveBreakTarget") return ResolveBreakTargetSEXP::generateFrom(obj);
    if (tag == "JSForOfIteratorClose") return JSForOfIteratorCloseSEXP::generateFrom(obj);
    if (tag == "PopCatchContext") return PopCatchContextSEXP::generateFrom(obj);
    if (tag == "InvokeFinalizer") return InvokeFinalizerSEXP::generateFrom(obj);
    if (tag == "JSForInStart") return JSForInStartSEXP::generateFrom(obj);
    if (tag == "JSForInNext") return JSForInNextSEXP::generateFrom(obj);
    if (tag == "StackRetain") return StackRetainSEXP::generateFrom(obj);
    if (tag == "StackPop") return StackPopSEXP::generateFrom(obj);
    if (tag == "JSForOfStart") return JSForOfStartSEXP::generateFrom(obj);
    if (tag == "JSForOfNext") return JSForOfNextSEXP::generateFrom(obj);
    if (tag == "PushCatchContext") return PushCatchContextSEXP::generateFrom(obj);
    if (tag == "JSCatchContext") return JSCatchContextSEXP::generateFrom(obj);
    if (tag == "Throw") return ThrowSEXP::generateFrom(obj);
    if (tag == "Ret") return RetSEXP::generateFrom(obj);
    if (tag == "ReturnAsync") return ReturnAsyncSEXP::generateFrom(obj);
    throw std::runtime_error("ParseIridiumTypes::specialize unhandled Tag: " + tag);
  }
};