#pragma once
#include "Generated/IridiumMeta.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include <napi.h>
#include <zlib.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace IRI_PARSE {

class IridiumParser {
  Napi::Array arr;
  uint32_t size = 0;
  uint32_t current_idx = 0;
  IRI_STORAGE::IridiumPool &pool;

public:
  std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>
      iridiumBuildContext;

  std::unordered_map<int, IRI_STORAGE::IRID> bbIdxToSEXPMap;

  IridiumParser(IRI_STORAGE::IridiumPool &pool) : pool(pool) {}

  // Initialize the parser state
  void initParseCTX(Napi::Array codeArray) {
    arr = codeArray;
    size = arr.Length();
    current_idx = 0;
  }

  IRI_STORAGE::IRID parse() {
    if (current_idx >= size) {
      throw std::runtime_error(
          "[FORGE] Iridium Parse Error: Unexpected EOF in flat array");
    }

    // 1. Read Tag
    uint32_t tag_val = arr.Get(current_idx++).As<Napi::Number>().Uint32Value();
    IRI_GEN::IRI_TAG tag = static_cast<IRI_GEN::IRI_TAG>(tag_val);

    // 2. Read Counts
    uint32_t num_args = arr.Get(current_idx++).As<Napi::Number>().Uint32Value();
    uint32_t num_flags =
        arr.Get(current_idx++).As<Napi::Number>().Uint32Value();

    // 3. Prepare Blocks for the Pool
    std::vector<IRI_STORAGE::IRID> args;
    if (num_args > 0) {
      args.reserve(num_args);
    }

    uint32_t flag_slots = IRI_GEN::IridiumMeta::get_flag_slots(tag);
    std::vector<IRI_STORAGE::FlagValue> flags(
        flag_slots); // Defaults to std::monostate

    // 4. Parse Flags
    for (uint32_t i = 0; i < num_flags; ++i) {
      uint32_t flag_enum_val =
          arr.Get(current_idx++).As<Napi::Number>().Uint32Value();
      IRI_GEN::IRI_FLAG flag_enum =
          static_cast<IRI_GEN::IRI_FLAG>(flag_enum_val);

      // Get the value as a generic Napi::Value
      Napi::Value val_obj = arr.Get(current_idx++);

      // Ask the meta class where this flag goes in the array
      int slot_idx = IRI_GEN::IridiumMeta::get_flag_index(tag, flag_enum);

      if (slot_idx >= 0) {
        if (val_obj.IsNumber()) {
          flags[slot_idx] = val_obj.As<Napi::Number>().DoubleValue();
        } else if (val_obj.IsBoolean()) {
          flags[slot_idx] = val_obj.As<Napi::Boolean>().Value();
        } else if (val_obj.IsString()) {
          // STRING POOLING: Hash it, store it in the pool, and save the integer
          // ID
          std::string temp_str = val_obj.As<Napi::String>().Utf8Value();
          StringID id = pool.strings.intern(temp_str);
          flags[slot_idx] = id;
        } else if (val_obj.IsNull()) {
          flags[slot_idx] = std::nullptr_t{};
        } else {
          throw std::runtime_error(
              "[FORGE] Iridium Parse Error: Unsupported flag type from V8");
        }
      } else {
        throw std::runtime_error(
            "[FORGE] Iridium Parse Error: " + IRI_GEN::dump_flag(flag_enum) +
            " slot not defined for " + IRI_GEN::dump_tag(tag));
      }
    }

    // 5. Parse Args Recursively
    for (uint32_t i = 0; i < num_args; ++i) {
      args.push_back(parse());
    }

    // 6. Commit to Pool!
    IRI_STORAGE::IRID node_id = pool.add_node(tag, args, flags);

    // If this is a BBSEXP, populate it in the map
    if (tag == IRI_GEN::IRI_TAG::BB) {
      // BBSEXP now takes a reference to the IridiumSEXP data
      IRI_GEN::BBSEXP bbSEXPView(node_id, pool);

      if (bbIdxToSEXPMap.count(bbSEXPView.getIDX()) > 0) {
        throw std::runtime_error(
            "Parse failed: Already populated BBSEXP, expected a unique IDX");
      }
      bbIdxToSEXPMap[bbSEXPView.getIDX()] = node_id;
    }

    return node_id;
  }

  void parseBuildContexts(Napi::Array buildContextArray) {
    for (uint32_t i = 0; i < buildContextArray.Length(); i++) {
      Napi::Value ctxVal = buildContextArray.Get(i);

      if (!ctxVal.IsObject()) {
        throw std::runtime_error("Expected Object inside build contexts array");
      }

      Napi::Object ctxObj = ctxVal.As<Napi::Object>();
      std::shared_ptr<IridiumBuildContext> res =
          std::make_shared<IridiumBuildContext>();

      // Primitive assignments
      res->parent = ctxObj.Get("parent").As<Napi::Number>().Int32Value();
      res->scopeIDX = ctxObj.Get("scopeIDX").As<Napi::Number>().Int32Value();

      Napi::Array argsArr = ctxObj.Get("args").As<Napi::Array>();
      for (uint32_t j = 0; j < argsArr.Length(); j++) {
        res->args.push_back(argsArr.Get(j).As<Napi::String>().Utf8Value());
      }

      res->isArgInitContext =
          ctxObj.Get("isArgInitContext").As<Napi::Boolean>().Value();
      res->bypassParent =
          ctxObj.Get("bypassParent").As<Napi::Number>().Int32Value();

      Napi::Array whitelistArr =
          ctxObj.Get("argInitContextWhitelist").As<Napi::Array>();
      for (uint32_t j = 0; j < whitelistArr.Length(); j++) {
        res->argInitContextWhitelist.insert(
            whitelistArr.Get(j).As<Napi::String>().Utf8Value());
      }

      res->hasRestArgs = ctxObj.Get("hasRestArgs").As<Napi::Boolean>().Value();

      // Loop Config
      Napi::Value loopConfigVal = ctxObj.Get("loopConfig");
      if (!loopConfigVal.IsNull() && !loopConfigVal.IsUndefined()) {
        Napi::Object lcObj = loopConfigVal.As<Napi::Object>();
        LoopConfig lc;
        std::string kindStr = lcObj.Get("kind").As<Napi::String>().Utf8Value();
        lc.kind = (kindStr == "for-of") ? LoopConfig::Kind::ForOf
                                        : LoopConfig::Kind::Standard;
        lc.loopHeadIDX =
            lcObj.Get("loopHeadIDX").As<Napi::Number>().Int32Value();
        lc.loopBodyIDX =
            lcObj.Get("loopBodyIDX").As<Napi::Number>().Int32Value();
        lc.loopInitIDX =
            lcObj.Get("loopInitIDX").As<Napi::Number>().Int32Value();

        Napi::Value labelVal = lcObj.Get("label");
        if (!labelVal.IsNull() && !labelVal.IsUndefined()) {
          lc.label = labelVal.As<Napi::String>().Utf8Value();
        }

        lc.breakTarget =
            lcObj.Get("breakTarget").As<Napi::Number>().Int32Value();
        lc.continueTarget =
            lcObj.Get("continueTarget").As<Napi::Number>().Int32Value();
        res->loopConfig = lc;
      }

      // Try Context
      Napi::Value tryContextVal = ctxObj.Get("tryContext");
      if (!tryContextVal.IsNull() && !tryContextVal.IsUndefined()) {
        Napi::Object tcObj = tryContextVal.As<Napi::Object>();
        TryContext tc;
        tc.tryContextIDX =
            tcObj.Get("tryContextIDX").As<Napi::Number>().Int32Value();
        tc.tryIDX = tcObj.Get("tryIDX").As<Napi::Number>().Int32Value();
        tc.udCatchIDX = tcObj.Get("udCatchIDX").As<Napi::Number>().Int32Value();
        tc.imCatchIDX = tcObj.Get("imCatchIDX").As<Napi::Number>().Int32Value();
        tc.finalizerIDX =
            tcObj.Get("finalizerIDX").As<Napi::Number>().Int32Value();
        tc.tryScopeIDX =
            tcObj.Get("tryScopeIDX").As<Napi::Number>().Int32Value();
        tc.udCatchScopeIDX =
            tcObj.Get("udCatchScopeIDX").As<Napi::Number>().Int32Value();
        tc.finalizerRetIDX =
            tcObj.Get("finalizerRetIDX").As<Napi::Number>().Int32Value();
        res->tryContext = tc;
      }

      res->kind = ctxObj.Get("kind").As<Napi::Number>().Int32Value();

      Napi::Value propInitVal = ctxObj.Get("propInitClos");
      if (!propInitVal.IsNull() && !propInitVal.IsUndefined()) {
        res->propInitClos = propInitVal.As<Napi::String>().Utf8Value();
      }

      res->argumentsKind =
          ctxObj.Get("argumentsKind").As<Napi::Number>().Int32Value();
      res->isAsync = ctxObj.Get("isAsync").As<Napi::Boolean>().Value();
      res->isGenerator = ctxObj.Get("isGenerator").As<Napi::Boolean>().Value();
      res->isStrict = ctxObj.Get("isStrict").As<Napi::Boolean>().Value();
      res->isModule = ctxObj.Get("isModule").As<Napi::Boolean>().Value();
      res->ecmaArgs = ctxObj.Get("ecmaArgs").As<Napi::Number>().Int32Value();
      res->name = ctxObj.Get("name").As<Napi::String>().Utf8Value();
      res->sourceLine =
          ctxObj.Get("sourceLine").As<Napi::Number>().DoubleValue();

      // Private Mapping
      Napi::Value pmVal = ctxObj.Get("privateMapping");
      if (!pmVal.IsNull() && !pmVal.IsUndefined()) {
        std::unordered_map<std::string, std::pair<std::string, std::string>>
            pmMap;
        Napi::Array pmArr = pmVal.As<Napi::Array>();
        for (uint32_t j = 0; j < pmArr.Length(); j++) {
          Napi::Array tuple = pmArr.Get(j).As<Napi::Array>();
          std::string key =
              tuple.Get((uint32_t)0).As<Napi::String>().Utf8Value();

          Napi::Array innerTuple = tuple.Get(1).As<Napi::Array>();
          std::string val1 =
              innerTuple.Get((uint32_t)0).As<Napi::String>().Utf8Value();
          std::string val2 = innerTuple.Get(1).As<Napi::String>().Utf8Value();

          pmMap[key] = {val1, val2};
        }
        res->privateMapping = pmMap;
      }

      // Module Request Map
      Napi::Value mrVal = ctxObj.Get("moduleRequestMap");
      if (!mrVal.IsNull() && !mrVal.IsUndefined()) {
        std::unordered_map<std::string, IRI_STORAGE::IRID> mrMap;
        Napi::Array pmArr = mrVal.As<Napi::Array>();

        // Save current state so we can recursively parse the inner arrays
        Napi::Array saved_arr = arr;
        uint32_t saved_size = size;
        uint32_t saved_idx = current_idx;

        for (uint32_t j = 0; j < pmArr.Length(); j++) {
          Napi::Array tuple = pmArr.Get(j).As<Napi::Array>();
          std::string key =
              tuple.Get((uint32_t)0).As<Napi::String>().Utf8Value();
          Napi::Array code = tuple.Get(1).As<Napi::Array>();

          initParseCTX(code);
          mrMap[key] = parse();
        }
        res->moduleRequestMap = mrMap;

        // Restore state
        arr = saved_arr;
        size = saved_size;
        current_idx = saved_idx;
      }

      // Basic Blocks (BB)
      Napi::Array bbArr = ctxObj.Get("BB").As<Napi::Array>();
      for (uint32_t j = 0; j < bbArr.Length(); j++) {
        int bbIdx = bbArr.Get(j).As<Napi::Number>().Int32Value();
        auto it = bbIdxToSEXPMap.find(bbIdx);
        if (it == bbIdxToSEXPMap.end()) {
          throw std::runtime_error("bbIdx not found: " + std::to_string(bbIdx));
        }
        res->BB.push_back(it->second); // Stores IRID
      }

      iridiumBuildContext[res->scopeIDX] = res;
    }
  }
};
} // namespace IRI_PARSE
