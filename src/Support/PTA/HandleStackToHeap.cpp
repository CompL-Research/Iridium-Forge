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

  // We can retain more precision by parking, instead of closing transience in
  // case of loops. Hopefully this doesnt come back to bite ~Meetesh
  for (auto b : ptactx.ctx.storage.nodes.get_args(ptactx.stmt.id)) {
    assert(G->hasNode(b));
    assert(G->getNodeTAG(b) == Prakriti::TAG::TSTKOBJ);
    TransientCell::park(G, b);
  }
}

} // namespace IRI_STRUCTURAL
