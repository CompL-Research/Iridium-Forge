#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include "Storage/StringPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#ifdef DEBUG_TIME_IRIS
#include <chrono>
#endif
#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

#ifdef DEBUG_TIME_IRIS
double TIME_IRIS_INIT = 0;
double TIME_IRIS_GLOBAL_CHECK = 0;
double TIME_IRIS_RESOLUTION = 0;
double TIME_IRIS_COMMIT = 0;
// Starts the clock
#define TIME_START(name)                                                       \
  auto _prof_start_##name = std::chrono::high_resolution_clock::now()

// Ends the clock and adds to the global accumulator
#define TIME_END(name, accumulator)                                            \
  do {                                                                         \
    auto _prof_finish_##name = std::chrono::high_resolution_clock::now();      \
    std::chrono::duration<double, std::milli> _elapsed =                       \
        _prof_finish_##name - _prof_start_##name;                              \
    (accumulator) += _elapsed.count();                                         \
  } while (0)

// Prints the report
#define PRINT_PROFILING_REPORT()                                               \
  do {                                                                         \
    std::cout << "-- TIME INFO --" << std::endl;                               \
    std::cout << "IRIS-Init -- " << TIME_IRIS_INIT << "ms" << std::endl;       \
    std::cout << "  IRIS-Global Check -- " << TIME_IRIS_GLOBAL_CHECK << "ms"   \
              << std::endl;                                                    \
    std::cout << "  IRIS-Resolution -- " << TIME_IRIS_RESOLUTION << "ms"       \
              << std::endl;                                                    \
    std::cout << "  IRIS-Commit -- " << TIME_IRIS_COMMIT << "ms" << std::endl; \
  } while (0)
#else
#define TIME_START(name)
#define TIME_END(name, accumulator)
#define PRINT_PROFILING_REPORT()
#endif

inline IRID resolveBinding(std::shared_ptr<IRIS> iris, IridiumPool &pool,
                           bool isASW, StringID bindingName,
                           double lookupStartScope) {

  TIME_START(globalCheck);
  bool isGlobal = iris->isGlobal(bindingName, lookupStartScope);
  TIME_END(globalCheck, TIME_IRIS_GLOBAL_CHECK);
  if (isGlobal) {
    if (isASW)
      throw std::runtime_error(
          "Tried to mark a global binding as an ASW binding: " +
          std::string(pool.strings.get(bindingName)));
    return pool.getGlobalBindingSEXP(bindingName);
  } else {
    TIME_START(resolve);
    IRID resolvedBinding = iris->resolve(bindingName, lookupStartScope);
    TIME_END(resolve, TIME_IRIS_RESOLUTION);
    if (isASW) {
      if (pool[resolvedBinding].tag == IRI_GEN::EnvBinding) {
        EnvBindingSEXP b(resolvedBinding, pool);
        b.setASW();
      }
    }
    return resolvedBinding;
  }
};

inline void patchNode(std::shared_ptr<IRIS> iris, FileSupport file,
                      IridiumPool &pool, IRID node, double startScopeIDX,
                      BUILD_CTX &iridiumBuildContext,
                      size_t &unresolvedReferences) {
  if (unresolvedReferences == 0)
    return;
  auto args = pool.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    auto currTag = pool[currID].tag;

    if (currTag == IRI_GEN::ResolveEnvBinding) {
      ResolveEnvBindingSEXP uBinding(currID, pool);

      auto resolvedbiii = resolveBinding(iris, pool, uBinding.hasASW(),
                                         uBinding.getNAME(), startScopeIDX);

      pool.update_arg_inplace(node, i, resolvedbiii);
      unresolvedReferences--;
      continue;
    }

    if (unresolvedReferences == 0)
      return;

    patchNode(iris, file, pool, args[i], startScopeIDX, iridiumBuildContext,
              unresolvedReferences);
  }
}

void _8_RREBS(IridiumPool &pool, IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {
  TIME_START(init);
  std::shared_ptr<IRIS> iris =
      std::make_shared<IRIS>(pool, iridiumBuildContext, fileSEXP);
  TIME_END(init, TIME_IRIS_INIT);

  FileSupport file(fileSEXP, pool);
  size_t toPatch = 0;
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);

    auto currentScopeIDX = bbc.getScopeIDX();

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);
      auto bbScopeIDX = bb.getScopeIDX();
      for (auto [stmtID, stmtIDX] : bb.stmts()) {
        IRI_TAG currStmtTag = pool[stmtID].tag;
        if (currStmtTag == IRI_GEN::SiblingSpecialWrite) {
          SiblingSpecialWriteSEXP siblingSpecialWrite(stmtID, pool);
          ResolveEnvBindingSEXP lval(siblingSpecialWrite.getArg_LValTarget(),
                                     pool);

          auto writeStmt = EnvWriteSEXP::create(
              pool,
              resolveBinding(iris, pool, true, lval.getNAME(),
                             siblingSpecialWrite.getScopeIDX()),
              siblingSpecialWrite.getArg_RVal(),
              siblingSpecialWrite.hasSLOPPY(),
              siblingSpecialWrite.hasTHROWERR(), true,
              siblingSpecialWrite.getTHISINIT());

          pool.update_arg_inplace(bbID, stmtIDX, writeStmt);

          size_t unresolvedReferences = 0;
          IRI_HELPERS::countNodeOccurenceWithPredicate(
              siblingSpecialWrite.getArg_RVal(), &pool,
              [&](IRID arg) {
                return pool[arg].tag == IRI_GEN::ResolveEnvBinding;
              },
              unresolvedReferences);
          if (unresolvedReferences > 0) {
            patchNode(iris, file, pool, siblingSpecialWrite.getArg_RVal(), bbScopeIDX, iridiumBuildContext,
                      unresolvedReferences);
            assert(unresolvedReferences == 0);
          }
        } else {
          size_t unresolvedReferences = 0;
          IRI_HELPERS::countNodeOccurenceWithPredicate(
              stmtID, &pool,
              [&](IRID arg) {
                return pool[arg].tag == IRI_GEN::ResolveEnvBinding;
              },
              unresolvedReferences);
          if (unresolvedReferences > 0) {
            patchNode(iris, file, pool, stmtID, bbScopeIDX, iridiumBuildContext,
                      unresolvedReferences);
            assert(unresolvedReferences == 0);
          }
        }
      }
    }
  }

  auto topLevelScope = IRI_HELPERS::getTopLevelScope(pool, fileSEXP);
  for (auto [sImportID, _] : file.staticImports()) {
    StaticImportSEXP sImport(sImportID, pool);
    ResolveEnvBindingSEXP uBinding(sImport.getArg_StorageLocation(), pool);

    auto resolved = resolveBinding(iris, pool, uBinding.hasASW(),
                                   uBinding.getNAME(), topLevelScope);

    if (pool[resolved].tag != IRI_GEN::RemoteEnvBinding) {
      throw std::runtime_error(
          "Expected static imports to resolve to RemoteEnvBindings");
    }
    sImport.setArg_StorageLocation(resolved);
  }

  for (auto [sExportID, _] : file.staticExports()) {
    auto sExportTag = pool[sExportID].tag;
    if (sExportTag != IRI_GEN::LocalStaticExport)
      continue;
    LocalStaticExportSEXP sExport(sExportID, pool);
    ResolveEnvBindingSEXP uBinding(sExport.getArg_StorageLocation(), pool);
    auto resolved = resolveBinding(iris, pool, uBinding.hasASW(),
                                   uBinding.getNAME(), topLevelScope);
    if (pool[resolved].tag != IRI_GEN::RemoteEnvBinding) {
      throw std::runtime_error(
          "Expected static exports to resolve to RemoteEnvBindings");
    }
    sExport.setArg_StorageLocation(resolved);
  }

  TIME_START(commit);
  // Make sure to commit after any pass to keep the
  // underlying storage in sync.
  iris->commit();
  TIME_END(commit, TIME_IRIS_COMMIT);

  pool.iris = iris;
  // #ifdef DEBUG_TIME_IRIS
  // iris.dumpFullState();
  // #endif

  PRINT_PROFILING_REPORT();
}
} // namespace IRI_CORE_PASSES
