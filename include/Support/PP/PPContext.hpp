// Generated: 2026-10-30 03:08:55
#pragma once

#include "Storage/IRIContext.h"
#include "Storage/IridiumSEXP.h"
#include "Generated/IridiumEnums.h"
#include <span>
#include <string_view>

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
  inline std::string_view str(StringID s) const {
    return ctx.storage.strings.get(s);
  }
};

} // namespace IRI_STRUCTURAL
