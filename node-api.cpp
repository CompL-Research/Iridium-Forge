#include "Entrypoint.h"
#include "Generated/IridiumTypes.h"
#include "IRIPerf.h"
#include "Parser/IridiumParser.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include <functional>
#include <msgpack.hpp>
#include <napi.h>
#include <string>
#include <zlib.h>

//
// Arg 0 (string)  : VERSION
// Arg 1 (string)  : Path
// Arg 2 (Array)   : IRIDIUM code
// Arg 3 (Array)   : IRIDIUM build context
//
Napi::Value execute(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Version string expected")
        .ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  if (!info[1].IsString()) {
    Napi::TypeError::New(env, "Filepath string expected")
        .ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  if (!info[2].IsArray()) {
    Napi::TypeError::New(env, "Iridium code array expected")
        .ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  if (!info[3].IsArray()) {
    Napi::TypeError::New(env, "Iridium build context array expected")
        .ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  if (!info[4].IsFunction()) {
    Napi::TypeError::New(env, "Tick function expected")
        .ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  if (!info[5].IsFunction()) {
    Napi::TypeError::New(env, "Tick function expected")
        .ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  // 1. Cast the arguments to Napi::Function
  Napi::Function NAPI_tick = info[4].As<Napi::Function>();
  Napi::Function NAPI_tock = info[5].As<Napi::Function>();

  IRIPerf perf;
  perf.tick = [&](std::string msg) {
    NAPI_tick.Call(env.Global(), {Napi::String::New(env, msg)});
  };

  perf.tock = [&](std::string msg) {
    NAPI_tock.Call(env.Global(), {Napi::String::New(env, msg)});
  };

  std::string VERSION = info[0].As<Napi::String>().Utf8Value();
  std::string PATH = info[1].As<Napi::String>().Utf8Value();
  Napi::Array IRIDIUM = info[2].As<Napi::Array>();
  Napi::Array BUILDCTX = info[3].As<Napi::Array>();

  IRI_STORAGE::IridiumPool pool;
  pool.NULL_SEXP = IRI_GEN::NullSEXP::create(pool, true);
  pool.NOP_SEXP = IRI_GEN::NOPSEXP::create(pool);
  pool.UNDEF_READ = IRI_GEN::EnvReadSEXP::create(pool, pool.getGlobalBindingSEXP("undefined"), false);
  pool.NUBD_SEXP = IRI_GEN::JSNUBDSEXP::create(pool);
  pool.TRUE_SEXP = IRI_GEN::BooleanSEXP::create(pool, true);
  pool.FALSE_SEXP = IRI_GEN::BooleanSEXP::create(pool, false);


  perf.tick("iri-forge-main");

  try {
    perf.tick("iri-forge-parse");

    IRI_PARSE::IridiumParser parser(pool);
    perf.tick("iri-forge-parse-code");
    parser.initParseCTX(IRIDIUM);
    IRI_STORAGE::IRID root = parser.parse();
    perf.tock("iri-forge-parse-code");

    perf.tick("iri-forge-parse-buildContext");
    parser.parseBuildContexts(BUILDCTX);
    perf.tock("iri-forge-parse-buildContext");

    perf.tock("iri-forge-parse");

    perf.tick("iri-forge-entrypoint");
    auto res = IRI_ENTRY::sharedEntrypoint(pool, root,
                                           parser.iridiumBuildContext, perf);
    perf.tock("iri-forge-entrypoint");

  } catch (const std::exception &e) {
    Napi::Error::New(env, std::string("[Forge] Parse Error: ") + e.what())
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  // std::ostringstream oss;

  // if (returnJSON)
  // {
  //   oss << "{";
  //   oss << "\"version\":" << "\"" << std::string(VERSION.via.str.ptr,
  //   VERSION.via.str.size) << "\","; oss << "\"absoluteFilePath\":" << "\"" <<
  //   std::string(path.via.str.ptr, path.via.str.size) << "\","; oss <<
  //   "\"iridium\":"; res->dump(oss, true); oss << "}";
  // }
  // else
  // {
  //   Napi::Error::New(env, "Binary format is not yet handled, use legacy JSON
  //   format").ThrowAsJavaScriptException();
  // }

  // std::string str = oss.str(); // keep it alive
  // size_t len = str.size();

  // Allocate raw buffer and copy data
  char *resData = new char[strlen("{}")];
  resData[0] = '{';
  resData[1] = '}';

  perf.tock("iri-forge-main");

  // Create Node Buffer that owns `data` and cleans up with delete[]
  return Napi::Buffer<char>::New(env, resData, 2,
                                 [](Napi::Env, char *data) { delete[] data; });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "execute"),
              Napi::Function::New(env, execute, "execute_iridium_cpp"));
  return exports;
}

NODE_API_MODULE(iridiumForge, Init)
