# Compiler & Runtime Lab Roadmap

## Phase 0 — Umbrella bootstrap

- [x] Create public `compiler-runtime-lab` repository.
- [x] Define the umbrella architecture and candidate project set.
- [x] Define history-preserving migration invariants.
- [x] Create a live migration ledger.

## Phase 1 — Source preflight and freeze points

- [x] Nova — frozen at `dcadc2238737b6f1e98887ab8fa658b23413d31b`; exact source/open-PR/CI, complete reachable-history attribution, tree equivalence, stable gates and Rust 1.85 MSRV passed.
- [x] tiny-c-compiler — frozen at `5607c3152d319353c42f05ed44ff53479272a74f`; 272-commit provenance audit accepts exactly 24 historical GitHub Actions bot co-author trailers as a preserved legacy exception, with zero other attribution/AI markers; exact source and imported-main gates are green.
- [x] sic-xe-assembler — frozen at `a58f4b5c7f34675fddf437d4af596cd81b5d891f`; stale PR #30 closed unmerged after supersession audit, source freeze audit passed, and exact imported-main verification is green.
- [x] mini-elf-toolchain — frozen at `3d452a8681bbfb092cd41465dba6f6eb97dfd224`.
- [x] mini-language-server — initially imported at `f8a4d642eaa721741ab3cea7eb02d2f261dbad01` and non-squashed-refreshed through `ab22b04e596f0a9b45441c7b0a3a6ff0b79b20a8`; source PR/main six-way CI, refresh ancestry/tree equivalence, umbrella six-way CI, and shared Nova/LSP integration all passed.
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
7. [x] `mini-language-server` — **IMPORTED / VERIFIED**; source refresh and exact merged-main six-way + Nova/LSP integration proof complete
8. [x] `tiny-c-compiler` — **IMPORTED / VERIFIED**; canonical history preserved without rewrite, legacy CI-bot provenance exception machine-checked, GCC/Clang/sanitizer gates green
9. [x] `sic-xe-assembler` — **IMPORTED / VERIFIED**; preserved source ancestry/tree identity and exact merged-main four-way Python verification complete

The original first-seven flagship checkpoint and the later SIC/XE eight-project checkpoint remain historical milestones. `tiny-c-compiler` has now resolved the final migration exception through an explicit preserve-not-rewrite provenance policy.

All nine planned projects now preserve source history and verified source trees in the umbrella. Existing executable integration claims remain bounded to the evidence actually exercised by their workflows; importing Tiny-C does not by itself rewire the previously pinned Tiny-C → mini-libc chain.

## Phase 3 — Umbrella integration

- [x] add path-scoped CI for imported projects without weakening project-local gates;
- [x] establish `tiny-c-compiler` → `mini-libc` → `mini-elf-toolchain` → executable bootstrap validation;
- [x] establish `mini-elf-toolchain` → sectionless ET_EXEC → `mini-debugger` address-level debugging validation;
- [x] preserve mini-wasm's external conformance boundary with Wasmtime differential, benchmark-policy and deterministic fuzz smoke;
- [x] import Nova as an independently verifiable language/compiler/runtime subtree;
- [x] history-preserve and independently verify mini-language-server;
- [x] close the discovered Nova ↔ mini-language-server typed-signature grammar gap and enforce a genuine shared-input semantic/diagnostic integration regression;
- [ ] document the full cross-project dependency graph beyond the currently verified executable edges;
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

```text
shared Nova fixtures
        ↓                 ↓
projects/Nova        projects/mini-language-server
        ↓                 ↓
nova check           semantic + diagnostic publication
```

`tiny-tensor-compiler` still needs a deliberate object/static-runtime or executable-artifact contract before any direct mini-ELF edge is claimed.

`mini-wasm-runtime` remains a runtime/conformance island whose verified boundary is the WebAssembly spec/reference ecosystem rather than an invented internal dependency.

### Nova ↔ mini-language-server verified contract

The original typed-signature mismatch is closed. The imported adapter now consumes the bounded legal Nova form `fn name(parameter: Type) -> ReturnType { ... }` for simple identifier/never surface types. Shared fixtures under `integration/nova-lsp/` are consumed by both projects: the legal fixture passes Nova and produces no LSP diagnostic, while the unresolved fixture produces Nova `N3003` and mini-language-server `nova.unresolved-name`.

The contract remains intentionally bounded; full Nova grammar/type-system/LSP parity is not claimed.

## Phase 4 — Seven-project checkpoint

The seven-project checkpoint is complete:

- [x] freeze the seven-project checkpoint in README/manifest/ledger;
- [x] verify umbrella `main` has zero open migration/integration PRs and applicable workflows are green;
- [x] record and execute the bounded Nova ↔ mini-language-server semantic/diagnostic contract;
- [x] keep `tiny-c-compiler` and `sic-xe-assembler` as explicit pending exceptions rather than blocking the flagship checkpoint;
- [x] hand off the next consolidation phase to `systems-lab` instead of extending compiler migration indefinitely.

## Phase 4A — Eight-project checkpoint

The post-flagship SIC/XE checkpoint is complete:

- [x] close stale SIC/XE PR #30 without merging superseded duplicate architecture;
- [x] freeze `sic-xe-assembler@a58f4b5c7f34675fddf437d4af596cd81b5d891f` with exact source CI and full reachable-history/hygiene audit;
- [x] import SIC/XE with preserved ancestry and exact tree identity;
- [x] verify Ubuntu/Windows × Python 3.10/3.13 plus Linux golden fixtures on PR head and exact merged umbrella main;
- [x] leave `tiny-c-compiler` as the sole explicit pending attribution-policy exception.

## Phase 4B — Nine-project checkpoint

The planned compiler/runtime migration set is complete:

- [x] freeze `tiny-c-compiler@5607c3152d319353c42f05ed44ff53479272a74f` with exact source CI and zero open source PRs;
- [x] audit all 272 canonical commits and preserve exactly 24 historical `github-actions[bot]` co-author trailers as an explicit legacy provenance exception;
- [x] verify zero Generated-By, Assisted-By, Signed-off-by, Claude, Anthropic, OpenAI or other attribution hits;
- [x] import Tiny-C with exact source ancestry and tree identity;
- [x] pass provenance, GCC, Clang and ASan+UBSan gates on the import PR and exact merged umbrella main;
- [x] reach 9/9 history-preserved, independently verified planned imports without rewriting genuine source history.

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
