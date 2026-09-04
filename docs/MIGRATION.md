# Migration Protocol & Preflight Ledger

This document is the durable migration ledger for `compiler-runtime-lab`.

## Status vocabulary

- **READY FOR IMPORT PREP** — first-pass source audit is healthy, but the exact source head and all gates must be refreshed immediately before import.
- **ATTRIBUTION REVIEW** — source code may be healthy, but reachable history contains attribution metadata that needs an explicit preservation/rewrite decision before import.
- **HOLD** — do not import while the stated blocker is active.
- **IMPORTED / VERIFIED** — a history-preserving import whose selected source tree and umbrella tree have been verified and whose applicable tests pass from the umbrella layout.

## First-pass preflight — 2026-09-05 (Asia/Taipei)

| Project | Observed source `main` | Open implementation PR | Latest observed main workflow | Attribution search | Status |
| --- | --- | --- | --- | --- | --- |
| Nova | `dcadc2238737b6f1e98887ab8fa658b23413d31b` | none | CI completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP |
| tiny-c-compiler | `5607c3152d319353c42f05ed44ff53479272a74f` | none | Tests completed / success | reachable commits include `Co-authored-by: github-actions[bot]` | ATTRIBUTION REVIEW |
| sic-xe-assembler | `a58f4b5c7f34675fddf437d4af596cd81b5d891f` | **#30 open** — cross-domain memory/register CFG fixed point | Tests completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | **HOLD** |
| mini-elf-toolchain | `3d452a8681bbfb092cd41465dba6f6eb97dfd224` | none | CI completed / success | complete reachable-history import scan passed | **IMPORTED / VERIFIED** |
| mini-language-server | `74ecddce7061abc6cae467728d12599f3403c28c` | none | CI completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP |
| mini-debugger | `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a` | none | CI `33928883989` completed / success | complete reachable-history import scan passed | **IMPORTED / VERIFIED** |
| mini-libc | `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1` | none | CI completed / success | complete reachable-history import scan passed | **IMPORTED / VERIFIED** |
| mini-wasm-runtime | `e923b27a2652aba88d50cdbb75d0fe959d40e457` | none | latest observed main workflow (`Differential reference smoke`) completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP; refresh all CI/fuzz/differential gates before freeze |
| tiny-tensor-compiler | `66ff2c6d02a22c621d01579b442af8b6fd43bcc5` | none | CI `33928929309` completed / success | complete reachable-history import scan passed | **IMPORTED / VERIFIED** |

### Important limitations of the first-pass attribution search

The GitHub commit search is evidence, not proof of absence. Every actual import must run a complete reachable-history scan over the exact fetched source history, including at least:

```bash
git log --all --format='%H%x00%an%x00%ae%x00%B' \
  | grep -Eai 'co-authored-by|generated-by|assisted-by|signed-off-by|anthropic|claude|openai'
```

Inspect matches rather than deleting them blindly. Genuine authorship must not be falsified. If attribution was erroneous metadata and a history rewrite is deliberately chosen, document the rewrite and verify tree equivalence separately.

## Import freeze gate

Immediately before importing a project:

1. `git fetch --all --prune`
2. Record exact source repository URL and exact `main` SHA.
3. Confirm no active implementation PR for that repository.
4. Confirm all required source CI/checks are completed and successful for the exact source candidate.
5. Run a complete reachable-history attribution scan locally or in the guarded migration runner.
6. Confirm README/documentation are present, preserve every existing project-local license file, and explicitly document license-file absence rather than inventing licensing metadata.
7. Check for generated binaries, huge artifacts, secrets, local caches, vendored dependency accidents, and temporary migration workflows/scripts.
8. Freeze the source SHA in this ledger before performing the import.

## Recommended history-preserving import procedure

Use a real Git client. Do **not** download ZIPs or copy only the current tree.

```bash
git clone https://github.com/Lei-TzuY/compiler-runtime-lab.git
cd compiler-runtime-lab

git remote add source-mini-elf https://github.com/Lei-TzuY/mini-elf-toolchain.git
git fetch source-mini-elf --tags

git subtree add \
  --prefix=projects/mini-elf-toolchain \
  source-mini-elf main
```

Do not use `--squash` when the purpose is to preserve source history. `git filter-repo --to-subdirectory-filter projects/<project>` in a temporary clone is an alternative when an explicitly documented history transformation is required.

## Verification after each import

For the selected source commit `SOURCE_SHA` and imported umbrella commit `UMBRELLA_SHA`:

1. Require `SOURCE_SHA` to remain reachable in umbrella ancestry.
2. Export/hash or enumerate the source tree and imported subtree independently.
3. Exclude only the intentional subtree path prefix.
4. Require content equivalence for project files.
5. Run the project's native formatter/lint/tests/build gates from `projects/<project>`.
6. Remove only explicitly recognized build-generated files that were absent from the frozen source tree; fail on any unexpected dirty path.
7. Ensure temporary migration workflow/scripts do not leak into the published import.
8. Perform a final umbrella-main race check immediately before push.
9. Record source SHA, umbrella SHA, verification run and outcome below.

## Verified imports

### mini-elf-toolchain — VERIFIED 2026-09-05

- Source repository: `Lei-TzuY/mini-elf-toolchain`
- Frozen source SHA: `3d452a8681bbfb092cd41465dba6f6eb97dfd224`
- First published verified umbrella main: `baa897d1f5c972b8a1854b0202de67d7c4cd2597`
- Target: `projects/mini-elf-toolchain/`
- Successful one-shot verification run: `33925540855`
- Source open implementation PRs at freeze: `0`
- Exact source main CI at freeze: completed / success
- Complete reachable source-history attribution scan: PASS for configured patterns
- Non-squashed subtree ancestry: PASS
- Blob-for-blob source-tree vs imported-subtree comparison: PASS
- `cargo fmt --all -- --check`: PASS
- `cargo clippy --all-targets --all-features -- -D warnings`: PASS
- `cargo test --all-targets --all-features`: PASS
- Generated `Cargo.lock`: source did not contain it; migration runner accepted exactly this generated untracked path and removed it before publication
- One-shot workflow: removed before final push
- Final race check: PASS

Post-publication verification through GitHub confirms `3d452a86...` remains an ancestor with `behind_by = 0`. The first four staging attempts were fail-closed while the migration harness itself was hardened; none published an imported project tree.

### mini-libc — VERIFIED 2026-09-05

- Source repository: `Lei-TzuY/mini-libc`
- Frozen source SHA: `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1`
- First published verified umbrella main: `26cd23866580026bc9d72644da3fda9e25d18828`
- Target: `projects/mini-libc/`
- Successful one-shot verification run: `33927869512`
- Source open implementation PRs at freeze: `0`
- Exact source main CI run at freeze: `33842616621`, completed / success
- Complete reachable source-history attribution scan: PASS
- Non-squashed subtree ancestry / blob equivalence: PASS
- GCC and Clang `make clean test` + `make inspect`: PASS
- Pinned `tiny-c-compiler` bootstrap: PASS
- Pinned source-style three-repo bootstrap with mini-ELF: PASS
- Build-output cleanup / clean tree / one-shot cleanup / final race check: PASS
- Frozen source has no top-level LICENSE; migration preserved that fact exactly

Permanent umbrella CI run `33927981587` passed runtime GCC, runtime Clang, tiny-C bootstrap, and the imported mini-ELF toolchain bootstrap.

### mini-debugger — VERIFIED 2026-09-05

- Source repository: `Lei-TzuY/mini-debugger`
- Frozen source SHA: `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a`
- Exact source CI run: `33928883989`, completed / success
- First published verified migration-branch commit: `ae6f11bc676546a1e3ff178463f2284918f7e962`
- Target: `projects/mini-debugger/`
- Successful one-shot verification run: `33928967178`
- Source open implementation PRs at freeze: `0`
- Complete reachable source-history attribution scan: PASS
- Non-squashed ancestry / blob equivalence / native CMake + CTest: PASS
- One-shot cleanup / final race checks: PASS
- Merged to umbrella main with a normal merge commit, preserving source ancestry
- Post-merge umbrella run `33929397068`: native PASS; mini-ELF integration PASS

The mini-debugger integration locks the real current boundary rather than inventing metadata:

```text
projects/mini-elf-toolchain
        ↓
sectionless ELF64 ET_EXEC, entry 0x400000
        ↓
projects/mini-debugger
        ↓
launch + memory read
        ↓
numeric breakpoint at 0x400001
        ↓
continue + breakpoint hit
```

Current mini-ELF output has no section-header table / `.symtab`, so symbol-level interoperability remains a future linker/metadata capability.

### tiny-tensor-compiler — VERIFIED 2026-09-05

- Source repository: `Lei-TzuY/tiny-tensor-compiler`
- Frozen source SHA: `66ff2c6d02a22c621d01579b442af8b6fd43bcc5`
- Exact source CI run at freeze: `33928929309`, completed / success
- First published verified migration-branch commit: `53a48fb97ebb8ea9139a349f1a4948fe3c5faa94`
- Target: `projects/tiny-tensor-compiler/`
- Successful one-shot verification run: `33929543578`
- Source open implementation PRs at freeze: `0`
- Complete reachable source-history attribution scan: PASS for configured patterns
- Non-squashed subtree ancestry: PASS
- Blob-for-blob source-tree vs imported-subtree comparison: PASS
- Python 3.13 editable install + `ruff check .` + `pytest`: PASS from the umbrella path
- Clean tracked tree after validation: PASS
- One-shot workflow removed before publication: PASS
- Final source-head / umbrella-main / migration-branch race check: PASS
- Frozen source includes README, CHANGELOG, docs and `pyproject.toml` but no top-level LICENSE file; migration preserves that source state exactly

Permanent `.github/workflows/tiny-tensor-compiler.yml` mirrors the source CI matrix across Ubuntu/Windows and Python 3.11/3.13.

No direct mini-ELF integration is claimed yet. The current tiny-tensor native backend emits deterministic C11, invokes the host GCC/Clang/MSVC toolchain to create a shared library, and loads it through `ctypes`; the current mini-ELF project links static object/archive input into `ET_EXEC`. A real cross-project edge therefore requires an explicitly designed object/static-runtime handoff or another executable-artifact contract. Treating the existing shared-library ABI as if it were already compatible with the static linker would be misleading.

## Source repository retirement policy

After a project reaches **IMPORTED / VERIFIED**:

- keep the original repository available while external links, issues, PRs and releases still matter;
- add a prominent canonical-path notice only after the umbrella import and ongoing CI strategy are stable;
- archive only after the umbrella version is stable and the redirect is clear;
- do not delete the original repository as part of routine portfolio consolidation.
