# Compiler & Runtime Lab

A portfolio-oriented umbrella repository for a family of compiler, runtime, toolchain, language-tooling, debugging, and tensor-compilation projects.

This repository is intentionally being assembled with **history-preserving migration**. Original repositories remain available while each subtree is migrated, verified, and placed under umbrella CI. No source repository is deleted as part of this process.

## Project map

| Project | Role | Migration status |
| --- | --- | --- |
| [Nova](https://github.com/Lei-TzuY/Nova) | Typed language, semantic analysis, interpreter/runtime | READY FOR IMPORT PREP |
| [tiny-c-compiler](https://github.com/Lei-TzuY/tiny-c-compiler) | Self-contained x86-64 C compiler | ATTRIBUTION REVIEW |
| [sic-xe-assembler](https://github.com/Lei-TzuY/sic-xe-assembler) | SIC/XE assembler and static-analysis tooling | HOLD — open implementation PR |
| [mini-elf-toolchain](projects/mini-elf-toolchain) | ELF/static-linking toolchain | **IMPORTED / VERIFIED** |
| [mini-language-server](https://github.com/Lei-TzuY/mini-language-server) | Version-safe semantic/LSP tooling | READY FOR IMPORT PREP |
| [mini-debugger](https://github.com/Lei-TzuY/mini-debugger) | ptrace-based debugger | READY FOR IMPORT PREP |
| [mini-libc](https://github.com/Lei-TzuY/mini-libc) | Freestanding libc subset and bootstrap target | READY FOR IMPORT PREP |
| [mini-wasm-runtime](https://github.com/Lei-TzuY/mini-wasm-runtime) | WebAssembly parser, validator, runtime and conformance lab | READY FOR IMPORT PREP |
| [tiny-tensor-compiler](https://github.com/Lei-TzuY/tiny-tensor-compiler) | Tensor IR, optimization and native compilation | READY FOR IMPORT PREP |

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
    ├── Nova/                    # planned
    ├── tiny-c-compiler/         # attribution review
    ├── sic-xe-assembler/        # hold: active PR
    ├── mini-language-server/    # planned
    ├── mini-debugger/           # planned
    ├── mini-libc/               # planned
    ├── mini-wasm-runtime/       # planned
    └── tiny-tensor-compiler/    # planned
```

A planned project directory is created only by a verified history-preserving import. Do not replace this process with ZIP/download-and-copy commits.

## First verified import

`mini-elf-toolchain` is the reference migration. Source SHA `3d452a8681bbfb092cd41465dba6f6eb97dfd224` was imported without squashing into `projects/mini-elf-toolchain/`. The source commit remains reachable in umbrella history, the subtree was verified blob-for-blob against the frozen source tree, its native Rust formatter/Clippy/test gates passed from the umbrella path, and a permanent path-scoped umbrella workflow now protects future changes.

See [docs/MIGRATION.md](docs/MIGRATION.md) for the full evidence ledger.

## Migration invariants

A project may enter this umbrella only when all applicable gates are satisfied:

1. Re-check the exact source `main`, open PRs, recent commits, CI/checks, and default branch immediately before migration.
2. Do not migrate a repository while an implementation PR is active on that repository.
3. Preserve source commit history and project-local license/documentation; do not flatten history into a single copy commit.
4. Audit reachable commit messages for attribution trailers before import. Known attribution must not be silently imported or silently rewritten.
5. Verify the imported project tree against the exact source commit selected for migration.
6. Run the imported project's formatter/lint/tests/build/CI from its new path and keep equivalent umbrella CI afterward.
7. Keep the original repository available until migration and the umbrella operating model are stable. Archival/redirect is a separate final step; deletion is not part of the migration plan.
8. Do not add `Co-Authored-By`, `Generated-By`, `Assisted-By`, `Signed-off-by`, or AI/bot attribution trailers to new umbrella commits.

## Why an umbrella repository?

The individual repositories remain useful engineering artifacts, but together they form a clearer systems story: language design → parsing/type checking → compilation/linking → runtime/library support → debugging/language tooling → alternative runtimes and domain-specific compilation.

The goal is not to hide or rewrite the original development timeline. The goal is to make the architecture and relationships between projects legible while preserving evidence.

See [ROADMAP.md](ROADMAP.md) for the integration order and [docs/MIGRATION.md](docs/MIGRATION.md) for the migration protocol and current preflight ledger.
