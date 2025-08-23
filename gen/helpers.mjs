// Argument accessors
export function genArgFuncs(name, index) {
  return `
  void set${name}(const IRISEXP &obj) { this->args.at(${index}) = obj; }
  bool has${name}() { return ${index} < this->args.size(); }
  IRISEXP get${name}() const { return this->args.at(${index}); }
`;
}

// Void flag accessors
export function genVoidFlag(name) {
  return `
  void set${name}() { setFlag("${name}"); }
  void unset${name}() { removeFlag("${name}"); }
  bool has${name}() { return hasFlag("${name}"); }
`;
}

// String flag accessors
export function genStringFlag(name) {
  return `
  void set${name}(const std::string &value) { setFlag("${name}", value); }
  void unset${name}() { removeFlag("${name}"); }
  bool has${name}() { return hasFlag("${name}"); }
  std::string get${name}() { return getFlagString("${name}"); }
`;
}

// Bool flag accessors
export function genBoolFlag(name) {
  return `
  void set${name}(bool value) { setFlag("${name}", value); }
  void unset${name}() { removeFlag("${name}"); }
  bool has${name}() { return hasFlag("${name}"); }
  bool get${name}() { return getFlagBoolean("${name}"); }
`;
}

// Double flag accessors
export function genDoubleFlag(name) {
  return `
  void set${name}(double value) { setFlag("${name}", value); }
  void unset${name}() { removeFlag("${name}"); }
  bool has${name}() { return hasFlag("${name}"); }
  double get${name}() { return getFlagDouble("${name}"); }
`;
}

// Expanded constructor
export function genExpandedCtor(spec) {
  const params = [
    ...spec.args.map(a => `IRISEXP ${a}`),
    ...spec.flags.string.map(f => `std::string ${f}`),
    ...spec.flags.void.map(f => `bool ${f}`),
    ...spec.flags.bool.map(f => `bool ${f}`),
    ...spec.flags.double.map(f => `double ${f}`),
  ].join(", ");

  const body = [
    `    this->tag = "${spec.tag}";`,
    ...spec.args.map(a => `    this->args.push_back(${a});`),
    ...spec.flags.string.map(f => `    this->set${f}(${f});`),
    ...spec.flags.void.map(f => `    if (${f}) this->set${f}();`),
    ...spec.flags.bool.map(f => `    this->set${f}(${f});`),
    ...spec.flags.double.map(f => `    this->set${f}(${f});`),
  ].join("\n");

  return `
  ${spec.tag}SEXP(${params}) {
${body}
  }
`;
}