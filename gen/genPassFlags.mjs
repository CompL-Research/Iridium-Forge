import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";
import spec from "../PASSFLAGSSPEC.mjs";

function getISTDateTime(date = new Date()) {
  return date
    .toLocaleString("sv-SE", { timeZone: "Asia/Kolkata" })
    .replace("T", " ");
}

const fields = spec
  .map((f) => `  bool ${f.name} = ${f.default}; // ${f.desc}`)
  .join("\n");

const forEachBody = spec
  .map((f) => `  fn("${f.name}", flags.${f.name});`)
  .join("\n");

const headerFile = [
  `// Generated: ${getISTDateTime()}`,
  `#pragma once`,
  `#include <functional>`,
  `#include <string>`,
  ``,
  `namespace IRI_GEN {`,
  ``,
  `struct PassFlags {`,
  fields,
  `};`,
  ``,
  `// Lets embedders (e.g. the Node addon) populate PassFlags generically`,
  `// without hardcoding each field name at the call site.`,
  `inline void forEachPassFlag(`,
  `    PassFlags &flags,`,
  `    const std::function<void(const std::string &, bool &)> &fn) {`,
  forEachBody,
  `}`,
  ``,
  `} // namespace IRI_GEN`,
];

const tsFields = spec
  .map((f) => `  ${f.name}?: boolean; // ${f.desc}`)
  .join("\n");
const tsDefaults = spec.map((f) => `  ${f.name}: ${f.default},`).join("\n");
const tsSpecEntries = spec
  .map(
    (f) =>
      `  { name: "${f.name}", default: ${f.default}, desc: ${JSON.stringify(f.desc)} },`,
  )
  .join("\n");

const tsFile = [
  `/** Generated: ${getISTDateTime()} */`,
  ``,
  `export interface PassFlags {`,
  tsFields,
  `}`,
  ``,
  `export const DEFAULT_PASS_FLAGS: Required<PassFlags> = {`,
  tsDefaults,
  `};`,
  ``,
  `// Machine-readable spec, kept in sync with PassFlags/DEFAULT_PASS_FLAGS above.`,
  `// Lets consumers (e.g. the CLI argparser) build options generically`,
  `// without hardcoding each field name at the call site.`,
  `export const PASS_FLAGS_SPEC: Array<{`,
  `  name: keyof PassFlags;`,
  `  default: boolean;`,
  `  desc: string;`,
  `}> = [`,
  tsSpecEntries,
  `];`,
];

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const ROOT_DIR = path.resolve(__dirname, "..");

const genIncludeDir = path.join(ROOT_DIR, "include", "Generated");
fs.mkdirSync(genIncludeDir, { recursive: true });

fs.writeFileSync(
  path.join(genIncludeDir, "IridiumPassFlags.h"),
  headerFile.join("\n"),
);

fs.writeFileSync(
  path.join(ROOT_DIR, "PassFlags.generated.ts"),
  tsFile.join("\n"),
);

const externalForgeTypesPath = path.resolve(
  ROOT_DIR,
  "../../classes/builder/IridiumV2/ForgePasses.ts",
);
if (fs.existsSync(path.dirname(externalForgeTypesPath))) {
  fs.writeFileSync(externalForgeTypesPath, tsFile.join("\n"));
}

console.log("Successfully generated Iridium pass flags files.");
