// Generated: 2026-10-30 00:38:17
#pragma once

#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * Renders a statement to its corresponding tag printer. Tags without a
 * printer (structural or transitional ones) fall back to ppGeneric.
 */
inline std::string ppStatement(const PPContext &pp, IRID node) {
  switch (pp.tagOf(node)) {
    case IRI_GEN::IRI_TAG::EnvRead:
      return printEnvRead(pp, node);
    case IRI_GEN::IRI_TAG::FieldRead:
      return printFieldRead(pp, node);
    case IRI_GEN::IRI_TAG::CallSite:
      return printCallSite(pp, node);
    case IRI_GEN::IRI_TAG::Apply:
      return printApply(pp, node);
    case IRI_GEN::IRI_TAG::ReturnAsync:
      return printReturnAsync(pp, node);
    case IRI_GEN::IRI_TAG::Return:
      return printReturn(pp, node);
    case IRI_GEN::IRI_TAG::IfJump:
      return printIfJump(pp, node);
    case IRI_GEN::IRI_TAG::IfElseJump:
      return printIfElseJump(pp, node);
    case IRI_GEN::IRI_TAG::Goto:
      return printGoto(pp, node);
    case IRI_GEN::IRI_TAG::NOP:
      return printNOP(pp, node);
    case IRI_GEN::IRI_TAG::JSCheckConstructor:
      return printJSCheckConstructor(pp, node);
    case IRI_GEN::IRI_TAG::PVTEnvRead:
      return printPVTEnvRead(pp, node);
    case IRI_GEN::IRI_TAG::JSPrivateFieldWrite:
      return printJSPrivateFieldWrite(pp, node);
    case IRI_GEN::IRI_TAG::JSADDBRAND:
      return printJSADDBRAND(pp, node);
    case IRI_GEN::IRI_TAG::JSPrivateFieldRead:
      return printJSPrivateFieldRead(pp, node);
    case IRI_GEN::IRI_TAG::JSForOfIteratorClose:
      return printJSForOfIteratorClose(pp, node);
    case IRI_GEN::IRI_TAG::PopCatchContext:
      return printPopCatchContext(pp, node);
    case IRI_GEN::IRI_TAG::ExitFinalizer:
      return printExitFinalizer(pp, node);
    case IRI_GEN::IRI_TAG::InvokeFinalizer:
      return printInvokeFinalizer(pp, node);
    case IRI_GEN::IRI_TAG::JSForOfStart:
      return printJSForOfStart(pp, node);
    case IRI_GEN::IRI_TAG::PushCatchContext:
      return printPushCatchContext(pp, node);
    case IRI_GEN::IRI_TAG::JSCatchContext:
      return printJSCatchContext(pp, node);
    case IRI_GEN::IRI_TAG::Throw:
      return printThrow(pp, node);
    case IRI_GEN::IRI_TAG::Ret:
      return printRet(pp, node);
    case IRI_GEN::IRI_TAG::JSBinop:
      return printJSBinop(pp, node);
    case IRI_GEN::IRI_TAG::FieldWrite:
      return printFieldWrite(pp, node);
    case IRI_GEN::IRI_TAG::JSUnop:
      return printJSUnop(pp, node);
    case IRI_GEN::IRI_TAG::Unop:
      return printUnop(pp, node);
    case IRI_GEN::IRI_TAG::JSDefineObjProp:
      return printJSDefineObjProp(pp, node);
    case IRI_GEN::IRI_TAG::Binop:
      return printBinop(pp, node);
    case IRI_GEN::IRI_TAG::JSComputedFieldRead:
      return printJSComputedFieldRead(pp, node);
    case IRI_GEN::IRI_TAG::JSComputedFieldWrite:
      return printJSComputedFieldWrite(pp, node);
    case IRI_GEN::IRI_TAG::JSToObject:
      return printJSToObject(pp, node);
    case IRI_GEN::IRI_TAG::JSAppend:
      return printJSAppend(pp, node);
    case IRI_GEN::IRI_TAG::JSDefineObjMethod:
      return printJSDefineObjMethod(pp, node);
    case IRI_GEN::IRI_TAG::JSCopyDataProperties:
      return printJSCopyDataProperties(pp, node);
    case IRI_GEN::IRI_TAG::UNOPDelMemberExpr:
      return printUNOPDelMemberExpr(pp, node);
    case IRI_GEN::IRI_TAG::UNOPDelVar:
      return printUNOPDelVar(pp, node);
    case IRI_GEN::IRI_TAG::JSTemplate:
      return printJSTemplate(pp, node);
    case IRI_GEN::IRI_TAG::Await:
      return printAwait(pp, node);
    case IRI_GEN::IRI_TAG::Yield:
      return printYield(pp, node);
    case IRI_GEN::IRI_TAG::JSInitialYield:
      return printJSInitialYield(pp, node);
    case IRI_GEN::IRI_TAG::IDOP:
      return printIDOP(pp, node);
    case IRI_GEN::IRI_TAG::JSIDOP:
      return printJSIDOP(pp, node);
    case IRI_GEN::IRI_TAG::StackToHeap:
      return printStackToHeap(pp, node);
    case IRI_GEN::IRI_TAG::JSSetHome:
      return printJSSetHome(pp, node);
    case IRI_GEN::IRI_TAG::JSSetName:
      return printJSSetName(pp, node);
    case IRI_GEN::IRI_TAG::JSSetPrototypeOf:
      return printJSSetPrototypeOf(pp, node);
    case IRI_GEN::IRI_TAG::GWrite:
      return printGWrite(pp, node);
    case IRI_GEN::IRI_TAG::LWrite:
      return printLWrite(pp, node);
    case IRI_GEN::IRI_TAG::RWrite:
      return printRWrite(pp, node);
    case IRI_GEN::IRI_TAG::MWrite:
      return printMWrite(pp, node);
    case IRI_GEN::IRI_TAG::DCTRRet:
      return printDCTRRet(pp, node);
    case IRI_GEN::IRI_TAG::ToNumeric:
      return printToNumeric(pp, node);
    case IRI_GEN::IRI_TAG::QJSModuleInit:
      return printQJSModuleInit(pp, node);
    case IRI_GEN::IRI_TAG::CompoundAssn:
      return printCompoundAssn(pp, node);
    case IRI_GEN::IRI_TAG::JSConstruct:
      return printJSConstruct(pp, node);
    case IRI_GEN::IRI_TAG::JSConstructApply:
      return printJSConstructApply(pp, node);
    case IRI_GEN::IRI_TAG::JSGetPrototypeOf:
      return printJSGetPrototypeOf(pp, node);
    default:
      return ppGeneric(pp, node);
  }
}

} // namespace IRI_STRUCTURAL
