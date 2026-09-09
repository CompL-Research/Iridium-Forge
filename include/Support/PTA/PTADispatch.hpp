// Generated: 2026-09-03 11:34:38
#pragma once

#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * Dispatches an IRIStatement transfer operation to its corresponding tag handler using PTAStatementContext.
 */
inline void dispatchPTAStatement(const PTAStatementContext &ctx) {
  auto currTAG = ctx.getTag();

  switch (currTAG) {
    case IRI_GEN::IRI_TAG::EnvRead:
      return handleEnvRead(ctx);
    case IRI_GEN::IRI_TAG::FieldRead:
      return handleFieldRead(ctx);
    case IRI_GEN::IRI_TAG::CallSite:
      return handleCallSite(ctx);
    case IRI_GEN::IRI_TAG::Apply:
      return handleApply(ctx);
    case IRI_GEN::IRI_TAG::ReturnAsync:
      return handleReturnAsync(ctx);
    case IRI_GEN::IRI_TAG::Return:
      return handleReturn(ctx);
    case IRI_GEN::IRI_TAG::IfJump:
      return handleIfJump(ctx);
    case IRI_GEN::IRI_TAG::IfElseJump:
      return handleIfElseJump(ctx);
    case IRI_GEN::IRI_TAG::Goto:
      return handleGoto(ctx);
    case IRI_GEN::IRI_TAG::NOP:
      return handleNOP(ctx);
    case IRI_GEN::IRI_TAG::JSCheckConstructor:
      return handleJSCheckConstructor(ctx);
    case IRI_GEN::IRI_TAG::PVTEnvRead:
      return handlePVTEnvRead(ctx);
    case IRI_GEN::IRI_TAG::JSPrivateFieldWrite:
      return handleJSPrivateFieldWrite(ctx);
    case IRI_GEN::IRI_TAG::JSADDBRAND:
      return handleJSADDBRAND(ctx);
    case IRI_GEN::IRI_TAG::JSPrivateFieldRead:
      return handleJSPrivateFieldRead(ctx);
    case IRI_GEN::IRI_TAG::JSForOfIteratorClose:
      return handleJSForOfIteratorClose(ctx);
    case IRI_GEN::IRI_TAG::PopCatchContext:
      return handlePopCatchContext(ctx);
    case IRI_GEN::IRI_TAG::PopFinalizerReturnTarget:
      return handlePopFinalizerReturnTarget(ctx);
    case IRI_GEN::IRI_TAG::InvokeFinalizer:
      return handleInvokeFinalizer(ctx);
    case IRI_GEN::IRI_TAG::JSForOfStart:
      return handleJSForOfStart(ctx);
    case IRI_GEN::IRI_TAG::PushCatchContext:
      return handlePushCatchContext(ctx);
    case IRI_GEN::IRI_TAG::JSCatchContext:
      return handleJSCatchContext(ctx);
    case IRI_GEN::IRI_TAG::Throw:
      return handleThrow(ctx);
    case IRI_GEN::IRI_TAG::Ret:
      return handleRet(ctx);
    case IRI_GEN::IRI_TAG::JSBinop:
      return handleJSBinop(ctx);
    case IRI_GEN::IRI_TAG::FieldWrite:
      return handleFieldWrite(ctx);
    case IRI_GEN::IRI_TAG::JSUnop:
      return handleJSUnop(ctx);
    case IRI_GEN::IRI_TAG::Unop:
      return handleUnop(ctx);
    case IRI_GEN::IRI_TAG::JSDefineObjProp:
      return handleJSDefineObjProp(ctx);
    case IRI_GEN::IRI_TAG::Binop:
      return handleBinop(ctx);
    case IRI_GEN::IRI_TAG::JSComputedFieldRead:
      return handleJSComputedFieldRead(ctx);
    case IRI_GEN::IRI_TAG::JSComputedFieldWrite:
      return handleJSComputedFieldWrite(ctx);
    case IRI_GEN::IRI_TAG::JSSuperFieldRead:
      return handleJSSuperFieldRead(ctx);
    case IRI_GEN::IRI_TAG::JSSuperFieldWrite:
      return handleJSSuperFieldWrite(ctx);
    case IRI_GEN::IRI_TAG::JSToObject:
      return handleJSToObject(ctx);
    case IRI_GEN::IRI_TAG::JSAppend:
      return handleJSAppend(ctx);
    case IRI_GEN::IRI_TAG::JSDefineObjMethod:
      return handleJSDefineObjMethod(ctx);
    case IRI_GEN::IRI_TAG::JSCopyDataProperties:
      return handleJSCopyDataProperties(ctx);
    case IRI_GEN::IRI_TAG::UNOPDelMemberExpr:
      return handleUNOPDelMemberExpr(ctx);
    case IRI_GEN::IRI_TAG::UNOPDelVar:
      return handleUNOPDelVar(ctx);
    case IRI_GEN::IRI_TAG::JSTemplate:
      return handleJSTemplate(ctx);
    case IRI_GEN::IRI_TAG::Await:
      return handleAwait(ctx);
    case IRI_GEN::IRI_TAG::Yield:
      return handleYield(ctx);
    case IRI_GEN::IRI_TAG::JSInitialYield:
      return handleJSInitialYield(ctx);
    case IRI_GEN::IRI_TAG::IDOP:
      return handleIDOP(ctx);
    case IRI_GEN::IRI_TAG::JSIDOP:
      return handleJSIDOP(ctx);
    case IRI_GEN::IRI_TAG::StackToHeap:
      return handleStackToHeap(ctx);
    case IRI_GEN::IRI_TAG::JSSetHome:
      return handleJSSetHome(ctx);
    case IRI_GEN::IRI_TAG::JSSetName:
      return handleJSSetName(ctx);
    case IRI_GEN::IRI_TAG::JSSetPrototypeOf:
      return handleJSSetPrototypeOf(ctx);
    case IRI_GEN::IRI_TAG::GWrite:
      return handleGWrite(ctx);
    case IRI_GEN::IRI_TAG::LWrite:
      return handleLWrite(ctx);
    case IRI_GEN::IRI_TAG::RWrite:
      return handleRWrite(ctx);
    case IRI_GEN::IRI_TAG::MWrite:
      return handleMWrite(ctx);
    case IRI_GEN::IRI_TAG::DCTRRet:
      return handleDCTRRet(ctx);
    case IRI_GEN::IRI_TAG::ToNumeric:
      return handleToNumeric(ctx);
    case IRI_GEN::IRI_TAG::QJSModuleInit:
      return handleQJSModuleInit(ctx);
    case IRI_GEN::IRI_TAG::CompoundAssn:
      return handleCompoundAssn(ctx);
    default:
      std::cerr << "[PTA Warning] Unhandled statement tag: " 
                << IRI_GEN::dump_tag(currTAG) << std::endl;
      return;
  }
}

/**
 * Convenience overload allowing direct dispatch from raw stmt, incomingState, and pool.
 */
inline void dispatchPTAStatement(const IRIStatement &stmt,
                                                Prakriti::ECMAGraph *incomingState,
                                                IRI_STORAGE::IridiumPool &pool) {
  PTAStatementContext ctx(stmt, incomingState, pool);
  return dispatchPTAStatement(ctx);
}

} // namespace IRI_STRUCTURAL
