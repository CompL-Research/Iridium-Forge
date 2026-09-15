// Generated Stub for IRI_TAG::JSCopyDataProperties (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCopyDataProperties
 * Meta:    AMP
 * Arguments:
 *   [0] IRID ExclusionObj -> sexp.getArg_ExclusionObj()
 *   [1] IRID SourceObj -> sexp.getArg_SourceObj()
 *   [2] IRID TargetObj -> sexp.getArg_TargetObj()
 * Flags: (none)
 */
void computeJSCopyDataPropertiesVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSCopyDataProperties");
  // IRI_GEN::JSCopyDataPropertiesSEXP sexp(node, ptactx.ctx);

  // === TODO : JSCopyDataProperties ===
  // if (sexp.hasArg_ExclusionObj()) { IRID arg_ExclusionObj = sexp.getArg_ExclusionObj(); }
  // if (sexp.hasArg_SourceObj()) { IRID arg_SourceObj = sexp.getArg_SourceObj(); }
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }

  return;
}

} // namespace IRI_STRUCTURAL
