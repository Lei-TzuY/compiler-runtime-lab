# Compiler & Runtime Lab

A portfolio-oriented umbrella repository for a family of compiler, runtime, toolchain, language-tooling, debugging, and tensor-compilation projects.

This repository is intentionally being assembled with **history-preserving migration**. The original repositories remain the source of truth until each migration passes its preflight and verification gates. No source repository is deleted as part of this process.

## Project map

| Project | Role | Migration status |
| --- | --- | --- |
| [Nova](https://github.com/Lei-TzuY/Nova) | Typed language, semantic analysis, interpreter/runtime | READY FOR IMPORT PREP |
| [tiny-c-compiler](https://github.com/Lei-TzuY/tiny-c-compiler) | Self-contained x86-64 C compiler | ATTRIBUTION REVIEW |
| [sic-xe-assembler](https://github.com/Lei-TzuY/sic-xe-assembler) | SIC/XE assembler and static-analysis tooling | HOLD — open implementation PR |
| [mini-elf-toolchain](https://github.com/Lei-TzuY/mini-elf-toolchain) | ELF/static-linking toolchain | READY FOR IMPORT PREP |
| [mini-language-server](https://github.com/Lei-TzuY/mini-language-server) | Version-safe semantic/LSP tooling | READY FOR IMPORT PREP |
| [mini-debugger](https://github.com/Lei-TzuY/mini-debugger) | ptrace-based debugger | READY FOR IMPORT PREP |
| [mini-libc](https://github.com/Lei-TzuY/mini-libc) | Freestanding libc subset and bootstrap target | READY FOR IMPORT PREP |
| [mini-wasm-runtime](https://github.com/Lei-TzuY/mini-wasm-runtime) | WebAssembly parser, validator, runtime and conformance lab | READY FOR IMPORT PREP |
| [tiny-tensor-compiler](https://github.com/Lei-TzuY/tiny-tensor-compiler) | Tensor IR, optimization and native compilation | READY FOR IMPORT PREP |

## Intended layout

```text
compiler-runtime-lab/
├── README.md
├── ROADMAP.md
├── docs/
│   └── MIGRATION.md
└── projects/
    ├── Nova/
    ├── tiny-c-compiler/
    ├── sic-xe-assembler/
    ├── mini-elf-toolchain/
    ├── mini-language-server/
    ├── mini-debugger/
    ├── mini-libc/
    ├── mini-wasm-runtime/
    └── tiny-tensor-compiler/
```

The `projects/` directories are **not** populated until their source history is imported and verified. Do not replace this process with ZIP/download-and-copy commits.

## Migration invariants

A project may enter this umbrella only when all applicable gates are satisfied:

1. Re-check the exact source `main`, open PRs, recent commits, CI/checks, and default branch immediately before migration.
2. Do not migrate a repository while an implementation PR is active on that repository.
3. Preserve the source commit history and project-local license/documentation; do not flatten history into a single copy commit.
4. Audit reachable commit messages for attribution trailers before import. Known AI attribution must not be silently imported or silently rewritten.
5. Verify the imported project tree against the exact source commit selected for migration.
6. Run the imported project's formatter/lint/tests/build/CI from its new path or through an umbrella orchestration layer.
7. Keep the original repository available until the umbrella import is verified. Archival/redirect is a separate final step; deletion is not part of the migration plan.
8. Do not add `Co-Authored-By`, `Generated-By`, `Assisted-By`, `Signed-off-by`, or AI/bot attribution trailers to new umbrella commits.

## Why an umbrella repository?

The individual repositories remain useful engineering artifacts, but together they form a clearer systems story: language design → parsing/type checking → compilation/linking → runtime/library support → debugging/language tooling → alternative runtimes and domain-specific compilation.

The goal is not to hide or rewrite the original development timeline. The goal is to make the architecture and relationships between projects legible while preserving evidence.

See [ROADMAP.md](ROADMAP.md) for the integration order and [docs/MIGRATION.md](docs/MIGRATION.md) for the migration protocol and current preflight ledger.
