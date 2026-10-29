// Generated Stub for IRI_TAG::Unop (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <cctype>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Unop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Val -> sexp.getArg_Val()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
std::string printUnopRVal(const PPContext &pp, IRID node) {
  IRI_GEN::UnopSEXP sexp(node, pp.ctx);
  std::string op(pp.str(sexp.getOP()));
  // ++/-- here only compute the value (OP_inc / OP_dec, BigInt-aware); the
  // write back is a separate statement: x .= inc(x)
  if (op == "++" || op == "--")
    return std::string(op == "++" ? "inc(" : "dec(") +
           ppRVal(pp, sexp.getArg_Val()) + ")";
  bool word = !op.empty() && std::isalpha(static_cast<unsigned char>(op[0]));
  return op + (word ? " " : "") + ppOperand(pp, sexp.getArg_Val());
}

} // namespace IRI_STRUCTURAL
