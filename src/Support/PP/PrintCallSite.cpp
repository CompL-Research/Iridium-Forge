// Generated Stub for IRI_TAG::CallSite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CallSite
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - void   CCall -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_CCall)
 *   - void   ConstructorCall -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_ConstructorCall)
 *   - void   PrivateCall -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_PrivateCall)
 *   - void   Import -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_Import)
 *   - void   Super -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_Super)
 *   - void   V8Intrinsic -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_V8Intrinsic)
 *   - void   TAILCALL -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_TAILCALL)
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printCallSite(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
