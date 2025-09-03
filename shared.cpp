#include "Iridium/entrypoint.h"
#include "generated/IridiumTypes.h"
#include "Iridium/Structure/FileView.h"


IRISEXP sharedEntrypoint(IRISEXP sexp, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{
  IRISEXP res = runCorePasses(sexp, iridiumBuildContext);
  auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(res);
  assert(fileSEXP);

  // res->prettyPrint(std::cout);
  // std::cout << std::endl;
  
  res = runOptPasses(fileSEXP, iridiumBuildContext);

  // res->prettyPrint(std::cout);
  // std::cout << std::endl;

  return res;
}