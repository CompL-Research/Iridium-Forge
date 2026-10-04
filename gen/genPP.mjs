#!/usr/bin/env node
import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";
import specRaw from "../TYPESPEC.mjs";

//
// I'll skip transitional nodes for now ~ Meetesh
//
// STMT tags print as a statement line. AMP tags are expressions that can
// *also* appear as a bare statement, so they get both a statement printer
// (default: print the expression) and an RVal printer. RVAL tags only ever
// appear as an operand. Anything else (structural or transitional tags) falls
// back to ppGeneric, so the printer is total.
//
const stmtSpec = specRaw.filter((e) => e.meta === "STMT" || e.meta === "AMP");
const rvalSpec = specRaw.filter((e) => e.meta === "RVAL" || e.meta === "AMP");

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const ROOT_DIR = path.resolve(__dirname, "..");

// Configuration & CLI Flags
const args = process.argv.slice(2);
const FORCE_OVERWRITE = args.includes("--force") || args.includes("-f");
const DRY_RUN = args.includes("--dry-run");
const HELP = args.includes("--help") || args.includes("-h");

if (HELP) {
  console.log(`
Usage: node gen/genPP.mjs [options]

Options:
  --force, -f    Force overwrite existing handler .cpp files
  --dry-run      Print planned operations without writing any files
  --help, -h     Show this help message
`);
  process.exit(0);
}

const INCLUDE_PP_DIR = path.join(ROOT_DIR, "include", "Support", "PP");
const SRC_PP_DIR = path.join(ROOT_DIR, "src", "Support", "PP");
const SRC_PP_RVAL_DIR = path.join(SRC_PP_DIR, "RVal");

if (!DRY_RUN) {
  fs.mkdirSync(INCLUDE_PP_DIR, { recursive: true });
  fs.mkdirSync(SRC_PP_DIR, { recursive: true });
  fs.mkdirSync(SRC_PP_RVAL_DIR, { recursive: true });
}

function getISTDateTime(date = new Date()) {
  return date.toLocaleString("sv-SE", { timeZone: "Asia/Kolkata" }).replace("T", " ");
}

function writeGenerated(filePath, contents) {
  if (!DRY_RUN) {
    fs.writeFileSync(filePath, contents, "utf8");
  }
  console.log(`[GENERATE] ${path.relative(ROOT_DIR, filePath)}`);
}

/**
 * Generate documentation/example comments for a tag based on its args and flags.
 */
function genTagDoc(item) {
  const lines = [];
  lines.push(` * AST Tag: ${item.tag}`);
  lines.push(` * Meta:    ${item.meta || "(none)"}`);

  if (item.args && item.args.length > 0) {
    lines.push(` * Arguments:`);
    item.args.forEach((arg, idx) => {
      lines.push(` *   [${idx}] IRID ${arg} -> sexp.getArg_${arg}()`);
    });
  } else {
    lines.push(` * Arguments: (none)`);
  }

  const flags = item.flags || {};
  const hasFlags = (flags.string?.length || 0) + (flags.void?.length || 0) +
                   (flags.bool?.length || 0) + (flags.double?.length || 0) > 0;

  if (hasFlags) {
    lines.push(` * Flags:`);
    (flags.void || []).forEach(f => lines.push(` *   - void   ${f} -> pp.on(node, IRI_GEN::${item.tag}SEXP::FLAG_IDX_${f})`));
    (flags.bool || []).forEach(f => lines.push(` *   - bool   ${f} -> pp.on(node, IRI_GEN::${item.tag}SEXP::FLAG_IDX_${f})`));
    (flags.string || []).forEach(f => lines.push(` *   - string ${f} -> sexp.get${f}()`));
    (flags.double || []).forEach(f => lines.push(` *   - double ${f} -> sexp.get${f}()`));
  } else {
    lines.push(` * Flags: (none)`);
  }
  return lines.join("\n");
}

function genSampleCalls(item) {
  const sampleCalls = [];
  if (item.args && item.args.length > 0) {
    item.args.forEach((a) => {
      sampleCalls.push(`  // std::string ${a} = ppOperand(pp, sexp.getArg_${a}());`);
    });
  }
  const flags = item.flags || {};
  [...(flags.void || []), ...(flags.bool || [])].forEach(f =>
    sampleCalls.push(`  // bool ${f} = pp.on(node, IRI_GEN::${item.tag}SEXP::FLAG_IDX_${f});`));
  (flags.string || []).forEach(f => sampleCalls.push(`  // if (sexp.has${f}()) { std::string_view str_${f} = pp.str(sexp.get${f}()); }`));
  (flags.double || []).forEach(f => sampleCalls.push(`  // if (sexp.has${f}()) { double dbl_${f} = sexp.get${f}(); }`));
  return sampleCalls;
}

/**
 * Generate a statement printer stub. AMP tags default to printing the
 * expression itself (e.g. `foo()` used as a bare statement).
 */
function genStmtHandlerCpp(item) {
  const doc = genTagDoc(item);
  const tag = item.tag;

  if (item.meta === "AMP") {
    return `// Generated Stub for IRI_TAG::${tag}
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
${doc}
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string print${tag}(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
`;
  }

  const sampleCalls = genSampleCalls(item);
  return `// Generated Stub for IRI_TAG::${tag}
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
${doc}
 */
std::string print${tag}(const PPContext &pp, IRID node) {
  // IRI_GEN::${tag}SEXP sexp(node, pp.ctx);

  // === TODO : ${tag} ===
${sampleCalls.length > 0 ? sampleCalls.join("\n") + "\n" : ""}
  return ppGeneric(pp, node);
}

} // namespace IRI_STRUCTURAL
`;
}

/**
 * Generate an RVal printer stub: renders a node used as an operand.
 */
function genRValHandlerCpp(item) {
  const doc = genTagDoc(item);
  const tag = item.tag;
  const sampleCalls = genSampleCalls(item);

  return `// Generated Stub for IRI_TAG::${tag} (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
${doc}
 */
std::string print${tag}RVal(const PPContext &pp, IRID node) {
  // IRI_GEN::${tag}SEXP sexp(node, pp.ctx);

  // === TODO : ${tag} ===
${sampleCalls.length > 0 ? sampleCalls.join("\n") + "\n" : ""}
  return ppGeneric(pp, node);
}

} // namespace IRI_STRUCTURAL
`;
}

/**
 * 1. Generate individual Handler C++ files (if not exists or FORCE_OVERWRITE)
 */
let createdCount = 0;
let skippedCount = 0;

function emitStub(dir, filename, contents) {
  const filePath = path.join(dir, filename);
  const exists = fs.existsSync(filePath);

  if (!exists || FORCE_OVERWRITE) {
    if (!DRY_RUN) {
      fs.writeFileSync(filePath, contents, "utf8");
    }
    console.log(`[${exists ? "OVERWRITE" : "CREATE"}] ${path.relative(ROOT_DIR, filePath)}`);
    createdCount++;
  } else {
    skippedCount++;
  }
}

for (const item of stmtSpec) {
  emitStub(SRC_PP_DIR, `Print${item.tag}.cpp`, genStmtHandlerCpp(item));
}

for (const item of rvalSpec) {
  emitStub(SRC_PP_RVAL_DIR, `Print${item.tag}RVal.cpp`, genRValHandlerCpp(item));
}

/**
 * 2. Generate include/Support/PP/PPContext.hpp
 */
const ppContextHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Storage/IRIContext.h"
#include "Storage/IridiumSEXP.h"
#include "Generated/IridiumEnums.h"
#include <span>
#include <string_view>
#include <variant>

namespace IRI_STRUCTURAL {

using IRI_STORAGE::IRID;

/**
 * Bundles the state for pretty printing (PP).
 * Allows easy evolution of PP options without changing individual handler signatures.
 */
struct PPContext {
  IRI_STORAGE::IRIContext &ctx;

  // Append "  ; #<IRID>" to every statement line.
  bool showIds = false;

  explicit PPContext(IRI_STORAGE::IRIContext &c) : ctx(c) {}

  // Convenient accessors
  inline IRI_GEN::IRI_TAG tagOf(IRID n) const { return IRI_NODE(ctx, n).tag; }
  inline std::span<const IRID> args(IRID n) const {
    return ctx.storage.nodes.get_args_view(n);
  }
  inline const IRI_STORAGE::FlagValue &flag(IRID n, uint32_t idx) const {
    return ctx.storage.nodes.get_flags(n)[idx];
  }
  // A void flag is set when present; a bool flag only when present and true.
  inline bool on(IRID n, uint32_t idx) const {
    const auto &v = flag(n, idx);
    if (std::holds_alternative<std::monostate>(v))
      return false;
    if (const bool *b = std::get_if<bool>(&v))
      return *b;
    return true;
  }
  inline std::string_view str(StringID s) const {
    return ctx.storage.strings.get(s);
  }
};

} // namespace IRI_STRUCTURAL
`;

writeGenerated(path.join(INCLUDE_PP_DIR, "PPContext.hpp"), ppContextHpp);

/**
 * 3. Generate include/Support/PP/PPHandlers.hpp (statement printer declarations)
 */
const stmtHeaderDecls = stmtSpec.map(item => {
  return `std::string print${item.tag}(const PPContext &pp, IRID node);`;
}).join("\n");

const ppHandlersHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PP/PPContext.hpp"
#include <string>

namespace IRI_STRUCTURAL {

// ============================================================================
// PP Statement Printer Declarations
//
// Each of these renders one statement as a single line (no indentation).
// ============================================================================
${stmtHeaderDecls}

} // namespace IRI_STRUCTURAL
`;

writeGenerated(path.join(INCLUDE_PP_DIR, "PPHandlers.hpp"), ppHandlersHpp);

/**
 * 4. Generate include/Support/PP/PPDispatch.hpp (statement dispatcher)
 */
const stmtSwitchCases = stmtSpec.map(item => {
  return `    case IRI_GEN::IRI_TAG::${item.tag}:\n` +
         `      return print${item.tag}(pp, node);`;
}).join("\n");

const ppDispatchHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * Renders a statement to its corresponding tag printer. Tags without a
 * printer (structural or transitional ones) fall back to ppGeneric.
 */
inline std::string ppStatement(const PPContext &pp, IRID node) {
  switch (pp.tagOf(node)) {
${stmtSwitchCases}
    default:
      return ppGeneric(pp, node);
  }
}

} // namespace IRI_STRUCTURAL
`;

writeGenerated(path.join(INCLUDE_PP_DIR, "PPDispatch.hpp"), ppDispatchHpp);

/**
 * 5. Generate include/Support/PP/PPRVALHandlers.hpp (RVal printer declarations)
 */
const rvalHeaderDecls = rvalSpec.map(item => {
  return `std::string print${item.tag}RVal(const PPContext &pp, IRID node);`;
}).join("\n");

const ppRValHandlersHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PP/PPContext.hpp"
#include <string>

namespace IRI_STRUCTURAL {

// ============================================================================
// PP RVal Printer Declarations
//
// Each of these renders a given IRID (an AMP or RVAL tag) as an expression.
// ============================================================================
${rvalHeaderDecls}

} // namespace IRI_STRUCTURAL
`;

writeGenerated(path.join(INCLUDE_PP_DIR, "PPRVALHandlers.hpp"), ppRValHandlersHpp);

/**
 * 6. Generate include/Support/PP/PPRVALDispatch.hpp (RVal dispatcher)
 */
const rvalSwitchCases = rvalSpec.map(item => {
  return `    case IRI_GEN::IRI_TAG::${item.tag}:\n` +
         `      return print${item.tag}RVal(pp, node);`;
}).join("\n");

const ppRValDispatchHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PP.hpp"
#include "Support/PP/PPRVALHandlers.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * Renders IRID node as an expression. Tags without a printer (structural or
 * transitional ones) fall back to ppGeneric.
 */
inline std::string ppRVal(const PPContext &pp, IRID node) {
  switch (pp.tagOf(node)) {
${rvalSwitchCases}
    default:
      return ppGeneric(pp, node);
  }
}

} // namespace IRI_STRUCTURAL
`;

writeGenerated(path.join(INCLUDE_PP_DIR, "PPRVALDispatch.hpp"), ppRValDispatchHpp);

console.log(`\nPP Code Generation Complete!`);
console.log(`- Handler stubs created: ${createdCount}`);
console.log(`- Existing handlers preserved: ${skippedCount}`);
console.log(`- Statement tags supported: ${stmtSpec.length}`);
console.log(`- RVal tags supported: ${rvalSpec.length}`);
