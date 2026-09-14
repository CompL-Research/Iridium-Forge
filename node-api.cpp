#include "Entrypoint.h"
#include "Generated/IridiumPassFlags.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumParser.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include <functional>
#include <napi.h>
#include <string>
#include <zlib.h>

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

  if (!info[6].IsObject()) {
    Napi::TypeError::New(env, "Pass flags object expected")
        .ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  // 1. Cast the arguments to Napi::Function
  Napi::Function NAPI_tick = info[4].As<Napi::Function>();
  Napi::Function NAPI_tock = info[5].As<Napi::Function>();

  std::string VERSION = info[0].As<Napi::String>().Utf8Value();
  std::string PATH = info[1].As<Napi::String>().Utf8Value();
  Napi::Array IRIDIUM = info[2].As<Napi::Array>();
  Napi::Array BUILDCTX = info[3].As<Napi::Array>();

  IRI_STORAGE::IRIContext ctx;
  ctx.debugger.tick = [&](std::string msg) {
    NAPI_tick.Call(env.Global(), {Napi::String::New(env, msg)});
  };

  ctx.debugger.tock = [&](std::string msg) {
    NAPI_tock.Call(env.Global(), {Napi::String::New(env, msg)});
  };
  ctx.storage.nodes.NULL_SEXP = IRI_GEN::NullSEXP::create(ctx, true);
  ctx.storage.nodes.NOP_SEXP = IRI_GEN::NOPSEXP::create(ctx);
  ctx.storage.nodes.NUBD_SEXP = IRI_GEN::JSNUBDSEXP::create(ctx);

  Napi::Object flagsObj = info[6].As<Napi::Object>();
  IRI_GEN::forEachPassFlag(ctx.flags,
                           [&](const std::string &name, bool &value) {
                             Napi::Value v = flagsObj.Get(name);
                             if (v.IsBoolean()) {
                               value = v.As<Napi::Boolean>().ToBoolean();
                             }
                           });

  ctx.debugger.tick("iri-forge-main");

  ctx.debugger.tick("iri-forge-parse");

  IRI_PARSE::IridiumParser parser(ctx);
  ctx.debugger.tick("iri-forge-parse-code");
  parser.initParseCTX(IRIDIUM);
  IRI_STORAGE::IRID root = parser.parse();
  ctx.debugger.tock("iri-forge-parse-code");

  ctx.debugger.tick("iri-forge-parse-buildContext");
  parser.parseBuildContexts(BUILDCTX);
  ctx.debugger.tock("iri-forge-parse-buildContext");

  ctx.debugger.tock("iri-forge-parse");

  ctx.debugger.tick("iri-forge-entrypoint");
  auto res = IRI_ENTRY::sharedEntrypoint(ctx, root, parser.iridiumBuildContext);
  ctx.debugger.tock("iri-forge-entrypoint");

  std::ostringstream oss;

  oss << "{";
  oss << "\"version\":" << "\"" << VERSION << "\",";
  oss << "\"absoluteFilePath\":" << "\"" << PATH << "\",";
  oss << "\"iridium\":";
  IRI_NODE(ctx, res).dump(oss, &ctx, true);
  oss << "}";

  std::string resultStr = oss.str();
  size_t length = resultStr.length();

  // 2. Allocate heap memory for the Buffer to "own"
  // We use new char[length] to ensure the buffer has its own copy
  char *resData = new char[length];
  std::memcpy(resData, resultStr.c_str(), length);

  // 3. Return the Napi::Buffer
  // The lambda at the end acts as a "finalizer" to clean up the memory
  return Napi::Buffer<char>::New(
      env, resData, length, [](Napi::Env env, char *data) { delete[] data; });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "execute"),
              Napi::Function::New(env, execute, "execute_iridium_cpp"));
  return exports;
}

NODE_API_MODULE(iridiumForge, Init)
