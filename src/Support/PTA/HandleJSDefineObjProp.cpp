#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
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

  defineProperty(G, objs, field, values);
}

} // namespace IRI_STRUCTURAL
