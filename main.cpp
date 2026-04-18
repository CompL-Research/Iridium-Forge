// #include <zlib.h>
// #include <msgpack.hpp>
// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <string>
// #include <cstdint>
// #include <optional>

// #include "Iridium/IridiumBuildContext.h"
// #include "Iridium/IridiumSEXP.h"

// #include "shared.h"

// std::string VERSION = "0.1a";

// static std::vector<uint8_t> read_all(const std::optional<std::string> &path);

// static std::vector<uint8_t> gunzip(const std::vector<uint8_t> &input);

// struct Options
// {
//   std::optional<std::string> input; // file path or "-" for stdin
//   bool print_summary = true;        // simple summary
//   bool dump_json = false;           // full JSON-like dump
// };

// static void print_usage(const char *argv0)
// {

//   auto & header = R"(
// ██╗██████╗ ██╗██████╗ ██╗██╗   ██╗███╗   ███╗
// ██║██╔══██╗██║██╔══██╗██║██║   ██║████╗ ████║
// ██║██████╔╝██║██║  ██║██║██║   ██║██╔████╔██║
// ██║██╔══██╗██║██║  ██║██║██║   ██║██║╚██╔╝██║
// ██║██║  ██║██║██████╔╝██║╚██████╔╝██║ ╚═╝ ██║
// ╚═╝╚═╝  ╚═╝╚═╝╚═════╝ ╚═╝ ╚═════╝ ╚═╝     ╚═╝

// )";

// std::cerr << header << std::endl << "Iridium Forge Version: " << VERSION << std::endl;
// std::cerr << "Usage: " << argv0 << " [-i <file|-]>\n" << "  -i, --in <path>   Read gzipped msgpack from file. Use '-' or omit to read from stdin.\n";
// }

// static Options parse_args(int argc, char **argv);

// int main(int argc, char **argv)
// {
//   try
//   {
//     auto opt = parse_args(argc, argv);
//     std::vector<uint8_t> gz = read_all(opt.input);
//     if (gz.empty())
//     {
//       std::cerr << "No input data (empty). Provide -i <file> or pipe data to stdin.\n";
//       return 1;
//     }
//     std::vector<uint8_t> raw = gunzip(gz);
//     msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char *>(raw.data()), raw.size());
//     msgpack::object obj = oh.get();

//     msgpack::object VERSION = obj.via.map.ptr[0].val;
//     msgpack::object path = obj.via.map.ptr[1].val;
//     msgpack::object iridium = obj.via.map.ptr[2].val;
//     msgpack::object buildContext = obj.via.map.ptr[3].val;

//     // Ensure the data is in order before we start
//     if (VERSION.type != msgpack::type::STR)
//       throw std::runtime_error("[Forge] Expected 'version' to be STR!");

//     if (path.type != msgpack::type::STR)
//       throw std::runtime_error("[Forge] Expected 'path' to be STR!");

//     if (iridium.type != msgpack::type::ARRAY)
//       throw std::runtime_error("[Forge] Expected 'iridium' to be ARRAY!");

//     if (buildContext.type != msgpack::type::ARRAY)
//       throw std::runtime_error("[Forge] Expected 'buildContext' to be ARRAY!");

//     // Read Iridium SEXP
//     std::unordered_map<int, std::shared_ptr<BBSEXP>> bbIdxToSEXPMap;
//     std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;
//     auto sexp = parseSEXP(iridium, &bbIdxToSEXPMap);

//     // Read Iridium Build Contexts
//     parseBuildContexts(buildContext, bbIdxToSEXPMap, iridiumBuildContext);

//     auto res = sharedEntrypoint(sexp, iridiumBuildContext);

//     std::ostringstream oss;
//     oss << "{";
//     oss << "\"version\":" << "\"" << std::string(VERSION.via.str.ptr, VERSION.via.str.size) << "\",";
//     oss << "\"absoluteFilePath\":" << "\"" << std::string(path.via.str.ptr, path.via.str.size) << "\",";
//     oss << "\"iridium\":";
//     res->dump(oss, true);
//     oss << "}";

//     std::cout << oss.str();
//     return 0;
//   }
//   catch (const std::exception &ex)
//   {
//     std::cerr << "Error: " << ex.what() << "\n";
//     return 1;
//   }
// }


// static std::vector<uint8_t> read_all(const std::optional<std::string> &path)
// {
//   std::istream *in = nullptr;
//   std::ifstream f;
//   if (path && *path != "-")
//   {
//     f.open(*path, std::ios::binary);
//     if (!f)
//     {
//       throw std::runtime_error("Failed to open input file: " + *path);
//     }
//     in = &f;
//   }
//   else
//   {
//     std::cin.sync_with_stdio(false);
//     std::cin.tie(nullptr);
//     in = &std::cin;
//   }
//   std::vector<uint8_t> data((std::istreambuf_iterator<char>(*in)), std::istreambuf_iterator<char>());
//   return data;
// }

// static std::vector<uint8_t> gunzip(const std::vector<uint8_t> &input)
// {
//   if (input.empty())
//     return {};

//   z_stream strm{};
//   // 16 + MAX_WBITS tells zlib to expect a gzip header/trailer.
//   if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK)
//   {
//     throw std::runtime_error("inflateInit2 failed");
//   }

//   strm.next_in = const_cast<Bytef *>(reinterpret_cast<const Bytef *>(input.data()));
//   strm.avail_in = static_cast<uInt>(input.size());

//   std::vector<uint8_t> output;
//   output.reserve(input.size() * 2); // heuristic

//   const size_t CHUNK = 1 << 15;
//   std::vector<uint8_t> buf(CHUNK);

//   int ret;
//   do
//   {
//     strm.next_out = reinterpret_cast<Bytef *>(buf.data());
//     strm.avail_out = static_cast<uInt>(buf.size());

//     ret = inflate(&strm, Z_NO_FLUSH);
//     if (ret != Z_OK && ret != Z_STREAM_END)
//     {
//       inflateEnd(&strm);
//       throw std::runtime_error(std::string("inflate failed: ") + (strm.msg ? strm.msg : ""));
//     }

//     size_t produced = buf.size() - strm.avail_out;
//     output.insert(output.end(), buf.begin(), buf.begin() + produced);
//   } while (ret != Z_STREAM_END);

//   inflateEnd(&strm);
//   return output;
// }

// static Options parse_args(int argc, char **argv)
// {
//   Options opt;
//   for (int i = 1; i < argc; ++i)
//   {
//     std::string a(argv[i]);
//     if ((a == "-i" || a == "--in") && i + 1 < argc)
//     {
//       opt.input = std::string(argv[++i]);
//     }
//     else if (a == "-h" || a == "--help")
//     {
//       print_usage(argv[0]);
//       std::exit(0);
//     }
//     else
//     {
//       std::cerr << "Unknown argument: " << a << "\n";
//       print_usage(argv[0]);
//       std::exit(2);
//     }
//   }
//   return opt;
// }


int main(int argc, char **argv)
{
  return 0;
}
