// Generated: 2026-09-03 11:34:38
#pragma once

#include "Storage/IridiumPool.h"
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
  IRI_STORAGE::IridiumPool &pool;

  PTAStatementContext(const IRIStatement &s,
                      Prakriti::ECMAGraph *st,
                      IRI_STORAGE::IridiumPool &p)
      : stmt(s), incomingState(st), pool(p) {}

  // Convenient accessors
  inline IRID getId() const { return stmt.id; }
  inline IRI_GEN::IRI_TAG getTag() const { return pool[stmt.id].tag; }
  inline IRIBB* getBB() const { return stmt.bb; }
};

using PTAContext = PTAStatementContext;

} // namespace IRI_STRUCTURAL
