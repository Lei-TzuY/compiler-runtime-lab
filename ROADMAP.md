# Compiler & Runtime Lab Roadmap

## Phase 0 — Umbrella bootstrap

- [x] Create public `compiler-runtime-lab` repository.
- [x] Define the umbrella architecture and candidate project set.
- [x] Define history-preserving migration invariants.
- [x] Create a live migration ledger.

## Phase 1 — Source preflight and freeze points

Before importing any project, establish an exact source commit and verify it again immediately before migration.

- [ ] Nova — refresh exact `main`, open PRs, CI, attribution and repository hygiene.
- [ ] tiny-c-compiler — resolve/accept bot co-author history policy before import.
- [ ] sic-xe-assembler — finish or close the active implementation PR before migration.
- [x] mini-elf-toolchain — frozen at `3d452a8681bbfb092cd41465dba6f6eb97dfd224`; exact source/open-PR/CI/attribution gates passed immediately before import.
- [ ] mini-language-server — refresh exact green source head and freeze import point.
- [x] mini-debugger — frozen at `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a`; exact source/open-PR/CI state refreshed immediately before import and complete reachable-history attribution scan passed.
- [x] mini-libc — frozen at `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1`; exact source/open-PR/CI/attribution gates passed immediately before import.
- [x] mini-wasm-runtime — frozen at `e923b27a2652aba88d50cdbb75d0fe959d40e457`; exact core CI, benchmark, differential, full reachable-history attribution, source-tree equivalence, stable and MSRV gates passed before publication.
- [x] tiny-tensor-compiler — synchronized through `4690df5747a1e7fc0af9b602f8be8d963e72d00f`; the initial import plus a 26-commit non-squashed refresh preserved ancestry/tree equivalence and passed source-equivalent umbrella CI.

A checked item here means a migration-ready **freeze point**, not merely that a repository looked healthy during an earlier audit.

## Phase 2 — History-preserving imports

Target paths:

```text
projects/Nova/
projects/tiny-c-compiler/
projects/sic-xe-assembler/
projects/mini-elf-toolchain/
projects/mini-language-server/
projects/mini-debugger/
projects/mini-libc/
projects/mini-wasm-runtime/
projects/tiny-tensor-compiler/
```

Execution order:

1. [x] `mini-elf-toolchain` — **IMPORTED / VERIFIED**
2. [x] `mini-libc` — **IMPORTED / VERIFIED**
3. [x] `mini-debugger` — **IMPORTED / VERIFIED**
4. [x] `tiny-tensor-compiler` — **IMPORTED / VERIFIED**
5. [x] `mini-wasm-runtime` — **IMPORTED / VERIFIED**
6. [ ] `Nova`
7. [ ] `mini-language-server` immediately after Nova so their semantic boundary can be tested honestly
8. [ ] `tiny-c-compiler` after attribution policy is resolved
9. [ ] `sic-xe-assembler` after its active-PR state is rechecked and any blocker is resolved

The first seven entries define the planned flagship checkpoint. `tiny-c-compiler` and `sic-xe-assembler` are allowed to remain explicit exceptions rather than blocking the checkpoint indefinitely.

For each remaining import:

- [ ] preserve reachable commit history;
- [ ] preserve existing project-local license files and documentation, or explicitly document license-file absence;
- [ ] record exact source repository and source commit;
- [ ] verify imported tree equivalence at the selected source commit;
- [ ] verify no migration-only files leaked into the project tree;
- [ ] run project tests/build/checks from the umbrella layout;
- [ ] record verification evidence in `docs/MIGRATION.md`.

Five imports now satisfy the reference process: `mini-elf-toolchain`, `mini-libc`, `mini-debugger`, `tiny-tensor-compiler`, and `mini-wasm-runtime` retain their selected source commits in umbrella ancestry, match their source trees, pass applicable project-native gates from the umbrella layout, and have permanent path-scoped CI.

## Phase 3 — Umbrella integration

The umbrella is now actively testing composition rather than only storing independent subtrees.

- [x] add path-scoped CI for imported projects without weakening project-local gates;
- [x] establish the first real cross-project bootstrap edge: pinned `tiny-c-compiler` → imported `mini-libc` → imported `mini-elf-toolchain` → executable;
- [x] establish the second real cross-project edge: imported `mini-elf-toolchain` → sectionless ET_EXEC → imported `mini-debugger` → memory read + numeric breakpoint;
- [x] preserve mini-wasm's external conformance boundary with Wasmtime differential testing, benchmark-policy smoke and deterministic parser fuzz smoke;
- [ ] import Nova, then mini-language-server, and define only the semantic/diagnostic interoperability their real contracts support;
- [ ] document the full cross-project dependency graph as additional projects are imported;
- [ ] define a common top-level developer entrypoint without forcing projects into one build system;
- [ ] preserve project independence: each subtree should remain understandable and testable on its own.

Current verified chains:

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
sectionless ELF64 ET_EXEC (entry 0x400000)
        ↓
projects/mini-debugger
        ↓
ptrace launch + entry-byte read
        ↓
numeric software breakpoint at 0x400001
        ↓
continue + breakpoint hit
```

The second chain deliberately does not claim symbol-level interoperability yet. Current mini-ELF executables have no section-header table or `.symtab`; symbol-level debugging becomes valid only after the linker emits suitable metadata or an explicit metadata handoff is designed.

`tiny-tensor-compiler` is imported and continuously verified, but it does not yet participate in a fabricated toolchain edge. Its native backend emits C11 and builds host-toolchain shared libraries loaded through `ctypes`, while `mini-elf-toolchain` currently provides a static-object/archive-to-ET_EXEC path. A future cross-project milestone must deliberately define an object/static-runtime handoff or another executable artifact contract before claiming direct interoperability.

`mini-wasm-runtime` is intentionally modeled as a separate runtime/conformance island rather than forced into an artificial dependency graph. Its current verified external boundary is:

```text
projects/mini-wasm-runtime
        ↓
pinned/spec-derived conformance coverage
        ↓
Wasmtime differential reference
        +
deterministic fuzz smoke / benchmark-policy smoke
```

Remaining planned relationships include:

```text
Nova → mini-language-server

tiny-tensor-compiler → explicit object/static-runtime or executable-artifact handoff
```

## Phase 4 — Seven-project checkpoint

After `Nova` and `mini-language-server` are imported and their applicable CI/integration is green:

- [ ] freeze a seven-project checkpoint in README/ledger;
- [ ] verify umbrella `main` has zero open migration PRs and all path-scoped workflows are green;
- [ ] keep `tiny-c-compiler` and `sic-xe-assembler` as explicit pending exceptions if their blockers remain;
- [ ] begin Phase 0 for `systems-lab` rather than extending compiler migration indefinitely.

## Phase 5 — Portfolio consolidation

Only after imports and umbrella CI are verified:

- [ ] update source repository READMEs with canonical umbrella paths;
- [ ] decide whether each original repository should remain active or become archived/read-only;
- [ ] preserve original repositories for PR/issues/releases/history references;
- [ ] never delete an original repository merely to make the portfolio look cleaner;
- [ ] update profile/portfolio documentation to present this repository as the compiler/runtime flagship.

## Non-goals

- Rewriting genuine authorship or dates to manufacture a cleaner timeline.
- Flattening every project into a single framework or build system.
- Importing repositories while implementation PRs are active.
- Treating an umbrella repository as a dumping ground for unrelated projects.
- Hiding unsupported claims, failed experiments, or historical engineering decisions.
