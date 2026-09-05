# Compiler & Runtime Lab Roadmap

## Phase 0 — Umbrella bootstrap

- [x] Create public `compiler-runtime-lab` repository.
- [x] Define the umbrella architecture and candidate project set.
- [x] Define history-preserving migration invariants.
- [x] Create a live migration ledger.

## Phase 1 — Source preflight and freeze points

- [x] Nova — frozen at `dcadc2238737b6f1e98887ab8fa658b23413d31b`; exact source/open-PR/CI, complete reachable-history attribution, tree equivalence, stable gates and Rust 1.85 MSRV passed.
- [ ] tiny-c-compiler — resolve/accept bot co-author history policy before import.
- [ ] sic-xe-assembler — recheck and resolve any active implementation PR before migration.
- [x] mini-elf-toolchain — frozen at `3d452a8681bbfb092cd41465dba6f6eb97dfd224`.
- [x] mini-language-server — frozen at `f8a4d642eaa721741ab3cea7eb02d2f261dbad01`; exact source CI `33967371120`, zero open PRs, complete reachable-history attribution, non-squashed ancestry, tree equivalence and Python gates passed in one-shot `33968415924`.
- [x] mini-debugger — frozen at `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a`.
- [x] mini-libc — frozen at `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1`.
- [x] mini-wasm-runtime — frozen at `e923b27a2652aba88d50cdbb75d0fe959d40e457`.
- [x] tiny-tensor-compiler — synchronized through `4690df5747a1e7fc0af9b602f8be8d963e72d00f`.

## Phase 2 — History-preserving imports

1. [x] `mini-elf-toolchain` — **IMPORTED / VERIFIED**
2. [x] `mini-libc` — **IMPORTED / VERIFIED**
3. [x] `mini-debugger` — **IMPORTED / VERIFIED**
4. [x] `tiny-tensor-compiler` — **IMPORTED / VERIFIED**
5. [x] `mini-wasm-runtime` — **IMPORTED / VERIFIED**
6. [x] `Nova` — **IMPORTED / VERIFIED**
7. [x] `mini-language-server` — **IMPORTED / VERIFIED CANDIDATE**; permanent six-way matrix pending PR/main proof
8. [ ] `tiny-c-compiler` after attribution policy is resolved
9. [ ] `sic-xe-assembler` after its active-PR state is rechecked and any blocker is resolved

The first seven entries define the compiler/runtime flagship checkpoint. `tiny-c-compiler` and `sic-xe-assembler` remain explicit exceptions rather than blocking the checkpoint indefinitely.

Seven projects now preserve source history and selected source trees in the umbrella candidate. The seventh is not final until its PR-head and merged-main six-way source-equivalent matrix are green.

## Phase 3 — Umbrella integration

- [x] add path-scoped CI for imported projects without weakening project-local gates;
- [x] establish `tiny-c-compiler` → `mini-libc` → `mini-elf-toolchain` → executable bootstrap validation;
- [x] establish `mini-elf-toolchain` → sectionless ET_EXEC → `mini-debugger` address-level debugging validation;
- [x] preserve mini-wasm's external conformance boundary with Wasmtime differential, benchmark-policy and deterministic fuzz smoke;
- [x] import Nova as an independently verifiable language/compiler/runtime subtree;
- [x] history-preserve and independently verify mini-language-server;
- [ ] close the discovered Nova ↔ mini-language-server grammar gap, then add a genuine shared-input semantic/diagnostic integration regression;
- [ ] document the full cross-project dependency graph at the seven-project checkpoint;
- [ ] define a common top-level developer entrypoint without forcing projects into one build system.

### Current verified chains

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

`tiny-tensor-compiler` still needs a deliberate object/static-runtime or executable-artifact contract before any direct mini-ELF edge is claimed.

`mini-wasm-runtime` remains a runtime/conformance island whose verified boundary is the WebAssembly spec/reference ecosystem rather than an invented internal dependency.

### Nova ↔ mini-language-server gap

A real integration audit found that imported Nova's normative grammar requires:

```text
fn name(parameter: Type) -> ReturnType { ... }
```

The frozen mini-language-server `NovaFunctionAdapter` currently recognizes only an intentionally bounded form equivalent to:

```text
fn name(parameter) { ... }
```

It already has function/local/reference semantics and deterministic `nova.unresolved-name` diagnostics, but the declaration/parameter grammar does not yet consume legal typed Nova function signatures. The next correctness slice is therefore concrete: update the adapter to parse typed parameters + explicit return type without weakening stale-result/snapshot guarantees, add regressions against real imported Nova snippets, and only then mark the Nova → mini-language-server edge verified.

## Phase 4 — Seven-project checkpoint

After mini-language-server PR/main CI and the grammar-integration slice are green:

- [ ] freeze the seven-project checkpoint in README/manifest/ledger;
- [ ] verify umbrella `main` has zero open migration/integration PRs and applicable workflows are green;
- [ ] record the bounded Nova ↔ mini-language-server semantic/diagnostic contract;
- [ ] keep `tiny-c-compiler` and `sic-xe-assembler` as explicit pending exceptions if their blockers remain;
- [ ] begin Phase 0 for `systems-lab` rather than extending compiler migration indefinitely.

## Phase 5 — Portfolio consolidation

- [ ] update source repository READMEs with canonical umbrella paths after the checkpoint is stable;
- [ ] decide whether each original repository should remain active or become archived/read-only;
- [ ] preserve original repositories for PR/issues/releases/history references;
- [ ] never delete an original repository merely to make the portfolio look cleaner;
- [ ] update profile/portfolio documentation to present this repository as the compiler/runtime flagship.

## Non-goals

- Rewriting genuine authorship or dates to manufacture a cleaner timeline.
- Flattening every project into a single framework or build system.
- Importing repositories while implementation PRs are active.
- Treating an umbrella repository as a dumping ground for unrelated projects.
- Claiming cross-project interoperability that the executable tests do not prove.
