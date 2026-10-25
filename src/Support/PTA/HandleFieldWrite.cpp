#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <string>

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
  Prakriti::TraceHelperAuto th("FieldWrite", ptactx.stmt.id);

  IRI_GEN::FieldWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs, values;
  {
    Prakriti::TraceHelperAuto th("FieldWrite::Obj", sexp.getArg_Obj());
    resolvePKRRVal(ptactx, sexp.getArg_Obj(), objs);
  }
  {
    Prakriti::TraceHelperAuto th("FieldWrite::Value", sexp.getArg_Value());
    resolvePKRRVal(ptactx, sexp.getArg_Value(), values);
  }
  assert(!objs.empty());
  assert(!values.empty());

  IRI_GEN::StringSEXP fieldSexp(sexp.getArg_Field(), ptactx.ctx);
  std::string field(
      Prakriti::PKRGlobalState::EdgeGet(fieldSexp.getIridiumPrimitive()));

  {
    Prakriti::TraceHelperAuto th("FieldWrite::Set", sexp.getArg_Obj());
    setProperty(G, objs, field, values);
  }
}

} // namespace IRI_STRUCTURAL
