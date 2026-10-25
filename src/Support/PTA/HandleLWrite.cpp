#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <vector>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: LWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID LValTarget -> sexp.getArg_LValTarget()
 *   [1] IRID RVal -> sexp.getArg_RVal()
 * Flags:
 *   - void   INIT -> sexp.hasINIT()
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   THISINIT -> sexp.hasTHISINIT()
 */
void handleLWrite(const PTAStatementContext &ptactx) {
  Prakriti::TraceHelperAuto th("LWrite", ptactx.stmt.id);

  IRI_GEN::LWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> values;
  {
    Prakriti::TraceHelperAuto th("LWrite::RVal", sexp.getArg_RVal());
    resolvePKRRVal(ptactx, sexp.getArg_RVal(), values);
  }
  if (values.empty())
    return;

  Prakriti::NodeUID target = sexp.getArg_LValTarget();
  assert(G->hasNode(target));

  std::vector<Prakriti::NodeUID> setArgs = {target};
  setArgs.insert(setArgs.end(), values.begin(), values.end());

  {
    Prakriti::TraceHelperAuto th("LWrite::LVal=RVal", target);
    auto setClosures =
        G->getPointees(target, Prakriti::PKRGlobalState::EdgeIntern(PKR_Set));
    assert(!setClosures.empty());
    Prakriti::KarmaJoin(G, Prakriti::Karma(G, setClosures, {nullptr, setArgs}));
  }
}

} // namespace IRI_STRUCTURAL
