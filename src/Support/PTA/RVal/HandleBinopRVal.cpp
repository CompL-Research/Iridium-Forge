// Generated Stub for IRI_TAG::Binop (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Binop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID LBinop -> sexp.getArg_LBinop()
 *   [1] IRID RBinop -> sexp.getArg_RBinop()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
void computeBinopVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case Binop");
  // IRI_GEN::BinopSEXP sexp(node, ptactx.ctx);

  // === TODO : Binop ===
  // if (sexp.hasArg_LBinop()) { IRID arg_LBinop = sexp.getArg_LBinop(); }
  // if (sexp.hasArg_RBinop()) { IRID arg_RBinop = sexp.getArg_RBinop(); }
  // if (sexp.hasOP()) { StringID str_OP = sexp.getOP(); }

  return;
}

} // namespace IRI_STRUCTURAL
