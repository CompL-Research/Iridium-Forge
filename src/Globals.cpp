#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
#include "generated/IridiumTypes.h"
#include "Iridium/IridiumBuildContext.h"

IRIBUILDCONTEXT findReturnTarget(
  double localScope,
  std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext,
  std::vector<std::variant<LoopConfig, TryContext>> & intermediateContexts
) {
  if (localScope == -1) throw std::runtime_error("Failed to find return target!!!");
  assert(iridiumBuildContext.find(localScope) != iridiumBuildContext.end());
  auto & buildContext = iridiumBuildContext[localScope];

  if (buildContext->tryContext)
  {
    intermediateContexts.push_back(buildContext->tryContext.value());
  }

  if (buildContext->loopConfig)
  {
    intermediateContexts.push_back(buildContext->loopConfig.value());
  }

  auto & startBB = buildContext->BB[0];

  if (startBB->hasTopLevel())
  {
    if (intermediateContexts.size() > 0) throw std::runtime_error("Top level return not expected to be wrapped inside intermediate contexts");
    if (buildContext->isModule)
    {
      throw std::runtime_error("Expected async returns in module top level code...");
    }
  }

  if (startBB->hasClosureBoundary()) return buildContext;

  return findReturnTarget(buildContext->parent, iridiumBuildContext, intermediateContexts);
}

bool hasNode(const IRISEXP &currNode, const std::function<bool(const IRISEXP &)> &pred)
{
  if (!currNode)
    return false;

  if (pred(currNode))
  {
    return true;
  }

  for (const auto &e : currNode->args)
  {
    if (hasNode(e, pred))
    {
      return true;
    }
  }

  return false;
}

void insertAfter(std::vector<IRISEXP> &vec, IRISEXP after, const std::vector<IRISEXP> &toInsert)
{
  auto it = std::find(vec.begin(), vec.end(), after);
  if (it == vec.end())
    return;
  vec.insert(it + 1, toInsert.begin(), toInsert.end());
}

void insertAfter(std::vector<IRISEXP> &vec, IRISEXP after, IRISEXP toInsert)
{
  auto it = std::find(vec.begin(), vec.end(), after);
  if (it == vec.end())
    return;
  vec.insert(it + 1, toInsert);
}

void insertBefore(std::vector<IRISEXP> &vec, IRISEXP before, const std::vector<IRISEXP> &toInsert)
{
  auto it = std::find(vec.begin(), vec.end(), before);
  if (it == vec.end())
    return;
  vec.insert(it, toInsert.begin(), toInsert.end());
}

void insertBefore(std::vector<IRISEXP> &vec, IRISEXP before, IRISEXP toInsert)
{
  auto it = std::find(vec.begin(), vec.end(), before);
  if (it == vec.end())
    return;
  vec.insert(it, toInsert);
}


IRISEXP getBinding(std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, std::shared_ptr<BindingsSEXP> bindingsSEXP, std::string name, double lookupScope)
{
  if (lookupScope == -1) return NULL;
  // Check if a local is declared
  for (auto b : bindingsSEXP->getLocalBindings()->args)
  {
    auto bSEXP = std::dynamic_pointer_cast<EnvBindingSEXP>(b);
    if (!bSEXP) throw std::runtime_error("Expected EnvBindingSEXP");
    if (bSEXP->getScope() == lookupScope && bSEXP->getNAME() == name) return bSEXP;
  }

  for (auto b : bindingsSEXP->getRemoteBindings()->args)
  {
    auto rbSEXP = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b);
    if (!rbSEXP) throw std::runtime_error("Expected RemoteEnvBindingSEXP");
    auto bSEXP = resolveRemoteBinding(rbSEXP);
    if (bSEXP->getScope() == lookupScope && bSEXP->getNAME() == name) return rbSEXP;
  }

  assert(iridiumBuildContext.find(lookupScope) != iridiumBuildContext.end());
  auto & buildContext = iridiumBuildContext[lookupScope];

  double nextScope = buildContext->parent;

  // If the scope is an ArgInit context, bypass lookup of non-argument bindings to parent scope
  if (buildContext->isArgInitContext) {
    if (buildContext->argInitContextWhitelist.find(name) != buildContext->argInitContextWhitelist.end())
    {
      nextScope = buildContext->bypassParent;
    }
  }
  
  return getBinding(iridiumBuildContext, bindingsSEXP, name, nextScope);
}

IRISEXP resolveScopedLookup(IRISEXP fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, std::string name, double startScope, std::shared_ptr<BindingsSEXP> bindingsSEXP)
{  
  auto res = getBinding(iridiumBuildContext, bindingsSEXP, name, startScope);
  if (res) return res;
  auto parentScope = bindingsSEXP->getParentScope();
  if (parentScope == -1) throw std::runtime_error("Failed to resolve lookup");
  auto bbContainer = getBBContainerSEXPByScopeId(fileSEXP, findParentClosureScope(parentScope, iridiumBuildContext));
  auto parentBindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(bbContainer->getBindings());
  if (!parentBindingsSEXP) throw std::runtime_error("Expected BindingsSEXP");
  // Populate newly resolved remote bindings in the frame
  auto res1 = std::make_shared<RemoteEnvBindingSEXP>(resolveScopedLookup(fileSEXP, iridiumBuildContext, name, startScope, parentBindingsSEXP), false, bindingsSEXP->getRemoteBindings()->args.size());
  addToListSEXP(bindingsSEXP->getRemoteBindings(), res1);
  return res1;
  }

bool isGlobalBinding(IRISEXP fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, std::string name, double startScope, std::shared_ptr<BindingsSEXP> bindingsSEXP)
{
  auto res = getBinding(iridiumBuildContext, bindingsSEXP, name, startScope);
  if (res) return false;
  auto parentScope = bindingsSEXP->getParentScope();
  if (parentScope == -1) return true;
  auto bbContainer = getBBContainerSEXPByScopeId(fileSEXP, findParentClosureScope(parentScope, iridiumBuildContext));
  auto parentBindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(bbContainer->getBindings());
  if (!parentBindingsSEXP) throw std::runtime_error("Expected BindingsSEXP");
  return isGlobalBinding(fileSEXP, iridiumBuildContext, name, startScope, parentBindingsSEXP);
}

std::shared_ptr<BBContainerSEXP> getBBContainerSEXPByScopeId(IRISEXP file, double scopeIDX)
{
  auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(file);
  if (!fileSEXP)
    throw std::runtime_error("fileSexp is undefined");

  for (auto & bbContainer : fileSEXP->args)
  {
    if (auto currBBContainer = std::dynamic_pointer_cast<BBContainerSEXP>(bbContainer))
    {
      if (currBBContainer->getScopeIDX() == scopeIDX) return currBBContainer;
    }
  }
  throw std::runtime_error("BBContainerSEXP not found for idx " + std::to_string(scopeIDX));
}

double findParentClosureScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " + std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  auto &startBB = buildContext->BB.at(0);
  if (startBB->hasClosureBoundary() || startBB->hasTopLevel())
    return startingScope;
  return findParentClosureScope(buildContext->parent, iridiumBuildContext);
}

double getLexicalScope(double startingScope, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " + std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  return buildContext->parent;
}

void setClosureFlags(double flag, std::shared_ptr<BBContainerSEXP> bbContainer)
{
  bbContainer->setContainerFlagID(flag);
  switch ((int)flag)
  {
  case 0:
    throw std::runtime_error("Invalid closure flag");
  case 1:
    break;
  case 2:
    bbContainer->setPROTO();
    bbContainer->setNEW();
    break; // flags.push("PROTO", "NEW");
  case 3:
    bbContainer->setPROTO();
    bbContainer->setNEW();
    bbContainer->setSCALL();
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    bbContainer->setDERIVED();
    break; // flags.push("PROTO", "NEW", "SCALL", "SOBJ", "HOME", "DERIVED");
  case 4:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 5:
    bbContainer->setHOME();
    break; // flags.push("HOME");
  case 6:
    break;
  case 7:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 8:
    bbContainer->setHOME();
    break; // flags.push("HOME");
  case 9:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 10:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME")
  case 11:
    break;
  case 12:
    bbContainer->setSOBJ();
    bbContainer->setHOME();
    break; // flags.push("SOBJ", "HOME");
  default:
    throw std::runtime_error("expected a valid closure flag");
  }
}

void addToListSEXP(IRISEXP list, IRISEXP elementToAdd)
{
  auto listSEXP = std::dynamic_pointer_cast<ListSEXP>(list);
  assert(listSEXP && "Tried to add to a non-list");

  if (listSEXP->getTYPE() != "")
    assert(listSEXP->getTYPE() == elementToAdd->tag);

  listSEXP->args.push_back(elementToAdd);
}

bool sameKindFlag(std::shared_ptr<EnvBindingSEXP> binding, EnvBindingSEXPKindFlag kindFlag)
{
  if (kindFlag == EnvBindingSEXPKindFlag::JSLET && binding->hasJSLET())
    return true;
  if (kindFlag == EnvBindingSEXPKindFlag::JSCONST && binding->hasJSCONST())
    return true;
  if (kindFlag == EnvBindingSEXPKindFlag::JSVAR && binding->hasJSVAR())
    return true;
  if (kindFlag == EnvBindingSEXPKindFlag::JSARG && binding->hasJSARG())
    return true;
  if (kindFlag == EnvBindingSEXPKindFlag::JSRESTARG && binding->hasJSRESTARG())
    return true;
  return false;
}

std::shared_ptr<EnvBindingSEXP> resolveRemoteBinding(std::shared_ptr<RemoteEnvBindingSEXP> rbin)
{
  auto containedBinding = rbin->getParentReference();
  if (auto bin = std::dynamic_pointer_cast<EnvBindingSEXP>(containedBinding))
  {
    return bin;
  }
  else if (auto bin = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(containedBinding))
  {
    return resolveRemoteBinding(bin);
  }
  else
    throw std::runtime_error("resolveRemoteBinding failed");
}

bool hasBindingReference(std::shared_ptr<BindingsSEXP> bindingsSEXP, double idx, std::string name, EnvBindingSEXPKindFlag kindFlag, double localScope, double parentScope)
{
  auto localBindings = bindingsSEXP->getLocalBindings();
  auto remoteBindings = bindingsSEXP->getRemoteBindings();
  for (auto &b : localBindings->args)
  {
    if (auto bin = std::dynamic_pointer_cast<EnvBindingSEXP>(b))
    {
      if (
          bin->getIDX() == idx && bin->getNAME() == name && sameKindFlag(bin, kindFlag) && bin->getScope() == localScope && bin->getParentScope() == parentScope)
        return true;
    }
    else
      throw std::runtime_error("localBindings is non EnvBinding SEXP");
  }

  for (auto &b : remoteBindings->args)
  {
    if (auto rbin = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b))
    {
      auto bin = resolveRemoteBinding(rbin);
      if (
          bin->getIDX() == idx && bin->getNAME() == name && sameKindFlag(bin, kindFlag) && bin->getScope() == localScope && bin->getParentScope() == parentScope)
        return true;
    }
    else
      throw std::runtime_error("remotebindings is non RemoteEnvBinding SEXP");
  }

  return false;
}

BBSEXPFLAGS getBBFlag(std::shared_ptr<BBSEXP> b)
{
  if (b->hasTopLevel())
    return BBSEXPFLAGS::TopLevel;
  if (b->hasClosureBoundary())
    return BBSEXPFLAGS::ClosureBoundary;
  if (b->hasLexical())
    return BBSEXPFLAGS::Lexical;
  throw std::runtime_error("Failed to get a valid flag from a BBSEXP");
}

void setBBFlag(std::shared_ptr<BBSEXP> b, BBSEXPFLAGS flagToSet)
{
  b->unsetTopLevel();
  b->unsetClosureBoundary();
  b->unsetLexical();
  if (flagToSet == BBSEXPFLAGS::TopLevel)
    return b->setTopLevel();
  if (flagToSet == BBSEXPFLAGS::ClosureBoundary)
    return b->setClosureBoundary();
  if (flagToSet == BBSEXPFLAGS::Lexical)
    return b->setLexical();
  throw std::runtime_error("Impossible case reached setBBFlag");
}

int getRegularClosureFlag() { return 1; }
int getConstructorClosureFlag() { return 2; }
int getDerivedConstructorClosureFlag() { return 3; }
int getDerivedMethodClosureFlag() { return 4; }
int getPrivateMethodClosureFlag() { return 5; }
int getPropInitNoPrivateClosureFlag() { return 6; }
int getPropInitDerivedNoPrivateClosureFlag() { return 7; }
int getPropInitPrivateClosureFlag() { return 8; }
int getPropInitDerivedPrivateClosureFlag() { return 9; }
int getPrivateDerivedMethodClosureFlag() { return 10; }
int getStaticPropInitClosureFlag() { return 11; }
int getStaticPropInitDerivedClosureFlag() { return 12; }