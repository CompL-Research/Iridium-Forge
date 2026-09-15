// Generated: 2026-09-15 12:01:18
#pragma once

#include "Support/PTA/PTAContext.hpp"

namespace IRI_STRUCTURAL {

// ============================================================================
// PTA Statement Handler Function Declarations
// ============================================================================
void handleEnvRead(const PTAStatementContext &ptactx);
void handleFieldRead(const PTAStatementContext &ptactx);
void handleCallSite(const PTAStatementContext &ptactx);
void handleApply(const PTAStatementContext &ptactx);
void handleReturnAsync(const PTAStatementContext &ptactx);
void handleReturn(const PTAStatementContext &ptactx);
void handleIfJump(const PTAStatementContext &ptactx);
void handleIfElseJump(const PTAStatementContext &ptactx);
void handleGoto(const PTAStatementContext &ptactx);
void handleNOP(const PTAStatementContext &ptactx);
void handleJSCheckConstructor(const PTAStatementContext &ptactx);
void handlePVTEnvRead(const PTAStatementContext &ptactx);
void handleJSPrivateFieldWrite(const PTAStatementContext &ptactx);
void handleJSADDBRAND(const PTAStatementContext &ptactx);
void handleJSPrivateFieldRead(const PTAStatementContext &ptactx);
void handleJSForOfIteratorClose(const PTAStatementContext &ptactx);
void handlePopCatchContext(const PTAStatementContext &ptactx);
void handlePopFinalizerReturnTarget(const PTAStatementContext &ptactx);
void handleInvokeFinalizer(const PTAStatementContext &ptactx);
void handleJSForOfStart(const PTAStatementContext &ptactx);
void handlePushCatchContext(const PTAStatementContext &ptactx);
void handleJSCatchContext(const PTAStatementContext &ptactx);
void handleThrow(const PTAStatementContext &ptactx);
void handleRet(const PTAStatementContext &ptactx);
void handleJSBinop(const PTAStatementContext &ptactx);
void handleFieldWrite(const PTAStatementContext &ptactx);
void handleJSUnop(const PTAStatementContext &ptactx);
void handleUnop(const PTAStatementContext &ptactx);
void handleJSDefineObjProp(const PTAStatementContext &ptactx);
void handleBinop(const PTAStatementContext &ptactx);
void handleJSComputedFieldRead(const PTAStatementContext &ptactx);
void handleJSComputedFieldWrite(const PTAStatementContext &ptactx);
void handleJSSuperFieldRead(const PTAStatementContext &ptactx);
void handleJSSuperFieldWrite(const PTAStatementContext &ptactx);
void handleJSToObject(const PTAStatementContext &ptactx);
void handleJSAppend(const PTAStatementContext &ptactx);
void handleJSDefineObjMethod(const PTAStatementContext &ptactx);
void handleJSCopyDataProperties(const PTAStatementContext &ptactx);
void handleUNOPDelMemberExpr(const PTAStatementContext &ptactx);
void handleUNOPDelVar(const PTAStatementContext &ptactx);
void handleJSTemplate(const PTAStatementContext &ptactx);
void handleAwait(const PTAStatementContext &ptactx);
void handleYield(const PTAStatementContext &ptactx);
void handleJSInitialYield(const PTAStatementContext &ptactx);
void handleIDOP(const PTAStatementContext &ptactx);
void handleJSIDOP(const PTAStatementContext &ptactx);
void handleStackToHeap(const PTAStatementContext &ptactx);
void handleJSSetHome(const PTAStatementContext &ptactx);
void handleJSSetName(const PTAStatementContext &ptactx);
void handleJSSetPrototypeOf(const PTAStatementContext &ptactx);
void handleGWrite(const PTAStatementContext &ptactx);
void handleLWrite(const PTAStatementContext &ptactx);
void handleRWrite(const PTAStatementContext &ptactx);
void handleMWrite(const PTAStatementContext &ptactx);
void handleDCTRRet(const PTAStatementContext &ptactx);
void handleToNumeric(const PTAStatementContext &ptactx);
void handleQJSModuleInit(const PTAStatementContext &ptactx);
void handleCompoundAssn(const PTAStatementContext &ptactx);

// ============================================================================
// Optional IPTATransferHandler Interface (for Visitor/Polymorphic Overrides)
// ============================================================================
class IPTATransferHandler {
public:
  virtual ~IPTATransferHandler() = default;

  virtual void visit_EnvRead(const PTAStatementContext &ptactx) {
    return handleEnvRead(ptactx);
  }
  virtual void visit_FieldRead(const PTAStatementContext &ptactx) {
    return handleFieldRead(ptactx);
  }
  virtual void visit_CallSite(const PTAStatementContext &ptactx) {
    return handleCallSite(ptactx);
  }
  virtual void visit_Apply(const PTAStatementContext &ptactx) {
    return handleApply(ptactx);
  }
  virtual void visit_ReturnAsync(const PTAStatementContext &ptactx) {
    return handleReturnAsync(ptactx);
  }
  virtual void visit_Return(const PTAStatementContext &ptactx) {
    return handleReturn(ptactx);
  }
  virtual void visit_IfJump(const PTAStatementContext &ptactx) {
    return handleIfJump(ptactx);
  }
  virtual void visit_IfElseJump(const PTAStatementContext &ptactx) {
    return handleIfElseJump(ptactx);
  }
  virtual void visit_Goto(const PTAStatementContext &ptactx) {
    return handleGoto(ptactx);
  }
  virtual void visit_NOP(const PTAStatementContext &ptactx) {
    return handleNOP(ptactx);
  }
  virtual void visit_JSCheckConstructor(const PTAStatementContext &ptactx) {
    return handleJSCheckConstructor(ptactx);
  }
  virtual void visit_PVTEnvRead(const PTAStatementContext &ptactx) {
    return handlePVTEnvRead(ptactx);
  }
  virtual void visit_JSPrivateFieldWrite(const PTAStatementContext &ptactx) {
    return handleJSPrivateFieldWrite(ptactx);
  }
  virtual void visit_JSADDBRAND(const PTAStatementContext &ptactx) {
    return handleJSADDBRAND(ptactx);
  }
  virtual void visit_JSPrivateFieldRead(const PTAStatementContext &ptactx) {
    return handleJSPrivateFieldRead(ptactx);
  }
  virtual void visit_JSForOfIteratorClose(const PTAStatementContext &ptactx) {
    return handleJSForOfIteratorClose(ptactx);
  }
  virtual void visit_PopCatchContext(const PTAStatementContext &ptactx) {
    return handlePopCatchContext(ptactx);
  }
  virtual void visit_PopFinalizerReturnTarget(const PTAStatementContext &ptactx) {
    return handlePopFinalizerReturnTarget(ptactx);
  }
  virtual void visit_InvokeFinalizer(const PTAStatementContext &ptactx) {
    return handleInvokeFinalizer(ptactx);
  }
  virtual void visit_JSForOfStart(const PTAStatementContext &ptactx) {
    return handleJSForOfStart(ptactx);
  }
  virtual void visit_PushCatchContext(const PTAStatementContext &ptactx) {
    return handlePushCatchContext(ptactx);
  }
  virtual void visit_JSCatchContext(const PTAStatementContext &ptactx) {
    return handleJSCatchContext(ptactx);
  }
  virtual void visit_Throw(const PTAStatementContext &ptactx) {
    return handleThrow(ptactx);
  }
  virtual void visit_Ret(const PTAStatementContext &ptactx) {
    return handleRet(ptactx);
  }
  virtual void visit_JSBinop(const PTAStatementContext &ptactx) {
    return handleJSBinop(ptactx);
  }
  virtual void visit_FieldWrite(const PTAStatementContext &ptactx) {
    return handleFieldWrite(ptactx);
  }
  virtual void visit_JSUnop(const PTAStatementContext &ptactx) {
    return handleJSUnop(ptactx);
  }
  virtual void visit_Unop(const PTAStatementContext &ptactx) {
    return handleUnop(ptactx);
  }
  virtual void visit_JSDefineObjProp(const PTAStatementContext &ptactx) {
    return handleJSDefineObjProp(ptactx);
  }
  virtual void visit_Binop(const PTAStatementContext &ptactx) {
    return handleBinop(ptactx);
  }
  virtual void visit_JSComputedFieldRead(const PTAStatementContext &ptactx) {
    return handleJSComputedFieldRead(ptactx);
  }
  virtual void visit_JSComputedFieldWrite(const PTAStatementContext &ptactx) {
    return handleJSComputedFieldWrite(ptactx);
  }
  virtual void visit_JSSuperFieldRead(const PTAStatementContext &ptactx) {
    return handleJSSuperFieldRead(ptactx);
  }
  virtual void visit_JSSuperFieldWrite(const PTAStatementContext &ptactx) {
    return handleJSSuperFieldWrite(ptactx);
  }
  virtual void visit_JSToObject(const PTAStatementContext &ptactx) {
    return handleJSToObject(ptactx);
  }
  virtual void visit_JSAppend(const PTAStatementContext &ptactx) {
    return handleJSAppend(ptactx);
  }
  virtual void visit_JSDefineObjMethod(const PTAStatementContext &ptactx) {
    return handleJSDefineObjMethod(ptactx);
  }
  virtual void visit_JSCopyDataProperties(const PTAStatementContext &ptactx) {
    return handleJSCopyDataProperties(ptactx);
  }
  virtual void visit_UNOPDelMemberExpr(const PTAStatementContext &ptactx) {
    return handleUNOPDelMemberExpr(ptactx);
  }
  virtual void visit_UNOPDelVar(const PTAStatementContext &ptactx) {
    return handleUNOPDelVar(ptactx);
  }
  virtual void visit_JSTemplate(const PTAStatementContext &ptactx) {
    return handleJSTemplate(ptactx);
  }
  virtual void visit_Await(const PTAStatementContext &ptactx) {
    return handleAwait(ptactx);
  }
  virtual void visit_Yield(const PTAStatementContext &ptactx) {
    return handleYield(ptactx);
  }
  virtual void visit_JSInitialYield(const PTAStatementContext &ptactx) {
    return handleJSInitialYield(ptactx);
  }
  virtual void visit_IDOP(const PTAStatementContext &ptactx) {
    return handleIDOP(ptactx);
  }
  virtual void visit_JSIDOP(const PTAStatementContext &ptactx) {
    return handleJSIDOP(ptactx);
  }
  virtual void visit_StackToHeap(const PTAStatementContext &ptactx) {
    return handleStackToHeap(ptactx);
  }
  virtual void visit_JSSetHome(const PTAStatementContext &ptactx) {
    return handleJSSetHome(ptactx);
  }
  virtual void visit_JSSetName(const PTAStatementContext &ptactx) {
    return handleJSSetName(ptactx);
  }
  virtual void visit_JSSetPrototypeOf(const PTAStatementContext &ptactx) {
    return handleJSSetPrototypeOf(ptactx);
  }
  virtual void visit_GWrite(const PTAStatementContext &ptactx) {
    return handleGWrite(ptactx);
  }
  virtual void visit_LWrite(const PTAStatementContext &ptactx) {
    return handleLWrite(ptactx);
  }
  virtual void visit_RWrite(const PTAStatementContext &ptactx) {
    return handleRWrite(ptactx);
  }
  virtual void visit_MWrite(const PTAStatementContext &ptactx) {
    return handleMWrite(ptactx);
  }
  virtual void visit_DCTRRet(const PTAStatementContext &ptactx) {
    return handleDCTRRet(ptactx);
  }
  virtual void visit_ToNumeric(const PTAStatementContext &ptactx) {
    return handleToNumeric(ptactx);
  }
  virtual void visit_QJSModuleInit(const PTAStatementContext &ptactx) {
    return handleQJSModuleInit(ptactx);
  }
  virtual void visit_CompoundAssn(const PTAStatementContext &ptactx) {
    return handleCompoundAssn(ptactx);
  }
};

} // namespace IRI_STRUCTURAL
