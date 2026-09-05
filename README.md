# Compiler & Runtime Lab

A portfolio-oriented umbrella repository for a family of compiler, runtime, toolchain, language-tooling, debugging, and tensor-compilation projects.

This repository is intentionally being assembled with **history-preserving migration**. Original repositories remain available while each subtree is migrated, verified, and placed under umbrella CI. No source repository is deleted as part of this process.

## Project map

| Project | Role | Migration status |
| --- | --- | --- |
| [Nova](projects/Nova) | Typed language, semantic analysis, interpreter/runtime | **IMPORTED / VERIFIED** |
| [tiny-c-compiler](https://github.com/Lei-TzuY/tiny-c-compiler) | Self-contained x86-64 C compiler | ATTRIBUTION REVIEW |
| [sic-xe-assembler](https://github.com/Lei-TzuY/sic-xe-assembler) | SIC/XE assembler and static-analysis tooling | HOLD — recheck active implementation state |
| [mini-elf-toolchain](projects/mini-elf-toolchain) | ELF/static-linking toolchain | **IMPORTED / VERIFIED** |
| [mini-language-server](https://github.com/Lei-TzuY/mini-language-server) | Version-safe semantic/LSP tooling | READY FOR IMPORT PREP |
| [mini-debugger](projects/mini-debugger) | ptrace-based debugger | **IMPORTED / VERIFIED** |
| [mini-libc](projects/mini-libc) | Freestanding libc subset and bootstrap target | **IMPORTED / VERIFIED** |
| [mini-wasm-runtime](projects/mini-wasm-runtime) | WebAssembly parser, validator, runtime and conformance lab | **IMPORTED / VERIFIED** |
| [tiny-tensor-compiler](projects/tiny-tensor-compiler) | Tensor IR, optimization and native compilation | **IMPORTED / VERIFIED** |

## Repository layout

```text
compiler-runtime-lab/
├── README.md
├── ROADMAP.md
├── docs/
│   └── MIGRATION.md
├── scripts/
│   ├── import-project.ps1
│   └── verify-import.ps1
└── projects/
    ├── mini-elf-toolchain/      # imported + verified
    ├── mini-libc/               # imported + verified
    ├── mini-debugger/           # imported + verified
    ├── tiny-tensor-compiler/    # imported + verified
    ├── mini-wasm-runtime/       # imported + verified
    ├── Nova/                    # imported + verified
    ├── mini-language-server/    # next import
    ├── tiny-c-compiler/         # attribution review
    └── sic-xe-assembler/        # hold / recheck
```

A planned project directory is created only by a verified history-preserving import. Do not replace this process with ZIP/download-and-copy commits.

## Verified imports and integration chains

`mini-elf-toolchain` is the reference migration. Source SHA `3d452a8681bbfb092cd41465dba6f6eb97dfd224` was imported without squashing into `projects/mini-elf-toolchain/`. The source commit remains reachable in umbrella history, the subtree was verified blob-for-blob against the frozen source tree, its native Rust formatter/Clippy/test gates passed from the umbrella path, and a permanent path-scoped umbrella workflow protects future changes.

`mini-libc` followed the same history-preserving process from source SHA `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1`. Its GCC and Clang runtime probes, host-libc-independence checks, pinned tiny-C bootstrap, and source-style three-repository bootstrap all passed before publication. The frozen source contains README/docs/Makefile but no top-level LICENSE file; that source state was preserved exactly rather than inventing licensing metadata.

`mini-debugger` was imported from source SHA `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a` after its exact source CI, complete reachable-history attribution scan, native CMake/CTest suite, ancestry check, and blob-for-blob subtree comparison passed. It is protected by a permanent umbrella workflow together with the imported mini-ELF toolchain.

`tiny-tensor-compiler` is synchronized through source SHA `4690df5747a1e7fc0af9b602f8be8d963e72d00f`. Its initial import and subsequent 26-commit non-squashed subtree refresh each preserved source ancestry and tree equivalence. Permanent CI mirrors the source matrix across Ubuntu/Windows and Python 3.11/3.13; the refreshed umbrella main passed all four jobs. The source has no top-level LICENSE file, and the umbrella preserves that state rather than manufacturing one.

`mini-wasm-runtime` was imported from source SHA `e923b27a2652aba88d50cdbb75d0fe959d40e457`. The complete reachable history passed the configured attribution scan, the subtree matched the frozen source blob-for-blob, and validation from the umbrella path passed stable and Rust 1.81 core formatter/Clippy/tests/docs, Wasmtime differential reference tests, and deterministic benchmark smoke. A permanent umbrella workflow also restores the source platform/MSRV matrix and deterministic parser fuzz smoke. The source `LICENSE` is preserved in the imported subtree.

`Nova` was imported from source SHA `dcadc2238737b6f1e98887ab8fa658b23413d31b`. The complete reachable history passed the configured attribution scan, the frozen source tree and imported subtree matched blob-for-blob, and source-equivalent stable gates plus the Rust 1.85 MSRV contract passed from `projects/Nova`. The source repository has README/docs/examples, `Cargo.lock`, and toolchain metadata but no top-level LICENSE file; the umbrella preserves that state exactly. A permanent path-scoped workflow mirrors Nova's rustfmt, Clippy, MSRV, tests, build and rustdoc gates.

The umbrella currently exercises two real cross-project integration chains.

```text
pinned tiny-c-compiler
        ↓
projects/mini-libc
        ↓
projects/mini-elf-toolchain
        ↓
freestanding x86-64 executable
        ↓
execution + host-libc-independence inspection
```

```text
projects/mini-elf-toolchain
        ↓
sectionless ELF64 ET_EXEC, entry 0x400000
        ↓
projects/mini-debugger
        ↓
ptrace launch + memory read
        ↓
numeric software breakpoint at 0x400001
        ↓
continue + breakpoint hit
```

The second chain intentionally validates address-level interoperability. The current mini-ELF executable writer emits no section-header table (`e_shoff = 0`, `e_shnum = 0`), so it would be false to claim `.symtab`-based debugging for these generated images. Symbol-level interoperability is a future capability boundary for the linker/metadata layer, not something hidden by a weaker test.

`tiny-tensor-compiler` is intentionally **not** shown as directly connected to `mini-elf-toolchain` yet. Its current native backend emits C11, invokes the host GCC/Clang/MSVC toolchain to build a shared library, and loads that artifact through `ctypes`; mini-ELF currently links static object/archive inputs into `ET_EXEC`. A future integration must define a real object/static-runtime or executable-artifact handoff before the umbrella claims that edge.

`mini-wasm-runtime` likewise does not need a fabricated dependency on another imported project. Its verified composition boundary is the external WebAssembly reference/conformance ecosystem: pinned spec coverage, Wasmtime differential testing, deterministic fuzz smoke, and benchmark-policy execution.

`Nova` is currently verified as an independent language/compiler/runtime subtree. The next import is `mini-language-server`; only after that migration is independently verified will the umbrella add the semantic/diagnostic interoperability that the existing bounded Nova adapter genuinely supports. This does not imply a complete production Nova LSP.

See [docs/MIGRATION.md](docs/MIGRATION.md) for the full evidence ledger.

## Migration invariants

A project may enter this umbrella only when all applicable gates are satisfied:

1. Re-check the exact source `main`, open PRs, recent commits, CI/checks, and default branch immediately before migration.
2. Do not migrate a repository while an implementation PR is active on that repository.
3. Preserve source commit history and all existing project-local license/documentation files; do not flatten history into a single copy commit or invent missing licensing metadata.
4. Audit reachable commit messages for attribution trailers before import. Known attribution must not be silently imported or silently rewritten.
5. Verify the imported project tree against the exact source commit selected for migration.
6. Run the imported project's formatter/lint/tests/build/CI from its new path and keep equivalent umbrella CI afterward.
7. Keep the original repository available until migration and the umbrella operating model are stable. Archival/redirect is a separate final step; deletion is not part of the migration plan.
8. Do not add `Co-Authored-By`, `Generated-By`, `Assisted-By`, `Signed-off-by`, or AI/bot attribution trailers to new umbrella commits.

## Why an umbrella repository?

The individual repositories remain useful engineering artifacts, but together they form a clearer systems story: language design → parsing/type checking → compilation/linking → runtime/library support → debugging/language tooling → alternative runtimes and domain-specific compilation.

The goal is not to hide or rewrite the original development timeline. The goal is to make the architecture and relationships between projects legible while preserving evidence.

See [ROADMAP.md](ROADMAP.md) for the integration order and [docs/MIGRATION.md](docs/MIGRATION.md) for the migration protocol and current preflight ledger.
