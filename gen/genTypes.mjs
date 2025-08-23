#!/usr/bin/env node
import fs from "fs";
import spec from "../TYPESPEC.mjs";
import { genExpandedCtor, genArgFuncs, genStringFlag, genVoidFlag, genBoolFlag, genDoubleFlag } from "./helpers.mjs";

function gen(spec) {
  const { tag } = spec;

  const className = `${tag}SEXP`;
  const base = "IridiumSEXP";

  const defConstructor = `
  ${className}() { this->tag = "${tag}"; }
  `;

  const defaultConstClash = (spec.args.length + spec.flags.string.length + spec.flags.void.length + spec.flags.bool.length + spec.flags.double.length) === 0;

  const methods = [
    genExpandedCtor(spec),
    ...spec.args.map((v, idx) => genArgFuncs(v, idx)),
    ...spec.flags.string.map(genStringFlag),
    ...spec.flags.void.map(genVoidFlag),
    ...spec.flags.bool.map(genBoolFlag),
    ...spec.flags.double.map(genDoubleFlag),
  ].join("\n");

  return `class ${className} : public ${base} {
private:
${!defaultConstClash ? defConstructor : "// default constructor and explicit one are the same, skipping..."}
public:
  static std::shared_ptr<${className}> generateFrom(IRISEXP obj) {
    assert(obj->tag == "${tag}");
    auto res = std::shared_ptr<${className}>(new ${className}());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }

${methods}
};
`;
}

function getISTDateTime(date = new Date()) {
  return date.toLocaleString("sv-SE", { timeZone: "Asia/Kolkata" }).replace("T", " ");
}




const typesFile = [
  `// Generated: ${getISTDateTime()}`,
  `#pragma once`,
  `#include "Iridium/Globals.h"`,
  `#include "Iridium/IridiumSEXP.h"`,
];

const typeCasts = [];

for (let s of spec) {
  typesFile.push(gen(s));
  typeCasts.push(`    if (tag == "${s.tag}") return ${s.tag}SEXP::generateFrom(obj);`);
}

const parseFile = [
  `// Generated: ${getISTDateTime()}`,
  `#pragma once`,
  `#include "Iridium/Globals.h"`,
  `#include "Iridium/IridiumSEXP.h"`,
  `#include "generated/IridiumTypes.h"`,
  `class ParseIridiumTypes {`,
  `public:`,
  `  static IRISEXP specialize(IRISEXP obj) {`,
  `    auto & tag = obj->tag;`,
  ...typeCasts,
  `    throw std::runtime_error("ParseIridiumTypes::specialize unhandled Tag: " + tag);`,
  `  }`,
  `};`,
];

fs.writeFileSync("../include/generated/IridiumTypes.h", typesFile.join("\n"));
fs.writeFileSync("../include/generated/ParseIridiumTypes.h", parseFile.join("\n"));