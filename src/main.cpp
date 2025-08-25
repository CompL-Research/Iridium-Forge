#include <zlib.h>
#include <msgpack.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <filesystem> // C++17 or later

#include "Iridium/IridiumBuildContext.h"
#include "Iridium/IridiumSEXP.h"
#include "Iridium/Passes/1_normailzeBBFlags.h"
#include "Iridium/Passes/2_hoistFunctionDeclarations.h"
#include "Iridium/Passes/3_filterNops.h"
#include "Iridium/Passes/4_1_groupIntoClosureGroups.h"
#include "Iridium/Passes/4_2_populateModuleBindings.h"
#include "Iridium/Passes/4_3_populateImplicitBindings.h"
#include "Iridium/Passes/4_4_reduceFunctionDeclarations.h"
#include "Iridium/Passes/4_5_populateExplicitBindings.h"
#include "Iridium/Passes/4_6_addClosureArgsBindings.h"
#include "Iridium/Passes/5_initializeStackFrame.h"
#include "Iridium/Passes/6_patchHeritageConstructorSuperCalls.h"
#include "Iridium/Passes/7_reduceResolvePrivateEnvBindingSEXP.h"
#include "Iridium/Passes/8_reduceResolveEnvBindingSEXP.h"
#include "Iridium/Passes/9_resolveLambdaTargets.h"
#include "Iridium/Passes/10_resolveBreakAndContinueTargets.h"

void dumpPass(IRISEXP sexp, std::string passname)
{
  // PASS3
  std::ofstream outFile("outputs/" + passname + ".json");
  if (!outFile)
  {
    std::cerr << "Error: failed to save pass.\n";
    return;
  }
  sexp->dumpJSON(0, outFile);
}

// --------- Read all bytes from file or stdin ----------
static std::vector<uint8_t> read_all(const std::optional<std::string> &path)
{
  std::istream *in = nullptr;
  std::ifstream f;
  if (path && *path != "-")
  {
    f.open(*path, std::ios::binary);
    if (!f)
    {
      throw std::runtime_error("Failed to open input file: " + *path);
    }
    in = &f;
  }
  else
  {
    std::cin.sync_with_stdio(false);
    std::cin.tie(nullptr);
    in = &std::cin;
  }
  std::vector<uint8_t> data((std::istreambuf_iterator<char>(*in)), std::istreambuf_iterator<char>());
  return data;
}

// --------- Gunzip (gzip -> raw bytes) ----------
static std::vector<uint8_t> gunzip(const std::vector<uint8_t> &input)
{
  if (input.empty())
    return {};

  z_stream strm{};
  // 16 + MAX_WBITS tells zlib to expect a gzip header/trailer.
  if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK)
  {
    throw std::runtime_error("inflateInit2 failed");
  }

  strm.next_in = const_cast<Bytef *>(reinterpret_cast<const Bytef *>(input.data()));
  strm.avail_in = static_cast<uInt>(input.size());

  std::vector<uint8_t> output;
  output.reserve(input.size() * 2); // heuristic

  const size_t CHUNK = 1 << 15;
  std::vector<uint8_t> buf(CHUNK);

  int ret;
  do
  {
    strm.next_out = reinterpret_cast<Bytef *>(buf.data());
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

// --------- Pretty print as JSON-like text ----------
static void dump_msgpack(std::ostream &os, const msgpack::object &obj, int indent = 0);

static void indent(std::ostream &os, int n)
{
  for (int i = 0; i < n; ++i)
    os.put(' ');
}

static void dump_str_escaped(std::ostream &os, const std::string &s)
{
  os << '"';
  for (char c : s)
  {
    switch (c)
    {
    case '\\':
      os << "\\\\";
      break;
    case '"':
      os << "\\\"";
      break;
    case '\n':
      os << "\\n";
      break;
    case '\r':
      os << "\\r";
      break;
    case '\t':
      os << "\\t";
      break;
    default:
      os << c;
      break;
    }
  }
  os << '"';
}

static void dump_msgpack(std::ostream &os, const msgpack::object &obj, int indentLvl)
{
  switch (obj.type)
  {
  case msgpack::type::NIL:
    os << "null";
    break;
  case msgpack::type::BOOLEAN:
    os << (obj.via.boolean ? "true" : "false");
    break;
  case msgpack::type::POSITIVE_INTEGER:
    os << obj.via.u64;
    break;
  case msgpack::type::NEGATIVE_INTEGER:
    os << obj.via.i64;
    break;
  case msgpack::type::FLOAT32:
  case msgpack::type::FLOAT64:
    os << obj.via.f64;
    break;
  case msgpack::type::STR:
  {
    os << '"';
    os.write(obj.via.str.ptr, obj.via.str.size);
    os << '"';
    break;
  }
  case msgpack::type::BIN:
  {
    os << "\"<bin:" << obj.via.bin.size << ">\"";
    break;
  }
  case msgpack::type::ARRAY:
  {
    os << "[\n";
    for (uint32_t i = 0; i < obj.via.array.size; ++i)
    {
      indent(os, indentLvl + 2);
      dump_msgpack(os, obj.via.array.ptr[i], indentLvl + 2);
      if (i + 1 < obj.via.array.size)
        os << ",";
      os << "\n";
    }
    indent(os, indentLvl);
    os << "]";
    break;
  }
  case msgpack::type::MAP:
  {
    os << "{\n";
    for (uint32_t i = 0; i < obj.via.map.size; ++i)
    {
      const auto &kv = obj.via.map.ptr[i];
      indent(os, indentLvl + 2);
      // keys are often strings in your data
      if (kv.key.type == msgpack::type::STR)
      {
        std::string key(kv.key.via.str.ptr, kv.key.via.str.size);
        dump_str_escaped(os, key);
      }
      else
      {
        // fallback for non-string keys
        os << "\"";
        std::ostringstream tmp;
        tmp << kv.key;
        os << tmp.str() << "\"";
      }
      os << ": ";
      dump_msgpack(os, kv.val, indentLvl + 2);
      if (i + 1 < obj.via.map.size)
        os << ",";
      os << "\n";
    }
    indent(os, indentLvl);
    os << "}";
    break;
  }
  case msgpack::type::EXT:
  {
    os << "\"<ext:" << int(obj.via.ext.type()) << ", size:" << obj.via.ext.size << ">\"";
    break;
  }
  default:
    os << "\"<unknown>\"";
  }
}

// --------- CLI parsing ----------
struct Options
{
  std::optional<std::string> input; // file path or "-" for stdin
  bool print_summary = true;        // simple summary
  bool dump_json = false;           // full JSON-like dump
};

static void print_usage(const char *argv0)
{
  std::cerr << "Usage: " << argv0 << " [-i <file|-]> [--dump-json]\n"
            << "  -i, --in <path>   Read gzipped msgpack from file. Use '-' or omit to read from stdin.\n"
            << "  --dump-json       Print full JSON-like dump of decoded data.\n";
}

static Options parse_args(int argc, char **argv)
{
  Options opt;
  for (int i = 1; i < argc; ++i)
  {
    std::string a(argv[i]);
    if ((a == "-i" || a == "--in") && i + 1 < argc)
    {
      opt.input = std::string(argv[++i]);
    }
    else if (a == "--dump-json")
    {
      opt.dump_json = true;
    }
    else if (a == "-h" || a == "--help")
    {
      print_usage(argv[0]);
      std::exit(0);
    }
    else
    {
      std::cerr << "Unknown argument: " << a << "\n";
      print_usage(argv[0]);
      std::exit(2);
    }
  }
  return opt;
}

int main(int argc, char **argv)
{
  try
  {
    // Ensure the folder "outputs" exists
    std::filesystem::create_directories("outputs");

    auto opt = parse_args(argc, argv);

    // 1) Read gzipped bytes
    std::vector<uint8_t> gz = read_all(opt.input);

    if (gz.empty())
    {
      std::cerr << "No input data (empty). Provide -i <file> or pipe data to stdin.\n";
      return 1;
    }

    // 2) Gunzip
    std::vector<uint8_t> raw = gunzip(gz);

    // 3) Unpack MessagePack
    msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char *>(raw.data()), raw.size());
    msgpack::object obj = oh.get();

    msgpack::object iridiumObj = obj.via.map.ptr[5].val; // find "iridium" properly by key

    // Iridium SEXP
    std::unordered_map<int, std::shared_ptr<BBSEXP>> bbIdxToSEXPMap;
    std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;
    auto sexp = parseSEXP(iridiumObj, &bbIdxToSEXPMap);

    msgpack::object buildContexts = obj.via.map.ptr[4].val; // find "iridium" properly by key
    parseBuildContexts(buildContexts, bbIdxToSEXPMap, iridiumBuildContext);

    normalizeBBFlags(iridiumBuildContext);

    hoistFunctionDeclarations(sexp, iridiumBuildContext);

    filterNOPs(sexp);

    groupIntoClosureGroups(sexp, iridiumBuildContext);

    populateModuleBindings(sexp, iridiumBuildContext);

    populateImplicitBindings(sexp, iridiumBuildContext);

    reduceFunctionDeclarations(sexp, iridiumBuildContext);

    populateExplicitBindings(sexp, iridiumBuildContext);

    addClosureArgsBindings(sexp, iridiumBuildContext);

    initializeStackFrame(sexp, iridiumBuildContext);

    patchHeritageConstructorSuperCalls(sexp, iridiumBuildContext);

    reduceResolvePrivateEnvBindingSEXP(sexp, iridiumBuildContext);
    
    reduceResolveEnvBindingSEXP(sexp, iridiumBuildContext);
    
    resolveLambdaTargets(sexp, sexp, iridiumBuildContext, -1);

    std::cout << "[BEFORE: resolveBreakAndContinueTargets]" << std::endl;
    sexp->dump();
    resolveBreakAndContinueTargets(sexp, sexp, iridiumBuildContext);
    std::cout << "[AFTER: resolveBreakAndContinueTargets]" << std::endl;
    sexp->dump();




    return 0;

    // // 4) Optional: print summary and/or full dump
    // if (opt.dump_json)
    // {
    //   dump_msgpack(std::cout, obj, 0);
    //   std::cout << "\n";
    // }
    // else
    // {
    //   // Light summary: try to print "version" if present
    //   if (obj.type == msgpack::type::MAP)
    //   {
    //     const auto *ptr = obj.via.map.ptr;
    //     uint32_t n = obj.via.map.size;
    //     bool printedVersion = false;

    //     for (uint32_t i = 0; i < n; ++i)
    //     {
    //       const auto &kv = ptr[i];
    //       if (kv.key.type == msgpack::type::STR)
    //       {
    //         std::string k(kv.key.via.str.ptr, kv.key.via.str.size);
    //         if (k == "version")
    //         {
    //           std::cout << "version: ";
    //           dump_msgpack(std::cout, kv.val, 0);
    //           std::cout << "\n";
    //           printedVersion = true;
    //         }
    //       }
    //     }
    //     if (!printedVersion)
    //     {
    //       std::cout << "Decoded top-level map with " << n << " keys.\n";
    //     }
    //   }
    //   else
    //   {
    //     std::cout << "Decoded top-level type: " << int(obj.type) << "\n";
    //   }
    //   std::cout << "(Use --dump-json for full output)\n";
    // }

    return 0;
  }
  catch (const std::exception &ex)
  {
    std::cerr << "Error: " << ex.what() << "\n";
    return 1;
  }
}