// // 
// // Hoist all function declarations to the top of their scope
// // 
// funcDeclHandler(currSEXP: IridiumSEXP, currScope: number, res: Map<number, Set<JSFuncDeclSEXP>>) {
//   if (isBBSEXP(currSEXP)) currScope = currSEXP.getScopeIDX();
//   for (let i = 0; i < currSEXP.args.length; i++) {
//     let s = currSEXP.args[i];
//     if (isJSFuncDeclSEXP(s)) {
//       if (!res.has(currScope)) res.set(currScope, new Set());
//       const funDeclList = res.get(currScope);
//       if (!funDeclList) throw new Error("funDeclList is undefined");
//       funDeclList.add(s);
//       currSEXP.args[i] = new NOPSEXP();
//     }
//   }
//   currSEXP.args.forEach(e => this.funcDeclHandler(e, currScope, res));
// }

// hoistFunctionDeclarations() {
//   let toHoist: Map<number, Set<JSFuncDeclSEXP>> = new Map();
//   if (!this.container) throw new Error("this.container is null");
//   this.funcDeclHandler(this.container, -1, toHoist);

//   for (let [scope, funDeclarations] of toHoist) {
//     const buildContext = IridiumBuildContext.CONTEXT_MAP.get(scope);
//     if (!buildContext) throw new Error("buildContext is undefined");
//     const targetBB = buildContext.BB[0];
//     targetBB.args = [...funDeclarations, ...targetBB.args];
//   }
// }
#include "Iridium/Passes/2_hoistFunctionDeclarations.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"

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
      currSEXP->args[i] = makeNOPSEXP();
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