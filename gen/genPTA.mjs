#!/usr/bin/env node
import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";
import specRaw from "../TYPESPEC.mjs";

const spec = specRaw.filter(e => e.meta === "STMT" || e.meta === "AMP");

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
Usage: node gen/genPTA.mjs [options]

Options:
  --force, -f    Force overwrite existing handler .cpp files in src/Support/PTA/
  --dry-run      Print planned operations without writing any files
  --help, -h     Show this help message
`);
  process.exit(0);
}

const INCLUDE_PTA_DIR = path.join(ROOT_DIR, "include", "Support", "PTA");
const SRC_PTA_DIR = path.join(ROOT_DIR, "src", "Support", "PTA");

// Ensure target directories exist
if (!DRY_RUN) {
  fs.mkdirSync(INCLUDE_PTA_DIR, { recursive: true });
  fs.mkdirSync(SRC_PTA_DIR, { recursive: true });
}

function getISTDateTime(date = new Date()) {
  return date.toLocaleString("sv-SE", { timeZone: "Asia/Kolkata" }).replace("T", " ");
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
    (flags.void || []).forEach(f => lines.push(` *   - void   ${f} -> sexp.has${f}()`));
    (flags.bool || []).forEach(f => lines.push(` *   - bool   ${f} -> sexp.get${f}()`));
    (flags.string || []).forEach(f => lines.push(` *   - string ${f} -> sexp.get${f}()`));
    (flags.double || []).forEach(f => lines.push(` *   - double ${f} -> sexp.get${f}()`));
  } else {
    lines.push(` * Flags: (none)`);
  }
  return lines.join("\n");
}

/**
 * Generate stub function implementation for a single tag.
 */
function genHandlerCpp(item) {
  const doc = genTagDoc(item);
  const tag = item.tag;

  // Generate example code lines for args and flags
  const sampleCalls = [];
  if (item.args && item.args.length > 0) {
    item.args.forEach((a) => {
      sampleCalls.push(`  // if (sexp.hasArg_${a}()) { IRID arg_${a} = sexp.getArg_${a}(); }`);
    });
  }
  const flags = item.flags || {};
  (flags.void || []).forEach(f => sampleCalls.push(`  // bool has_${f} = sexp.has${f}();`));
  (flags.bool || []).forEach(f => sampleCalls.push(`  // if (sexp.has${f}()) { bool val_${f} = sexp.get${f}(); }`));
  (flags.string || []).forEach(f => sampleCalls.push(`  // if (sexp.has${f}()) { StringID str_${f} = sexp.get${f}(); }`));
  (flags.double || []).forEach(f => sampleCalls.push(`  // if (sexp.has${f}()) { double dbl_${f} = sexp.get${f}(); }`));

  return `// Generated Stub for IRI_TAG::${tag}
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
${doc}
 */
void handle${tag}(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case ${tag}");
  // IRI_GEN::${tag}SEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : ${tag} ===
${sampleCalls.length > 0 ? sampleCalls.join("\n") + "\n" : ""}
  return ;
}

} // namespace IRI_STRUCTURAL
`;
}

/**
 * 1. Generate individual Handler C++ files (if not exists or FORCE_OVERWRITE)
 */
let createdCount = 0;
let skippedCount = 0;

for (const item of spec) {
  const filename = `Handle${item.tag}.cpp`;
  const filePath = path.join(SRC_PTA_DIR, filename);
  const exists = fs.existsSync(filePath);

  if (!exists || FORCE_OVERWRITE) {
    if (!DRY_RUN) {
      fs.writeFileSync(filePath, genHandlerCpp(item), "utf8");
    }
    console.log(`[${exists ? "OVERWRITE" : "CREATE"}] src/Support/PTA/${filename}`);
    createdCount++;
  } else {
    skippedCount++;
  }
}

/**
 * 2. Generate include/Support/PTA/PTAContext.hpp
 */
const ptaContextHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Storage/IRIContext.h"
#include "Storage/IridiumSEXP.h"
#include "Generated/IridiumEnums.h"
#include "Support/IRICFG.hpp"
#include "external/Prakriti.hpp"

namespace IRI_STRUCTURAL {

/**
 * Bundles the execution context for Points-To Analysis (PTA) transfer functions.
 * Allows easy evolution of PTA state/parameters without changing individual handler signatures.
 */
struct PTAStatementContext {
  const IRIStatement &stmt;
  Prakriti::ECMAGraph *incomingState;
  IRI_STORAGE::IRIContext &ctx;

  PTAStatementContext(const IRIStatement &s,
                      Prakriti::ECMAGraph *st,
                      IRI_STORAGE::IRIContext &c)
      : stmt(s), incomingState(st), ctx(c) {}

  // Convenient accessors
  inline IRID getId() const { return stmt.id; }
  inline IRI_GEN::IRI_TAG getTag() const { return IRI_NODE(ctx, stmt.id).tag; }
  inline IRIBB* getBB() const { return stmt.bb; }
};

using PTAContext = PTAStatementContext;

} // namespace IRI_STRUCTURAL
`;

const contextHppPath = path.join(INCLUDE_PTA_DIR, "PTAContext.hpp");
if (!DRY_RUN) {
  fs.writeFileSync(contextHppPath, ptaContextHpp, "utf8");
}
console.log(`[GENERATE] include/Support/PTA/PTAContext.hpp`);

/**
 * 3. Generate include/Support/PTA/PTAHandlers.hpp (Header declarations and Interface)
 */
const headerDecls = spec.map(item => {
  return `void handle${item.tag}(const PTAStatementContext &ptactx);`;
}).join("\n");

const visitorMethods = spec.map(item => {
  return `  virtual void visit_${item.tag}(const PTAStatementContext &ptactx) {\n` +
         `    return handle${item.tag}(ptactx);\n` +
         `  }`;
}).join("\n");

const ptaHandlersHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PTA/PTAContext.hpp"

namespace IRI_STRUCTURAL {

// ============================================================================
// PTA Transfer Handler Function Declarations
// ============================================================================
${headerDecls}

// ============================================================================
// Optional IPTATransferHandler Interface (for Visitor/Polymorphic Overrides)
// ============================================================================
class IPTATransferHandler {
public:
  virtual ~IPTATransferHandler() = default;

${visitorMethods}
};

} // namespace IRI_STRUCTURAL
`;

const handlersHppPath = path.join(INCLUDE_PTA_DIR, "PTAHandlers.hpp");
if (!DRY_RUN) {
  fs.writeFileSync(handlersHppPath, ptaHandlersHpp, "utf8");
}
console.log(`[GENERATE] include/Support/PTA/PTAHandlers.hpp`);

/**
 * 4. Generate include/Support/PTA/PTADispatch.hpp (Fast switch &nextStatedispatcher)
 */
const switchCases = spec.map(item => {
  return `    case IRI_GEN::IRI_TAG::${item.tag}:\n` +
         `      return handle${item.tag}(ptactx);`;
}).join("\n");

const ptaDispatchHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * Dispatches an IRIStatement transfer operation to its corresponding tag handler using PTAStatementContext.
 */
inline void dispatchPTAStatement(const PTAStatementContext &ptactx) {
  auto currTAG = ptactx.getTag();

  switch (currTAG) {
${switchCases}
    default:
      std::cerr << "[PTA Warning] Unhandled statement tag: "
                << IRI_GEN::dump_tag(currTAG) << std::endl;
      return;
  }
}

/**
 * Convenience overload allowing direct dispatch from raw stmt, incomingState, and ctx.
 */
inline void dispatchPTAStatement(const IRIStatement &stmt,
                                                Prakriti::ECMAGraph *incomingState,
                                                IRI_STORAGE::IRIContext &ctx) {
  PTAStatementContext ptactx(stmt, incomingState, ctx);
  return dispatchPTAStatement(ptactx);
}

} // namespace IRI_STRUCTURAL
`;

const dispatchHppPath = path.join(INCLUDE_PTA_DIR, "PTADispatch.hpp");
if (!DRY_RUN) {
  fs.writeFileSync(dispatchHppPath, ptaDispatchHpp, "utf8");
}
console.log(`[GENERATE] include/Support/PTA/PTADispatch.hpp`);

console.log(`\nPTA Code Generation Complete!`);
console.log(`- Handler stubs created: ${createdCount}`);
console.log(`- Existing handlers preserved: ${skippedCount}`);
console.log(`- Total tags supported: ${spec.length}`);
