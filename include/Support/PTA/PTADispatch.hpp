// Generated: 2026-09-15 02:13:51
#pragma once

#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * Dispatches an IRIStatement transfer operation to its corresponding tag handler using PTAStatementContext.
 */
inline void dispatchPTAStatement(const PTAStatementContext &ptactx) {
  auto currTAG = ptactx.getTag();

  switch (currTAG) {
    case IRI_GEN::IRI_TAG::EnvRead:
      return handleEnvRead(ptactx);
    case IRI_GEN::IRI_TAG::FieldRead:
      return handleFieldRead(ptactx);
    case IRI_GEN::IRI_TAG::CallSite:
      return handleCallSite(ptactx);
    case IRI_GEN::IRI_TAG::Apply:
      return handleApply(ptactx);
    case IRI_GEN::IRI_TAG::ReturnAsync:
      return handleReturnAsync(ptactx);
    case IRI_GEN::IRI_TAG::Return:
      return handleReturn(ptactx);
    case IRI_GEN::IRI_TAG::IfJump:
      return handleIfJump(ptactx);
    case IRI_GEN::IRI_TAG::IfElseJump:
      return handleIfElseJump(ptactx);
    case IRI_GEN::IRI_TAG::Goto:
      return handleGoto(ptactx);
    case IRI_GEN::IRI_TAG::NOP:
      return handleNOP(ptactx);
    case IRI_GEN::IRI_TAG::JSCheckConstructor:
      return handleJSCheckConstructor(ptactx);
    case IRI_GEN::IRI_TAG::PVTEnvRead:
      return handlePVTEnvRead(ptactx);
    case IRI_GEN::IRI_TAG::JSPrivateFieldWrite:
      return handleJSPrivateFieldWrite(ptactx);
    case IRI_GEN::IRI_TAG::JSADDBRAND:
      return handleJSADDBRAND(ptactx);
    case IRI_GEN::IRI_TAG::JSPrivateFieldRead:
      return handleJSPrivateFieldRead(ptactx);
    case IRI_GEN::IRI_TAG::JSForOfIteratorClose:
      return handleJSForOfIteratorClose(ptactx);
    case IRI_GEN::IRI_TAG::PopCatchContext:
      return handlePopCatchContext(ptactx);
    case IRI_GEN::IRI_TAG::PopFinalizerReturnTarget:
      return handlePopFinalizerReturnTarget(ptactx);
    case IRI_GEN::IRI_TAG::InvokeFinalizer:
      return handleInvokeFinalizer(ptactx);
    case IRI_GEN::IRI_TAG::JSForOfStart:
      return handleJSForOfStart(ptactx);
    case IRI_GEN::IRI_TAG::PushCatchContext:
      return handlePushCatchContext(ptactx);
    case IRI_GEN::IRI_TAG::JSCatchContext:
      return handleJSCatchContext(ptactx);
    case IRI_GEN::IRI_TAG::Throw:
      return handleThrow(ptactx);
    case IRI_GEN::IRI_TAG::Ret:
      return handleRet(ptactx);
    case IRI_GEN::IRI_TAG::JSBinop:
      return handleJSBinop(ptactx);
    case IRI_GEN::IRI_TAG::FieldWrite:
      return handleFieldWrite(ptactx);
    case IRI_GEN::IRI_TAG::JSUnop:
      return handleJSUnop(ptactx);
    case IRI_GEN::IRI_TAG::Unop:
      return handleUnop(ptactx);
    case IRI_GEN::IRI_TAG::JSDefineObjProp:
      return handleJSDefineObjProp(ptactx);
    case IRI_GEN::IRI_TAG::Binop:
      return handleBinop(ptactx);
    case IRI_GEN::IRI_TAG::JSComputedFieldRead:
      return handleJSComputedFieldRead(ptactx);
    case IRI_GEN::IRI_TAG::JSComputedFieldWrite:
      return handleJSComputedFieldWrite(ptactx);
    case IRI_GEN::IRI_TAG::JSSuperFieldRead:
      return handleJSSuperFieldRead(ptactx);
    case IRI_GEN::IRI_TAG::JSSuperFieldWrite:
      return handleJSSuperFieldWrite(ptactx);
    case IRI_GEN::IRI_TAG::JSToObject:
      return handleJSToObject(ptactx);
    case IRI_GEN::IRI_TAG::JSAppend:
      return handleJSAppend(ptactx);
    case IRI_GEN::IRI_TAG::JSDefineObjMethod:
      return handleJSDefineObjMethod(ptactx);
    case IRI_GEN::IRI_TAG::JSCopyDataProperties:
      return handleJSCopyDataProperties(ptactx);
    case IRI_GEN::IRI_TAG::UNOPDelMemberExpr:
      return handleUNOPDelMemberExpr(ptactx);
    case IRI_GEN::IRI_TAG::UNOPDelVar:
      return handleUNOPDelVar(ptactx);
    case IRI_GEN::IRI_TAG::JSTemplate:
      return handleJSTemplate(ptactx);
    case IRI_GEN::IRI_TAG::Await:
      return handleAwait(ptactx);
    case IRI_GEN::IRI_TAG::Yield:
      return handleYield(ptactx);
    case IRI_GEN::IRI_TAG::JSInitialYield:
      return handleJSInitialYield(ptactx);
    case IRI_GEN::IRI_TAG::IDOP:
      return handleIDOP(ptactx);
    case IRI_GEN::IRI_TAG::JSIDOP:
      return handleJSIDOP(ptactx);
    case IRI_GEN::IRI_TAG::StackToHeap:
      return handleStackToHeap(ptactx);
    case IRI_GEN::IRI_TAG::JSSetHome:
      return handleJSSetHome(ptactx);
    case IRI_GEN::IRI_TAG::JSSetName:
      return handleJSSetName(ptactx);
    case IRI_GEN::IRI_TAG::JSSetPrototypeOf:
      return handleJSSetPrototypeOf(ptactx);
    case IRI_GEN::IRI_TAG::GWrite:
      return handleGWrite(ptactx);
    case IRI_GEN::IRI_TAG::LWrite:
      return handleLWrite(ptactx);
    case IRI_GEN::IRI_TAG::RWrite:
      return handleRWrite(ptactx);
    case IRI_GEN::IRI_TAG::MWrite:
      return handleMWrite(ptactx);
    case IRI_GEN::IRI_TAG::DCTRRet:
      return handleDCTRRet(ptactx);
    case IRI_GEN::IRI_TAG::ToNumeric:
      return handleToNumeric(ptactx);
    case IRI_GEN::IRI_TAG::QJSModuleInit:
      return handleQJSModuleInit(ptactx);
    case IRI_GEN::IRI_TAG::CompoundAssn:
      return handleCompoundAssn(ptactx);
    default:
      std::cerr << "[PTA Warning] Unhandled statement tag: "
                << IRI_GEN::dump_tag(currTAG) << std::endl;
      return;
  }
}

/**
 * Convenience overload allowing direct dispatch from raw stmt, incomingState, and ctx.
 */
inline void dispatchPTAStatement(const IRIStatement &stmt,
                                                Prakriti::ECMAGraph *incomingState,
                                                IRI_STORAGE::IRIContext &ctx) {
  PTAStatementContext ptactx(stmt, incomingState, ctx);
  return dispatchPTAStatement(ptactx);
}

} // namespace IRI_STRUCTURAL
