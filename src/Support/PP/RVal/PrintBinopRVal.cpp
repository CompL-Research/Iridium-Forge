// Generated Stub for IRI_TAG::Binop (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

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
std::string printBinopRVal(const PPContext &pp, IRID node) {
  IRI_GEN::BinopSEXP sexp(node, pp.ctx);
  return ppOperand(pp, sexp.getArg_LBinop()) + " " +
         std::string(pp.str(sexp.getOP())) + " " +
         ppOperand(pp, sexp.getArg_RBinop());
}

} // namespace IRI_STRUCTURAL
