#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <stdexcept>
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
  Prakriti::TraceHelperAuto th("FieldReadRVal", node);

  IRI_GEN::FieldReadSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs;
  {
    Prakriti::TraceHelperAuto th("FieldReadRVal::Obj", sexp.getArg_Obj());
    resolvePKRRVal(ptactx, sexp.getArg_Obj(), objs);
  }
  assert(!objs.empty());

  IRI_GEN::StringSEXP fieldSexp(sexp.getArg_Field(), ptactx.ctx);
  std::string field(
      Prakriti::PKRGlobalState::EdgeGet(fieldSexp.getIridiumPrimitive()));

  // Each receiver is an alternative, so each reads from the same incoming
  // state; threading G through the loop would let one observe another.
  std::vector<Prakriti::NodeUID> vals;
  std::vector<Prakriti::ECMAGraph> branches;
  {
    Prakriti::TraceHelperAuto th("FieldReadRVal::Get", sexp.getArg_Obj());
    for (auto obj : objs) {
      assert(G->hasNode(obj));
      auto getClosures =
          G->getPointees(obj, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
      if (getClosures.empty())
        throw std::runtime_error(
            "PTA: property read on a primitive receiver is not modelled");
      Prakriti::Karma(G, getClosures, {nullptr, {obj, obj}, {field}}, vals,
                      branches);
    }
    Prakriti::KarmaJoin(G, branches);
  }
  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
