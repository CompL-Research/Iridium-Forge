#pragma once
#include "external/Prakriti.hpp"
#include "external/pta_trace.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace IRI_STORAGE {

class IRIDebug {
public:
  // Tick Tock -> Hooks into IRI frontend
  //
  // This was moved from the deprecated IRIPerf.h... Basically just hooks into
  // hooks provided from the frontend ~Meetesh
  //
  std::function<void(std::string)> tick = [&](std::string) {
    throw std::runtime_error("TICK STUB IMPLEMENTATION");
  };
  std::function<void(std::string)> tock = [&](std::string) {
    throw std::runtime_error("TOCK STUB IMPLEMENTATION");
  };

  // Prakriti TraceWriter
  std::shared_ptr<ptf::TraceWriter> traceWriter = nullptr;
  std::function<std::unordered_map<std::string, std::string>(Prakriti::NodeUID)>
      traceNodeMetaMapper = nullptr;
};

} // namespace IRI_STORAGE
