// Generated: 2026-10-29 23:52:39
#pragma once

#include "Support/PP/PPContext.hpp"
#include <string>

namespace IRI_STRUCTURAL {

// ============================================================================
// PP Statement Printer Declarations
//
// Each of these renders one statement as a single line (no indentation).
// ============================================================================
std::string printEnvRead(const PPContext &pp, IRID node);
std::string printFieldRead(const PPContext &pp, IRID node);
std::string printCallSite(const PPContext &pp, IRID node);
std::string printApply(const PPContext &pp, IRID node);
std::string printReturnAsync(const PPContext &pp, IRID node);
std::string printReturn(const PPContext &pp, IRID node);
std::string printIfJump(const PPContext &pp, IRID node);
std::string printIfElseJump(const PPContext &pp, IRID node);
std::string printGoto(const PPContext &pp, IRID node);
std::string printNOP(const PPContext &pp, IRID node);
std::string printJSCheckConstructor(const PPContext &pp, IRID node);
std::string printPVTEnvRead(const PPContext &pp, IRID node);
std::string printJSPrivateFieldWrite(const PPContext &pp, IRID node);
std::string printJSADDBRAND(const PPContext &pp, IRID node);
std::string printJSPrivateFieldRead(const PPContext &pp, IRID node);
std::string printJSForOfIteratorClose(const PPContext &pp, IRID node);
std::string printPopCatchContext(const PPContext &pp, IRID node);
std::string printExitFinalizer(const PPContext &pp, IRID node);
std::string printInvokeFinalizer(const PPContext &pp, IRID node);
std::string printJSForOfStart(const PPContext &pp, IRID node);
std::string printPushCatchContext(const PPContext &pp, IRID node);
std::string printJSCatchContext(const PPContext &pp, IRID node);
std::string printThrow(const PPContext &pp, IRID node);
std::string printRet(const PPContext &pp, IRID node);
std::string printJSBinop(const PPContext &pp, IRID node);
std::string printFieldWrite(const PPContext &pp, IRID node);
std::string printJSUnop(const PPContext &pp, IRID node);
std::string printUnop(const PPContext &pp, IRID node);
std::string printJSDefineObjProp(const PPContext &pp, IRID node);
std::string printBinop(const PPContext &pp, IRID node);
std::string printJSComputedFieldRead(const PPContext &pp, IRID node);
std::string printJSComputedFieldWrite(const PPContext &pp, IRID node);
std::string printJSSuperFieldRead(const PPContext &pp, IRID node);
std::string printJSSuperFieldWrite(const PPContext &pp, IRID node);
std::string printJSToObject(const PPContext &pp, IRID node);
std::string printJSAppend(const PPContext &pp, IRID node);
std::string printJSDefineObjMethod(const PPContext &pp, IRID node);
std::string printJSCopyDataProperties(const PPContext &pp, IRID node);
std::string printUNOPDelMemberExpr(const PPContext &pp, IRID node);
std::string printUNOPDelVar(const PPContext &pp, IRID node);
std::string printJSTemplate(const PPContext &pp, IRID node);
std::string printAwait(const PPContext &pp, IRID node);
std::string printYield(const PPContext &pp, IRID node);
std::string printJSInitialYield(const PPContext &pp, IRID node);
std::string printIDOP(const PPContext &pp, IRID node);
std::string printJSIDOP(const PPContext &pp, IRID node);
std::string printStackToHeap(const PPContext &pp, IRID node);
std::string printJSSetHome(const PPContext &pp, IRID node);
std::string printJSSetName(const PPContext &pp, IRID node);
std::string printJSSetPrototypeOf(const PPContext &pp, IRID node);
std::string printGWrite(const PPContext &pp, IRID node);
std::string printLWrite(const PPContext &pp, IRID node);
std::string printRWrite(const PPContext &pp, IRID node);
std::string printMWrite(const PPContext &pp, IRID node);
std::string printDCTRRet(const PPContext &pp, IRID node);
std::string printToNumeric(const PPContext &pp, IRID node);
std::string printQJSModuleInit(const PPContext &pp, IRID node);
std::string printCompoundAssn(const PPContext &pp, IRID node);
std::string printJSGetPrototypeOf(const PPContext &pp, IRID node);

} // namespace IRI_STRUCTURAL
