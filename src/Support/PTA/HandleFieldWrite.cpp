#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
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
  IRI_GEN::FieldWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs, values;
  resolvePKRRVal(ptactx, sexp.getArg_Obj(), objs);
  resolvePKRRVal(ptactx, sexp.getArg_Value(), values);
  assert(!objs.empty());
  assert(!values.empty());

  IRI_GEN::StringSEXP fieldSexp(sexp.getArg_Field(), ptactx.ctx);
  std::string field(
      Prakriti::PKRGlobalState::EdgeGet(fieldSexp.getIridiumPrimitive()));

  //
  // Here we want to do
  // A.F = B
  // A = {o1, o2...}
  // B = {ox, oy...}
  // o1[[Set]](f) = ox U
  // o1[[Set]](f) = oy U
  // o2[[Set]](f) = ox U
  // o2[[Set]](f) = oy U
  //
  std::vector<Prakriti::ECMAGraph> finalRes;
  for (auto A_a : objs) {
    for (auto B_b : values) {
      assert(G->hasNode(A_a));
      // [[Set]] belongs to the target, not the value being written.
      auto setClosures =
          G->getPointees(A_a, Prakriti::PKRGlobalState::EdgeIntern(PKR_Set));

      if (setClosures.empty())
        continue; // A_a can be a primitive, which has no [[Set]]

      for (auto kr :
           Prakriti::Karma(G, setClosures, {nullptr, {A_a, B_b}, {field}})) {
        finalRes.push_back(kr.clonedG);
      }
    }
  }

  // Each receiver's write lands in its own clone; their union is the new
  // state. Merging into G instead would keep the pre-write values alive.
  Prakriti::KarmaJoin(G, finalRes);
}

} // namespace IRI_STRUCTURAL
