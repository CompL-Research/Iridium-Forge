// Generated: 2026-09-15 01:52:09
#pragma once

#include "Support/PTA/PTAContext.hpp"

namespace IRI_STRUCTURAL {

// ============================================================================
// PTA Transfer Handler Function Declarations
// ============================================================================
void handleEnvRead(const PTAStatementContext &ctx);
void handleFieldRead(const PTAStatementContext &ctx);
void handleCallSite(const PTAStatementContext &ctx);
void handleApply(const PTAStatementContext &ctx);
void handleReturnAsync(const PTAStatementContext &ctx);
void handleReturn(const PTAStatementContext &ctx);
void handleIfJump(const PTAStatementContext &ctx);
void handleIfElseJump(const PTAStatementContext &ctx);
void handleGoto(const PTAStatementContext &ctx);
void handleNOP(const PTAStatementContext &ctx);
void handleJSCheckConstructor(const PTAStatementContext &ctx);
void handlePVTEnvRead(const PTAStatementContext &ctx);
void handleJSPrivateFieldWrite(const PTAStatementContext &ctx);
void handleJSADDBRAND(const PTAStatementContext &ctx);
void handleJSPrivateFieldRead(const PTAStatementContext &ctx);
void handleJSForOfIteratorClose(const PTAStatementContext &ctx);
void handlePopCatchContext(const PTAStatementContext &ctx);
void handlePopFinalizerReturnTarget(const PTAStatementContext &ctx);
void handleInvokeFinalizer(const PTAStatementContext &ctx);
void handleJSForOfStart(const PTAStatementContext &ctx);
void handlePushCatchContext(const PTAStatementContext &ctx);
void handleJSCatchContext(const PTAStatementContext &ctx);
void handleThrow(const PTAStatementContext &ctx);
void handleRet(const PTAStatementContext &ctx);
void handleJSBinop(const PTAStatementContext &ctx);
void handleFieldWrite(const PTAStatementContext &ctx);
void handleJSUnop(const PTAStatementContext &ctx);
void handleUnop(const PTAStatementContext &ctx);
void handleJSDefineObjProp(const PTAStatementContext &ctx);
void handleBinop(const PTAStatementContext &ctx);
void handleJSComputedFieldRead(const PTAStatementContext &ctx);
void handleJSComputedFieldWrite(const PTAStatementContext &ctx);
void handleJSSuperFieldRead(const PTAStatementContext &ctx);
void handleJSSuperFieldWrite(const PTAStatementContext &ctx);
void handleJSToObject(const PTAStatementContext &ctx);
void handleJSAppend(const PTAStatementContext &ctx);
void handleJSDefineObjMethod(const PTAStatementContext &ctx);
void handleJSCopyDataProperties(const PTAStatementContext &ctx);
void handleUNOPDelMemberExpr(const PTAStatementContext &ctx);
void handleUNOPDelVar(const PTAStatementContext &ctx);
void handleJSTemplate(const PTAStatementContext &ctx);
void handleAwait(const PTAStatementContext &ctx);
void handleYield(const PTAStatementContext &ctx);
void handleJSInitialYield(const PTAStatementContext &ctx);
void handleIDOP(const PTAStatementContext &ctx);
void handleJSIDOP(const PTAStatementContext &ctx);
void handleStackToHeap(const PTAStatementContext &ctx);
void handleJSSetHome(const PTAStatementContext &ctx);
void handleJSSetName(const PTAStatementContext &ctx);
void handleJSSetPrototypeOf(const PTAStatementContext &ctx);
void handleGWrite(const PTAStatementContext &ctx);
void handleLWrite(const PTAStatementContext &ctx);
void handleRWrite(const PTAStatementContext &ctx);
void handleMWrite(const PTAStatementContext &ctx);
void handleDCTRRet(const PTAStatementContext &ctx);
void handleToNumeric(const PTAStatementContext &ctx);
void handleQJSModuleInit(const PTAStatementContext &ctx);
void handleCompoundAssn(const PTAStatementContext &ctx);

// ============================================================================
// Optional IPTATransferHandler Interface (for Visitor/Polymorphic Overrides)
// ============================================================================
class IPTATransferHandler {
public:
  virtual ~IPTATransferHandler() = default;

  virtual void visit_EnvRead(const PTAStatementContext &ctx) {
    return handleEnvRead(ctx);
  }
  virtual void visit_FieldRead(const PTAStatementContext &ctx) {
    return handleFieldRead(ctx);
  }
  virtual void visit_CallSite(const PTAStatementContext &ctx) {
    return handleCallSite(ctx);
  }
  virtual void visit_Apply(const PTAStatementContext &ctx) {
    return handleApply(ctx);
  }
  virtual void visit_ReturnAsync(const PTAStatementContext &ctx) {
    return handleReturnAsync(ctx);
  }
  virtual void visit_Return(const PTAStatementContext &ctx) {
    return handleReturn(ctx);
  }
  virtual void visit_IfJump(const PTAStatementContext &ctx) {
    return handleIfJump(ctx);
  }
  virtual void visit_IfElseJump(const PTAStatementContext &ctx) {
    return handleIfElseJump(ctx);
  }
  virtual void visit_Goto(const PTAStatementContext &ctx) {
    return handleGoto(ctx);
  }
  virtual void visit_NOP(const PTAStatementContext &ctx) {
    return handleNOP(ctx);
  }
  virtual void visit_JSCheckConstructor(const PTAStatementContext &ctx) {
    return handleJSCheckConstructor(ctx);
  }
  virtual void visit_PVTEnvRead(const PTAStatementContext &ctx) {
    return handlePVTEnvRead(ctx);
  }
  virtual void visit_JSPrivateFieldWrite(const PTAStatementContext &ctx) {
    return handleJSPrivateFieldWrite(ctx);
  }
  virtual void visit_JSADDBRAND(const PTAStatementContext &ctx) {
    return handleJSADDBRAND(ctx);
  }
  virtual void visit_JSPrivateFieldRead(const PTAStatementContext &ctx) {
    return handleJSPrivateFieldRead(ctx);
  }
  virtual void visit_JSForOfIteratorClose(const PTAStatementContext &ctx) {
    return handleJSForOfIteratorClose(ctx);
  }
  virtual void visit_PopCatchContext(const PTAStatementContext &ctx) {
    return handlePopCatchContext(ctx);
  }
  virtual void visit_PopFinalizerReturnTarget(const PTAStatementContext &ctx) {
    return handlePopFinalizerReturnTarget(ctx);
  }
  virtual void visit_InvokeFinalizer(const PTAStatementContext &ctx) {
    return handleInvokeFinalizer(ctx);
  }
  virtual void visit_JSForOfStart(const PTAStatementContext &ctx) {
    return handleJSForOfStart(ctx);
  }
  virtual void visit_PushCatchContext(const PTAStatementContext &ctx) {
    return handlePushCatchContext(ctx);
  }
  virtual void visit_JSCatchContext(const PTAStatementContext &ctx) {
    return handleJSCatchContext(ctx);
  }
  virtual void visit_Throw(const PTAStatementContext &ctx) {
    return handleThrow(ctx);
  }
  virtual void visit_Ret(const PTAStatementContext &ctx) {
    return handleRet(ctx);
  }
  virtual void visit_JSBinop(const PTAStatementContext &ctx) {
    return handleJSBinop(ctx);
  }
  virtual void visit_FieldWrite(const PTAStatementContext &ctx) {
    return handleFieldWrite(ctx);
  }
  virtual void visit_JSUnop(const PTAStatementContext &ctx) {
    return handleJSUnop(ctx);
  }
  virtual void visit_Unop(const PTAStatementContext &ctx) {
    return handleUnop(ctx);
  }
  virtual void visit_JSDefineObjProp(const PTAStatementContext &ctx) {
    return handleJSDefineObjProp(ctx);
  }
  virtual void visit_Binop(const PTAStatementContext &ctx) {
    return handleBinop(ctx);
  }
  virtual void visit_JSComputedFieldRead(const PTAStatementContext &ctx) {
    return handleJSComputedFieldRead(ctx);
  }
  virtual void visit_JSComputedFieldWrite(const PTAStatementContext &ctx) {
    return handleJSComputedFieldWrite(ctx);
  }
  virtual void visit_JSSuperFieldRead(const PTAStatementContext &ctx) {
    return handleJSSuperFieldRead(ctx);
  }
  virtual void visit_JSSuperFieldWrite(const PTAStatementContext &ctx) {
    return handleJSSuperFieldWrite(ctx);
  }
  virtual void visit_JSToObject(const PTAStatementContext &ctx) {
    return handleJSToObject(ctx);
  }
  virtual void visit_JSAppend(const PTAStatementContext &ctx) {
    return handleJSAppend(ctx);
  }
  virtual void visit_JSDefineObjMethod(const PTAStatementContext &ctx) {
    return handleJSDefineObjMethod(ctx);
  }
  virtual void visit_JSCopyDataProperties(const PTAStatementContext &ctx) {
    return handleJSCopyDataProperties(ctx);
  }
  virtual void visit_UNOPDelMemberExpr(const PTAStatementContext &ctx) {
    return handleUNOPDelMemberExpr(ctx);
  }
  virtual void visit_UNOPDelVar(const PTAStatementContext &ctx) {
    return handleUNOPDelVar(ctx);
  }
  virtual void visit_JSTemplate(const PTAStatementContext &ctx) {
    return handleJSTemplate(ctx);
  }
  virtual void visit_Await(const PTAStatementContext &ctx) {
    return handleAwait(ctx);
  }
  virtual void visit_Yield(const PTAStatementContext &ctx) {
    return handleYield(ctx);
  }
  virtual void visit_JSInitialYield(const PTAStatementContext &ctx) {
    return handleJSInitialYield(ctx);
  }
  virtual void visit_IDOP(const PTAStatementContext &ctx) {
    return handleIDOP(ctx);
  }
  virtual void visit_JSIDOP(const PTAStatementContext &ctx) {
    return handleJSIDOP(ctx);
  }
  virtual void visit_StackToHeap(const PTAStatementContext &ctx) {
    return handleStackToHeap(ctx);
  }
  virtual void visit_JSSetHome(const PTAStatementContext &ctx) {
    return handleJSSetHome(ctx);
  }
  virtual void visit_JSSetName(const PTAStatementContext &ctx) {
    return handleJSSetName(ctx);
  }
  virtual void visit_JSSetPrototypeOf(const PTAStatementContext &ctx) {
    return handleJSSetPrototypeOf(ctx);
  }
  virtual void visit_GWrite(const PTAStatementContext &ctx) {
    return handleGWrite(ctx);
  }
  virtual void visit_LWrite(const PTAStatementContext &ctx) {
    return handleLWrite(ctx);
  }
  virtual void visit_RWrite(const PTAStatementContext &ctx) {
    return handleRWrite(ctx);
  }
  virtual void visit_MWrite(const PTAStatementContext &ctx) {
    return handleMWrite(ctx);
  }
  virtual void visit_DCTRRet(const PTAStatementContext &ctx) {
    return handleDCTRRet(ctx);
  }
  virtual void visit_ToNumeric(const PTAStatementContext &ctx) {
    return handleToNumeric(ctx);
  }
  virtual void visit_QJSModuleInit(const PTAStatementContext &ctx) {
    return handleQJSModuleInit(ctx);
  }
  virtual void visit_CompoundAssn(const PTAStatementContext &ctx) {
    return handleCompoundAssn(ctx);
  }
};

} // namespace IRI_STRUCTURAL
