#pragma once
#include <unordered_map>
#include <memory>
#include <vector>

enum EnvBindingSEXPKindFlag
{
  JSARG,
  JSRESTARG,
  JSLET,
  JSCONST,
  JSVAR
};

class IridiumSEXP;
class IridiumBuildContext;
class BBSEXP;
class BBContainerSEXP;

typedef std::shared_ptr<IridiumSEXP> IRISEXP;
typedef std::shared_ptr<IridiumBuildContext> IRIBUILDCONTEXT;

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

double findParentClosureScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &buildContext);
double getLexicalScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
void setClosureFlags(double flag, std::shared_ptr<BBContainerSEXP> bbContainer);
void addToListSEXP(IRISEXP list, IRISEXP elementToAdd);
class BindingsSEXP;
class EnvBindingSEXP;
class RemoteEnvBindingSEXP;
std::shared_ptr<EnvBindingSEXP> resolveRemoteBinding(std::shared_ptr<RemoteEnvBindingSEXP> rbin);
bool hasBindingReference(std::shared_ptr<BindingsSEXP> bindingsSEXP, double idx, std::string name, EnvBindingSEXPKindFlag kindFlag, double localScope, double parentScope);
