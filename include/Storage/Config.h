#pragma once
#include <cstdint>
#include <variant>

namespace IRI_STORAGE {
using IRID = uint32_t;
using StringID = uint32_t;

using FlagValue =
    std::variant<std::monostate, std::nullptr_t, double, bool, StringID>;

} // namespace IRI_STORAGE
