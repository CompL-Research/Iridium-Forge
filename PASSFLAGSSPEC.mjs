export default [
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
    default: true,
    desc: "Dump the closure tree to stdout after optimization passes run",
  },
  {
    name: "dumpIrisInfo",
    default: true,
    desc: "Dump IRIS scope-resolution info to stdout after optimization passes run",
  },
];
