#pragma once
#include <unordered_map>
#include <memory>

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

double findParentClosureScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &buildContext);
double getLexicalScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
void setClosureFlags(double flag, std::shared_ptr<BBContainerSEXP> bbContainer);
void addToListSEXP(IRISEXP list, IRISEXP elementToAdd);
class BindingsSEXP;
class EnvBindingSEXP;
class RemoteEnvBindingSEXP;
std::shared_ptr<EnvBindingSEXP> resolveRemoteBinding(std::shared_ptr<RemoteEnvBindingSEXP> rbin);
bool hasBindingReference(std::shared_ptr<BindingsSEXP> bindingsSEXP, double idx, std::string name, EnvBindingSEXPKindFlag kindFlag, double localScope, double parentScope);
