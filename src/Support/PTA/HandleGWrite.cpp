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
 * AST Tag: GWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID LValTarget -> sexp.getArg_LValTarget()
 *   [1] IRID RVal -> sexp.getArg_RVal()
 * Flags:
 *   - void   INIT -> sexp.hasINIT()
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   DECLVAR -> sexp.hasDECLVAR()
 *   - void   DECLFUN -> sexp.hasDECLFUN()
 */
void handleGWrite(const PTAStatementContext &ptactx) {
  IRI_GEN::GWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> values;
  resolvePKRRVal(ptactx, sexp.getArg_RVal(), values);
  if (values.empty())
    return;

  IRID lval = sexp.getArg_LValTarget();
  auto lvalTag = IRI_NODE(ptactx.ctx, lval).tag;

  Prakriti::NodeUID target =
      lvalTag == IRI_GEN::GlobalBinding
          ? Prakriti::PKRGlobalState::getGlobal(
                IRI_GEN::GlobalBindingSEXP(lval, ptactx.ctx).getNAME())
          : lval;
  assert(G->hasNode(target));

  std::vector<Prakriti::NodeUID> setArgs = {target};
  setArgs.insert(setArgs.end(), values.begin(), values.end());

  auto setClosures =
      G->getPointees(target, Prakriti::PKRGlobalState::EdgeIntern(PKR_Set));
  assert(!setClosures.empty());
  Prakriti::KarmaJoin(G, Prakriti::Karma(G, setClosures, {nullptr, setArgs}));
}

} // namespace IRI_STRUCTURAL
