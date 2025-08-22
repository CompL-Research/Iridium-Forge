#pragma once
#include "Iridium/Globals.h"
#include <msgpack.hpp>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include <unordered_set>
#include <sstream>
#include <iostream>
#include <memory>

// Forward-declare your IridiumSEXP type
class IridiumSEXP;
class BBSEXP;

class IridiumBuildContext
{
public:
  int parent;
  int scopeIdx;
  std::vector<std::string> args;
  bool isArgInitContext;
  int bypassParent;
  std::unordered_set<std::string> argInitContextWhitelist;
  bool hasRestArgs;

  struct LoopConfig
  {
    enum class Kind
    {
      ForOf,
      Standard
    } kind;
    int loopHeadIDX;
    int loopBodyIDX;
    int loopInitIDX;
    std::optional<std::string> label;
    int breakTarget;
    int continueTarget;
  };
  std::optional<LoopConfig> loopConfig;

  struct TryContext
  {
    int tryContextIDX;
    int tryIDX;
    int udCatchIDX;
    int imCatchIDX;
    int finalizerIDX;
  };
  std::optional<TryContext> tryContext;

  int kind;
  std::optional<std::string> propInitClos;
  int argumentsKind;
  bool isAsync;
  bool isGenerator;
  bool isStrict;
  bool isModule;

  int ecmaArgs;

  std::optional<std::unordered_map<std::string, std::pair<std::string, std::string>>> privateMapping;
  std::optional<std::unordered_map<std::string, IRISEXP>> moduleRequestMap;
  std::vector<std::shared_ptr<BBSEXP>> BB;

  // Constructor
  IridiumBuildContext(
      int parent,
      int scopeIdx,
      std::vector<std::string> args,
      bool isArgInitContext,
      int bypassParent,
      std::unordered_set<std::string> argInitContextWhitelist,
      bool hasRestArgs,
      std::optional<LoopConfig> loopConfig,
      std::optional<TryContext> tryContext,
      int kind,
      std::optional<std::string> propInitClos,
      int argumentsKind,
      bool isAsync,
      bool isGenerator,
      bool isStrict,
      bool isModule,
      int ecmaArgs,
      std::optional<std::unordered_map<std::string, std::pair<std::string, std::string>>> privateMapping,
      std::optional<std::unordered_map<std::string, IRISEXP>> moduleRequestMap,
      std::vector<std::shared_ptr<BBSEXP>> BB)
      : parent(parent),
        scopeIdx(scopeIdx),
        args(std::move(args)),
        isArgInitContext(isArgInitContext),
        bypassParent(bypassParent),
        argInitContextWhitelist(std::move(argInitContextWhitelist)),
        hasRestArgs(hasRestArgs),
        loopConfig(std::move(loopConfig)),
        tryContext(std::move(tryContext)),
        kind(kind),
        propInitClos(std::move(propInitClos)),
        argumentsKind(argumentsKind),
        isAsync(isAsync),
        isGenerator(isGenerator),
        isStrict(isStrict),
        isModule(isModule),
        ecmaArgs(ecmaArgs),
        privateMapping(std::move(privateMapping)),
        moduleRequestMap(std::move(moduleRequestMap)),
        BB(std::move(BB)) {}

  // Pretty printer
  void dump(std::ostream & oss) const;
};

void parseBuildContexts(const msgpack::object &obj, std::unordered_map<int, std::shared_ptr<BBSEXP>> & bbIdxToSEXPMap, std::unordered_map<int, IRIBUILDCONTEXT> & iridiumBuildContext);
