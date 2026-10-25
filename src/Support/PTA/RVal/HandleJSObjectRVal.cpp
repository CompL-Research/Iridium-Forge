#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSObject
 * Meta:    RVAL
 * Arguments: (none)
 * Flags: (none)
 */
void computeJSObjectVals(const PTAStatementContext &ptactx, IRID node,
                        std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("JSObjectRVal", node);

  Prakriti::ECMAGraph *G = ptactx.incomingState;

  if (!G->hasNode(node)) {
    Prakriti::TraceHelperAuto th("JSObjectRVal::Alloc", node);
    Prakriti::AllocOrdinaryObject(
        G, node, Prakriti::PKRGlobalState::getTRUE(),
        Prakriti::PKRGlobalState::getGOOBJ_Object_prototype());
  }

  res_.insert(node);
}

} // namespace IRI_STRUCTURAL
