#pragma once
#include "Config.h"
#include <cmath>
#include <vector>

namespace IRI_STORAGE {

struct FlagValueHash {
  size_t operator()(FlagValue const &v) const {
    size_t seed = v.index(); // type tag (stable)

    size_t value_hash = std::visit(
        [](auto const &x) -> size_t {
          using T = std::decay_t<decltype(x)>;

          if constexpr (std::is_same_v<T, std::monostate>) {
            return 0x9e3779b97f4a7c15ULL;

          } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return 0x85ebca6b;

          } else if constexpr (std::is_same_v<T, double>) {
            // Bitwise-stable hashing (important!)
            uint64_t bits = std::bit_cast<uint64_t>(x);

            // Normalize -0.0 → +0.0
            if (bits == 0x8000000000000000ULL)
              bits = 0;

            return std::hash<uint64_t>{}(bits);

          } else {
            return std::hash<T>{}(x);
          }
        },
        v);

    // canonical hash_combine
    seed ^= value_hash + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    return seed;
  }
};

struct FlagVectorHash {
  size_t operator()(std::vector<FlagValue> const &vec) const {
    size_t seed = vec.size(); // include size for stability

    FlagValueHash hasher;

    for (auto const &v : vec) {
      size_t h = hasher(v);

      seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    }

    return seed;
  }
};

struct FlagVectorEq {
  bool operator()(std::vector<FlagValue> const &a,
                  std::vector<FlagValue> const &b) const {
    if (a.size() != b.size())
      return false;

    for (size_t i = 0; i < a.size(); ++i) {
      if (a[i].index() != b[i].index())
        return false;

      bool matched = std::visit(
          [&](auto const &valA) {
            using T = std::decay_t<decltype(valA)>;
            auto const &valB = std::get<T>(b[i]); // We know indices match

            if constexpr (std::is_same_v<T, double>) {
              // Treat two NaNs as identical for map lookup
              if (std::isnan(valA) && std::isnan(valB))
                return true;
              return valA == valB;
            } else {
              return valA == valB;
            }
          },
          a[i]);

      if (!matched)
        return false;
    }
    return true;
  }
};
} // namespace IRI_STORAGE
