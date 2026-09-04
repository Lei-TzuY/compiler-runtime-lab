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
- [ ] mini-elf-toolchain — refresh exact green source head and freeze import point.
- [ ] mini-language-server — refresh exact green source head and freeze import point.
- [ ] mini-debugger — refresh exact green source head and freeze import point.
- [ ] mini-libc — refresh exact green source head and freeze import point.
- [ ] mini-wasm-runtime — refresh all applicable CI/fuzz/differential gates and freeze import point.
- [ ] tiny-tensor-compiler — refresh exact green source head and freeze import point.

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

Suggested import order:

1. `mini-elf-toolchain`
2. `mini-libc`
3. `mini-debugger`
4. `tiny-tensor-compiler`
5. `mini-wasm-runtime`
6. `mini-language-server`
7. `Nova`
8. `tiny-c-compiler` after attribution policy is resolved
9. `sic-xe-assembler` after its active PR is resolved

The order is intentionally conservative: start with smaller, independently verifiable repositories, then migrate projects with stronger cross-project coupling or live-development blockers.

For every imported project:

- [ ] preserve reachable commit history;
- [ ] preserve project-local license and documentation;
- [ ] record exact source repository and source commit;
- [ ] verify imported tree equivalence at the selected source commit;
- [ ] verify no migration-only files leaked into the project tree;
- [ ] run project tests/build/checks from the umbrella layout;
- [ ] record verification evidence in `docs/MIGRATION.md`.

## Phase 3 — Umbrella integration

After at least two verified imports:

- [ ] add path-scoped CI orchestration without weakening project-local gates;
- [ ] document cross-project dependency relationships;
- [ ] add reproducible cross-project bootstrap/integration tests where they already exist conceptually;
- [ ] define a common top-level developer entrypoint without forcing projects into one build system;
- [ ] preserve project independence: each subtree should remain understandable and testable on its own.

Potential cross-project chains include:

```text
Nova → mini-language-server

tiny-c-compiler → mini-libc → mini-elf-toolchain → executable

mini-elf-toolchain → mini-debugger

mini-wasm-runtime → conformance/differential reference tooling

tiny-tensor-compiler → native generated code/toolchain boundary
```

## Phase 4 — Portfolio consolidation

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
