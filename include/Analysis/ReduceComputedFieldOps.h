#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/IRICFG.hpp"
#include <cmath>
#include <string>

namespace IRI_STRUCTURAL {

// ============================================================================
// Reduce Computed Field Ops Transformation Pass
// ============================================================================
struct ReduceComputedFieldOpsPass {
  static IRID keyOf(IRIContext &ctx, IRID field) {
    std::string key;
    switch (IRI_NODE(ctx, field).tag) {
    case IRI_GEN::String:
      key = std::string(ctx.storage.strings.get(
          IRI_GEN::StringSEXP(field, ctx).getIridiumPrimitive()));
      break;
    case IRI_GEN::Boolean:
      key = IRI_GEN::BooleanSEXP(field, ctx).getIridiumPrimitive() ? "true"
                                                                   : "false";
      break;
    case IRI_GEN::Null:
      key = "null";
      break;
    case IRI_GEN::Number: {
      double v = IRI_GEN::NumberSEXP(field, ctx).getIridiumPrimitive();
      if (std::trunc(v) != v || !std::isfinite(v) ||
          std::abs(v) > 9007199254740992.0)
        return NO_IRID;
      key = std::to_string(static_cast<long long>(v));
      break;
    }
    default:
      return NO_IRID;
    }
    return IRI_GEN::StringSEXP::create(ctx, ctx.storage.strings.intern(key));
  }

  // obj[k] -> obj.key
  static IRID reduceRead(IRIContext &ctx, IRID node) {
    IRI_GEN::JSComputedFieldReadSEXP op(node, ctx);
    IRID key = keyOf(ctx, op.getArg_Field());
    if (key == NO_IRID)
      return node;
    std::vector<IRID> args = {op.getArg_Obj(), key};
    if (op.hasArg_Receiver())
      args.push_back(op.getArg_Receiver());
    return ctx.storage.nodes.add_node(IRI_GEN::FieldRead, args, {});
  }

  // obj[k] = v -> obj.key = v
  static IRID reduceWrite(IRIContext &ctx, IRID node) {
    IRI_GEN::JSComputedFieldWriteSEXP op(node, ctx);
    IRID key = keyOf(ctx, op.getArg_Field());
    if (key == NO_IRID)
      return node;
    std::vector<IRID> args = {op.getArg_Obj(), key, op.getArg_Value()};
    if (op.hasArg_Receiver())
      args.push_back(op.getArg_Receiver());
    return ctx.storage.nodes.add_node(IRI_GEN::FieldWrite, args, {});
  }

  // { [k]: v } -> { key: v }
  static bool reduceDefine(IRIContext &ctx, IRID node) {
    IRI_GEN::JSDefineObjPropSEXP op(node, ctx);
    if (IRI_NODE(ctx, op.getArg_Key()).tag == IRI_GEN::String)
      return false;
    IRID key = keyOf(ctx, op.getArg_Key());
    if (key == NO_IRID)
      return false;
    op.setArg_Key(key);
    return true;
  }

  // Recurse only for JSComputedFieldRead, much much easier to read and reason
  // about than the earlier implementation :)
  static void patch(IRIContext &ctx, IRID node, bool &changed) {
    auto args = ctx.storage.nodes.get_args(node);
    bool dirty = false;
    for (size_t i = 0; i < args.size(); i++) {
      patch(ctx, args[i], changed);
      if (IRI_NODE(ctx, args[i]).tag == IRI_GEN::JSComputedFieldRead) {
        IRID next = reduceRead(ctx, args[i]);
        dirty |= next != args[i];
        args[i] = next;
      }
    }
    if (dirty) {
      ctx.storage.nodes.set_args(node, args);
      changed = true;
    }
  }

  bool run(IRICFG &cfg, AnalysisManager &am) {
    auto &ctx = cfg.ctx;
    bool changed = false;

    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      BBEditor ed(cfg, h);
      for (IRID stmt : cfg.at(h).stmts) {
        patch(ctx, stmt, changed);

        IRID next = stmt;
        switch (IRI_NODE(ctx, stmt).tag) {
        case IRI_GEN::JSComputedFieldWrite:
          next = reduceWrite(ctx, stmt);
          break;
        case IRI_GEN::JSDefineObjProp:
          changed |= reduceDefine(ctx, stmt);
          break;
        case IRI_GEN::JSComputedFieldRead:
          next = reduceRead(ctx, stmt);
          break;
        default:
          break;
        }

        if (next != stmt) {
          ed.replace(stmt, next);
          changed = true;
        }
      }
      patch(ctx, cfg.at(h).terminal, changed);
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
