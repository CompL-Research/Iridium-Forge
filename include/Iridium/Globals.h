#pragma once
#include <unordered_map>
#include <memory>

class IridiumSEXP;
class IridiumBuildContext;
class BBSEXP;
class BBContainerSEXP;
typedef std::shared_ptr<IridiumSEXP> IRISEXP;
typedef std::shared_ptr<IridiumBuildContext> IRIBUILDCONTEXT;

double findParentClosureScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &buildContext);
double getLexicalScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);
void setClosureFlags(double flag, std::shared_ptr<BBContainerSEXP> bbContainer);
void addBBToContainer(std::shared_ptr<BBSEXP> bb, std::shared_ptr<BBContainerSEXP> bbContainer);