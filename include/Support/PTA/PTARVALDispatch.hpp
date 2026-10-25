// Generated: 2026-09-15 12:01:18
#pragma once

#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <iostream>
#include <set>

namespace IRI_STRUCTURAL {

/**
 * Resolves the set of nodes that IRID node evaluates to. node need not be
 * ptactx.stmt.id -- this is also used to resolve operands nested inside the
 * current statement.
 */
inline void resolvePKRRVal(const PTAStatementContext &ptactx, IRID node,
                           std::set<Prakriti::NodeUID> &res_) {
  auto currTAG = IRI_NODE(ptactx.ctx, node).tag;

  switch (currTAG) {
    case IRI_GEN::IRI_TAG::EnvRead:
      return computeEnvReadVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::String:
      return computeStringVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::FieldRead:
      return computeFieldReadVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::CallSite:
      return computeCallSiteVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Apply:
      return computeApplyVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Lambda:
      return computeLambdaVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::EnvBinding:
      return computeEnvBindingVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::RemoteEnvBinding:
      return computeRemoteEnvBindingVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::GlobalBinding:
      return computeGlobalBindingVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::ScriptBinding:
      return computeScriptBindingVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSNUBD:
      return computeJSNUBDVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Number:
      return computeNumberVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSClass:
      return computeJSClassVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::PVTEnvRead:
      return computePVTEnvReadVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSPrivate:
      return computeJSPrivateVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSPrivateFieldRead:
      return computeJSPrivateFieldReadVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::PoolBinding:
      return computePoolBindingVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSForInStart:
      return computeJSForInStartVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSForInNext:
      return computeJSForInNextVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSForOfNext:
      return computeJSForOfNextVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSCatchContext:
      return computeJSCatchContextVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSBinop:
      return computeJSBinopVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSUnop:
      return computeJSUnopVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Unop:
      return computeUnopVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSObject:
      return computeJSObjectVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Boolean:
      return computeBooleanVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSArray:
      return computeJSArrayVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Binop:
      return computeBinopVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Null:
      return computeNullVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSComputedFieldRead:
      return computeJSComputedFieldReadVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSSuperFieldRead:
      return computeJSSuperFieldReadVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSToObject:
      return computeJSToObjectVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSAppend:
      return computeJSAppendVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSCopyDataProperties:
      return computeJSCopyDataPropertiesVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::RegExp:
      return computeRegExpVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::UNOPDelMemberExpr:
      return computeUNOPDelMemberExprVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::UNOPDelVar:
      return computeUNOPDelVarVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSTemplate:
      return computeJSTemplateVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSBigInt:
      return computeJSBigIntVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Await:
      return computeAwaitVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::Yield:
      return computeYieldVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::IDOP:
      return computeIDOPVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSIDOP:
      return computeJSIDOPVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::DCTRRet:
      return computeDCTRRetVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::ToNumeric:
      return computeToNumericVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::JSCTX:
      return computeJSCTXVals(ptactx, node, res_);
    case IRI_GEN::IRI_TAG::ThisINIT:
      return computeThisINITVals(ptactx, node, res_);
    default:
      std::cerr << "[PTA Warning] Unhandled RVal tag: "
                << IRI_GEN::dump_tag(currTAG) << std::endl;
      return;
  }
}

} // namespace IRI_STRUCTURAL
