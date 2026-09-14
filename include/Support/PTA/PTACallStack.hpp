#pragma once

#include "Support/IRICFG.hpp"
#include "external/Prakriti.hpp"
#include <algorithm>
#include <set>
#include <unordered_map>
#include <vector>

namespace IRI_STRUCTURAL {

//
// We are using value contexts here, this should hopefully give us termination.
// Instead of relying on comparison, we are comparing the hash, maybe thats
// enough. idk, probably this is redundant, we are already using immer, we dont
// need hash to compare for equality right, it should just work...
// TODO, figure this out later
//
// ~Meetesh
//
//

struct PTACallFrame {
  std::set<Prakriti::NodeUID> thisVals;
  Prakriti::ECMAGraph::StateHash stateHash;
};

class PTACallStack {
public:
  inline static std::unordered_map<IRICFG *, std::vector<PTACallFrame>> frames;

  static void push(IRICFG *cfg, PTACallFrame frame) {
    frames[cfg].push_back(std::move(frame));
  }

  static void pop(IRICFG *cfg) { frames[cfg].pop_back(); }

  static bool isActive(IRICFG *cfg, Prakriti::ECMAGraph::StateHash stateHash) {
    auto it = frames.find(cfg);
    if (it == frames.end())
      return false;
    return std::ranges::any_of(it->second, [&](const PTACallFrame &frame) {
      return frame.stateHash == stateHash;
    });
  }

  static const PTACallFrame *current(IRICFG *cfg) {
    auto it = frames.find(cfg);
    if (it == frames.end() || it->second.empty())
      return nullptr;
    return &it->second.back();
  }
};

} // namespace IRI_STRUCTURAL
