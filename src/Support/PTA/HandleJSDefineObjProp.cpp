#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSDefineObjProp
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID Key -> sexp.getArg_Key()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags: (none)
 */
void handleJSDefineObjProp(const PTAStatementContext &ptactx) {
  IRI_GEN::JSDefineObjPropSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  IRID keyNode = sexp.getArg_Key();
  if (IRI_NODE(ptactx.ctx, keyNode).tag != IRI_GEN::IRI_TAG::String)
    throw std::runtime_error(
        "PTA unhandled case JSDefineObjProp: computed key");

  std::set<Prakriti::NodeUID> objs, values;
  resolvePKRRVal(ptactx, sexp.getArg_TargetObj(), objs);
  resolvePKRRVal(ptactx, sexp.getArg_Value(), values);
  assert(!objs.empty());
  assert(!values.empty());

  IRI_GEN::StringSEXP keySexp(keyNode, ptactx.ctx);
  std::string field(
      Prakriti::PKRGlobalState::EdgeGet(keySexp.getIridiumPrimitive()));

  //
  // Define :: A.F = B (node this is different from field write!)
  // A = {o1, o2...}
  // B = {ox, oy...}
  // o1[[DefineOwnProperty]](f) = FieldDescriptor[[Value]] -> ox U
  // o1[[DefineOwnProperty]](f) = FieldDescriptor[[Value]] -> oy U
  // o2[[DefineOwnProperty]](f) = FieldDescriptor[[Value]] -> ox U
  // o2[[DefineOwnProperty]](f) = FieldDescriptor[[Value]] -> oy U
  //

  std::vector<Prakriti::ECMAGraph> finalRes;
  for (auto A_a : objs) {
    for (auto B_b : values) {
      assert(G->hasNode(A_a));
      // [[DefineOwnProperty]] belongs to the target, not the value.
      auto targetClosures = G->getPointees(
          A_a, Prakriti::PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty));

      if (targetClosures.empty())
        continue; // Same reasoning as FieldWrite...

      auto desc = std::make_shared<Prakriti::TempFieldDescriptor>();
      desc->addValue(B_b);
      desc->addWritable(Prakriti::PKRGlobalState::getTRUE());
      desc->addEnumerable(Prakriti::PKRGlobalState::getTRUE());
      desc->addConfigurable(Prakriti::PKRGlobalState::getTRUE());
      for (auto kr : Prakriti::Karma(
               G, targetClosures,
               {nullptr, {A_a}, {field}, {desc}})) {
        finalRes.push_back(kr.clonedG);
      }
    }
  }

  G->mutateMergeUnion(finalRes);
}

} // namespace IRI_STRUCTURAL
