#pragma once
#include <cstdint>
#include <variant>

namespace IRI_STORAGE {
using PoolOffset = uint32_t;
using UIndex = uint32_t;

using IRID = uint32_t;
using StringID = PoolOffset;

// One integer slot serves both kinds; the schema says which is which.
using FlagValue =
    std::variant<std::monostate, std::nullptr_t, double, bool, uint32_t>;

} // namespace IRI_STORAGE
