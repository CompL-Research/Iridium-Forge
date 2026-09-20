#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
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
  IRI_GEN::JSComputedFieldWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs, keys, vals;
  resolvePKRRVal(ptactx, sexp.getArg_Obj(), objs);
  resolvePKRRVal(ptactx, sexp.getArg_Field(), keys);
  resolvePKRRVal(ptactx, sexp.getArg_Value(), vals);
  assert(!objs.empty());
  if (vals.empty())
    return;

  setProperty(G, objs, PKR_UNKNOWN_FIELD, vals);
}

} // namespace IRI_STRUCTURAL
