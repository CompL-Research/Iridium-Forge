// Generated Stub for IRI_TAG::JSPrivateFieldWrite
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSPrivateFieldWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   DECL -> sexp.hasDECL()
 */
void handleJSPrivateFieldWrite(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSPrivateFieldWrite");
  // IRI_GEN::JSPrivateFieldWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSPrivateFieldWrite ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }
  // if (sexp.hasArg_Value()) { IRID arg_Value = sexp.getArg_Value(); }
  // bool has_DECL = sexp.hasDECL();

  return;
}

} // namespace IRI_STRUCTURAL
