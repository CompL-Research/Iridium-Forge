#pragma once

#include "Storage/IridiumPool.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/IRICFG.hpp"
#include "external/Prakriti.hpp"
#include <unordered_map>

namespace IRI_STRUCTURAL {

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
  static void solve(IridiumPool &);
};

} // namespace IRI_STRUCTURAL
