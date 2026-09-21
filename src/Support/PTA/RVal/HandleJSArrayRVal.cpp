// Generated Stub for IRI_TAG::JSArray (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <set>
#include <cassert>
#include <set>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSArray
 * Meta:    RVAL
 * Arguments: (none)
 * Flags: (none)
 */
void computeJSArrayVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("JSArrayRVal", node);

  Prakriti::ECMAGraph *G = ptactx.incomingState;

  if (!G->hasNode(node)) {
    Prakriti::TraceHelperAuto th("JSArrayRVal::Alloc", node);
    Prakriti::AllocArrayObject(G, node);
  }

  // Elements are this node's own variadic args, redefined on every visit so a
  // loop-varying element is not fixed by the first one.
  auto elems = ptactx.ctx.storage.nodes.get_args(node);
  {
    Prakriti::TraceHelperAuto th("JSArrayRVal::Elements", node);
    for (size_t i = 0; i < elems.size(); i++) {
      std::set<Prakriti::NodeUID> vals;
      resolvePKRRVal(ptactx, elems[i], vals);
      if (vals.empty())
        continue;
      defineProperty(G, {node}, std::to_string(i), vals);
    }
  }

  res_.insert(node);
}

} // namespace IRI_STRUCTURAL
