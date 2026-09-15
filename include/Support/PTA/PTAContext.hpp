// Generated: 2026-09-15 12:01:18
#pragma once

#include "Storage/IRIContext.h"
#include "Storage/IridiumSEXP.h"
#include "Generated/IridiumEnums.h"
#include "Support/IRICFG.hpp"
#include "external/Prakriti.hpp"

namespace IRI_STRUCTURAL {

/**
 * Bundles the execution context for Points-To Analysis (PTA) transfer functions.
 * Allows easy evolution of PTA state/parameters without changing individual handler signatures.
 */
struct PTAStatementContext {
  const IRIStatement &stmt;
  Prakriti::ECMAGraph *incomingState;
  IRI_STORAGE::IRIContext &ctx;

  PTAStatementContext(const IRIStatement &s,
                      Prakriti::ECMAGraph *st,
                      IRI_STORAGE::IRIContext &c)
      : stmt(s), incomingState(st), ctx(c) {}

  // Convenient accessors
  inline IRID getId() const { return stmt.id; }
  inline IRI_GEN::IRI_TAG getTag() const { return IRI_NODE(ctx, stmt.id).tag; }
  inline IRIBB* getBB() const { return stmt.bb; }
};

using PTAContext = PTAStatementContext;

} // namespace IRI_STRUCTURAL
