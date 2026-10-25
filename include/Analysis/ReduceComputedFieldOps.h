#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/IRICFG.hpp"
#include <cmath>
#include <string>

namespace IRI_STRUCTURAL {

// ============================================================================
// Reduce computed field operations into static ones
// ============================================================================
struct ReduceComputedFieldOpsPass {
  static bool literalKey(IRI_STORAGE::IRIContext &ctx, IRID field,
                         std::string &out) {
    switch (IRI_NODE(ctx, field).tag) {
    case IRI_GEN::String:
      out = std::string(ctx.storage.strings.get(
          IRI_GEN::StringSEXP(field, ctx).getIridiumPrimitive()));
      return true;
    case IRI_GEN::Boolean:
      out = IRI_GEN::BooleanSEXP(field, ctx).getIridiumPrimitive() ? "true"
                                                                   : "false";
      return true;
    case IRI_GEN::Null:
      out = "null";
      return true;
    case IRI_GEN::Number: {
      // This was broken in the old implementation...
      double v = IRI_GEN::NumberSEXP(field, ctx).getIridiumPrimitive();
      if (std::trunc(v) != v || !std::isfinite(v) ||
          std::abs(v) > 9007199254740992.0)
        return false;
      out = std::to_string(static_cast<long long>(v));
      return true;
    }
    default:
      return false;
    }
  }

  // Returns the replacement for node, or node itself when nothing reduced.
  // Sets `changed` for any rewrite, including ones nested under `node`.
  static IRID reduce(IRI_STORAGE::IRIContext &ctx, IRID node, bool &changed) {
    auto args = ctx.storage.nodes.get_args(node);
    bool dirty = false;
    for (size_t i = 0; i < args.size(); i++) {
      IRID next = reduce(ctx, args[i], changed);
      if (next != args[i]) {
        args[i] = next;
        dirty = true;
      }
    }
    if (dirty) {
      ctx.storage.nodes.set_args(node, args);
      changed = true;
    }

    std::string k;
    auto tag = IRI_NODE(ctx, node).tag;
    if (tag == IRI_GEN::JSComputedFieldRead) {
      IRI_GEN::JSComputedFieldReadSEXP sexp(node, ctx);
      if (literalKey(ctx, sexp.getArg_Field(), k))
        return IRI_GEN::FieldReadSEXP::create(
            ctx, sexp.getArg_Obj(),
            IRI_GEN::StringSEXP::create(ctx, ctx.storage.strings.intern(k)));
    } else if (tag == IRI_GEN::JSDefineObjProp) {
      // Not a computed op, but the same question about its key.
      IRI_GEN::JSDefineObjPropSEXP sexp(node, ctx);
      IRID key = sexp.getArg_Key();
      if (IRI_NODE(ctx, key).tag != IRI_GEN::String && literalKey(ctx, key, k)) {
        ctx.storage.nodes.update_arg_inplace(
            node, 1,
            IRI_GEN::StringSEXP::create(ctx, ctx.storage.strings.intern(k)));
        changed = true;
      }
    } else if (tag == IRI_GEN::JSComputedFieldWrite) {
      IRI_GEN::JSComputedFieldWriteSEXP sexp(node, ctx);
      if (literalKey(ctx, sexp.getArg_Field(), k))
        return IRI_GEN::FieldWriteSEXP::create(
            ctx, sexp.getArg_Obj(),
            IRI_GEN::StringSEXP::create(ctx, ctx.storage.strings.intern(k)),
            sexp.getArg_Value());
    }
    return node;
  }

  bool run(IRICFG &cfg, AnalysisManager &) {
    auto &ctx = cfg.ctx;
    bool changed = false;

    for (const auto &[idx, bb] : cfg.nodeMap) {
      for (IRIStatement *s = bb->head; s != nullptr; s = s->next) {
        IRID next = reduce(ctx, s->id, changed);
        if (next != s->id) {
          s->id = next;
          changed = true;
        }
      }
      if (bb->tail) {
        IRID next = reduce(ctx, bb->tail->id, changed);
        if (next != bb->tail->id) {
          bb->tail->id = next;
          changed = true;
        }
      }
    }

    if (changed)
      cfg.markDirty();
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
