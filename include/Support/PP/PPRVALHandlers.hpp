// Generated: 2026-10-29 19:45:21
#pragma once

#include "Support/PP/PPContext.hpp"
#include <string>

namespace IRI_STRUCTURAL {

// ============================================================================
// PP RVal Printer Declarations
//
// Each of these renders a given IRID (an AMP or RVAL tag) as an expression.
// ============================================================================
std::string printEnvReadRVal(const PPContext &pp, IRID node);
std::string printStringRVal(const PPContext &pp, IRID node);
std::string printFieldReadRVal(const PPContext &pp, IRID node);
std::string printCallSiteRVal(const PPContext &pp, IRID node);
std::string printApplyRVal(const PPContext &pp, IRID node);
std::string printLambdaRVal(const PPContext &pp, IRID node);
std::string printEnvBindingRVal(const PPContext &pp, IRID node);
std::string printRemoteEnvBindingRVal(const PPContext &pp, IRID node);
std::string printGlobalBindingRVal(const PPContext &pp, IRID node);
std::string printScriptBindingRVal(const PPContext &pp, IRID node);
std::string printJSNUBDRVal(const PPContext &pp, IRID node);
std::string printNumberRVal(const PPContext &pp, IRID node);
std::string printJSClassRVal(const PPContext &pp, IRID node);
std::string printPVTEnvReadRVal(const PPContext &pp, IRID node);
std::string printJSPrivateRVal(const PPContext &pp, IRID node);
std::string printJSPrivateFieldReadRVal(const PPContext &pp, IRID node);
std::string printPoolBindingRVal(const PPContext &pp, IRID node);
std::string printJSForInStartRVal(const PPContext &pp, IRID node);
std::string printJSForInNextRVal(const PPContext &pp, IRID node);
std::string printJSForOfNextRVal(const PPContext &pp, IRID node);
std::string printJSCatchContextRVal(const PPContext &pp, IRID node);
std::string printJSBinopRVal(const PPContext &pp, IRID node);
std::string printJSUnopRVal(const PPContext &pp, IRID node);
std::string printUnopRVal(const PPContext &pp, IRID node);
std::string printJSObjectRVal(const PPContext &pp, IRID node);
std::string printBooleanRVal(const PPContext &pp, IRID node);
std::string printJSArrayRVal(const PPContext &pp, IRID node);
std::string printBinopRVal(const PPContext &pp, IRID node);
std::string printNullRVal(const PPContext &pp, IRID node);
std::string printJSComputedFieldReadRVal(const PPContext &pp, IRID node);
std::string printJSSuperFieldReadRVal(const PPContext &pp, IRID node);
std::string printJSToObjectRVal(const PPContext &pp, IRID node);
std::string printJSAppendRVal(const PPContext &pp, IRID node);
std::string printJSCopyDataPropertiesRVal(const PPContext &pp, IRID node);
std::string printRegExpRVal(const PPContext &pp, IRID node);
std::string printUNOPDelMemberExprRVal(const PPContext &pp, IRID node);
std::string printUNOPDelVarRVal(const PPContext &pp, IRID node);
std::string printJSTemplateRVal(const PPContext &pp, IRID node);
std::string printJSBigIntRVal(const PPContext &pp, IRID node);
std::string printAwaitRVal(const PPContext &pp, IRID node);
std::string printYieldRVal(const PPContext &pp, IRID node);
std::string printIDOPRVal(const PPContext &pp, IRID node);
std::string printJSIDOPRVal(const PPContext &pp, IRID node);
std::string printDCTRRetRVal(const PPContext &pp, IRID node);
std::string printToNumericRVal(const PPContext &pp, IRID node);
std::string printJSCTXRVal(const PPContext &pp, IRID node);
std::string printThisINITRVal(const PPContext &pp, IRID node);

} // namespace IRI_STRUCTURAL
