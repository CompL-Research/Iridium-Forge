#pragma once
#include "Storage/Config.h"
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace IRI_PARSE {
class IridiumSEXP;

struct LoopConfig {
  enum class Kind { ForOf, Standard } kind;
  int loopHeadIDX;
  int loopBodyIDX;
  int loopInitIDX;
  std::optional<std::string> label = std::nullopt;
  int breakTarget;
  int continueTarget;
};

struct TryContext {
  int tryContextIDX;
  int tryIDX;
  int udCatchIDX;
  int imCatchIDX;
  int finalizerIDX;
};

class IridiumBuildContext {
public:
  int parent;
  int scopeIDX;
  std::vector<std::string> args;
  bool isArgInitContext;
  int bypassParent;
  std::unordered_set<std::string> argInitContextWhitelist;
  bool hasRestArgs;

  std::optional<LoopConfig> loopConfig = std::nullopt;
  std::optional<TryContext> tryContext = std::nullopt;

  int kind;
  std::optional<std::string> propInitClos = std::nullopt;
  int argumentsKind;
  bool isAsync;
  bool isGenerator;
  bool isStrict;
  bool isModule;

  int ecmaArgs;
  std::string name;
  double sourceLine;

  std::optional<
      std::unordered_map<std::string, std::pair<std::string, std::string>>>
      privateMapping = std::nullopt;
  std::optional<std::unordered_map<std::string, IRI_STORAGE::IRID>>
      moduleRequestMap = std::nullopt;
  std::vector<IRI_STORAGE::IRID> BB;

  IridiumBuildContext() = default;
};

// Helper to print lists (vector/set)
template <typename T>
inline void printIterable(std::ostream &os, const T &container) {
  os << "[";
  bool first = true;
  for (const auto &item : container) {
    if (!first)
      os << ", ";
    else
      os << " ";
    // Simple heuristic: if it's a string type, wrap in quotes
    if constexpr (std::is_convertible_v<decltype(item), std::string_view>) {
      os << "'" << item << "'";
    } else {
      os << item;
    }
    first = false;
  }
  if (!first)
    os << " ";
  os << "]";
}

// 1. LoopConfig Overload
inline std::ostream &operator<<(std::ostream &os, const LoopConfig &lc) {
  os << "{ kind: "
     << (lc.kind == LoopConfig::Kind::ForOf ? "'for-of'" : "'standard'")
     << ", loopHeadIDX: " << lc.loopHeadIDX
     << ", loopBodyIDX: " << lc.loopBodyIDX
     << ", loopInitIDX: " << lc.loopInitIDX
     << ", label: " << (lc.label ? ("'" + *lc.label + "'") : "null")
     << ", breakTarget: " << lc.breakTarget
     << ", continueTarget: " << lc.continueTarget << " }";
  return os;
}

// 2. TryContext Overload
inline std::ostream &operator<<(std::ostream &os, const TryContext &tc) {
  os << "{ tryContextIDX: " << tc.tryContextIDX << ", tryIDX: " << tc.tryIDX
     << ", udCatchIDX: " << tc.udCatchIDX << ", imCatchIDX: " << tc.imCatchIDX
     << ", finalizerIDX: " << tc.finalizerIDX << " }";
  return os;
}

// 3. IridiumBuildContext Overload
inline std::ostream &operator<<(std::ostream &os,
                                const IridiumBuildContext &ctx) {
  os << "  {\n"
     << "    parent: " << ctx.parent << ",\n"
     << "    scopeIDX: " << ctx.scopeIDX << ",\n"
     << "    args: ";
  printIterable(os, ctx.args);
  os << ",\n"
     << "    isArgInitContext: " << (ctx.isArgInitContext ? "true" : "false")
     << ",\n"
     << "    bypassParent: " << ctx.bypassParent << ",\n"
     << "    argInitContextWhitelist: ";
  printIterable(os, ctx.argInitContextWhitelist);
  os << ",\n"
     << "    hasRestArgs: " << (ctx.hasRestArgs ? "true" : "false") << ",\n"
     << "    loopConfig: ";
  if (ctx.loopConfig)
    os << *ctx.loopConfig;
  else
    os << "null";
  os << ",\n"
     << "    tryContext: ";
  if (ctx.tryContext)
    os << *ctx.tryContext;
  else
    os << "null";
  os << ",\n"
     << "    kind: " << ctx.kind << ",\n"
     << "    propInitClos: "
     << (ctx.propInitClos ? ("'" + *ctx.propInitClos + "'") : "null") << ",\n"
     << "    argumentsKind: " << ctx.argumentsKind << ",\n"
     << "    isAsync: " << (ctx.isAsync ? "true" : "false") << ",\n"
     << "    isGenerator: " << (ctx.isGenerator ? "true" : "false") << ",\n"
     << "    isStrict: " << (ctx.isStrict ? "true" : "false") << ",\n"
     << "    isModule: " << (ctx.isModule ? "true" : "false") << ",\n"
     << "    ecmaArgs: " << ctx.ecmaArgs << ",\n"
     << "    name: '" << ctx.name << "',\n"
     << "    privateMapping: ";

  // Expand privateMapping
  if (ctx.privateMapping) {
    os << "{";
    bool pmFirst = true;
    for (const auto &[k, v] : *ctx.privateMapping) {
      if (!pmFirst)
        os << ", ";
      else
        os << " ";
      os << "'" << k << "': ['" << v.first << "', '" << v.second << "']";
      pmFirst = false;
    }
    if (!pmFirst)
      os << " ";
    os << "}";
  } else {
    os << "null";
  }
  os << ",\n"
     << "    moduleRequestMap: ";

  // Expand moduleRequestMap
  if (ctx.moduleRequestMap) {
    os << "{";
    bool mrFirst = true;
    for (const auto &[k, v] : *ctx.moduleRequestMap) {
      if (!mrFirst)
        os << ", ";
      else
        os << " ";
      // Assuming v (IridiumSEXP*) prints its address or has an overloaded
      // operator<<
      os << "'" << k << "': " << v;
      mrFirst = false;
    }
    if (!mrFirst)
      os << " ";
    os << "}";
  } else {
    os << "null"; // Or "[]" if you specifically want it to mirror the JS empty
                  // array representation
  }
  os << ",\n"
     << "    BB: [";

  bool bbFirst = true;
  for (size_t i = 0; i < ctx.BB.size(); ++i) {
    if (!bbFirst)
      os << ", ";
    else
      os << " ";
    os << ctx.BB[i];
    bbFirst = false;
  }
  if (!bbFirst)
    os << " ";
  os << "]\n  }";

  return os;
}
} // namespace IRI_PARSE
