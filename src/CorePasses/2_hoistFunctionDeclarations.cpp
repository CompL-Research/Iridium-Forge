#include "Iridium/CorePasses/2_hoistFunctionDeclarations.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void funcDeclHandler(IRISEXP currSEXP, double currScope, std::unordered_map<double, std::set<std::shared_ptr<JSFuncDeclSEXP>>> & res)
{
  if (auto bb = std::dynamic_pointer_cast<BBSEXP>(currSEXP)) {
    currScope = bb->getScopeIDX();
  }

  for (int i = 0; i < currSEXP->args.size(); i++)
  {
    IRISEXP s = currSEXP->args[i];
    if (auto funcDeclSEXP = std::dynamic_pointer_cast<JSFuncDeclSEXP>(s)) {
      res[currScope].insert(funcDeclSEXP);
      currSEXP->args[i] = std::make_shared<NOPSEXP>();
    }
  }

  for (auto & e : currSEXP->args) funcDeclHandler(e, currScope, res);
}



void hoistFunctionDeclarations(IRISEXP container, std::unordered_map<int, IRIBUILDCONTEXT> & iridiumBuildContext)
{
  std::unordered_map<double, std::set<std::shared_ptr<JSFuncDeclSEXP>>> toHoist;

  funcDeclHandler(container, -1, toHoist);


  for (auto & e : toHoist) {
    auto & scope = e.first;
    auto & funDeclarations = e.second;

    auto & buildContext = iridiumBuildContext.at(scope);
    auto & targetBB = buildContext->BB[0];

    auto & args = targetBB->args;

    // Apparently allocating a new vector is faster??!! 
    // appending to a vector in cpp can be O (N + k), move all elements to right, also dynamic
    // memory allocation, a mess... 
    std::vector<IRISEXP> newArgs;
    newArgs.reserve(funDeclarations.size() + args.size());

    for (auto & f : funDeclarations) {
        newArgs.push_back(f);
    }

    newArgs.insert(newArgs.end(), args.begin(), args.end());

    targetBB->args = std::move(newArgs);
  }
  
}