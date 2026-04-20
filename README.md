## Iridium Forge

A robust compiler toolchain for transforming, optimizing, and lowering transitional IRIDIUM code into executable VM binaries.

### Overview

Iridium Forge is a pipeline designed to take transitional IRIDIUM code and convert it into an executable format tailored for Virtual Machine consumption. 

Currently supported target VMs: QuickJS.

### Core Pipeline

The Forge processes IRIDIUM code through three primary, high-level operations:

1. **Normalization**

The foundational structuring phase. 
A dedicated set of core passes organizes the raw IRIDIUM code into a strict, well-defined internal representation.

- *Node Resolution*: Identifies and resolves all transitional nodes.

- *ECMAScript Semantics*: Automatically injects necessary decorators/instructions to accurately model ECMAScript Semantics.

2. **Analysis & Optimization**

A suite of standard and ECMAScript-specific analysis passes run over the normalized code.

*Note*: Optimization passes are strictly optional and can be toggled based on your build requirements.

3. **Instruction Selection (Lowering)**

The final code generation phase. 
The normalized (and optionally optimized) IRIDIUM is translated and lowered into a compact binary format.

Maps IRIDIUM instructions to the optimal opcodes for the target environment.

Outputs a final binary ready to be directly consumed by the QuickJS VM.


### Install Dependencies

This project requires `npm` to function.
The build system uses `cmake-js`, which is used to export this project as a node module.

```bash
apt install build-essential cmake pkg-config
npm install
```

### Building for debug
```bash
npm run dev # or dev-dump -- prints outputs of transformation-passes
```

### Building fore release
```bash
npm run build
```
