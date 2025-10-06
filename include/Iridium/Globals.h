#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <set>
#include <functional>
#include <variant>

#define IRIDIUM_OUTPUTS_FOLDER "outputs/"

#define IRIDIUM_DEBUG_STATEMENTS 0
#define IRIDIUM_DUMP_INITIAL_SEXP 0
#define IRIDIUM_DUMP_FINAL_SEXP 0
#define IRIDIUM_DUMP_BEFORE_STACK_COLLAPSE 0
#define IRIDIUM_DUMP_AFTER_STACK_COLLAPSE 0
#define IRIDIUM_DUMP_AFTER_CORE_PASSES 0 // <- Right before Opt

#define PASSMGR_DEBUG 0

#define IRIDIUM_DUMP_INITIAL_CFG 0
#define IRIDIUM_DUMP_FINAL_CFG 0
#define IRIDIUM_DUMP_FLATTENING_CFG 0


#if IRIDIUM_DEBUG_STATEMENTS == 1
#define DBG(msg)                                                                       \
  do                                                                                   \
  {                                                                                    \
    std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " " << msg << std::endl; \
  } while (0)
#else
#define DBG(msg) \
  do             \
  {              \
  } while (0)
#endif

enum EnvBindingSEXPKindFlag
{
  JSARG,
  JSRESTARG,
  JSLET,
  JSCONST,
  JSVAR
};

enum BBSEXPFLAGS
{
  TopLevel,
  ClosureBoundary,
  Lexical
};

class IridiumSEXP;
class IridiumBuildContext;
class BBSEXP;
class BBContainerSEXP;

class BindingsSEXP;
class EnvBindingSEXP;
class RemoteEnvBindingSEXP;

typedef std::shared_ptr<IridiumSEXP> IRISEXP;
typedef std::shared_ptr<IridiumBuildContext> IRIBUILDCONTEXT;

struct SEXPPath
{
  double scopeIdx;
  double bbIdx;
  IRISEXP inst;
};

// This is a synchronous data structure, meaning that it needs to be refreshed after every transformation...
struct SymbolMetadata
{
  bool isTopLevelModuleBinding = false;
  std::shared_ptr<EnvBindingSEXP> binding;
  std::vector<SEXPPath> localWrites;
  std::vector<SEXPPath> localReads;
  std::vector<SEXPPath> remoteWrites;
  std::vector<SEXPPath> remoteReads;

  void dump(std::ostringstream &oss, bool compressed = false, int indent = 0) const;
};

using SymbolTable = std::unordered_map<IRISEXP, SymbolMetadata>;

template <typename T>
void prependArgs(std::vector<T> &target, const std::vector<T> &toPrepend)
{
  std::vector<T> newArgs;
  newArgs.reserve(toPrepend.size() + target.size());

  // first insert the new items
  newArgs.insert(newArgs.end(), toPrepend.begin(), toPrepend.end());

  // then append the old ones
  newArgs.insert(newArgs.end(),
                 std::make_move_iterator(target.begin()),
                 std::make_move_iterator(target.end()));

  target = std::move(newArgs);
}

template <typename T, typename U>
inline void prepend(std::vector<T> &vec, U &&value)
{
  vec.insert(vec.begin(), std::forward<U>(value));
}



struct LoopConfig;
struct TryContext;

IRIBUILDCONTEXT findReturnTarget(
    double localScope,
    std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext,
    std::vector<std::variant<LoopConfig, TryContext>> &intermediateContexts);

bool hasNode(const IRISEXP &currNode, const std::function<bool(const IRISEXP &)> &pred);
size_t countNode(const IRISEXP &currNode, const std::function<bool(const IRISEXP &)> &pred);
std::set<IRISEXP> getAllNodes(const IRISEXP &currNode, const std::function<bool(const IRISEXP &)> &pred, std::set<IRISEXP> & result);
void insertAfter(std::vector<IRISEXP> &vec, IRISEXP after, const std::vector<IRISEXP> &toInsert);
void insertAfter(std::vector<IRISEXP> &vec, IRISEXP after, IRISEXP toInsert);
void insertBefore(std::vector<IRISEXP> &vec, IRISEXP before, const std::vector<IRISEXP> &toInsert);
void insertBefore(std::vector<IRISEXP> &vec, IRISEXP before, IRISEXP toInsert);
IRISEXP getBinding(std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, std::shared_ptr<BindingsSEXP> bindingsSEXP, std::string name, double lookupScope);
bool hasScopePath(int currScope, int targetScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
int getBBScopeIDX(int bbIDX, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
IRISEXP resolveScopedLookup(IRISEXP fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, std::string name, double startScope, std::shared_ptr<BindingsSEXP> bindingsSEXP);
bool isGlobalBinding(IRISEXP fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, std::string name, double startScope, std::shared_ptr<BindingsSEXP> bindingsSEXP);
std::shared_ptr<BBContainerSEXP> getBBContainerSEXPByScopeId(IRISEXP file, double scopeIDX);
double findParentClosureScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &buildContext);
double getLexicalScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
bool isScopeReachable(double startingScope, double targetScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
IRIBUILDCONTEXT maybeGetEnclosingTryCatchContext(int currScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
void setClosureFlags(double flag, std::shared_ptr<BBContainerSEXP> bbContainer);
void addToListSEXP(IRISEXP list, IRISEXP elementToAdd);

std::shared_ptr<EnvBindingSEXP> resolveRemoteBinding(std::shared_ptr<RemoteEnvBindingSEXP> rbin);
bool hasBindingReference(std::shared_ptr<BindingsSEXP> bindingsSEXP, double idx, std::string name, EnvBindingSEXPKindFlag kindFlag, double localScope, double parentScope);
BBSEXPFLAGS getBBFlag(std::shared_ptr<BBSEXP> b);
void setBBFlag(std::shared_ptr<BBSEXP> b, BBSEXPFLAGS flagToSet);

void balanceStackFrame(std::shared_ptr<BBContainerSEXP> container, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
void rebalanceStackFrame(std::shared_ptr<BBContainerSEXP> container, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, bool isTopLevel);
int getRegularClosureFlag();
int getConstructorClosureFlag();
int getDerivedConstructorClosureFlag();
int getDerivedMethodClosureFlag();
int getPrivateMethodClosureFlag();
int getPropInitNoPrivateClosureFlag();
int getPropInitDerivedNoPrivateClosureFlag();
int getPropInitPrivateClosureFlag();
int getPropInitDerivedPrivateClosureFlag();
int getPrivateDerivedMethodClosureFlag();
int getStaticPropInitClosureFlag();
int getStaticPropInitDerivedClosureFlag();
