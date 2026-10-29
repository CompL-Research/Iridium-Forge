// Generated Stub for IRI_TAG::JSSuperFieldWrite
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSuperFieldWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID This -> sexp.getArg_This()
 *   [1] IRID Home -> sexp.getArg_Home()
 *   [2] IRID Field -> sexp.getArg_Field()
 *   [3] IRID Value -> sexp.getArg_Value()
 * Flags: (none)
 */
void handleJSSuperFieldWrite(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSSuperFieldWrite");
  // IRI_GEN::JSSuperFieldWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSSuperFieldWrite ===
  // if (sexp.hasArg_This()) { IRID arg_This = sexp.getArg_This(); }
  // if (sexp.hasArg_Home()) { IRID arg_Home = sexp.getArg_Home(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }
  // if (sexp.hasArg_Value()) { IRID arg_Value = sexp.getArg_Value(); }

  return;
}

} // namespace IRI_STRUCTURAL
