#include <zlib.h>
#include <msgpack.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <filesystem>

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
#include "Iridium/Passes/11_decorateReturnTargets.h"
#include "Iridium/Passes/12_promoteAsyncReturns.h"
#include "Iridium/Passes/13_markNamespaceImports.h"
#include "Iridium/Passes/14_markSloppyWrites.h"
#include "Iridium/Passes/15_loosenWritestoASWs.h"
#include "Iridium/Passes/16_markDirectEvals.h"

std::string VERSION = "0.1a";

static std::vector<uint8_t> read_all(const std::optional<std::string> &path);

static std::vector<uint8_t> gunzip(const std::vector<uint8_t> &input);

void generateLegacyOutput(msgpack::object initiallyParsedObject, IRISEXP fileSEXP)
{
  // 

}

struct Options
{
  std::optional<std::string> input; // file path or "-" for stdin
  bool print_summary = true;        // simple summary
  bool dump_json = false;           // full JSON-like dump
};

static void print_usage(const char *argv0)
{

  auto & header = R"(
██╗██████╗ ██╗██████╗ ██╗██╗   ██╗███╗   ███╗
██║██╔══██╗██║██╔══██╗██║██║   ██║████╗ ████║
██║██████╔╝██║██║  ██║██║██║   ██║██╔████╔██║
██║██╔══██╗██║██║  ██║██║██║   ██║██║╚██╔╝██║
██║██║  ██║██║██████╔╝██║╚██████╔╝██║ ╚═╝ ██║
╚═╝╚═╝  ╚═╝╚═╝╚═════╝ ╚═╝ ╚═════╝ ╚═╝     ╚═╝                                

)";

std::cerr << header << std::endl << "Iridium Forge Version: " << VERSION << std::endl;
std::cerr << "Usage: " << argv0 << " [-i <file|-]>\n" << "  -i, --in <path>   Read gzipped msgpack from file. Use '-' or omit to read from stdin.\n";
}

static Options parse_args(int argc, char **argv);

int main(int argc, char **argv)
{
  try
  {
    auto opt = parse_args(argc, argv);
    std::vector<uint8_t> gz = read_all(opt.input);
    if (gz.empty())
    {
      std::cerr << "No input data (empty). Provide -i <file> or pipe data to stdin.\n";
      return 1;
    }
    std::vector<uint8_t> raw = gunzip(gz);
    msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char *>(raw.data()), raw.size());
    msgpack::object obj = oh.get();

    // Read Iridium SEXP
    msgpack::object iridiumObj = obj.via.map.ptr[5].val; 
    std::unordered_map<int, std::shared_ptr<BBSEXP>> bbIdxToSEXPMap;
    std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;
    auto sexp = parseSEXP(iridiumObj, &bbIdxToSEXPMap);

    // Read Iridium Build Contexts
    msgpack::object buildContexts = obj.via.map.ptr[4].val;
    parseBuildContexts(buildContexts, bbIdxToSEXPMap, iridiumBuildContext);

    // Run Core Passes
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

    resolveBreakAndContinueTargets(sexp, sexp, iridiumBuildContext);

    decorateReturnTargets(sexp, sexp, iridiumBuildContext);

    promoteAsyncReturns(sexp, iridiumBuildContext);

    markNamespaceImports(sexp, iridiumBuildContext);

    markSloppyWrites(sexp, iridiumBuildContext, -1);

    loosenWritestoASWs(sexp);

    markDirectEvals(sexp, iridiumBuildContext, -1);

    // Generate legacy output 
    generateLegacyOutput(iridiumObj, sexp);

    return 0;
  }
  catch (const std::exception &ex)
  {
    std::cerr << "Error: " << ex.what() << "\n";
    return 1;
  }
}


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
