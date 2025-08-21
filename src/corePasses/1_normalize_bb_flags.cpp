// // 
// // Ensure all BBs operating on the same scope has the same scope flag 
// // 
// normailzeBBFlags() {
//   for (let [scopeIdx, buildContext] of IridiumBuildContext.CONTEXT_MAP) {
//     let mainBBFlag: BBSEXPFlags = buildContext.BB[0].getBBFlag();
//     buildContext.BB.forEach(bb => {
//       if (isBBSEXP(bb)) {
//         bb.setBBFlag(mainBBFlag);
//       }
//     });
//   }
// }

// #include "Iridium/Globals.h"
// #include "Iridium/IridiumBuildContext.h"

// void normalizeBBFlags() {
//   for (auto & e : iridiumBuildContext) {
//     int scopeIdx = e.first;
//     IRIBUILDCONTEXT buildContext = e.second;
//     for (auto & bb : buildContext->BB) {
      
//     }

//   }
// }