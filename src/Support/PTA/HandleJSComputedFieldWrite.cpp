#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSComputedFieldWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 */
void handleJSComputedFieldWrite(const PTAStatementContext &ptactx) {
  Prakriti::TraceHelperAuto th("JSComputedFieldWrite", ptactx.stmt.id);

  IRI_GEN::JSComputedFieldWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs, keys, vals;
  {
    Prakriti::TraceHelperAuto th("JSComputedFieldWrite::Obj",
                                 sexp.getArg_Obj());
    resolvePKRRVal(ptactx, sexp.getArg_Obj(), objs);
  }
  {
    Prakriti::TraceHelperAuto th("JSComputedFieldWrite::Field",
                                 sexp.getArg_Field());
    resolvePKRRVal(ptactx, sexp.getArg_Field(), keys);
  }
  {
    Prakriti::TraceHelperAuto th("JSComputedFieldWrite::Value",
                                 sexp.getArg_Value());
    resolvePKRRVal(ptactx, sexp.getArg_Value(), vals);
  }
  assert(!objs.empty());
  if (vals.empty())
    return;

  {
    Prakriti::TraceHelperAuto th("JSComputedFieldWrite::Set",
                                 sexp.getArg_Obj());
    setProperty(G, objs, computedFieldName(keys), vals);
  }
}

} // namespace IRI_STRUCTURAL
