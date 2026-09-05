# Migration Protocol & Preflight Ledger

This document is the durable migration ledger for `compiler-runtime-lab`.

## Status vocabulary

- **READY FOR IMPORT PREP** — first-pass source audit is healthy, but the exact source head and all gates must be refreshed immediately before import.
- **ATTRIBUTION REVIEW** — source code may be healthy, but reachable history contains attribution metadata that needs an explicit preservation/rewrite decision before import.
- **HOLD** — do not import while the stated blocker is active.
- **IMPORTED / VERIFIED** — a history-preserving import whose selected source tree and umbrella tree have been verified and whose applicable tests pass from the umbrella layout.

## Current preflight / migration state — 2026-09-05 (Asia/Taipei)

| Project | Observed source `main` | Open implementation PR at last freeze/recheck | Evidence | Status |
| --- | --- | --- | --- | --- |
| Nova | `dcadc2238737b6f1e98887ab8fa658b23413d31b` | none | source CI `33920772855`; one-shot `33967555408`; full history/tree/stable/MSRV gates passed | **IMPORTED / VERIFIED — pending umbrella PR merge** |
| tiny-c-compiler | `5607c3152d319353c42f05ed44ff53479272a74f` | none at first pass | reachable commits include `Co-authored-by: github-actions[bot]` | ATTRIBUTION REVIEW |
| sic-xe-assembler | `a58f4b5c7f34675fddf437d4af596cd81b5d891f` | #30 observed during first pass | must recheck before import | **HOLD / RECHECK** |
| mini-elf-toolchain | `3d452a8681bbfb092cd41465dba6f6eb97dfd224` | none | complete reachable-history import scan + native Rust gates | **IMPORTED / VERIFIED** |
| mini-language-server | `fe4a941f11e538fcafc9255362926b0ee17cf3d4` | none at latest read-only preflight | source CI `33964865348`; full reachable-history scan still required at freeze | READY FOR IMPORT PREP |
| mini-debugger | `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a` | none | source CI `33928883989`; one-shot `33928967178` | **IMPORTED / VERIFIED** |
| mini-libc | `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1` | none | one-shot `33927869512`; permanent umbrella CI `33927981587` | **IMPORTED / VERIFIED** |
| mini-wasm-runtime | `e923b27a2652aba88d50cdbb75d0fe959d40e457` | none | one-shot `33966895454`; post-merge 7-job CI `33967377142` | **IMPORTED / VERIFIED** |
| tiny-tensor-compiler | `4690df5747a1e7fc0af9b602f8be8d963e72d00f` | none at refresh freeze | refresh one-shot `33966522648`; post-merge four-way CI `33966708522` | **IMPORTED / VERIFIED** |

### Attribution-scan rule

A GitHub commit search is only first-pass evidence. Every actual import runs a complete reachable-history scan over the exact fetched source history, including configured patterns such as:

```bash
git log --all --format='%H%x00%an%x00%ae%x00%B' \
  | grep -Eai 'co-authored-by|generated-by|assisted-by|signed-off-by|anthropic|claude|openai'
```

Matches are inspected rather than deleted blindly. Genuine authorship is not falsified. New umbrella commits do not add AI/bot attribution trailers.

## Import freeze gate

Immediately before importing a project:

1. `git fetch --all --prune`.
2. Record exact source repository URL and exact `main` SHA.
3. Confirm no active implementation PR for that repository.
4. Confirm required source CI/checks are completed and successful for the exact candidate.
5. Run the complete reachable-history attribution scan.
6. Preserve README/docs and every existing project-local license file; document license-file absence instead of inventing metadata.
7. Check for generated binaries, huge artifacts, secrets, local caches, vendored dependency accidents and temporary migration machinery.
8. Freeze the source SHA before the import.

## History-preserving import procedure

Use a real Git client. Do **not** download ZIPs or copy only the current tree.

```bash
git clone https://github.com/Lei-TzuY/compiler-runtime-lab.git
cd compiler-runtime-lab

git remote add source-project https://github.com/Lei-TzuY/<project>.git
git fetch source-project --tags

git subtree add \
  --prefix=projects/<project> \
  source-project main
```

Do not use `--squash` when the purpose is to preserve source history. `git filter-repo --to-subdirectory-filter projects/<project>` in a temporary clone is an alternative only when an explicitly documented history transformation is required.

## Verification after each import

For selected source commit `SOURCE_SHA` and migration candidate `UMBRELLA_SHA`:

1. Require `SOURCE_SHA` to remain reachable in umbrella ancestry.
2. Export/hash or enumerate the source tree and imported subtree independently.
3. Require project-file content equivalence after applying only the intentional subtree prefix.
4. Run the project's native formatter/lint/tests/build/checks from `projects/<project>`.
5. Remove only explicitly recognized build-generated files that were absent from the frozen source tree; fail on unexpected dirty paths.
6. Ensure one-shot migration workflows/scripts do not leak into the published result.
7. Perform a final source-head / umbrella-main race check before publication.
8. Merge migration PRs with a normal merge commit, never squash/rebase, so source ancestry remains reachable.
9. Re-run permanent path-scoped CI on the exact merged umbrella `main`.

## Verified imports

### mini-elf-toolchain — VERIFIED 2026-09-05

- Source: `Lei-TzuY/mini-elf-toolchain`
- Frozen source SHA: `3d452a8681bbfb092cd41465dba6f6eb97dfd224`
- First published verified umbrella main: `baa897d1f5c972b8a1854b0202de67d7c4cd2597`
- Target: `projects/mini-elf-toolchain/`
- Successful one-shot run: `33925540855`
- Complete reachable source-history attribution scan: PASS
- Non-squashed source ancestry: PASS
- Blob-for-blob tree equivalence: PASS
- `cargo fmt`, Clippy `-D warnings`, all-target/all-feature tests: PASS
- Source lacked `Cargo.lock`; the generated lockfile was recognized and removed before publication.
- One-shot workflow cleanup and final race check: PASS

### mini-libc — VERIFIED 2026-09-05

- Source: `Lei-TzuY/mini-libc`
- Frozen source SHA: `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1`
- First published verified umbrella main: `26cd23866580026bc9d72644da3fda9e25d18828`
- Target: `projects/mini-libc/`
- Successful one-shot run: `33927869512`
- Exact source CI at freeze: `33842616621`, success
- Complete reachable-history attribution scan / non-squashed ancestry / blob equivalence: PASS
- GCC and Clang `make clean test` + `make inspect`: PASS
- Pinned `tiny-c-compiler` bootstrap and source-style three-repo bootstrap with mini-ELF: PASS
- Permanent umbrella CI `33927981587`: PASS
- Frozen source has no top-level LICENSE; that absence was preserved.

### mini-debugger — VERIFIED 2026-09-05

- Source: `Lei-TzuY/mini-debugger`
- Frozen source SHA: `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a`
- Exact source CI: `33928883989`, success
- First verified migration-branch commit: `ae6f11bc676546a1e3ff178463f2284918f7e962`
- Successful one-shot run: `33928967178`
- Complete reachable-history attribution / ancestry / blob equivalence / native CMake + CTest: PASS
- Post-merge umbrella run `33929397068`: native PASS; mini-ELF integration PASS

The integration intentionally validates the current address-level boundary:

```text
projects/mini-elf-toolchain
        ↓
sectionless ELF64 ET_EXEC, entry 0x400000
        ↓
projects/mini-debugger
        ↓
launch + memory read + numeric breakpoint at 0x400001
        ↓
continue + breakpoint hit
```

Current mini-ELF output has no section-header table / `.symtab`, so symbol-level interoperability remains a future capability.

### tiny-tensor-compiler — VERIFIED / REFRESHED 2026-09-05

- Source: `Lei-TzuY/tiny-tensor-compiler`
- Initial frozen SHA: `66ff2c6d02a22c621d01579b442af8b6fd43bcc5`
- Refreshed source SHA: `4690df5747a1e7fc0af9b602f8be8d963e72d00f`
- Initial migration branch: `53a48fb97ebb8ea9139a349f1a4948fe3c5faa94`
- Initial one-shot run: `33929543578`
- Refresh one-shot run: `33966522648`
- Merged umbrella SHA: `ae03589a39a36d2df8e8220d933471722708cedc`
- Post-merge Ubuntu/Windows × Python 3.11/3.13 run: `33966708522`, four jobs PASS
- The 26 newly reachable source commits were separately attribution-scanned before the non-squashed refresh.
- Source ancestry and blob-for-blob equivalence remained intact.
- `ruff` and `pytest` passed from the umbrella path.
- Source has no top-level LICENSE; that state is preserved.

No direct mini-ELF integration is claimed. The native backend currently emits C11, uses the host toolchain to build a shared library, and loads it through `ctypes`; mini-ELF currently links static object/archive input into `ET_EXEC`.

### mini-wasm-runtime — VERIFIED 2026-09-05

- Source: `Lei-TzuY/mini-wasm-runtime`
- Frozen source SHA: `e923b27a2652aba88d50cdbb75d0fe959d40e457`
- Source core CI: `33892678967`, stable Ubuntu/Windows/macOS + Rust 1.81 Ubuntu PASS
- Source benchmark smoke: `33892678903`, PASS
- Source differential reference: `33892679049`, PASS
- Successful one-shot run: `33966895454`
- Merged umbrella SHA: `e7b1df4734d6c5b4b04c2e6a99de424932c2079f`
- Post-merge permanent 7-job CI: `33967377142`, PASS
- Complete reachable-history attribution scan / non-squashed ancestry / blob-for-blob equivalence: PASS
- Stable and Rust 1.81 formatter/Clippy/tests/docs: PASS
- Wasmtime differential reference: PASS
- Deterministic benchmark-policy smoke: PASS
- Deterministic parser + parse/validation fuzz smoke, reviewed corpus replay and coverage rendering: PASS
- The source 15-minute coverage-guided fuzz campaign remains scheduled/manual and is not misrepresented as a per-PR gate.
- Source top-level LICENSE is preserved.

### Nova — VERIFIED MIGRATION CANDIDATE 2026-09-05

- Source: `Lei-TzuY/Nova`
- Frozen source SHA: `dcadc2238737b6f1e98887ab8fa658b23413d31b`
- Exact source CI: `33920772855`, PASS
- Source open implementation PRs at freeze/recheck: `0`
- Successful one-shot run: `33967555408`
- First published verified migration-branch commit after one-shot cleanup: `e725af64a9f05fd37acb51b969b01f635c20c37b`
- Target: `projects/Nova/`
- Complete reachable source-history attribution scan: PASS
- Non-squashed source ancestry: PASS
- Blob-for-blob source-tree vs imported-subtree comparison: PASS
- Stable `cargo fmt --all -- --check`: PASS
- Stable `cargo clippy --workspace --all-targets --all-features --locked -- -D warnings`: PASS
- Stable workspace tests, all-target build and rustdoc with warnings denied: PASS
- Rust 1.85 MSRV workspace/all-target check: PASS
- Clean tracked tree, one-shot self-removal and final source/main/branch race check: PASS
- Frozen source has README/docs/examples, `Cargo.lock`, and toolchain metadata but no top-level LICENSE; that state is preserved exactly.
- Permanent `.github/workflows/nova.yml` mirrors Nova's source rustfmt, Clippy, MSRV, tests, build and docs gates from the umbrella path.

Nova is not yet claimed as integrated with `mini-language-server`. That project must first complete its own history-preserving import. The subsequent integration should test the bounded Nova syntax/semantic/diagnostic surface that the existing language-server adapter actually supports, not claim a complete production Nova LSP.

## Seven-project checkpoint plan

The checkpoint is reached after `mini-language-server` becomes the seventh imported/verified project and the Nova ↔ language-server bounded semantic/diagnostic integration is green. At that point:

- verify zero open migration PRs;
- verify all path-scoped workflows on exact umbrella `main`;
- freeze README/manifest/ledger evidence;
- keep `tiny-c-compiler` and `sic-xe-assembler` as explicit exceptions if their blockers remain;
- begin the separate `systems-lab` consolidation rather than extending this migration indefinitely.

## Source repository retirement policy

After a project reaches **IMPORTED / VERIFIED**:

- keep the original repository available while external links, issues, PRs and releases still matter;
- add a prominent canonical-path notice only after the umbrella import and ongoing CI strategy are stable;
- archive only after the umbrella version is stable and the redirect is clear;
- do not delete the original repository as part of routine portfolio consolidation.
