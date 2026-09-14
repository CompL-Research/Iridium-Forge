#pragma once
#include "BindingsPool.h"
#include "IridiumPool.h"
#include "StringPool.h"
#include <iostream>
#include <utility>
#include <vector>

namespace IRI_STORAGE {

//
// The old IridiumPool was a neat idea, the only problem was I started bloating
// it with anything and everything, so this is the replacement. This Storage
// Interface deals with everything memory related in Iridium now. Basically,
// IridiumPool purely deals with node storage StringPool purely deals with
// string interning BindingsPool purely deals with bindings metadata storage (we
// will eventually maintain use-def chains here)...
//
// ~Meetesh
//

class IRIStorage {
public:
  IridiumPool nodes;
  StringPool strings;
  BindingsPool bindings;

  IRIStorage() = default;
  IRIStorage(const IRIStorage &) = delete;
  IRIStorage &operator=(const IRIStorage &) = delete;

  void dumpDiagnostics(std::ostream &os = std::cout) const {
    auto t = nodes.telemetry();
    if (t.nodes.count == 0)
      return;

    std::vector<std::pair<const char *, PoolMemStats>> pools = {
        {"Nodes", t.nodes},
        {"Args", t.args},
        {"Flags", t.flags},
        {"Strings", strings.telemetry()},
        {"Bindings", bindings.telemetry()},
    };

    size_t totalUsed = 0, totalCap = 0;
    for (auto &[label, s] : pools) {
      totalUsed += s.usedBytes;
      totalCap += s.capBytes;
    }
    double efficiency =
        totalCap > 0 ? 100.0 * static_cast<double>(totalUsed) / totalCap : 0.0;

    os << "\n--- IRIStorage Diagnostics ---\n";
    for (auto &[label, s] : pools)
      os << label << ": " << s.count << " (" << s.usedBytes / 1024
         << " KB used / " << s.capBytes / 1024 << " KB alloc)\n";
    os << "-------------------------------\n";
    os << "Total Allocation: " << totalCap / 1024 << " KB\n";
    os << "Usage Efficiency: " << efficiency << "%\n";
    if (efficiency < 70.0 && totalCap > 128 * 1024)
      os << ">> ADVICE: High vector fragmentation. Consider calling "
            "reserve() upfront.\n";
    os << "-------------------------------\n"
       << std::endl;
  }
};

} // namespace IRI_STORAGE
