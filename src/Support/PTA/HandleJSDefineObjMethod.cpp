// Generated Stub for IRI_TAG::JSDefineObjMethod
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSDefineObjMethod
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID Key -> sexp.getArg_Key()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   NOENUM -> sexp.hasNOENUM()
 *   - void   METHOD -> sexp.hasMETHOD()
 *   - void   GET -> sexp.hasGET()
 *   - void   SET -> sexp.hasSET()
 */
void handleJSDefineObjMethod(const PTAStatementContext &ptactx) {

  Prakriti::TraceHelperAuto th("JSDefineObjMethod", ptactx.stmt.id);

  IRI_GEN::JSDefineObjMethodSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::set<Prakriti::NodeUID> objs, values;
  {
    Prakriti::TraceHelperAuto th("JSDefineObjMethod::TargetObj",
                                 sexp.getArg_TargetObj());
    resolvePKRRVal(ptactx, sexp.getArg_TargetObj(), objs);
  }
  {
    Prakriti::TraceHelperAuto th("JSDefineObjMethod::Value",
                                 sexp.getArg_Value());
    resolvePKRRVal(ptactx, sexp.getArg_Value(), values);
    for (auto acID : values) {
      // Assert that the RVal is infact callable
      Prakriti::PKRGlobalState::getActionClosure(acID);
    }
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
    Prakriti::TraceHelperAuto thk("JSDefineObjMethod::Key", keyNode);
    resolvePKRRVal(ptactx, keyNode, keys);
    field = computedFieldName(keys);
  }

  {
    if (sexp.hasGET()) {
      Prakriti::TraceHelperAuto th("JSDefineObjMethod::DefineGETTER",
                                   sexp.getArg_TargetObj());
      definePropertyGetter(G, objs, field, values);
    } else if (sexp.hasSET()) {
      Prakriti::TraceHelperAuto th("JSDefineObjMethod::DefineSETTER",
                                   sexp.getArg_TargetObj());
      definePropertySetter(G, objs, field, values);
    } else {
      Prakriti::TraceHelperAuto th("JSDefineObjMethod::DefineVALUE",
                                   sexp.getArg_TargetObj());
      definePropertyValue(G, objs, field, values);
    }
  }
}

} // namespace IRI_STRUCTURAL
