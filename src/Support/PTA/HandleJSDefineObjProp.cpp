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

  for (auto obj : objs) {
    assert(G->hasNode(obj));
    auto defineClosures = G->getPointees(
        obj, Prakriti::PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty));
    assert(!defineClosures.empty());
    for (auto val : values) {
      auto desc = std::make_shared<Prakriti::TempFieldDescriptor>();
      desc->addValue(val);
      desc->addWritable(Prakriti::PKRGlobalState::getTRUE());
      desc->addEnumerable(Prakriti::PKRGlobalState::getTRUE());
      desc->addConfigurable(Prakriti::PKRGlobalState::getTRUE());
      Prakriti::KarmaBindu(G, defineClosures, {nullptr, {obj}, {field}, {desc}});
    }
  }
}

} // namespace IRI_STRUCTURAL
