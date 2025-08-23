#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
#include "generated/IridiumTypes.h"
#include "Iridium/IridiumBuildContext.h"

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
  if (b->hasTopLevel()) return BBSEXPFLAGS::TopLevel;
  if (b->hasClosureBoundary()) return BBSEXPFLAGS::ClosureBoundary;
  if (b->hasLexical()) return BBSEXPFLAGS::Lexical;
  throw std::runtime_error("Failed to get a valid flag from a BBSEXP");
}

void setBBFlag(std::shared_ptr<BBSEXP> b, BBSEXPFLAGS flagToSet) 
{
  b->unsetTopLevel();
  b->unsetClosureBoundary();
  b->unsetLexical();
  if (flagToSet == BBSEXPFLAGS::TopLevel) return b->setTopLevel();
  if (flagToSet == BBSEXPFLAGS::ClosureBoundary) return b->setClosureBoundary();
  if (flagToSet == BBSEXPFLAGS::Lexical) return b->setLexical();
  throw std::runtime_error("Impossible case reached setBBFlag");
}

