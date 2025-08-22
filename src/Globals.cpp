#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
#include "Iridium/IridiumTypes.h"
#include "Iridium/IridiumBuildContext.h"

// findParentClosureScope(localScope: number): number {
//   if (localScope === -1) return -1;
//   if (!IridiumBuildContext.CONTEXT_MAP.has(localScope)) throw new Error(`build context not found for scope: ${localScope}`)

//   let buildContext = IridiumBuildContext.CONTEXT_MAP.get(localScope);
//   if (!buildContext) throw new Error("buildContext is undefined");
//   let startBB = buildContext.BB[0];
//   if (startBB.isClosureBoundary() || startBB.isTopLevel()) return localScope;
//   else return this.findParentClosureScope(buildContext.parent);
// }

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
  switch ((int)flag) {
    case 0: throw std::runtime_error("Invalid closure flag");
    case 1: break;
    case 2: bbContainer->setPROTO(); bbContainer->setNEW(); break; // flags.push("PROTO", "NEW"); 
    case 3: bbContainer->setPROTO(); bbContainer->setNEW(); bbContainer->setSCALL(); bbContainer->setSOBJ(); bbContainer->setHOME(); bbContainer->setDERIVED(); break; // flags.push("PROTO", "NEW", "SCALL", "SOBJ", "HOME", "DERIVED");
    case 4: bbContainer->setSOBJ(); bbContainer->setHOME(); break; // flags.push("SOBJ", "HOME"); 
    case 5: bbContainer->setHOME(); break; // flags.push("HOME");
    case 6: break;
    case 7: bbContainer->setSOBJ(); bbContainer->setHOME(); break; // flags.push("SOBJ", "HOME");
    case 8: bbContainer->setHOME(); break; // flags.push("HOME");
    case 9: bbContainer->setSOBJ(); bbContainer->setHOME(); break; // flags.push("SOBJ", "HOME");
    case 10: bbContainer->setSOBJ(); bbContainer->setHOME(); break; // flags.push("SOBJ", "HOME")
    case 11: break;
    case 12: bbContainer->setSOBJ(); bbContainer->setHOME(); break; // flags.push("SOBJ", "HOME");
    default: throw std::runtime_error("expected a valid closure flag");
  }
}


void addToListSEXP(IRISEXP list, IRISEXP elementToAdd)
{
  auto listSEXP = std::dynamic_pointer_cast<ListSEXP>(list);
  assert(listSEXP && "Tried to add to a non-list");

  if (listSEXP->hasTYPE()) assert(listSEXP->getTYPE() == elementToAdd->tag);
  
  listSEXP->args.push_back(elementToAdd);
}