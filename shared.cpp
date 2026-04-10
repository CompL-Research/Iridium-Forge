#include "Iridium/entrypoint.h"
#include "generated/IridiumTypes.h"

IRISEXP sharedEntrypoint(IRISEXP sexp, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{
  DBG("Completed Initial Parsing");
#if IRIDIUM_DUMP_INITIAL_SEXP == 1
  {
    std::string savePath = IRIDIUM_OUTPUTS_FOLDER + std::string("Initial.iridump");
    std::filesystem::path dir = IRIDIUM_OUTPUTS_FOLDER;
    try
    {
      if (!std::filesystem::exists(dir))
      {
        std::filesystem::create_directories(dir);
      }

      std::ofstream outFile(savePath);
      if (!outFile)
      {
        throw std::runtime_error("Could not open file: " + savePath);
      }
      DBG("IRIDIUM_DUMP_INITIAL_SEXP");
      sexp->prettyPrint(outFile);
      outFile << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      std::cerr << "[IRIDIUM_DUMP_INITIAL_SEXP] Filesystem error: " << e.what() << '\n';
    }
  }
#endif

  DBG("Starting core passes");
  IRISEXP res = runCorePasses(sexp, iridiumBuildContext);
  auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(res);
  assert(fileSEXP);
  DBG("Completed core passes");

#if IRIDIUM_DUMP_AFTER_CORE_PASSES == 1
  {
    std::string savePath = IRIDIUM_OUTPUTS_FOLDER + std::string("BeforeOpt.iridump");
    std::filesystem::path dir = IRIDIUM_OUTPUTS_FOLDER;
    try
    {
      if (!std::filesystem::exists(dir))
      {
        std::filesystem::create_directories(dir);
      }

      std::ofstream outFile(savePath);
      if (!outFile)
      {
        throw std::runtime_error("Could not open file: " + savePath);
      }
      DBG("IRIDIUM_DUMP_AFTER_CORE_PASSES");
      res->prettyPrint(outFile);
      outFile << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      std::cerr << "[IRIDIUM_DUMP_AFTER_CORE_PASSES] Filesystem error: " << e.what() << '\n';
    }
  }
#endif

  DBG("Starting opt passes");
  res = runOptPasses(fileSEXP, iridiumBuildContext);
  DBG("Completed opt passes");

#if IRIDIUM_DUMP_FINAL_SEXP == 1
  {
    std::string savePath = IRIDIUM_OUTPUTS_FOLDER + std::string("Final.iridump");
    std::filesystem::path dir = IRIDIUM_OUTPUTS_FOLDER;
    try
    {
      if (!std::filesystem::exists(dir))
      {
        std::filesystem::create_directories(dir);
      }

      std::ofstream outFile(savePath);
      if (!outFile)
      {
        throw std::runtime_error("Could not open file: " + savePath);
      }
      DBG("IRIDIUM_DUMP_FINAL_SEXP");
      res->prettyPrint(outFile);
      outFile << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      std::cerr << "[IRIDIUM_DUMP_FINAL_SEXP] Filesystem error: " << e.what() << '\n';
    }
  }
#endif
  return res;
}
