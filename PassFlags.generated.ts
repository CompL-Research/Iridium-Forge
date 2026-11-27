/** Generated: 2026-11-26 23:32:20 */

export interface PassFlags {
  dumpForgePassesSEXP?: boolean; // Dump Forge Passes (Raw SEXP)
  dumpForgePasses?: boolean; // Dump Forge Passes (Pretty Printing)
  constantProp?: boolean; // Constant propagation optimization pass
  tdz?: boolean; // TDZ-check elimination optimization pass (MTDZS)
  copyProp?: boolean; // (To be deprecated soon) Copy propagation optimization pass
  dce?: boolean; // Dead code elimination optimization pass
  deadBindings?: boolean; // Delete bindings that nothing reads or writes
  effectProp?: boolean; // Effect propagation optimization pass
  dumpClosureTree?: boolean; // Dump the closure tree to stdout after optimization passes run
  dumpIrisInfo?: boolean; // Dump IRIS scope-resolution info to stdout after optimization passes run
  debugStorage?: boolean; // Dump IRIStorage pool memory diagnostics after optimization passes run
  pta?: boolean; // (WIP) Run the Points-To Analysis solver after optimization passes run
  dumpCFG?: boolean; // Write each closure's CFG to out.cfg as DOT, and a PNG if dot is on PATH
  dumpPTA?: boolean; // (WIP) Write the PTA solver trace to out.pta (requires --pta)
}

export const DEFAULT_PASS_FLAGS: Required<PassFlags> = {
  dumpForgePassesSEXP: false,
  dumpForgePasses: false,
  constantProp: true,
  tdz: true,
  copyProp: true,
  dce: true,
  deadBindings: true,
  effectProp: false,
  dumpClosureTree: false,
  dumpIrisInfo: false,
  debugStorage: false,
  pta: false,
  dumpCFG: false,
  dumpPTA: false,
};

// Machine-readable spec, kept in sync with PassFlags/DEFAULT_PASS_FLAGS above.
// Lets consumers (e.g. the CLI argparser) build options generically
// without hardcoding each field name at the call site.
export const PASS_FLAGS_SPEC: Array<{
  name: keyof PassFlags;
  default: boolean;
  desc: string;
}> = [
  { name: "dumpForgePassesSEXP", default: false, desc: "Dump Forge Passes (Raw SEXP)" },
  { name: "dumpForgePasses", default: false, desc: "Dump Forge Passes (Pretty Printing)" },
  { name: "constantProp", default: true, desc: "Constant propagation optimization pass" },
  { name: "tdz", default: true, desc: "TDZ-check elimination optimization pass (MTDZS)" },
  { name: "copyProp", default: true, desc: "(To be deprecated soon) Copy propagation optimization pass" },
  { name: "dce", default: true, desc: "Dead code elimination optimization pass" },
  { name: "deadBindings", default: true, desc: "Delete bindings that nothing reads or writes" },
  { name: "effectProp", default: false, desc: "Effect propagation optimization pass" },
  { name: "dumpClosureTree", default: false, desc: "Dump the closure tree to stdout after optimization passes run" },
  { name: "dumpIrisInfo", default: false, desc: "Dump IRIS scope-resolution info to stdout after optimization passes run" },
  { name: "debugStorage", default: false, desc: "Dump IRIStorage pool memory diagnostics after optimization passes run" },
  { name: "pta", default: false, desc: "(WIP) Run the Points-To Analysis solver after optimization passes run" },
  { name: "dumpCFG", default: false, desc: "Write each closure's CFG to out.cfg as DOT, and a PNG if dot is on PATH" },
  { name: "dumpPTA", default: false, desc: "(WIP) Write the PTA solver trace to out.pta (requires --pta)" },
];