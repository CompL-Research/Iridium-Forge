// Generated Stub for IRI_TAG::FieldWrite
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: FieldWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags: (none)
 */
void handleFieldWrite(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case FieldWrite");
  // IRI_GEN::FieldWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : FieldWrite ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }
  // if (sexp.hasArg_Value()) { IRID arg_Value = sexp.getArg_Value(); }

  return;
}

} // namespace IRI_STRUCTURAL
