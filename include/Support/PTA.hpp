#pragma once

#include "Storage/IRIContext.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/IRICFG.hpp"
#include "external/Prakriti.hpp"
#include <set>
#include <unordered_map>
#include <vector>

namespace IRI_STRUCTURAL {

namespace TransientCell {

inline Prakriti::EdgeUID label(const char *l) {
  return Prakriti::PKRGlobalState::EdgeIntern(l);
}

inline bool isExecuting(Prakriti::ECMAGraph *G, Prakriti::NodeUID b) {
  return !G->getPointees(b, label(PKR_IS_EXECUTING)).empty();
}

inline void setExecuting(Prakriti::ECMAGraph *G, Prakriti::NodeUID b, bool on) {
  G->removeAllOutgoingEdgesByLabel(b, label(PKR_IS_EXECUTING));
  if (on)
    G->addEdge(b, Prakriti::PKRGlobalState::getTRUE(), label(PKR_IS_EXECUTING));
}

inline void setStrong(Prakriti::ECMAGraph *G, Prakriti::NodeUID b,
                      bool strong) {
  G->removeAllOutgoingEdgesByLabel(b, label(PKR_TRANSIENCE));
  G->addEdge(b,
             strong ? Prakriti::PKRGlobalState::getTRUE()
                    : Prakriti::PKRGlobalState::getFALSE(),
             label(PKR_TRANSIENCE));
}

// Copies rather than moves: a read before this activation's first write would
// otherwise see an empty set and trip assertions downstream.
inline void park(Prakriti::ECMAGraph *G, Prakriti::NodeUID b) {
  for (const auto v : G->getPointees(b, label(PKR_STK)))
    G->addEdge(b, v, label(PKR_TRANSIENCE_BACKUP));
}

inline void unpark(Prakriti::ECMAGraph *G, Prakriti::NodeUID b) {
  for (const auto v : G->getPointees(b, label(PKR_TRANSIENCE_BACKUP)))
    G->addEdge(b, v, label(PKR_STK));
}

// Ends the window early, without ending the frame that owns it: the parked
// values come back first, because whatever prompted this can observe them.
inline void close(Prakriti::ECMAGraph *G, Prakriti::NodeUID b) {
  unpark(G, b);
  setStrong(G, b, false);
}

} // namespace TransientCell

class PTATransfer : public TransferFunction<Prakriti::ECMAGraph> {
public:
  Prakriti::ECMAGraph
  transferStatement(const IRIStatement &stmt,
                    const Prakriti::ECMAGraph &incomingState) override;
  Prakriti::ECMAGraph
  transferEdge(BBIDX from, BBIDX to,
               const Prakriti::ECMAGraph &exitState) override;
};

class PTASolver {
private:
  // Maintain a union of all the results here for each processed statement...
  //
  // IRIStatement -> ECMAGraph
  //
  inline static std::unordered_map<IRIStatement *, Prakriti::ECMAGraph> finRes;

  // To prevent cycles we need to ensure repeated contexts are never rescheduled
  //
  // Closure -> { ( context, result? ) }
  //
  inline static std::unordered_map<
      Prakriti::NodeUID,
      std::vector<std::tuple<Prakriti::ECMAGraph::StateHash,
                             std::optional<Prakriti::ECMAGraph>>>>
      progressTracker;

public:
  static void solve(IRIContext &);

  // Fixed point computation for closures
  static std::set<Prakriti::NodeUID>
  invokeClosure(IRICFG *calleeCFG, Prakriti::NodeUID closureID,
                Prakriti::ECMAGraph *G,
                const std::set<Prakriti::NodeUID> &thisVal,
                const std::vector<std::set<Prakriti::NodeUID>> &actualArgs);
};

} // namespace IRI_STRUCTURAL
