// Generated Stub for IRI_TAG::JSComputedFieldRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>
#include <cassert>
#include <set>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSComputedFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 */
void computeJSComputedFieldReadVals(const PTAStatementContext &ptactx,
                                    IRID node,
                                    std::set<Prakriti::NodeUID> &res_) {
  IRI_GEN::JSComputedFieldReadSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs, keys;
  resolvePKRRVal(ptactx, sexp.getArg_Obj(), objs);
  assert(!objs.empty());

  // Resolved for its effects only. A literal key was already turned into a
  // FieldRead by ReduceComputedFieldOpsPass, so whatever reaches here is a key
  // nothing knows, and the read goes to the may-alias bucket.
  resolvePKRRVal(ptactx, sexp.getArg_Field(), keys);

  auto vals = getProperty(G, objs, PKR_UNKNOWN_FIELD);
  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
