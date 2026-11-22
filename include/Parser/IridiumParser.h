#pragma once
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumMeta.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include <napi.h>
#include <zlib.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace IRI_PARSE {

// V8's Utf8Value() is lossy: it replaces unpaired surrogates with U+FFFD.
// JS strings are UTF-16 code unit sequences and may legitimately contain
// lone surrogates, and both the Iridium string pool (raw bytes) and
// quickjs's UTF-8 decoder (utf8_decode accepts 0xED lead bytes) round-trip
// WTF-8/CESU sequences. So encode from UTF-16 by hand: valid pairs become
// 4-byte UTF-8, lone surrogates become 3-byte CESU sequences, preserving
// string fidelity end-to-end (pool -> .iri dump -> JS_NewStringLen).
inline std::string napiStringToWTF8(const Napi::String &str) {
  std::u16string u16 = str.Utf16Value();
  std::string out;
  out.reserve(u16.size());
  for (size_t i = 0; i < u16.size(); i++) {
    uint32_t cp = u16[i];
    if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < u16.size() &&
        u16[i + 1] >= 0xDC00 && u16[i + 1] <= 0xDFFF) {
      cp = 0x10000 + ((cp - 0xD800) << 10) + (uint32_t)(u16[++i] - 0xDC00);
    }
    if (cp < 0x80) {
      out.push_back((char)cp);
    } else if (cp < 0x800) {
      out.push_back((char)(0xC0 | (cp >> 6)));
      out.push_back((char)(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
      out.push_back((char)(0xE0 | (cp >> 12)));
      out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
      out.push_back((char)(0x80 | (cp & 0x3F)));
    } else {
      out.push_back((char)(0xF0 | (cp >> 18)));
      out.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
      out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
      out.push_back((char)(0x80 | (cp & 0x3F)));
    }
  }
  return out;
}

class IridiumParser {
  Napi::Array arr;
  uint32_t size = 0;
  uint32_t current_idx = 0;
  IRI_STORAGE::IRIContext &ctx;

  std::unordered_map<IRI_GEN::IRI_TAG, IRI_STORAGE::IRID> internedTags;

public:
  std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>
      iridiumBuildContext;

  std::unordered_map<int, IRI_STORAGE::IRID> bbIdxToSEXPMap;

  IridiumParser(IRI_STORAGE::IRIContext &ctx) : ctx(ctx) {}

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
        auto ftype = IRI_GEN::IridiumMeta::get_flag_type(tag, flag_enum);
        if (ftype == IRI_GEN::IridiumMeta::FT_UINT32) {
          // New case for Uint32
          if (!val_obj.IsNumber())
            throw std::runtime_error(
                "[FORGE] Iridium Parse Error: " +
                IRI_GEN::dump_flag(flag_enum) +
                " expects an unsigned index, but V8 sent a non-number");
          double d = val_obj.As<Napi::Number>().DoubleValue();
          if (d < 0 || d >= (double)UINT32_MAX || d != (uint32_t)d)
            throw std::runtime_error(
                "[FORGE] Iridium Parse Error: " +
                IRI_GEN::dump_flag(flag_enum) +
                " is not a valid unsigned index");
          flags[slot_idx] = IRI_STORAGE::UInt32{(uint32_t)d};
        } else if (ftype == IRI_GEN::IridiumMeta::FT_INT32) {
          // New case for int32
          if (!val_obj.IsNumber())
            throw std::runtime_error(
                "[FORGE] Iridium Parse Error: " +
                IRI_GEN::dump_flag(flag_enum) +
                " expects an integer, but V8 sent a non-number");
          double d = val_obj.As<Napi::Number>().DoubleValue();
          if (d < (double)INT32_MIN || d > (double)INT32_MAX ||
              d != (int32_t)d)
            throw std::runtime_error(
                "[FORGE] Iridium Parse Error: " +
                IRI_GEN::dump_flag(flag_enum) +
                " is not a valid int32");
          flags[slot_idx] = IRI_STORAGE::Int32{(int32_t)d};
        } else if (val_obj.IsNumber()) {
          flags[slot_idx] = val_obj.As<Napi::Number>().DoubleValue();
        } else if (val_obj.IsBoolean()) {
          flags[slot_idx] = val_obj.As<Napi::Boolean>().Value();
        } else if (val_obj.IsString()) {
          // STRING POOLING: Hash it, store it in the ctx, and save the integer
          // ID
          std::string temp_str =
              napiStringToWTF8(val_obj.As<Napi::String>());
          StringID id = ctx.storage.strings.intern(temp_str);
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
    IRI_STORAGE::IRID node_id;

    // Intern constant tags
    if (tag == IRI_GEN::IRI_TAG::Null) {
      node_id = ctx.storage.nodes.NULL_SEXP;
    } else if (tag == IRI_GEN::IRI_TAG::NOP) {
      node_id = ctx.storage.nodes.NOP_SEXP;
    } else if (tag == IRI_GEN::IRI_TAG::JSNUBD) {
      node_id = ctx.storage.nodes.NUBD_SEXP;
    } else {
      node_id = ctx.storage.nodes.add_node(tag, args, flags);
    }

    // If this is a BBSEXP, populate it in the map
    if (tag == IRI_GEN::IRI_TAG::BB) {
      // BBSEXP now takes a reference to the IridiumSEXP data
      IRI_GEN::BBSEXP bbSEXPView(node_id, ctx);

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
        res->args.push_back(
            napiStringToWTF8(argsArr.Get(j).As<Napi::String>()));
      }

      res->isArgInitContext =
          ctxObj.Get("isArgInitContext").As<Napi::Boolean>().Value();
      res->bypassParent =
          ctxObj.Get("bypassParent").As<Napi::Number>().Int32Value();

      Napi::Array whitelistArr =
          ctxObj.Get("argInitContextWhitelist").As<Napi::Array>();
      for (uint32_t j = 0; j < whitelistArr.Length(); j++) {
        res->argInitContextWhitelist.insert(
            napiStringToWTF8(whitelistArr.Get(j).As<Napi::String>()));
      }

      res->hasRestArgs = ctxObj.Get("hasRestArgs").As<Napi::Boolean>().Value();

      // Loop Config
      Napi::Value loopConfigVal = ctxObj.Get("loopConfig");
      if (!loopConfigVal.IsNull() && !loopConfigVal.IsUndefined()) {
        Napi::Object lcObj = loopConfigVal.As<Napi::Object>();
        LoopConfig lc;
        std::string kindStr =
            napiStringToWTF8(lcObj.Get("kind").As<Napi::String>());
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
          lc.label = napiStringToWTF8(labelVal.As<Napi::String>());
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
        res->propInitClos = napiStringToWTF8(propInitVal.As<Napi::String>());
      }

      res->argumentsKind =
          ctxObj.Get("argumentsKind").As<Napi::Number>().Int32Value();
      res->isAsync = ctxObj.Get("isAsync").As<Napi::Boolean>().Value();
      res->isGenerator = ctxObj.Get("isGenerator").As<Napi::Boolean>().Value();
      res->isStrict = ctxObj.Get("isStrict").As<Napi::Boolean>().Value();
      res->isModule = ctxObj.Get("isModule").As<Napi::Boolean>().Value();
      res->ecmaArgs = ctxObj.Get("ecmaArgs").As<Napi::Number>().Int32Value();
      res->name = napiStringToWTF8(ctxObj.Get("name").As<Napi::String>());
      res->sourceLine =
          ctxObj.Get("sourceLine").As<Napi::Number>().DoubleValue();

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
              napiStringToWTF8(tuple.Get((uint32_t)0).As<Napi::String>());
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
