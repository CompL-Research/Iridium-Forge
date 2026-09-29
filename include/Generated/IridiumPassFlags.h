// Generated: 2026-09-29 19:25:58
#pragma once
#include <functional>
#include <string>

namespace IRI_GEN {

struct PassFlags {
  bool dumpForgePasses = false; // Dump Forge Passes
  bool constantProp = true; // Constant propagation optimization pass
  bool tdz = true; // TDZ-check elimination optimization pass (MTDZS)
  bool dce = true; // Dead code elimination optimization pass
  bool effectProp = true; // Effect propagation optimization pass
  bool dumpClosureTree = false; // Dump the closure tree to stdout after optimization passes run
  bool dumpIrisInfo = false; // Dump IRIS scope-resolution info to stdout after optimization passes run
  bool debugStorage = false; // Dump IRIStorage pool memory diagnostics after optimization passes run
  bool pta = false; // (WIP) Run the Points-To Analysis solver after optimization passes run
  bool dumpCFG = false; // Write each closure's CFG to out.cfg as DOT, and a PNG if dot is on PATH
  bool dumpPTA = false; // (WIP) Write the PTA solver trace to out.pta (requires --pta)
};

// Lets embedders (e.g. the Node addon) populate PassFlags generically
// without hardcoding each field name at the call site.
inline void forEachPassFlag(
    PassFlags &flags,
    const std::function<void(const std::string &, bool &)> &fn) {
  fn("dumpForgePasses", flags.dumpForgePasses);
  fn("constantProp", flags.constantProp);
  fn("tdz", flags.tdz);
  fn("dce", flags.dce);
  fn("effectProp", flags.effectProp);
  fn("dumpClosureTree", flags.dumpClosureTree);
  fn("dumpIrisInfo", flags.dumpIrisInfo);
  fn("debugStorage", flags.debugStorage);
  fn("pta", flags.pta);
  fn("dumpCFG", flags.dumpCFG);
  fn("dumpPTA", flags.dumpPTA);
}

} // namespace IRI_GEN