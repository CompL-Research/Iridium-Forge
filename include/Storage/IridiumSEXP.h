#pragma once
#include "Generated/IridiumEnums.h"
#include <cstdint>
#include <sstream>
#include <string_view>

namespace IRI_STORAGE {

extern void append_escaped_json_string(std::ostringstream &oss,
                                       std::string_view s);
class IridiumPool;

class IridiumSEXP {
public:
  // Tag
  IRI_GEN::IRI_TAG tag;

  // Args
  uint32_t num_args = 0;
  uint32_t args_start_index = 0;

  // Flags
  uint32_t num_flag_slots = 0;
  uint32_t flags_start_index = 0; // Replaces the pointer

  void dump(std::ostream &oss, IridiumPool *pool, bool compressed = false,
            int indent = 0) const;

  void dumpFlat(std::ostream &oss, IridiumPool *pool,
                int depth = 0, bool full = true) const;
};
} // namespace IRI_STORAGE
