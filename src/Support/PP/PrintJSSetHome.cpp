// Generated Stub for IRI_TAG::JSSetHome
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSetHome
 * Meta:    STMT
 * Arguments:
 *   [0] IRID HomeObj -> sexp.getArg_HomeObj()
 *   [1] IRID FuncObj -> sexp.getArg_FuncObj()
 * Flags: (none)
 */
std::string printJSSetHome(const PPContext &pp, IRID node) {
  IRI_GEN::JSSetHomeSEXP sexp(node, pp.ctx);
  return ppOperand(pp, sexp.getArg_FuncObj()) + ".[[HomeObject]] = " +
         ppRVal(pp, sexp.getArg_HomeObj());
}

} // namespace IRI_STRUCTURAL
