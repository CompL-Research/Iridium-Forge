#include "Iridium/CorePasses/12_promoteAsyncReturns.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void markNamespaceImports(IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (auto listSEXP = std::dynamic_pointer_cast<ListSEXP>(currSEXP))
  {
    for (auto &e : listSEXP->args)
    {
      if (auto staticImportSEXP = std::dynamic_pointer_cast<StaticImportSEXP>(e))
      {
        auto literal = staticImportSEXP->getFIELD();
        auto binding = staticImportSEXP->getStorageLocation();
        if (literal == "*")
        {
          std::cerr << "[IRIDIUM::WARN] -- * imports are not fully supported yet..." << std::endl;
          
          // WIP FIX THIS... 
          // auto remoteBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(binding);
          // if (!remoteBinding)
          //   throw std::runtime_error("Expected a remote env binding SEXP here....: " + binding->tag);
          // remoteBinding->setNSIMPORT();
        }
      }
    }
  }

  for (auto & e : currSEXP->args) markNamespaceImports(e, iridiumBuildContext);
}