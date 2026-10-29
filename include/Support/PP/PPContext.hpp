// Generated: 2026-10-30 04:34:39
#pragma once

#include "Storage/IRIContext.h"
#include "Storage/IridiumSEXP.h"
#include "Generated/IridiumEnums.h"
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>

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

  // Names with more than one binding in the function being printed
  // (shadowing); they print with their declaring scope: x#3.
  std::unordered_set<std::string> scopedNames;

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
