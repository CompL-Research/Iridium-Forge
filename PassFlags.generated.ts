/** Generated: 2026-09-14 22:47:27 */

export interface PassFlags {
  constantProp?: boolean; // Constant propagation optimization pass
  tdz?: boolean; // TDZ-check elimination optimization pass (MTDZS)
  dce?: boolean; // Dead code elimination optimization pass
  effectProp?: boolean; // Effect propagation optimization pass
  dumpClosureTree?: boolean; // Dump the closure tree to stdout after optimization passes run
  dumpIrisInfo?: boolean; // Dump IRIS scope-resolution info to stdout after optimization passes run
}

export const DEFAULT_PASS_FLAGS: Required<PassFlags> = {
  constantProp: true,
  tdz: true,
  dce: true,
  effectProp: true,
  dumpClosureTree: true,
  dumpIrisInfo: true,
};

// Machine-readable spec, kept in sync with PassFlags/DEFAULT_PASS_FLAGS above.
// Lets consumers (e.g. the CLI argparser) build options generically
// without hardcoding each field name at the call site.
export const PASS_FLAGS_SPEC: Array<{
  name: keyof PassFlags;
  default: boolean;
  desc: string;
}> = [
  { name: "constantProp", default: true, desc: "Constant propagation optimization pass" },
  { name: "tdz", default: true, desc: "TDZ-check elimination optimization pass (MTDZS)" },
  { name: "dce", default: true, desc: "Dead code elimination optimization pass" },
  { name: "effectProp", default: true, desc: "Effect propagation optimization pass" },
  { name: "dumpClosureTree", default: true, desc: "Dump the closure tree to stdout after optimization passes run" },
  { name: "dumpIrisInfo", default: true, desc: "Dump IRIS scope-resolution info to stdout after optimization passes run" },
];