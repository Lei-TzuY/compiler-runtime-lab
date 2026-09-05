# Compiler & Runtime Lab

A portfolio-oriented umbrella repository for a family of compiler, runtime, toolchain, language-tooling, debugging, and tensor-compilation projects.

This repository is intentionally assembled with **history-preserving migration**. Original repositories remain available while each subtree is migrated, verified, and placed under umbrella CI. No source repository is deleted as part of this process.

## Project map

| Project | Role | Migration status |
| --- | --- | --- |
| [Nova](projects/Nova) | Typed language, semantic analysis, interpreter/runtime | **IMPORTED / VERIFIED** |
| [tiny-c-compiler](https://github.com/Lei-TzuY/tiny-c-compiler) | Self-contained x86-64 C compiler | ATTRIBUTION REVIEW |
| [sic-xe-assembler](https://github.com/Lei-TzuY/sic-xe-assembler) | SIC/XE assembler and static-analysis tooling | HOLD — recheck active implementation state |
| [mini-elf-toolchain](projects/mini-elf-toolchain) | ELF/static-linking toolchain | **IMPORTED / VERIFIED** |
| [mini-language-server](projects/mini-language-server) | Version-safe semantic/LSP tooling | **IMPORTED / VERIFIED** |
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
├── integration/
│   └── nova-lsp/
└── projects/
    ├── mini-elf-toolchain/      # imported + verified
    ├── mini-libc/               # imported + verified
    ├── mini-debugger/           # imported + verified
    ├── tiny-tensor-compiler/    # imported + verified
    ├── mini-wasm-runtime/       # imported + verified
    ├── Nova/                    # imported + verified
    ├── mini-language-server/    # imported + verified
    ├── tiny-c-compiler/         # attribution review
    └── sic-xe-assembler/        # hold / recheck
```

A project directory is created only by a verified history-preserving import. ZIP/download-and-copy commits are not used as a substitute for history.

## Verified imports

`mini-elf-toolchain` is the reference migration. Source SHA `3d452a8681bbfb092cd41465dba6f6eb97dfd224` was imported without squashing into `projects/mini-elf-toolchain/`; source ancestry, tree equivalence and native Rust gates were verified and permanent path-scoped CI protects future changes.

`mini-libc` was imported from source SHA `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1`. GCC/Clang runtime probes, host-libc-independence checks, pinned tiny-C bootstrap and source-style mini-ELF bootstrap all passed. Its source has no top-level LICENSE, and the umbrella preserves that state rather than inventing one.

`mini-debugger` was imported from source SHA `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a` after exact source CI, full reachable-history attribution scanning, native CMake/CTest, ancestry and blob-for-blob verification passed.

`tiny-tensor-compiler` is synchronized through source SHA `4690df5747a1e7fc0af9b602f8be8d963e72d00f`. Its initial import and subsequent 26-commit non-squashed refresh both preserved source ancestry and tree equivalence. Permanent CI mirrors Ubuntu/Windows × Python 3.11/3.13. Its source also has no top-level LICENSE.

`mini-wasm-runtime` was imported from source SHA `e923b27a2652aba88d50cdbb75d0fe959d40e457`. Validation covers stable + Rust 1.81 core gates, Wasmtime differential reference testing, deterministic benchmark smoke and deterministic parser/validation fuzz smoke. Its source LICENSE is preserved.

`Nova` was imported from source SHA `dcadc2238737b6f1e98887ab8fa658b23413d31b`. Full reachable history, blob equivalence, stable rustfmt/Clippy/tests/build/rustdoc and Rust 1.85 MSRV all passed; exact merged umbrella main later passed all five permanent Nova jobs. The source has no top-level LICENSE, which is preserved as-is.

`mini-language-server` was initially imported at source SHA `f8a4d642eaa721741ab3cea7eb02d2f261dbad01` and then non-squashed-refreshed through `ab22b04e596f0a9b45441c7b0a3a6ff0b79b20a8`. The refreshed source remains reachable umbrella ancestry and matches source tree content exactly. Source PR/main CI and umbrella PR/main CI all pass Ubuntu/Windows/macOS × Python 3.11/3.13, and exact merged umbrella main also passes the bounded Nova ↔ LSP shared-source contract. The source contains no top-level LICENSE; that state is preserved.

## Verified integration chains

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

```text
shared legal / unresolved Nova fixtures
        ↓                         ↓
projects/Nova                projects/mini-language-server
nova check                   NovaFunctionAdapter semantic publication
        ↓                         ↓
valid acceptance / N3003     symbols + no diagnostic / nova.unresolved-name
```

The debugger chain deliberately validates address-level interoperability. Current mini-ELF output has no section-header table / `.symtab`, so symbol-level debugging is not claimed.

`tiny-tensor-compiler` is not falsely shown as directly connected to mini-ELF: its native backend currently emits C11 and builds a shared library with the host toolchain, while mini-ELF links static object/archive inputs into `ET_EXEC`. A real object/static-runtime or executable-artifact handoff is still required.

`mini-wasm-runtime` is also not forced into an artificial internal dependency graph. Its current verified composition boundary is the external WebAssembly spec/reference ecosystem: pinned conformance coverage, Wasmtime differential testing, deterministic fuzz smoke and benchmark-policy execution.

## Nova ↔ mini-language-server boundary

A bounded executable integration is now verified. Source PR #41 taught the `NovaFunctionAdapter` to consume legal Nova-style typed parameters and explicit simple return types while preserving the adapter's legacy bounded surface and snapshot/stale-result guarantees.

Both imported projects consume the exact same fixtures under `integration/nova-lsp/`: `valid.nv` must pass `nova check` and publish function/parameter/local symbols with no mini-language-server diagnostics; `unresolved.nv` must fail Nova with `N3003` and publish exactly one `nova.unresolved-name` diagnostic for `missing`. Permanent `nova-lsp-integration` CI enforces that shared contract whenever Nova, mini-language-server, the shared fixtures, or the integration workflow changes.

This is intentionally not a claim that mini-language-server implements Nova's complete grammar, type system, or production LSP surface. It proves the exact typed-function/name-resolution slice both projects currently promise.

See [docs/MIGRATION.md](docs/MIGRATION.md) for the evidence ledger and [ROADMAP.md](ROADMAP.md) for the checkpoint plan.

## Migration invariants

A project may enter this umbrella only when all applicable gates are satisfied:

1. Re-check exact source `main`, open PRs and CI immediately before migration.
2. Do not migrate a repository while an implementation PR is active.
3. Preserve source commit history and project-local documentation/licenses; do not flatten history or invent licensing metadata.
4. Audit reachable commit messages for attribution trailers before import; do not silently rewrite genuine authorship.
5. Verify the imported tree against the exact selected source commit.
6. Run native formatter/lint/tests/build gates from the new path and keep source-equivalent umbrella CI.
7. Keep original repositories available until the umbrella operating model is stable; deletion is not part of migration.
8. Do not add `Co-Authored-By`, `Generated-By`, `Assisted-By`, `Signed-off-by`, or AI/bot attribution trailers to new umbrella commits.

## Why an umbrella repository?

The individual repositories remain useful engineering artifacts, but together they form a clearer systems story: language design → parsing/type checking → compilation/linking → runtime/library support → debugging/language tooling → alternative runtimes and domain-specific compilation.

The goal is not to hide or rewrite the original development timeline. The goal is to make the architecture and relationships legible while preserving evidence.
