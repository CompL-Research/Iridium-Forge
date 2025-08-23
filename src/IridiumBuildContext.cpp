#include "Iridium/IridiumBuildContext.h"
#include "Iridium/IridiumSEXP.h"
#include "generated/IridiumTypes.h"

void parseBuildContexts(const msgpack::object &obj, std::unordered_map<int, std::shared_ptr<BBSEXP>> & bbIdxToSEXPMap, std::unordered_map<int, IRIBUILDCONTEXT> & iridiumBuildContext)
{
  if (obj.type != msgpack::type::ARRAY)
    throw std::runtime_error("Expected ARRAY for build contexts");

  for (uint32_t i = 0; i < obj.via.array.size; i++)
  {
    const auto &ctxObj = obj.via.array.ptr[i];
    if (ctxObj.type != msgpack::type::MAP)
      throw std::runtime_error("Expected MAP inside build contexts array");

    int parent = -1;
    int scopeIdx = 0;
    std::vector<std::string> args;
    bool isArgInitContext = false;
    int bypassParent = -1;
    std::unordered_set<std::string> argInitContextWhitelist;
    bool hasRestArgs = false;
    std::optional<IridiumBuildContext::LoopConfig> loopConfig = std::nullopt;
    std::optional<IridiumBuildContext::TryContext> tryContext = std::nullopt;
    int kind = 0;
    std::optional<std::string> propInitClos = std::nullopt;
    int argumentsKind = 0;
    bool isAsync = false;
    bool isGenerator = false;
    bool isStrict = false;
    bool isModule = false;
    int ecmaArgs = 0;
    std::optional<std::unordered_map<std::string, std::pair<std::string, std::string>>> privateMapping = std::nullopt;
    std::optional<std::unordered_map<std::string, IRISEXP>> moduleRequestMap = std::nullopt;
    std::vector<std::shared_ptr<BBSEXP>> bbs;

    // Parse each key/value
    for (uint32_t j = 0; j < ctxObj.via.map.size; j++)
    {
      auto &keyObj = ctxObj.via.map.ptr[j].key;
      auto &valObj = ctxObj.via.map.ptr[j].val;
      std::string key = keyObj.as<std::string>();

      if (key == "parent")
        parent = valObj.as<int>();
      else if (key == "scopeIdx")
        scopeIdx = valObj.as<int>();
      else if (key == "args" && valObj.type == msgpack::type::ARRAY)
      {
        for (uint32_t k = 0; k < valObj.via.array.size; k++)
          args.push_back(valObj.via.array.ptr[k].as<std::string>());
      }
      else if (key == "isArgInitContext")
        isArgInitContext = valObj.as<bool>();
      else if (key == "bypassParent")
        bypassParent = valObj.as<int>();
      else if (key == "argInitContextWhitelist" && valObj.type == msgpack::type::ARRAY)
      {
        for (uint32_t k = 0; k < valObj.via.array.size; k++)
          argInitContextWhitelist.insert(valObj.via.array.ptr[k].as<std::string>());
      }
      else if (key == "hasRestArgs")
        hasRestArgs = valObj.as<bool>();
      else if (key == "loopConfig")
      {
        if (valObj.type != msgpack::type::MAP) continue;
        IridiumBuildContext::LoopConfig lc;
        for (uint32_t k = 0; k < valObj.via.map.size; k++)
        {
          auto lk = valObj.via.map.ptr[k].key.as<std::string>();
          auto lv = valObj.via.map.ptr[k].val;
          if (lk == "kind")
          {
            std::string kindStr = lv.as<std::string>();
            lc.kind = (kindStr == "for-of"
                           ? IridiumBuildContext::LoopConfig::Kind::ForOf
                           : IridiumBuildContext::LoopConfig::Kind::Standard);
          }
          else if (lk == "loopHeadIDX")
            lc.loopHeadIDX = lv.as<int>();
          else if (lk == "loopBodyIDX")
            lc.loopBodyIDX = lv.as<int>();
          else if (lk == "loopInitIDX")
            lc.loopInitIDX = lv.as<int>();
          else if (lk == "label")
          {
            if (lv.type != msgpack::type::NIL) lc.label = lv.as<std::string>();
          }
          else if (lk == "breakTarget")
            lc.breakTarget = lv.as<int>();
          else if (lk == "continueTarget")
            lc.continueTarget = lv.as<int>();
          else
          {
            throw std::runtime_error("loopConfig key mismatch: " + lk);
          }
        }
        loopConfig = lc;
      }
      else if (key == "tryContext")
      {
        if (valObj.type != msgpack::type::MAP) continue;
        IridiumBuildContext::TryContext tc;
        for (uint32_t k = 0; k < valObj.via.map.size; k++)
        {
          auto tk = valObj.via.map.ptr[k].key.as<std::string>();
          auto tv = valObj.via.map.ptr[k].val;
          if (tk == "tryContextIDX")
            tc.tryContextIDX = tv.as<int>();
          else if (tk == "tryIDX")
            tc.tryIDX = tv.as<int>();
          else if (tk == "udCatchIDX")
            tc.udCatchIDX = tv.as<int>();
          else if (tk == "imCatchIDX")
            tc.imCatchIDX = tv.as<int>();
          else if (tk == "finalizerIDX")
            tc.finalizerIDX = tv.as<int>();
          else
          {
            throw std::runtime_error("tryContext key mismatch");
          }
        }
        tryContext = tc;
      }
      else if (key == "kind")
        kind = valObj.as<int>();
      else if (key == "propInitClos")
      {
        if (valObj.type != msgpack::type::STR) continue;
        propInitClos = valObj.as<std::string>();
      }
      else if (key == "argumentsKind")
        argumentsKind = valObj.as<int>();
      else if (key == "isAsync")
        isAsync = valObj.as<bool>();
      else if (key == "isGenerator")
        isGenerator = valObj.as<bool>();
      else if (key == "isStrict")
        isStrict = valObj.as<bool>();
      else if (key == "isModule")
        isModule = valObj.as<bool>();
      else if (key == "ecmaArgs")
        ecmaArgs = valObj.as<int>();
      else if (key == "privateMapping")
      {
        if (valObj.type != msgpack::type::ARRAY) continue;
        std::unordered_map<std::string, std::pair<std::string, std::string>> pm;
        for (uint32_t k = 0; k < valObj.via.array.size; k++)
        {
          const auto &pairObj = valObj.via.array.ptr[k];
          if (pairObj.type == msgpack::type::ARRAY && pairObj.via.array.size == 2)
          {
            std::string k1 = pairObj.via.array.ptr[0].as<std::string>();
            auto inner = pairObj.via.array.ptr[1];
            if (inner.type == msgpack::type::ARRAY && inner.via.array.size == 2)
            {
              std::string v1 = inner.via.array.ptr[0].as<std::string>();
              std::string v2 = inner.via.array.ptr[1].as<std::string>();
              pm[k1] = { v1, v2 };
            }
            else
            {
              throw std::runtime_error("privateMapping inner element size mismatch");
            }
          }
          else
          {
            throw std::runtime_error("privateMapping element size mismatch");
          }
        }
        privateMapping = pm;
      }
      else if (key == "moduleRequestMap") 
      {
        if (valObj.type != msgpack::type::ARRAY) continue;
        std::unordered_map<std::string, IRISEXP> mrm;
        for (uint32_t k = 0; k < valObj.via.array.size; k++)
        {
          const auto &pairObj = valObj.via.array.ptr[k];
          if (pairObj.type == msgpack::type::ARRAY && pairObj.via.array.size == 2)
          {
            std::string key = pairObj.via.array.ptr[0].as<std::string>();
            mrm[key] = parseSEXP(pairObj.via.array.ptr[1]);
          }
          else
          {
            throw std::runtime_error("moduleRequestMap element size mismatch");
          }
        }
        moduleRequestMap = mrm;
      }
      else if (key == "BB" && valObj.type == msgpack::type::ARRAY)
      {
        for (uint32_t k = 0; k < valObj.via.array.size; k++)
        {
          const auto &val = valObj.via.array.ptr[k];
          int bbIdx = val.as<int>();
          if (bbIdxToSEXPMap.find(bbIdx) == bbIdxToSEXPMap.end()) {
            throw std::runtime_error("bbIdx not found!!");
          }

          bbs.push_back(bbIdxToSEXPMap[bbIdx]);
        }
      }
      else
      {
        throw std::runtime_error("ukn key in parse build contexts: " + key + ", type: " + std::to_string(valObj.type));
      }
    }

    // Construct the object
    iridiumBuildContext[scopeIdx] =
        std::make_shared<IridiumBuildContext>(
            parent, scopeIdx, args, isArgInitContext, bypassParent,
            argInitContextWhitelist, hasRestArgs, loopConfig, tryContext,
            kind, propInitClos, argumentsKind, isAsync, isGenerator,
            isStrict, isModule, ecmaArgs, privateMapping, moduleRequestMap, bbs);

  }
}

void IridiumBuildContext::dump(std::ostream & oss = std::cout) const
  {
    oss << "IridiumBuildContext {\n";
    oss << "  parent: " << parent << "\n";
    oss << "  scopeIdx: " << scopeIdx << "\n";
    oss << "  args: [";
    for (size_t i = 0; i < args.size(); ++i)
    {
      oss << args[i];
      if (i + 1 < args.size())
        oss << ", ";
    }
    oss << "]\n";
    oss << "  isArgInitContext: " << (isArgInitContext ? "true" : "false") << "\n";
    oss << "  bypassParent: " << bypassParent << "\n";

    oss << "  argInitContextWhitelist: {";
    bool first = true;
    for (auto &s : argInitContextWhitelist)
    {
      if (!first)
        oss << ", ";
      oss << s;
      first = false;
    }
    oss << "}\n";

    oss << "  hasRestArgs: " << (hasRestArgs ? "true" : "false") << "\n";

    if (loopConfig)
    {
      oss << "  loopConfig: { kind: "
          << (loopConfig->kind == LoopConfig::Kind::ForOf ? "for-of" : "standard")
          << ", loopHeadIDX: " << loopConfig->loopHeadIDX
          << ", loopBodyIDX: " << loopConfig->loopBodyIDX
          << ", loopInitIDX: " << loopConfig->loopInitIDX
          << ", label: " << (loopConfig->label ? *loopConfig->label : "null")
          << ", breakTarget: " << loopConfig->breakTarget
          << ", continueTarget: " << loopConfig->continueTarget
          << " }\n";
    }
    else
    {
      oss << "  loopConfig: null\n";
    }

    if (tryContext)
    {
      oss << "  tryContext: { tryContextIDX: " << tryContext->tryContextIDX
          << ", tryIDX: " << tryContext->tryIDX
          << ", udCatchIDX: " << tryContext->udCatchIDX
          << ", imCatchIDX: " << tryContext->imCatchIDX
          << ", finalizerIDX: " << tryContext->finalizerIDX << " }\n";
    }
    else
    {
      oss << "  tryContext: null\n";
    }

    oss << "  kind: " << kind << "\n";
    oss << "  propInitClos: " << (propInitClos ? *propInitClos : "null") << "\n";
    oss << "  argumentsKind: " << argumentsKind << "\n";
    oss << "  isAsync: " << (isAsync ? "true" : "false") << "\n";
    oss << "  isGenerator: " << (isGenerator ? "true" : "false") << "\n";
    oss << "  isStrict: " << (isStrict ? "true" : "false") << "\n";
    oss << "  isModule: " << (isModule ? "true" : "false") << "\n";
    oss << "  ecmaArgs: " << ecmaArgs << "\n";

    if (privateMapping)
    {
      oss << "  privateMapping: [\n";
      for (auto &entry : *privateMapping)
      {
        oss << "    { key: " << entry.first
            << ", value: (" << entry.second.first << ", " << entry.second.second << ") }\n";
      }
      oss << "  ]\n";
    }
    else
    {
      oss << "  privateMapping: null\n";
    }

    if (moduleRequestMap)
    {
      oss << "  moduleRequestMap: [\n";
      for (auto &entry : *moduleRequestMap)
      {
        oss << "    key: " << entry.first << "\n";
        entry.second->dump(6, oss);
      }
      oss << "  ]\n";
    }
    else
    {
      oss << "  moduleRequestMap: null\n";
    }

    oss << "  BB: [\n";
    for (size_t i = 0; i < BB.size(); ++i)
    {
      BB[i]->dump(4, oss);
    }
    oss << "  ]\n";

    oss << "}";
  }