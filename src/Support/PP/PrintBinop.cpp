// Generated Stub for IRI_TAG::Binop
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
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
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printBinop(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
