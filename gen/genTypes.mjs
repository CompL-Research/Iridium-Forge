// #!/usr/bin/env node
import fs from "fs";
import spec from "../TYPESPEC.mjs";

const TAGS = new Set();
const FLAGS = new Set();

function genSchema(spec) {
  const { tag, flags, args } = spec;

  TAGS.add(tag);

  // 1. Flatten all flags into an indexed list
  // CHANGED: string flags now map to StringID
  const allFlags = [];
  if (flags.string) flags.string.forEach(f => { allFlags.push({ name: f, type: 'StringID', isVoid: false }); FLAGS.add(f); });
  if (flags.void)   flags.void.forEach(f => { allFlags.push({ name: f, type: 'bool', isVoid: true }); FLAGS.add(f); });
  if (flags.bool)   flags.bool.forEach(f => { allFlags.push({ name: f, type: 'bool', isVoid: false }); FLAGS.add(f); });
  if (flags.double) flags.double.forEach(f => { allFlags.push({ name: f, type: 'double', isVoid: false }); FLAGS.add(f); });

  // 2. Generate static indices
  const indexDefs = allFlags.map((f, idx) => `    static constexpr uint32_t FLAG_IDX_${f.name} = ${idx};`).join("\n");

  // 3. Generate Getters/Setters for Flags
  // CHANGED: Now uses the inline get_flag() helper instead of node->getSlot()
  const flagMethods = allFlags.map(f => {
    if (f.isVoid) {
      return `    bool has${f.name}() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_${f.name})); }\n` +
             `    void set${f.name}() { mutate_flag(FLAG_IDX_${f.name}) = std::nullptr_t{}; }\n` +
             `    void clear${f.name}() { mutate_flag(FLAG_IDX_${f.name}) = std::monostate{}; }`;
    } else {
      return `    ${f.type} get${f.name}() const { return std::get<${f.type}>(get_flag(FLAG_IDX_${f.name})); }\n` +
             `    void set${f.name}(${f.type} val) { mutate_flag(FLAG_IDX_${f.name}) = val; }\n` +
             `    bool has${f.name}() const { return std::holds_alternative<${f.type}>(get_flag(FLAG_IDX_${f.name})); }\n` +
             `    void clear${f.name}() { mutate_flag(FLAG_IDX_${f.name}) = std::monostate{}; }`;
    }
  }).join("\n\n");

  // 4. Generate Getters/Setters for Arguments
  // CHANGED: Now returns and accepts IRID instead of IridiumSEXP*
  const argMethods = (args || []).map((a, idx) => {
    return `    IRID getArg_${a}() const { return pool->get_args(id)[${idx}]; }\n` +
           `    void setArg_${a}(IRID val) { pool->update_arg_inplace(id,${idx},val); }`;
  }).join("\n\n");

  const createArgs = [];

  (args || []).forEach((a) => createArgs.push(`IRID ${a}`) );
  flags.string.forEach((f) => createArgs.push(`StringID ${f}`));
  flags.void.forEach((f) => createArgs.push(`bool ${f}`));
  flags.bool.forEach((f) => createArgs.push(`bool ${f}`));
  flags.double.forEach((f) => createArgs.push(`double ${f}`));

  const createFlags = allFlags.map((a) => {
    if (a.type === "bool" && a.isVoid === true) {
      return `${a.name} ? FlagValue(${a.name}) : FlagValue(std::monostate())`;
    }
    return `FlagValue(${a.name})`;
  }).join(", ");

  // Return the lightweight schema wrapper
  // CHANGED: Constructor accepts IRID and IridiumPool&
  return `  struct ${tag}SEXP {
    IRID id;
    IridiumPool* pool;

    explicit ${tag}SEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::${tag}) {
        throw std::runtime_error("Schema Cast Error: Expected ${tag}, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p${createArgs.length === 0 ? '' : ', ' + createArgs.join(", ")}) {
      return p.add_node(IRI_GEN::IRI_TAG::${tag}, {${(args || []).join(", ")}}, {${createFlags}});
    }

    static constexpr uint32_t TOTAL_ARGS = ${(args || []).length};
    static constexpr uint32_t TOTAL_FLAGS = ${allFlags.length};

${indexDefs}

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
${argMethods}

    // --- Flags ---
${flagMethods}
  };
`;
}

function getISTDateTime(date = new Date()) {
  return date.toLocaleString("sv-SE", { timeZone: "Asia/Kolkata" }).replace("T", " ");
}

const schemas = [];
const slotCounts = [];
const flagEnumCases = [];
const flagIndexCases = [];

for (let s of spec) {
  schemas.push(genSchema(s));

  const allFlags = [
    ...(s.flags.string || []),
    ...(s.flags.void || []),
    ...(s.flags.bool || []),
    ...(s.flags.double || [])
  ];

  slotCounts.push(`      case IRI_GEN::${s.tag}: return ${allFlags.length};`);

  if (allFlags.length > 0) {
    // Reverse lookup for the dump() method
    const innerEnumCases = allFlags.map((f, idx) => `        case ${idx}: return IRI_GEN::${f};`).join("\n");
    flagEnumCases.push(`      case IRI_GEN::${s.tag}:
        switch(index) {
${innerEnumCases}
          default: throw std::runtime_error("Invalid flag index");
        }
        break;`);

    // Forward lookup for the Parser
    const innerIndexCases = allFlags.map((f, idx) => `        case IRI_GEN::${f}: return ${idx};`).join("\n");
    flagIndexCases.push(`      case IRI_GEN::${s.tag}:
        switch(flag) {
${innerIndexCases}
          default: return -1;
        }
        break;`);
  }
}

const typesFile = [
  `// Generated: ${getISTDateTime()}`,
  `#pragma once`,
  `#include "Storage/Config.h"`,
  `#include "Storage/IridiumSEXP.h"`,
  `#include "Storage/IridiumPool.h"`,
  `#include "IridiumEnums.h"`,
  `#include <variant>`,
  `#include <span>`,
  `#include <stdexcept>`,
  ``,
  `namespace IRI_GEN {`,
  `using IRI_STORAGE::IridiumPool;`,
  `using IRI_STORAGE::IRID;`,
  `using IRI_STORAGE::FlagValue;`,
  ...schemas,
  `} // namespace IRI_GEN`
];

const metaFile = [
  `// Generated: ${getISTDateTime()}`,
  `#pragma once`,
  `#include "IridiumEnums.h"`,
  `#include <cstdint>`,
  `#include <stdexcept>`,
  ``,
  "namespace IRI_GEN {",
  `class IridiumMeta {`,
  `public:`,
  `  // Used by the Iridium Pool to know how many flag slots to reserve`,
  `  static uint32_t get_flag_slots(IRI_GEN::IRI_TAG tag) {`,
  `    switch(tag) {`,
  ...slotCounts,
  `      default: return 0;`,
  `    }`,
  `  }`,
  ``,
  `  // Used by the Parser to map an incoming msgpack flag enum to its static slot index`,
  `  static int get_flag_index(IRI_GEN::IRI_TAG tag, IRI_GEN::IRI_FLAG flag) {`,
  `    switch(tag) {`,
  ...flagIndexCases,
  `      default: return -1;`,
  `    }`,
  `  }`,
  ``,
  `  // Used by IridiumSEXP::dump() to recover the flag name`,
  `  static IRI_GEN::IRI_FLAG get_flag_enum(IRI_GEN::IRI_TAG tag, uint32_t index) {`,
  `    switch(tag) {`,
  ...flagEnumCases,
  `      default: throw std::runtime_error("Invalid tag for flag lookup");`,
  `    }`,
  `  }`,
  `};`,
  "} // namespace IRI_GEN"
];

const enumsFile = [
  `// Generated: ${getISTDateTime()}`,
  `#pragma once`,
  `#include <string>`,
  "",
  "namespace IRI_GEN {",
  "  enum IRI_TAG {",
  ...[...TAGS].map((e, idx) => idx === TAGS.size - 1 ? `    ${e}` : `    ${e},`),
  "  };",
  "",
  "  inline std::string dump_tag(IRI_TAG value) {",
  "    switch(value) {",
  ...[...TAGS].map((e) => `      case ${e}: return "${e}";`),
  '      default: return "unknown_tag";',
  "    }",
  "  }",
  "",
  "  enum IRI_FLAG {",
  ...[...FLAGS].map((e, idx) => idx === FLAGS.size - 1 ? `    ${e}` : `    ${e},`),
  "  };",
  "",
  "  inline std::string dump_flag(IRI_FLAG value) {",
  "    switch(value) {",
  ...[...FLAGS].map((e) => `      case ${e}: return "${e}";`),
  '      default: return "unknown_flag";',
  "    }",
  "  }",
  "} // namespace IRI_GEN"
];

const enumsFileTS = [
  `/** Generated: ${getISTDateTime()} */`,
  "",
  "export enum IriTag {",
  ...[...TAGS].map((e, idx) => `  ${e} = ${idx},`),
  "}",
  "",
  "export enum IriFlag {",
  ...[...FLAGS].map((e, idx) => `  ${e} = ${idx},`),
  "}"
];

// Write outputs
fs.writeFileSync("../include/Generated/IridiumTypes.h", typesFile.join("\n"));
fs.writeFileSync("../include/Generated/IridiumMeta.h", metaFile.join("\n"));
fs.writeFileSync("../include/Generated/IridiumEnums.h", enumsFile.join("\n"));
fs.writeFileSync("../../../classes/builder/IridiumV2/Types/TSTypes.ts", enumsFileTS.join("\n"));

console.log("Successfully generated Iridium AST Schema files.");


// import fs from "fs";
// import spec from "../TYPESPEC.mjs";

// const TAGS = new Set();
// const FLAGS = new Set();

// function genSchema(spec) {
//   const { tag, flags, args } = spec;

//   TAGS.add(tag);

//   // 1. Flatten all flags into an indexed list
//   const allFlags = [];
//   if (flags.string) flags.string.forEach(f => { allFlags.push({ name: f, type: 'std::string_view', isVoid: false }); FLAGS.add(f); });
//   if (flags.void)   flags.void.forEach(f => { allFlags.push({ name: f, type: 'bool', isVoid: true }); FLAGS.add(f); });
//   if (flags.bool)   flags.bool.forEach(f => { allFlags.push({ name: f, type: 'bool', isVoid: false }); FLAGS.add(f); });
//   if (flags.double) flags.double.forEach(f => { allFlags.push({ name: f, type: 'double', isVoid: false }); FLAGS.add(f); });

//   // 2. Generate static indices
//   const indexDefs = allFlags.map((f, idx) => `    static constexpr uint32_t FLAG_IDX_${f.name} = ${idx};`).join("\n");

//   // 3. Generate Getters/Setters for Flags
//   const flagMethods = allFlags.map(f => {
//     if (f.isVoid) {
//       return `    bool has${f.name}() const { return !std::holds_alternative<std::monostate>(node->getSlot(FLAG_IDX_${f.name})); }\n` +
//              `    void set${f.name}() { node->getSlot(FLAG_IDX_${f.name}) = true; }\n` +
//              `    void clear${f.name}() { node->getSlot(FLAG_IDX_${f.name}) = std::monostate{}; }`;
//     } else {
//       return `    ${f.type} get${f.name}() const { return std::get<${f.type}>(node->getSlot(FLAG_IDX_${f.name})); }\n` +
//              `    void set${f.name}(${f.type} val) { node->getSlot(FLAG_IDX_${f.name}) = val; }\n` +
//              `    bool has${f.name}() const { return std::holds_alternative<${f.type}>(node->getSlot(FLAG_IDX_${f.name})); }\n` +
//              `    void clear${f.name}() { node->getSlot(FLAG_IDX_${f.name}) = std::monostate{}; }`;
//     }
//   }).join("\n\n");

//   // 4. Generate Getters/Setters for Arguments
//   const argMethods = (args || []).map((a, idx) => {
//     return `    IridiumSEXP* getArg_${a}() const { return node->args[${idx}]; }\n` +
//            `    void setArg_${a}(IridiumSEXP* val) { node->args[${idx}] = val; }`;
//   }).join("\n\n");

//   // Return the lightweight schema wrapper
//   return `  struct ${tag}SEXP {
//     IridiumSEXP* node;

//     explicit ${tag}SEXP(IridiumSEXP* n) : node(n) {
//       if (n->tag != IRI_GEN::${tag}) {
//         throw std::runtime_error("Schema Cast Error: Expected ${tag}, but got " + IRI_GEN::dump_tag(n->tag));
//       }
//     }

//     static constexpr uint32_t TOTAL_ARGS = ${(args || []).length};
//     static constexpr uint32_t TOTAL_FLAGS = ${allFlags.length};

// ${indexDefs}

//     // --- Arguments ---
// ${argMethods}

//     // --- Flags ---
// ${flagMethods}
//   };
// `;
// }

// function getISTDateTime(date = new Date()) {
//   return date.toLocaleString("sv-SE", { timeZone: "Asia/Kolkata" }).replace("T", " ");
// }

// const schemas = [];
// const slotCounts = [];
// const flagEnumCases = [];
// const flagIndexCases = [];

// for (let s of spec) {
//   schemas.push(genSchema(s));

//   const allFlags = [
//     ...(s.flags.string || []),
//     ...(s.flags.void || []),
//     ...(s.flags.bool || []),
//     ...(s.flags.double || [])
//   ];

//   slotCounts.push(`      case IRI_GEN::${s.tag}: return ${allFlags.length};`);

//   if (allFlags.length > 0) {
//     // Reverse lookup for the dump() method
//     const innerEnumCases = allFlags.map((f, idx) => `        case ${idx}: return IRI_GEN::${f};`).join("\n");
//     flagEnumCases.push(`      case IRI_GEN::${s.tag}:
//         switch(index) {
// ${innerEnumCases}
//           default: throw std::runtime_error("Invalid flag index");
//         }
//         break;`);

//     // Forward lookup for the Parser
//     const innerIndexCases = allFlags.map((f, idx) => `        case IRI_GEN::${f}: return ${idx};`).join("\n");
//     flagIndexCases.push(`      case IRI_GEN::${s.tag}:
//         switch(flag) {
// ${innerIndexCases}
//           default: return -1;
//         }
//         break;`);
//   }
// }

// const typesFile = [
//   `// Generated: ${getISTDateTime()}`,
//   `#pragma once`,
//   `#include "Iridium/Globals.h"`,
//   `#include "Iridium/IridiumSEXP.h"`,
//   `#include "generated/IridiumEnums.h"`,
//   `#include <variant>`,
//   `#include <string_view>`,
//   `#include <stdexcept>`,
//   ``,
//   `namespace IRI_SCHEMA {`,
//   ...schemas,
//   `} // namespace IRI_SCHEMA`
// ];

// const metaFile = [
//   `// Generated: ${getISTDateTime()}`,
//   `#pragma once`,
//   `#include "generated/IridiumEnums.h"`,
//   `#include <cstdint>`,
//   `#include <stdexcept>`,
//   ``,
//   `class IridiumMeta {`,
//   `public:`,
//   `  // Used by the Arena Allocator to know how many flag slots to reserve`,
//   `  static uint32_t get_flag_slots(IRI_GEN::IRI_TAG tag) {`,
//   `    switch(tag) {`,
//   ...slotCounts,
//   `      default: return 0;`,
//   `    }`,
//   `  }`,
//   ``,
//   `  // Used by the Parser to map an incoming msgpack flag enum to its static slot index`,
//   `  static int get_flag_index(IRI_GEN::IRI_TAG tag, IRI_GEN::IRI_FLAG flag) {`,
//   `    switch(tag) {`,
//   ...flagIndexCases,
//   `      default: return -1;`,
//   `    }`,
//   `  }`,
//   ``,
//   `  // Used by IridiumSEXP::dump() to recover the flag name`,
//   `  static IRI_GEN::IRI_FLAG get_flag_enum(IRI_GEN::IRI_TAG tag, uint32_t index) {`,
//   `    switch(tag) {`,
//   ...flagEnumCases,
//   `      default: throw std::runtime_error("Invalid tag for flag lookup");`,
//   `    }`,
//   `  }`,
//   `};`
// ];

// const enumsFile = [
//   `// Generated: ${getISTDateTime()}`,
//   `#pragma once`,
//   `#include <string>`,
//   "",
//   "namespace IRI_GEN {",
//   "  enum IRI_TAG {",
//   ...[...TAGS].map((e, idx) => idx === TAGS.size - 1 ? `    ${e}` : `    ${e},`),
//   "  };",
//   "",
//   "  inline std::string dump_tag(IRI_TAG value) {",
//   "    switch(value) {",
//   ...[...TAGS].map((e) => `      case ${e}: return "${e}";`),
//   '      default: return "unknown_tag";',
//   "    }",
//   "  }",
//   "",
//   "  enum IRI_FLAG {",
//   ...[...FLAGS].map((e, idx) => idx === FLAGS.size - 1 ? `    ${e}` : `    ${e},`),
//   "  };",
//   "",
//   "  inline std::string dump_flag(IRI_FLAG value) {",
//   "    switch(value) {",
//   ...[...FLAGS].map((e) => `      case ${e}: return "${e}";`),
//   '      default: return "unknown_flag";',
//   "    }",
//   "  }",
//   "} // namespace IRI_GEN"
// ];

// const enumsFileTS = [
//   `/** Generated: ${getISTDateTime()} */`,
//   "",
//   "export enum IriTag {",
//   ...[...TAGS].map((e, idx) => `  ${e} = ${idx},`),
//   "}",
//   "",
//   "export enum IriFlag {",
//   ...[...FLAGS].map((e, idx) => `  ${e} = ${idx},`),
//   "}"
// ];

// // Write outputs
// fs.writeFileSync("../include/generated/IridiumTypes.h", typesFile.join("\n"));
// fs.writeFileSync("../include/generated/IridiumMeta.h", metaFile.join("\n"));
// fs.writeFileSync("../include/generated/IridiumEnums.h", enumsFile.join("\n"));
// fs.writeFileSync("../../../classes/builder/IridiumV2/Types/TSTypes.ts", enumsFileTS.join("\n"));

// console.log("Successfully generated Iridium AST Schema files.");
