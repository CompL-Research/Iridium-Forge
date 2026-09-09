// Generated Stub for IRI_TAG::JSCopyDataProperties
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

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
void handleJSCopyDataProperties(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSCopyDataProperties");
  // IRI_GEN::JSCopyDataPropertiesSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSCopyDataProperties ===
  // if (sexp.hasArg_ExclusionObj()) { IRID arg_ExclusionObj = sexp.getArg_ExclusionObj(); }
  // if (sexp.hasArg_SourceObj()) { IRID arg_SourceObj = sexp.getArg_SourceObj(); }
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }

  return ;
}

} // namespace IRI_STRUCTURAL
