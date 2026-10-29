// Generated: 2026-10-29 19:59:04
#pragma once

#include "Support/PP.hpp"
#include "Support/PP/PPRVALHandlers.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * Renders IRID node as an expression. Tags without a printer (structural or
 * transitional ones) fall back to ppGeneric.
 */
inline std::string ppRVal(const PPContext &pp, IRID node) {
  switch (pp.tagOf(node)) {
    case IRI_GEN::IRI_TAG::EnvRead:
      return printEnvReadRVal(pp, node);
    case IRI_GEN::IRI_TAG::String:
      return printStringRVal(pp, node);
    case IRI_GEN::IRI_TAG::FieldRead:
      return printFieldReadRVal(pp, node);
    case IRI_GEN::IRI_TAG::CallSite:
      return printCallSiteRVal(pp, node);
    case IRI_GEN::IRI_TAG::Apply:
      return printApplyRVal(pp, node);
    case IRI_GEN::IRI_TAG::Lambda:
      return printLambdaRVal(pp, node);
    case IRI_GEN::IRI_TAG::EnvBinding:
      return printEnvBindingRVal(pp, node);
    case IRI_GEN::IRI_TAG::RemoteEnvBinding:
      return printRemoteEnvBindingRVal(pp, node);
    case IRI_GEN::IRI_TAG::GlobalBinding:
      return printGlobalBindingRVal(pp, node);
    case IRI_GEN::IRI_TAG::ScriptBinding:
      return printScriptBindingRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSNUBD:
      return printJSNUBDRVal(pp, node);
    case IRI_GEN::IRI_TAG::Number:
      return printNumberRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSClass:
      return printJSClassRVal(pp, node);
    case IRI_GEN::IRI_TAG::PVTEnvRead:
      return printPVTEnvReadRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSPrivate:
      return printJSPrivateRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSPrivateFieldRead:
      return printJSPrivateFieldReadRVal(pp, node);
    case IRI_GEN::IRI_TAG::PoolBinding:
      return printPoolBindingRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSForInStart:
      return printJSForInStartRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSForInNext:
      return printJSForInNextRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSForOfNext:
      return printJSForOfNextRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSCatchContext:
      return printJSCatchContextRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSBinop:
      return printJSBinopRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSUnop:
      return printJSUnopRVal(pp, node);
    case IRI_GEN::IRI_TAG::Unop:
      return printUnopRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSObject:
      return printJSObjectRVal(pp, node);
    case IRI_GEN::IRI_TAG::Boolean:
      return printBooleanRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSArray:
      return printJSArrayRVal(pp, node);
    case IRI_GEN::IRI_TAG::Binop:
      return printBinopRVal(pp, node);
    case IRI_GEN::IRI_TAG::Null:
      return printNullRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSComputedFieldRead:
      return printJSComputedFieldReadRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSSuperFieldRead:
      return printJSSuperFieldReadRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSToObject:
      return printJSToObjectRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSAppend:
      return printJSAppendRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSCopyDataProperties:
      return printJSCopyDataPropertiesRVal(pp, node);
    case IRI_GEN::IRI_TAG::RegExp:
      return printRegExpRVal(pp, node);
    case IRI_GEN::IRI_TAG::UNOPDelMemberExpr:
      return printUNOPDelMemberExprRVal(pp, node);
    case IRI_GEN::IRI_TAG::UNOPDelVar:
      return printUNOPDelVarRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSTemplate:
      return printJSTemplateRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSBigInt:
      return printJSBigIntRVal(pp, node);
    case IRI_GEN::IRI_TAG::Await:
      return printAwaitRVal(pp, node);
    case IRI_GEN::IRI_TAG::Yield:
      return printYieldRVal(pp, node);
    case IRI_GEN::IRI_TAG::IDOP:
      return printIDOPRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSIDOP:
      return printJSIDOPRVal(pp, node);
    case IRI_GEN::IRI_TAG::DCTRRet:
      return printDCTRRetRVal(pp, node);
    case IRI_GEN::IRI_TAG::ToNumeric:
      return printToNumericRVal(pp, node);
    case IRI_GEN::IRI_TAG::JSCTX:
      return printJSCTXRVal(pp, node);
    case IRI_GEN::IRI_TAG::ThisINIT:
      return printThisINITRVal(pp, node);
    default:
      return ppGeneric(pp, node);
  }
}

} // namespace IRI_STRUCTURAL
