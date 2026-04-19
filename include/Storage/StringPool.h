#pragma once
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>

using StringID = uint32_t;

class StringPool {
    // Each string is a unique allocation; pointers are stable.
    // std::deque<std::string> is also acceptable here.
    std::vector<std::unique_ptr<std::string>> storage;
    std::unordered_map<std::string_view, StringID> intern_map;
    std::vector<std::string_view> id_to_view;

public:
    StringID intern(std::string_view str) {
        if (auto it = intern_map.find(str); it != intern_map.end()) {
            return it->second;
        }

        // Allocate a new string and store it
        auto& kept_str = storage.emplace_back(std::make_unique<std::string>(str));
        std::string_view stable_view(*kept_str);

        StringID new_id = static_cast<StringID>(id_to_view.size());
        id_to_view.push_back(stable_view);
        intern_map[stable_view] = new_id;

        return new_id;
    }
    std::string_view get(StringID id) const { return id_to_view[id]; }
};
