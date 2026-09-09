// Generated Stub for IRI_TAG::LWrite
#include "Generated/IridiumTypes.h"
#include "Storage/IridiumPool.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include "external/Prakriti.hpp"
#include <iostream>
#include <stdexcept>

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
void handleLWrite(const PTAStatementContext &ctx) {
  Prakriti::ECMAGraph *G = ctx.incomingState;
  IRI_GEN::LWriteSEXP sexp(ctx.stmt.id, ctx.pool);
  std::cout << "CASE LWrite" << std::endl;
  ctx.pool[ctx.stmt.id].dumpFlat(std::cout, &ctx.pool);
  std::cout << std::endl;
  IRID lval = sexp.getArg_LValTarget();
  IRID rval = sexp.getArg_RVal();
  bool has_INIT = sexp.hasINIT();
  bool has_SAFE = sexp.hasSAFE();
  bool has_THISINIT = sexp.hasTHISINIT();

  if (!G->hasNode(lval)) {
    throw std::runtime_error("Expected lval");
  }

  std::vector<Prakriti::NodeUID> pjsslArgs;
  pjsslArgs.push_back(lval);

  std::set<Prakriti::NodeUID> rvals;
  handlePKRRVals(ctx, rval, rvals);

  if (rvals.size() == 0)
    throw std::runtime_error("Rvals size is zero... not an invairant, WIP");

  for (auto &r : rvals) {
    pjsslArgs.push_back(r);
  }

  // Karma [[Set]]
  KarmaBindu(
      G, G->getPointees(lval, Prakriti::PKRGlobalState::EdgeIntern(PKR_Set)),
      {NULL, pjsslArgs});

  return;
}

} // namespace IRI_STRUCTURAL
