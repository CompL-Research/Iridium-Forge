// Generated: 2026-09-30 23:30:42
#pragma once

#include "Storage/IRIContext.h"
#include "Storage/IridiumSEXP.h"
#include "Generated/IridiumEnums.h"
#include <span>
#include <string_view>
#include <variant>

namespace IRI_STRUCTURAL {

using IRI_STORAGE::IRID;

/**
 * Bundles the state for pretty printing (PP).
 * Allows easy evolution of PP options without changing individual handler signatures.
 */
struct PPContext {
  IRI_STORAGE::IRIContext &ctx;

  // Append "  ; #<IRID>" to every statement line.
  bool showIds = false;

  explicit PPContext(IRI_STORAGE::IRIContext &c) : ctx(c) {}

  // Convenient accessors
  inline IRI_GEN::IRI_TAG tagOf(IRID n) const { return IRI_NODE(ctx, n).tag; }
  inline std::span<const IRID> args(IRID n) const {
    return ctx.storage.nodes.get_args_view(n);
  }
  inline const IRI_STORAGE::FlagValue &flag(IRID n, uint32_t idx) const {
    return ctx.storage.nodes.get_flags(n)[idx];
  }
  // A void flag is set when present; a bool flag only when present and true.
  inline bool on(IRID n, uint32_t idx) const {
    const auto &v = flag(n, idx);
    if (std::holds_alternative<std::monostate>(v))
      return false;
    if (const bool *b = std::get_if<bool>(&v))
      return *b;
    return true;
  }
  inline std::string_view str(StringID s) const {
    return ctx.storage.strings.get(s);
  }
};

} // namespace IRI_STRUCTURAL
