#pragma once
#include <cstddef>

namespace IRI_STORAGE {

struct PoolMemStats {
  size_t count = 0;
  size_t usedBytes = 0;
  size_t capBytes = 0;
};

} // namespace IRI_STORAGE
