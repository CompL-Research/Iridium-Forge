#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: FieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
void computeFieldReadVals(const PTAStatementContext &ptactx, IRID node,
                        std::set<Prakriti::NodeUID> &res_) {
  IRI_GEN::FieldReadSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs;
  resolvePKRRVal(ptactx, sexp.getArg_Obj(), objs);
  assert(!objs.empty());

  IRI_GEN::StringSEXP fieldSexp(sexp.getArg_Field(), ptactx.ctx);
  std::string field(
      Prakriti::PKRGlobalState::EdgeGet(fieldSexp.getIridiumPrimitive()));

  for (auto obj : objs) {
    assert(G->hasNode(obj));
    auto getClosures =
        G->getPointees(obj, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
    assert(!getClosures.empty());
    auto vals = Prakriti::KarmaBindu(G, getClosures, {nullptr, {obj, obj}, {field}});
    res_.insert(vals.begin(), vals.end());
  }
}

} // namespace IRI_STRUCTURAL
