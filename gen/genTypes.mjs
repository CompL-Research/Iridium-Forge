#!/usr/bin/env node
import fs from "fs";
import spec from "../TYPESPEC.mjs";
import { genExpandedCtor, genArgFuncs, genStringFlag, genVoidFlag, genBoolFlag, genDoubleFlag } from "./helpers.mjs";

function gen(spec) {
  const { tag } = spec;

  const className = `${tag}SEXP`;
  const base = "IridiumSEXP";

  const ctorFromObj = `
  ${className}(IRISEXP obj) {
    assert(obj->tag == "${tag}");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }
`;

  const methods = [
    genExpandedCtor(spec),
    ...spec.args.map((v, idx) => genArgFuncs(v, idx)),
    ...spec.flags.string.map(genStringFlag),
    ...spec.flags.void.map(genVoidFlag),
    ...spec.flags.bool.map(genBoolFlag),
    ...spec.flags.double.map(genDoubleFlag),
  ].join("\n");

  return `class ${className} : public ${base} {
public:
${ctorFromObj}
${methods}
};
`;
}

function getISTDateTime(date = new Date()) {
  return date.toLocaleString("sv-SE", { timeZone: "Asia/Kolkata" }).replace("T", " ");
}


const result = [
  `// Generated: ${getISTDateTime()}`,
  `#pragma once`,
  `#include "Iridium/Globals.h"`,
  `#include "Iridium/IridiumSEXP.h"`,
]

for (let s of spec) {
  result.push(gen(s));
}
const outFile = "../include/generated/IridiumTypes.h";
fs.writeFileSync(outFile, result.join("\n"));