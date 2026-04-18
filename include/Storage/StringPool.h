#pragma once
#include <cstdint>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <vector>

using StringID = uint32_t;

class StringPool {
  // 1. The actual character data (Deque guarantees addresses don't change on
  // growth)
  std::deque<char> char_buffer;

  // 2. Fast lookup to deduplicate strings: O(1) hash check
  std::unordered_map<std::string_view, StringID> intern_map;

  // 3. Fast reverse lookup: O(1) ID to String
  std::vector<std::string_view> id_to_view;

public:
  /**
   * @brief Hashes the string. If it exists, returns the existing ID.
   * If not, copies it to the pool and creates a new ID.
   */
  StringID intern(std::string_view str) {
    // Did we already hash and store this string?
    auto it = intern_map.find(str);
    if (it != intern_map.end()) {
      return it->second; // Return existing ID
    }

    // --- Not found. We must allocate it. ---

    // Save where we are starting in the deque
    size_t start_idx = char_buffer.size();

    // Push characters into our stable buffer
    for (char c : str) {
      char_buffer.push_back(c);
    }

    // Create a stable view pointing to the newly added characters
    std::string_view stable_view(&char_buffer[start_idx], str.size());

    // Assign an ID
    StringID new_id = static_cast<StringID>(id_to_view.size());

    // Save the reverse lookup
    id_to_view.push_back(stable_view);

    // Save the hash lookup
    intern_map[stable_view] = new_id;

    return new_id;
  }

  /**
   * @brief Retrieves the string view for a given ID.
   */
  std::string_view get(StringID id) const { return id_to_view[id]; }
};
