#pragma once

#include "external/Prakriti.hpp"
#include <set>
#include <string>
#include <vector>

namespace IRI_STRUCTURAL {

//
// As PJSSL.L takes a flat list, positional arguments are flattened and their
// positional informatio is encoded as foo({a, b}, {c}, {d,e,f}) -> {a, b, c, d,
// e, f} + "0-1,2-2,3-5" ~ Meetesh
//
inline std::string
encodeArgRanges(const std::vector<std::set<Prakriti::NodeUID>> &positional) {
  std::string out;
  size_t start = 0;
  for (size_t i = 0; i < positional.size(); i++) {
    size_t count = positional[i].size();
    size_t end = start + (count == 0 ? 0 : count - 1);
    if (i)
      out += ",";
    out += std::to_string(start) + "-" + std::to_string(end);
    start += count;
  }
  return out;
}

inline std::vector<std::set<Prakriti::NodeUID>>
decodeArgRanges(const std::vector<Prakriti::NodeUID> &flatArgs,
                const std::string &encoded) {
  std::vector<std::set<Prakriti::NodeUID>> positional;
  size_t pos = 0;
  while (pos < encoded.size()) {
    size_t comma = encoded.find(',', pos);
    std::string token = encoded.substr(pos, comma - pos);
    size_t dash = token.find('-');
    size_t start = std::stoul(token.substr(0, dash));
    size_t end = std::stoul(token.substr(dash + 1));

    std::set<Prakriti::NodeUID> slice(flatArgs.begin() + start,
                                      flatArgs.begin() + end + 1);
    positional.push_back(slice);

    if (comma == std::string::npos)
      break;
    pos = comma + 1;
  }
  return positional;
}

} // namespace IRI_STRUCTURAL
