export default [
  {
    name: "dumpForgePasses",
    default: false,
    desc: "Dump Forge Passes",
  },
  {
    name: "constantProp",
    default: true,
    desc: "Constant propagation optimization pass",
  },
  {
    name: "tdz",
    default: true,
    desc: "TDZ-check elimination optimization pass (MTDZS)",
  },
  {
    name: "dce",
    default: true,
    desc: "Dead code elimination optimization pass",
  },
  {
    name: "effectProp",
    default: true,
    desc: "Effect propagation optimization pass",
  },
  {
    name: "dumpClosureTree",
    default: false,
    desc: "Dump the closure tree to stdout after optimization passes run",
  },
  {
    name: "dumpIrisInfo",
    default: false,
    desc: "Dump IRIS scope-resolution info to stdout after optimization passes run",
  },
  {
    name: "debugStorage",
    default: false,
    desc: "Dump IRIStorage pool memory diagnostics after optimization passes run",
  },
  {
    name: "pta",
    default: false,
    desc: "Run the Points-To Analysis solver after optimization passes run",
  },
  {
    name: "dumpCFG",
    default: false,
    desc: "Write each closure's CFG to out.cfg as DOT, and a PNG if dot is on PATH",
  },
  {
    name: "dumpPTA",
    default: false,
    desc: "Write the PTA solver trace to out.pta (requires --pta)",
  },
];
