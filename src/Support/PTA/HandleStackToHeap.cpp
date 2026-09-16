#include "Generated/IridiumTypes.h"
#include "Support/PTA.hpp"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "external/Prakriti.hpp"
#include <cassert>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: StackToHeap
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleStackToHeap(const PTAStatementContext &ptactx) {
  Prakriti::ECMAGraph *G = ptactx.incomingState;
  auto bindings = ptactx.ctx.storage.nodes.get_args(ptactx.stmt.id);

  for (auto b : bindings) {
    assert(G->hasNode(b));
    assert(G->getNodeTAG(b) == Prakriti::TAG::TSTKOBJ);
    closeTransience(G, b);
  }
}

} // namespace IRI_STRUCTURAL
