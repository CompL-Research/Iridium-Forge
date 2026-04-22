#pragma once
#include <cstdint>
#include <variant>

namespace IRI_STORAGE {
using IRID = uint32_t;
using StringID = uint32_t; // Even Links take up the same space as a stringID

using FlagValue =
    std::variant<std::monostate, std::nullptr_t, double, bool, StringID>;

} // namespace IRI_STORAGE
