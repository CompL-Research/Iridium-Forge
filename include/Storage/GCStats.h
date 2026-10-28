#pragma once
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>

namespace IRI_STORAGE {

// check whether environment variable set for checking stats
inline bool statsOn() {
  static const bool v = std::getenv("IRI_STATS") != nullptr;
  return v;
}

// Reads a line from /proc/self/status, e.g. for "VmRSS", "VmHWM"
inline size_t procStatusKb(const char *key) {
  std::ifstream f("/proc/self/status");
  std::string line;
  const size_t klen = std::strlen(key);

  while (std::getline(f, line)) {
    if (line.compare(0, klen, key) == 0) {
      return std::strtoul(line.c_str() + klen + 1, nullptr, 10);
    }
  }
  return 0;
}

// Resets the process peak-RSS counter (VmHWM) to the current RSS
// Silently does nothing if the kernel refuses
inline void resetPeakRss() {
  std::ofstream f("/proc/self/clear_refs");
  if (f) f << "5";
}

}