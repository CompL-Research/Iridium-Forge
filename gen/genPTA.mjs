#!/usr/bin/env node
import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";
import specRaw from "../TYPESPEC.mjs";

// STMT tags dispatch at the statement level. AMP tags are expressions that
// can *also* appear as a bare statement (e.g. `foo();`), so they get both a
// statement handler (default: evaluate for side effects, discard the value)
// and an RVal handler (compute the value). RVAL tags only ever appear as an
// operand of something else, so they only get an RVal handler.
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
Usage: node gen/genPTA.mjs [options]

Options:
  --force, -f    Force overwrite existing handler .cpp files
  --dry-run      Print planned operations without writing any files
  --help, -h     Show this help message
`);
  process.exit(0);
}

const INCLUDE_PTA_DIR = path.join(ROOT_DIR, "include", "Support", "PTA");
const SRC_PTA_DIR = path.join(ROOT_DIR, "src", "Support", "PTA");
const SRC_PTA_RVAL_DIR = path.join(SRC_PTA_DIR, "RVal");

if (!DRY_RUN) {
  fs.mkdirSync(INCLUDE_PTA_DIR, { recursive: true });
  fs.mkdirSync(SRC_PTA_DIR, { recursive: true });
  fs.mkdirSync(SRC_PTA_RVAL_DIR, { recursive: true });
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

function genSampleCalls(item) {
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
  return sampleCalls;
}

/**
 * Generate a statement handler stub. AMP tags default to evaluating the
 * node through the RVal resolver and discarding the result -- that's the
 * correct behavior for e.g. `foo();`, a call used as a bare statement.
 */
function genStmtHandlerCpp(item) {
  const doc = genTagDoc(item);
  const tag = item.tag;

  if (item.meta === "AMP") {
    return `// Generated Stub for IRI_TAG::${tag}
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
${doc}
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handle${tag}(const PTAStatementContext &ptactx) {
  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
`;
  }

  const sampleCalls = genSampleCalls(item);
  return `// Generated Stub for IRI_TAG::${tag}
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
${doc}
 */
void handle${tag}(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case ${tag}");
  // IRI_GEN::${tag}SEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : ${tag} ===
${sampleCalls.length > 0 ? sampleCalls.join("\n") + "\n" : ""}
  return;
}

} // namespace IRI_STRUCTURAL
`;
}

/**
 * Generate an RVal handler stub: computes the set of nodes a node evaluates
 * to (used both for AMP nodes used as operands and for plain RVAL nodes).
 */
function genRValHandlerCpp(item) {
  const doc = genTagDoc(item);
  const tag = item.tag;
  const sampleCalls = genSampleCalls(item);

  return `// Generated Stub for IRI_TAG::${tag} (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
${doc}
 */
void compute${tag}Vals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case ${tag}");
  // IRI_GEN::${tag}SEXP sexp(node, ptactx.ctx);

  // === TODO : ${tag} ===
${sampleCalls.length > 0 ? sampleCalls.join("\n") + "\n" : ""}
  return;
}

} // namespace IRI_STRUCTURAL
`;
}

/**
 * 1. Generate individual Handler C++ files (if not exists or FORCE_OVERWRITE)
 */
let createdCount = 0;
let skippedCount = 0;

for (const item of stmtSpec) {
  const filename = `Handle${item.tag}.cpp`;
  const filePath = path.join(SRC_PTA_DIR, filename);
  const exists = fs.existsSync(filePath);

  if (!exists || FORCE_OVERWRITE) {
    if (!DRY_RUN) {
      fs.writeFileSync(filePath, genStmtHandlerCpp(item), "utf8");
    }
    console.log(`[${exists ? "OVERWRITE" : "CREATE"}] src/Support/PTA/${filename}`);
    createdCount++;
  } else {
    skippedCount++;
  }
}

for (const item of rvalSpec) {
  const filename = `Handle${item.tag}RVal.cpp`;
  const filePath = path.join(SRC_PTA_RVAL_DIR, filename);
  const exists = fs.existsSync(filePath);

  if (!exists || FORCE_OVERWRITE) {
    if (!DRY_RUN) {
      fs.writeFileSync(filePath, genRValHandlerCpp(item), "utf8");
    }
    console.log(`[${exists ? "OVERWRITE" : "CREATE"}] src/Support/PTA/RVal/${filename}`);
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
 * 3. Generate include/Support/PTA/PTAHandlers.hpp (statement handler declarations + visitor)
 */
const stmtHeaderDecls = stmtSpec.map(item => {
  return `void handle${item.tag}(const PTAStatementContext &ptactx);`;
}).join("\n");

const stmtVisitorMethods = stmtSpec.map(item => {
  return `  virtual void visit_${item.tag}(const PTAStatementContext &ptactx) {\n` +
         `    return handle${item.tag}(ptactx);\n` +
         `  }`;
}).join("\n");

const ptaHandlersHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PTA/PTAContext.hpp"

namespace IRI_STRUCTURAL {

// ============================================================================
// PTA Statement Handler Function Declarations
// ============================================================================
${stmtHeaderDecls}

// ============================================================================
// Optional IPTATransferHandler Interface (for Visitor/Polymorphic Overrides)
// ============================================================================
class IPTATransferHandler {
public:
  virtual ~IPTATransferHandler() = default;

${stmtVisitorMethods}
};

} // namespace IRI_STRUCTURAL
`;

const handlersHppPath = path.join(INCLUDE_PTA_DIR, "PTAHandlers.hpp");
if (!DRY_RUN) {
  fs.writeFileSync(handlersHppPath, ptaHandlersHpp, "utf8");
}
console.log(`[GENERATE] include/Support/PTA/PTAHandlers.hpp`);

/**
 * 4. Generate include/Support/PTA/PTADispatch.hpp (statement dispatcher)
 */
const stmtSwitchCases = stmtSpec.map(item => {
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
${stmtSwitchCases}
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

/**
 * 5. Generate include/Support/PTA/PTARVALHandlers.hpp (RVal handler declarations)
 */
const rvalHeaderDecls = rvalSpec.map(item => {
  return `void compute${item.tag}Vals(const PTAStatementContext &ptactx, IRID node,\n` +
         `                       std::set<Prakriti::NodeUID> &res_);`;
}).join("\n");

const ptaRValHandlersHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PTA/PTAContext.hpp"
#include <set>

namespace IRI_STRUCTURAL {

// ============================================================================
// PTA RVal Handler Function Declarations
//
// Each of these computes the set of nodes a given IRID (an AMP or RVAL tag)
// evaluates to, appending them to res_.
// ============================================================================
${rvalHeaderDecls}

} // namespace IRI_STRUCTURAL
`;

const rvalHandlersHppPath = path.join(INCLUDE_PTA_DIR, "PTARVALHandlers.hpp");
if (!DRY_RUN) {
  fs.writeFileSync(rvalHandlersHppPath, ptaRValHandlersHpp, "utf8");
}
console.log(`[GENERATE] include/Support/PTA/PTARVALHandlers.hpp`);

/**
 * 6. Generate include/Support/PTA/PTARVALDispatch.hpp (RVal dispatcher)
 */
const rvalSwitchCases = rvalSpec.map(item => {
  return `    case IRI_GEN::IRI_TAG::${item.tag}:\n` +
         `      return compute${item.tag}Vals(ptactx, node, res_);`;
}).join("\n");

const ptaRValDispatchHpp = `// Generated: ${getISTDateTime()}
#pragma once

#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <iostream>
#include <set>

namespace IRI_STRUCTURAL {

/**
 * Resolves the set of nodes that IRID node evaluates to. node need not be
 * ptactx.stmt.id -- this is also used to resolve operands nested inside the
 * current statement.
 */
inline void resolvePKRRVal(const PTAStatementContext &ptactx, IRID node,
                           std::set<Prakriti::NodeUID> &res_) {
  auto currTAG = IRI_NODE(ptactx.ctx, node).tag;

  switch (currTAG) {
${rvalSwitchCases}
    default:
      std::cerr << "[PTA Warning] Unhandled RVal tag: "
                << IRI_GEN::dump_tag(currTAG) << std::endl;
      return;
  }
}

} // namespace IRI_STRUCTURAL
`;

const rvalDispatchHppPath = path.join(INCLUDE_PTA_DIR, "PTARVALDispatch.hpp");
if (!DRY_RUN) {
  fs.writeFileSync(rvalDispatchHppPath, ptaRValDispatchHpp, "utf8");
}
console.log(`[GENERATE] include/Support/PTA/PTARVALDispatch.hpp`);

console.log(`\nPTA Code Generation Complete!`);
console.log(`- Handler stubs created: ${createdCount}`);
console.log(`- Existing handlers preserved: ${skippedCount}`);
console.log(`- Statement tags supported: ${stmtSpec.length}`);
console.log(`- RVal tags supported: ${rvalSpec.length}`);
