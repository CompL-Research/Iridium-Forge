#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <memory>
#include <set>
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
  Prakriti::TraceHelperAuto th("JSDefineObjProp", ptactx.stmt.id);

  IRI_GEN::JSDefineObjPropSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs, values;
  {
    Prakriti::TraceHelperAuto th("JSDefineObjProp::TargetObj",
                                 sexp.getArg_TargetObj());
    resolvePKRRVal(ptactx, sexp.getArg_TargetObj(), objs);
  }
  {
    Prakriti::TraceHelperAuto th("JSDefineObjProp::Value", sexp.getArg_Value());
    resolvePKRRVal(ptactx, sexp.getArg_Value(), values);
  }
  assert(!objs.empty());
  assert(!values.empty());

  IRID keyNode = sexp.getArg_Key();
  std::string field;
  if (IRI_NODE(ptactx.ctx, keyNode).tag == IRI_GEN::IRI_TAG::String) {
    field = Prakriti::PKRGlobalState::EdgeGet(
        IRI_GEN::StringSEXP(keyNode, ptactx.ctx).getIridiumPrimitive());
  } else {
    std::set<Prakriti::NodeUID> keys;
    Prakriti::TraceHelperAuto thk("JSDefineObjProp::Key", keyNode);
    resolvePKRRVal(ptactx, keyNode, keys);
    field = computedFieldName(keys);
  }

  {
    Prakriti::TraceHelperAuto th("JSDefineObjProp::Define",
                                 sexp.getArg_TargetObj());
    definePropertyValue(G, objs, field, values);
  }
}

} // namespace IRI_STRUCTURAL
