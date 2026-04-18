#pragma once
#include <functional>
#include <stdexcept>
#include <string>

class IRIPerf {
public:
  std::function<void(std::string)> tick = [&] (std::string) { throw std::runtime_error("TICK STUB IMPLEMENTATION"); };
  std::function<void(std::string)> tock = [&] (std::string) { throw std::runtime_error("TOCK STUB IMPLEMENTATION"); };
};
