#pragma once
#include <cstdint>
#include <functional>
#include <variant>

namespace IRI_STORAGE {
using PoolOffset = uint32_t;
using UIndex = uint32_t;

using IRID = uint32_t;
inline constexpr IRID NO_IRID = UINT32_MAX;

// BBIDX -> unit32's
// Scope -> int32's (-1 is used to check scope termination by IRIS)
using BBIDX = uint32_t;
inline constexpr BBIDX NO_BB = UINT32_MAX;
using SCPIDX = int32_t;
inline constexpr SCPIDX NO_SCOPE = -1;
// CIDX -> uint32's
using CIDX = uint32_t;
inline constexpr CIDX NO_CIDX = UINT32_MAX;

// Separate Fields
struct StringID {
  uint32_t v;
  auto operator<=>(const StringID &o) const = default;
};
struct UInt32 {
  uint32_t v;
  auto operator<=>(const UInt32 &o) const = default;
};
struct Int32 {
  int32_t v;
  auto operator<=>(const Int32 &o) const = default;
};

using FlagValue =
    std::variant<std::monostate, std::nullptr_t, double, bool, StringID, UInt32,
                 Int32>;

} // namespace IRI_STORAGE

template <> struct std::hash<IRI_STORAGE::StringID> {
  size_t operator()(IRI_STORAGE::StringID s) const {
    return std::hash<uint32_t>{}(s.v);
  }
};
template <> struct std::hash<IRI_STORAGE::UInt32> {
  size_t operator()(IRI_STORAGE::UInt32 u) const {
    return std::hash<uint32_t>{}(u.v);
  }
};
template <> struct std::hash<IRI_STORAGE::Int32> {
  size_t operator()(IRI_STORAGE::Int32 i) const {
    return std::hash<int32_t>{}(i.v);
  }
};
