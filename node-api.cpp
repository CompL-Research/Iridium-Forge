#include <napi.h>
#include <iostream>
#include <sstream>

#include <zlib.h>
#include <msgpack.hpp>

#include "Iridium/entrypoint.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
#include "Iridium/Structure/BBContainerView.h"
#include "Iridium/Structure/FileView.h"

#include "shared.h"

static std::vector<uint8_t> gunzip(const void *data, size_t size);

//
// Arg 0 (buffer)  : Code Buffer
// Arg 1 (number)  : {0 = Only Core Passes} {1 = Level 1 Passes} {2 = Level 2 Passes} {3 = Level 3 Passes}
// Arg 2 (boolean)  : {true = return JSON} {false = return binary}
//
Napi::Value execute(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (!info[0].IsBuffer())
  {
    Napi::TypeError::New(env, "Buffer expected").ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  if (!info[1].IsNumber())
  {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  if (!info[2].IsBoolean())
  {
    Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  Napi::Buffer<uint8_t> buffer = info[0].As<Napi::Buffer<uint8_t>>();
  Napi::Number optFlag = info[1].As<Napi::Number>();
  bool returnJSON = info[2].As<Napi::Boolean>().Value();

  // Uncompress and load the
  size_t length = buffer.Length();
  char *data = reinterpret_cast<char *>(buffer.Data());
  std::vector<uint8_t> raw = gunzip(data, length);

  // FileData
  msgpack::object_handle oh =
      msgpack::unpack(reinterpret_cast<const char *>(raw.data()), raw.size());
  msgpack::object parsedObj = oh.get();

  msgpack::object VERSION = parsedObj.via.map.ptr[0].val;
  msgpack::object path = parsedObj.via.map.ptr[1].val;
  msgpack::object iridium = parsedObj.via.map.ptr[2].val;
  msgpack::object buildContext = parsedObj.via.map.ptr[3].val;

  // Ensure the data is in order before we start
  if (VERSION.type != msgpack::type::STR)
    Napi::Error::New(env, "[Forge] Expected 'version' to be STR!").ThrowAsJavaScriptException();

  if (path.type != msgpack::type::STR)
    Napi::Error::New(env, "[Forge] Expected 'path' to be STR!").ThrowAsJavaScriptException();

  if (iridium.type != msgpack::type::ARRAY)
    Napi::Error::New(env, "[Forge] Expected 'iridium' to be ARRAY!").ThrowAsJavaScriptException();

  if (buildContext.type != msgpack::type::ARRAY)
    Napi::Error::New(env, "[Forge] Expected 'buildContext' to be ARRAY!").ThrowAsJavaScriptException();


  // Read Iridium SEXP
  std::unordered_map<int, std::shared_ptr<BBSEXP>> bbIdxToSEXPMap;
  std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;
  auto sexp = parseSEXP(iridium, &bbIdxToSEXPMap);

  // Read Iridium Build Contexts
  parseBuildContexts(buildContext, bbIdxToSEXPMap, iridiumBuildContext);

  auto res = sharedEntrypoint(sexp, iridiumBuildContext);

  std::ostringstream oss;

  if (returnJSON)
  {
    oss << "{";
    oss << "\"version\":" << "\"" << std::string(VERSION.via.str.ptr, VERSION.via.str.size) << "\",";
    oss << "\"absoluteFilePath\":" << "\"" << std::string(path.via.str.ptr, path.via.str.size) << "\",";
    oss << "\"iridium\":";
    res->dump(oss, true);
    oss << "}";
  }
  else
  {
    Napi::Error::New(env, "Binary format is not yet handled, use legacy JSON format").ThrowAsJavaScriptException();
  }

  std::string str = oss.str(); // keep it alive
  size_t len = str.size();

  // Allocate raw buffer and copy data
  char *resData = new char[len];
  std::memcpy(resData, str.data(), len);

  // Create Node Buffer that owns `data` and cleans up with delete[]
  return Napi::Buffer<char>::New(
      env,
      resData,
      len,
      [](Napi::Env, char *data)
      {
        delete[] data;
      });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set("execute", Napi::Function::New(env, execute));
  return exports;
}

NODE_API_MODULE(iridiumForge, Init)

static std::vector<uint8_t> gunzip(const void *data, size_t size)
{
  if (size == 0)
    return {};

  z_stream strm{};
  if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK)
    throw std::runtime_error("inflateInit2 failed");

  strm.next_in = reinterpret_cast<Bytef *>(const_cast<void *>(data));
  strm.avail_in = static_cast<uInt>(size);

  std::vector<uint8_t> output;
  const size_t CHUNK = 1 << 15;
  std::vector<uint8_t> buf(CHUNK);

  int ret;
  do
  {
    strm.next_out = buf.data();
    strm.avail_out = static_cast<uInt>(buf.size());

    ret = inflate(&strm, Z_NO_FLUSH);
    if (ret != Z_OK && ret != Z_STREAM_END)
    {
      inflateEnd(&strm);
      throw std::runtime_error(std::string("inflate failed: ") + (strm.msg ? strm.msg : ""));
    }

    size_t produced = buf.size() - strm.avail_out;
    output.insert(output.end(), buf.begin(), buf.begin() + produced);
  } while (ret != Z_STREAM_END);

  inflateEnd(&strm);
  return output;
}
