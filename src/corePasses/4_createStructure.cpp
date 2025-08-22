// //
// // Generate BBContainerSEXP to group compilation targets
// //
// generateBBContainerSEXP() {
//   const fileSexp = this.container;

//   // console.log("[Iridium] generateBBContainerSEXP -- entry");

//   // Group BBs into closure groups
//   let bbGroups: Map<number, BBContainerSEXP> = new Map();
//   if (!fileSexp) throw new Error("fileSexp is undefined");
//   for (let bb of fileSexp.args) {
//     if (isBBSEXP(bb)) {
//       let targetScopeIDX = this.findParentClosureScope(bb.getScopeIDX());

//       if (!bbGroups.has(targetScopeIDX)) {
//         const buildContext = IridiumBuildContext.CONTEXT_MAP.get(targetScopeIDX);
//         if (!buildContext) throw new Error("buildContext is undefined");
//         let startBB = buildContext.BB[0];
//         if (isBBSEXP(startBB)) {
//           let bbContainer = new BBContainerSEXP(startBB.idx, targetScopeIDX, []);
//           const currContext = IridiumBuildContext.CONTEXT_MAP.get(targetScopeIDX);
//           if (!currContext) throw new Error("currContext is undefined");
//           if (currContext.isAsync) bbContainer.setAsync();
//           if (currContext.isGenerator) bbContainer.setGenerator();
//           if (currContext.isStrict) bbContainer.setStrict();
//           bbContainer.setClosureFlags(currContext.kind);
//           bbGroups.set(targetScopeIDX, bbContainer);
//         } else throw new Error("Expected BBSEXP")
//       }
//       const bbGroup = bbGroups.get(targetScopeIDX);
//       if (!bbGroup) throw new Error("bbGroup is undefined");
//       bbGroup.addBB(bb);
//     }
//   }

//   // console.log("[Iridium] generateBBContainerSEXP -- closure grouping complete");

//   fileSexp.args = [...bbGroups.values()];

//   // const isSloppy = !IridiumBuildContext.CONTEXT_MAP.get(0).isStrict;
//   const buildContext = IridiumBuildContext.CONTEXT_MAP.get(0);
//   if (!buildContext) throw new Error("buildContext is undefined");
//   const isModule = buildContext.isModule;
//   const moduleRequests = new ListSEXP([]);
//   moduleRequests.setType("ModuleRequest");

//   const staticImports = new ListSEXP([]);
//   staticImports.setType("StaticImport");

//   const staticExports = new ListSEXP([]);

//   const staticStarExports = new ListSEXP([]);
//   staticStarExports.setType("StarExport");

//   // console.log("[Iridium] resolving -- numContainers: " + fileSexp.args.length);

//   let idx = 0;

//   for (let bbContainer of fileSexp.args) {
//     if (isBBContainerSEXP(bbContainer)) {
//       idx++;
//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: start`);
//       const bbContainerScopeIDX = bbContainer.getScopeIDX();
//       const buildContext = IridiumBuildContext.CONTEXT_MAP.get(bbContainerScopeIDX);
//       if (!buildContext) throw new Error("buildContext is undefined");
//       const bbContainerParentScopeIDX = buildContext.parent;
//       let isTopLevelContainer = bbContainerParentScopeIDX === -1;
//       if (isTopLevelContainer) {
//         bbContainer.setFlag("TopLevel");
//       }

//       // If this is a function, set the number of expected ECMAArgs
//       bbContainer.setECMAArgsLen(buildContext.ecmaArgs);

//       const sloppyDeclarations: Array<[string, JSEnvBindingFlags]> = [];
//       const hoistingInfo = new Map<number, Array<[Array<string>, JSEnvBindingFlags]>>();
//       const toRemove: Map<BBSEXP, Set<IridiumSEXP>> = new Map();
//       const staticModuleImports: Array<StaticImportSEXP> = [];

//       const implicitBindings: Set<JSImplicitBindingDeclarationSEXP> = new Set();

//       // Identify bindings
//       for (let bb of bbContainer.getBBs()) {
//         if (isBBSEXP(bb)) {
//           const localScope = bb.getScopeIDX();
//           if (!hoistingInfo.has(localScope)) hoistingInfo.set(localScope, new Array());

//           const parentClosureScope = this.findParentClosureScope(localScope);
//           if (!hoistingInfo.has(parentClosureScope)) hoistingInfo.set(parentClosureScope, new Array());

//           for (let stmt of bb.args) {

//             if (isJSImplicitBindingDeclarationSEXP(stmt)) {
//               implicitBindings.add(stmt);
//             }

//             if (isStarExportSEXP(stmt)) {
//               staticStarExports.args.push(stmt);
//               if (!toRemove.has(bb)) toRemove.set(bb, new Set());
//               const toRemoveList = toRemove.get(bb);
//               if (!toRemoveList) throw new Error("toRemoveList is undefined");
//               toRemoveList.add(stmt);
//             }

//             if (isStaticImportSEXP(stmt)) {
//               const localBinding = stmt.args[0];
//               if (isResolveEnvBindingSEXP(localBinding)) {
//                 staticModuleImports.push(stmt);
//               } else throw new Error("Expected static imported binding to be ResolveEnvBindingSEXP");
//               if (!toRemove.has(bb)) toRemove.set(bb, new Set());
//               const toRemoveList = toRemove.get(bb);
//               if (!toRemoveList) throw new Error("toRemoveList is undefined");
//               toRemoveList.add(stmt);
//             }

//             if (isLocalStaticExportSEXP(stmt)) {
//               const localBinding = stmt.args[0];
//               if (isResolveEnvBindingSEXP(localBinding)) {
//                 staticExports.args.push(stmt);
//               } else throw new Error("Expected static imported binding to be ResolveEnvBindingSEXP");
//               if (!toRemove.has(bb)) toRemove.set(bb, new Set());
//               const toRemoveList = toRemove.get(bb);
//               if (!toRemoveList) throw new Error("toRemoveList is undefined");
//               toRemoveList.add(stmt);
//             }

//             if (isNamedReexportSEXP(stmt)) {
//               staticExports.args.push(stmt);
//               if (!toRemove.has(bb)) toRemove.set(bb, new Set());
//               const toRemoveList = toRemove.get(bb);
//               if (!toRemoveList) throw new Error("toRemoveList is undefined");
//               toRemoveList.add(stmt);
//             }

//             // Function Declaration
//             if (isJSFuncDeclSEXP(stmt)) {
//               if (!isModule && localScope === 0) {
//                 // NADA
//               } else {
//                 stmt.reduceDecl();
//               }
//             }

//             // Declaration Statements
//             if (isJSExplicitBindingDeclarationSEXP(stmt) && stmt.isDecl()) {
//               let scopeToHoistTo: number;
//               let hoistingKind: JSEnvBindingFlags;

//               // Depending on the declaration kind they must be hoisted to different scopes
//               if (stmt.isLetDecl() || stmt.isConstDecl()) {
//                 scopeToHoistTo = localScope;
//                 if (stmt.isLetDecl()) hoistingKind = "JSLET";
//                 else hoistingKind = "JSCONST";
//               } else {
//                 scopeToHoistTo = parentClosureScope;
//                 hoistingKind = "JSVAR";
//               }

//               // Bindings created at this statement
//               let declarations = stmt.getDeclaredBindings();

//               // If the statement has no RVal, set it to undefined
//               if (!stmt.hasRVal()) {
//                 if (stmt.isVarDecl()) {
//                   // This statement must be removed
//                   if (!toRemove.has(bb)) toRemove.set(bb, new Set());
//                   const toRemoveList = toRemove.get(bb);
//                   if (!toRemoveList) throw new Error("toRemoveList is undefined");
//                   toRemoveList.add(stmt);
//                 } else if (stmt.isLetDecl()) {
//                   stmt.setRVal(new GlobalBindingSEXP("undefined"));
//                 } else {
//                   // Can happen when const destructuring stmts are present
//                   stmt.setRVal(new GlobalBindingSEXP("undefined"));
//                   // throw new Error("Const declaration without RVal");
//                 }
//               }

//               // This statement is no longer a declaration
//               stmt.reduceJSDecl();

//               if (scopeToHoistTo === 0 && !isModule) { // Top Level Global Declaration for script mode
//                 declarations.forEach(d => sloppyDeclarations.push([d, hoistingKind]));
//                 // stmt.markSloppyDecl();
//               } else {
//                 // Scope where these bindings must be initialized
//                 const hoistingInfoList = hoistingInfo.get(scopeToHoistTo);
//                 if (!hoistingInfoList) throw new Error("hoistingInfoList is undefined");
//                 hoistingInfoList.push([declarations, hoistingKind]);
//               }
//             }
//           }

//         } else throw new Error("Expected BBSEXP");
//       }

//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: identified bindings`);

//       // Remote uninitialized declarations
//       for (let [bb, toRemoveStmts] of toRemove) bb.args = bb.args.filter(e => !toRemoveStmts.has(e));

//       // Create Scope Descriptor
//       let i = 0, j = 0;
//       const bindingsSEXP = new BindingsSEXP(bbContainerParentScopeIDX);

//       // Scope bindings that will not be initialized implicitly by the codegen
//       const toSkipInit: Set<IridiumSEXP> = new Set();

//       // Add implicit bindings
//       for (const stmt of implicitBindings) {
//         const name = stmt.getName();
//         const kind = stmt.getKind();
//         const skipInit = stmt.isSkipInit();
//         const binding = new EnvBindingSEXP(i++, bbContainerScopeIDX, name, [[kind, null]], bbContainerScopeIDX, bbContainerParentScopeIDX);
//         bindingsSEXP.addLocalBinding(binding);
//         if (skipInit) toSkipInit.add(binding);
//       }

//       if (buildContext.moduleRequestMap) {
//         // Initialize Module Imports
//         if (bbContainerScopeIDX !== 0) throw new Error("Expected module imports only to be resolved for the top level container with scopeIDX 0");
//         for (let b of staticModuleImports) {
//           // Get the name of the binding we want...
//           const bb = b.args[0];
//           let bindingName: string;
//           if (isResolveEnvBindingSEXP(bb)) bindingName = bb.getBindingName();
//           else throw new Error("Expected the binding name to be resolveEnvBindingSEXP");

//           // Declare the binding in the bindings object
//           let binding = new EnvBindingSEXP(j++, bbContainerScopeIDX, bindingName, [["JSLET", null]], bbContainerScopeIDX, bbContainerParentScopeIDX);
//           let remoteBinding = new RemoteEnvBindingSEXP(binding, -1);
//           bindingsSEXP.addRemoteBinding(remoteBinding);
//           toSkipInit.add(remoteBinding);
//           staticImports.args.push(b);
//         }

//         for (let [, v] of buildContext.moduleRequestMap) {
//           moduleRequests.args.push(v);
//         }
//       }

//       for (let [localScope, bindings] of hoistingInfo) {
//         const currentContext = IridiumBuildContext.CONTEXT_MAP.get(localScope);
//         if (!currentContext) throw new Error("currentContext is undefined");

//         let parentScope = currentContext.parent;
//         const isTopLevel = currentContext.BB[0].isTopLevel();
//         for (let [bbs, flag] of bindings) {
//           for (let b of bbs) {
//             if (!bindingsSEXP.hasBindingReference(bbContainerScopeIDX, b, flag, localScope, parentScope)) {
//               if (isTopLevel) {
//                 let binding = new EnvBindingSEXP(j++, bbContainerScopeIDX, b, [[flag, null]], localScope, parentScope);
//                 let remoteBinding = new RemoteEnvBindingSEXP(binding, -1);
//                 bindingsSEXP.addRemoteBinding(remoteBinding);
//               } else {
//                 let binding = new EnvBindingSEXP(i++, bbContainerScopeIDX, b, [[flag, null]], localScope, parentScope);
//                 bindingsSEXP.addLocalBinding(binding);
//               }
//             }
//           }
//         }
//       }

//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: processed hoistingInfo`);

//       // 0 -> No Arguments Object
//       // 1 -> Mapped Arguments
//       // 2 -> Unmapped Arguments
//       if (buildContext.argumentsKind > 0) {
//         bbContainer.setArguments();
//         const bindingName = "arguments";
//         const flags: [JSEnvBindingFlags, null][] = [];
//         flags.push(["JSVAR", null]);
//         let binding = new EnvBindingSEXP(i++, bbContainerScopeIDX, bindingName, flags, bbContainerScopeIDX, bbContainerParentScopeIDX);
//         bindingsSEXP.addLocalBinding(binding);

//         let stmt = new JSImplicitBindingDeclarationSEXP(bindingName, "JSVAR", buildContext.argumentsKind === 1 ? 1 : 0);

//         let startBB = buildContext.BB[0];
//         startBB.args = [stmt, ...startBB.args]
//       }

//       // Adding function arguments, if required
//       const argsList: Array<EnvBindingSEXP> = [];
//       for (let k = 0; k < buildContext.args.length; k++) {
//         const flags: [JSEnvBindingFlags, null][] = [];
//         if (k + 1 === buildContext.args.length && buildContext.hasRestArgs) {
//           flags.push(["JSRESTARG", null]);
//         } else {
//           flags.push(["JSARG", null]);
//         }
//         let binding = new EnvBindingSEXP(k, bbContainerScopeIDX, buildContext.args[k], flags, bbContainerScopeIDX, bbContainerParentScopeIDX);

//         argsList.push(binding);
//       }

//       bindingsSEXP.getLocalBindings().args = [...argsList, ...bindingsSEXP.getLocalBindings().args]

//       // Add initializers to local scopes
//       const bindingsToInit = [...bindingsSEXP.getLocalBindings().args, ...bindingsSEXP.getRemoteBindings().args].filter(b => !toSkipInit.has(b));

//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: starting binding initialization`);

//       for (let binding of bindingsToInit) {
//         if (isEnvBindingSEXP(binding)) {
//           const bindingContext = IridiumBuildContext.CONTEXT_MAP.get(binding.getScope());
//           if (!bindingContext) throw new Error("bindingContext is undefined");
//           let startBB = bindingContext.BB[0];
//           let lValName = binding.getDeclaration();
//           let rVal;
//           if (binding.getKind() === "JSVAR") {
//             rVal = new EnvReadSEXP("undefined");
//           } else if (binding.getKind() === "JSLET" || binding.getKind() === "JSCONST") {
//             rVal = new JSNUBDSEXP();
//           } else {
//             continue;
//           }
//           startBB.args = [new EnvWriteSEXP(lValName, rVal, true, false), ...startBB.args];
//         } else if (isRemoteEnvBindingSEXP(binding)) {
//           if (isTopLevelContainer) {
//             let resolvedBinding = bindingsSEXP.resolveRemoteBinding(binding);
//             const resolvedBindingContext = IridiumBuildContext.CONTEXT_MAP.get(resolvedBinding.getScope());
//             if (!resolvedBindingContext) throw new Error("resolvedBindingContext is undefined");
//             let startBB = resolvedBindingContext.BB[0];
//             let lValName = resolvedBinding.getDeclaration();
//             let rVal;
//             if (resolvedBinding.getKind() === "JSVAR") {
//               rVal = new EnvReadSEXP("undefined");
//             } else if (resolvedBinding.getKind() === "JSLET" || resolvedBinding.getKind() === "JSCONST") {
//               rVal = new JSNUBDSEXP();
//             } else {
//               continue;
//             }
//             startBB.args = [new EnvWriteSEXP(lValName, rVal, true, false), ...startBB.args];
//           }
//         } else
//           throw new Error("Expected EnvBindingSEXP");
//       }

//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: binding initialization complete`);

//       const topLevelContext = IridiumBuildContext.CONTEXT_MAP.get(0);
//       if (!topLevelContext) throw new Error("topLevelContext is undefined");

//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: adding sloppy declaration init`);

//       for (let [name, kind] of sloppyDeclarations) {
//         let startBB = topLevelContext.BB[0];
//         let lValName = name;
//         let rVal;
//         if (kind === "JSVAR") {
//           rVal = new EnvReadSEXP("undefined");
//         } else {
//           rVal = new JSNUBDSEXP();
//         }
//         const val = new EnvWriteSEXP(lValName, rVal, true, false);
//         // val.markSloppyDecl();
//         // startBB.args = [val, ...startBB.args];  O(n + a)
//         startBB.args.unshift(val); //  O(n + a)
//       }

//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: JSSloppyDeclSEXP`);

//       for (let [name, kind] of sloppyDeclarations) {
//         if (kind === "JSLET" || kind === "JSCONST" || kind === "JSVAR") {
//           let startBB = topLevelContext.BB[0];
//           startBB.args = [new JSSloppyDeclSEXP(name, kind), ...startBB.args];
//         } else throw new Error("The declaration kind for SloppyDeclarationCheck is invalid!!!");
//       }

//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: setting bindingsSEXP`);
//       bbContainer.setBindings(bindingsSEXP);
//       // console.log(`[Iridium] building container ${idx}/${fileSexp.args.length}: done`);
//     } else throw new Error("Expected BBContainerSEXP");
//   }

//   // console.log("[Iridium] generateBBContainerSEXP -- bindings declaration and hoisting complete");

//   fileSexp.initializeModuleRequests(moduleRequests, staticImports, staticExports, staticStarExports);
// }

#include "Iridium/Passes/4_createStructure.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"

void groupIntoClosureGroups(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  std::unordered_map<double, std::shared_ptr<BBContainerSEXP>> bbGroups;

  for (auto &bbContainer : fileSexp->args)
  {
    auto bb = std::dynamic_pointer_cast<BBSEXP>(bbContainer);
    assert(bb && "Expected a BBSEXP");

    auto targetScopeIDX = findParentClosureScope(bb->getScopeIDX(), iridiumBuildContext);

    if (bbGroups.find(targetScopeIDX) == bbGroups.end()) {
      auto bbContainer = makeBBContainerSEXP(getLexicalScope(targetScopeIDX, iridiumBuildContext));
      auto & currContext = iridiumBuildContext[targetScopeIDX];

      if (currContext->isAsync) bbContainer->setASYNC();
      if (currContext->isGenerator) bbContainer->setGENERATOR();
      if (currContext->isStrict) bbContainer->setSTRICT();
      setClosureFlags(currContext->kind, bbContainer);
      bbGroups[targetScopeIDX] = bbContainer;
    }
    addBBToContainer(bb, bbGroups[targetScopeIDX]);
  }

  // std::vector<IRISEXP> newArgs;

  // for (auto & e : bbGroups) {
  //   newArgs.push_back(std::move(e.second));
  // }
  // fileSexp->args = std::move(newArgs);

  fileSexp->args.clear();
  fileSexp->args.reserve(bbGroups.size());
  for (auto & e : bbGroups) {
    fileSexp->args.push_back(std::move(e.second));
  }
}

void createStructure(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  groupIntoClosureGroups(fileSexp, iridiumBuildContext);
}