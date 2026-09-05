# Migration Protocol & Preflight Ledger

This document is the durable migration ledger for `compiler-runtime-lab`.

## Status vocabulary

- **READY FOR IMPORT PREP** — first-pass source audit is healthy, but the exact source head and all gates must be refreshed immediately before import.
- **ATTRIBUTION REVIEW** — source code may be healthy, but reachable history contains attribution metadata that needs an explicit preservation/rewrite decision before import.
- **HOLD** — do not import while the stated blocker is active.
- **IMPORTED / VERIFIED** — a history-preserving import whose selected source tree and umbrella tree have been verified and whose applicable tests pass from the umbrella layout.

## Current preflight / migration state — 2026-09-05 (Asia/Taipei)

| Project | Frozen / observed source `main` | Evidence | Status |
| --- | --- | --- | --- |
| Nova | `dcadc2238737b6f1e98887ab8fa658b23413d31b` | source CI `33920772855`; one-shot `33967555408`; PR 5/5; merged `7be12c6d...`; post-merge `33968264098` 5/5 | **IMPORTED / VERIFIED** |
| tiny-c-compiler | `5607c3152d319353c42f05ed44ff53479272a74f` | reachable history includes `Co-authored-by: github-actions[bot]` | ATTRIBUTION REVIEW |
| sic-xe-assembler | `a58f4b5c7f34675fddf437d4af596cd81b5d891f` | #30 observed during first pass; recheck before import | **HOLD / RECHECK** |
| mini-elf-toolchain | `3d452a8681bbfb092cd41465dba6f6eb97dfd224` | complete history/tree/native Rust verification | **IMPORTED / VERIFIED** |
| mini-language-server | `f8a4d642eaa721741ab3cea7eb02d2f261dbad01` | source CI `33967371120`; one-shot `33968415924`; six-way umbrella CI staged | **IMPORTED / VERIFIED CANDIDATE** |
| mini-debugger | `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a` | source CI `33928883989`; one-shot `33928967178`; post-merge integration green | **IMPORTED / VERIFIED** |
| mini-libc | `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1` | one-shot `33927869512`; permanent umbrella CI `33927981587` | **IMPORTED / VERIFIED** |
| mini-wasm-runtime | `e923b27a2652aba88d50cdbb75d0fe959d40e457` | one-shot `33966895454`; post-merge seven-job CI `33967377142` | **IMPORTED / VERIFIED** |
| tiny-tensor-compiler | `4690df5747a1e7fc0af9b602f8be8d963e72d00f` | refresh one-shot `33966522648`; post-merge four-way CI `33966708522` | **IMPORTED / VERIFIED** |

### Attribution-scan rule

A GitHub commit search is only first-pass evidence. Every actual import runs a complete reachable-history scan over the exact fetched source history, including configured patterns such as:

```bash
git log --all --format='%H%x00%an%x00%ae%x00%B' \
  | grep -Eai 'co-authored-by|generated-by|assisted-by|signed-off-by|anthropic|claude|openai'
```

Matches are inspected rather than deleted blindly. Genuine authorship is not falsified. New umbrella commits do not add AI/bot attribution trailers.

## Import freeze gate

Immediately before importing a project:

1. Fetch/prune and record the exact source `main` SHA.
2. Confirm no active implementation PR.
3. Confirm required source CI/checks are successful for the exact source candidate.
4. Run the complete reachable-history attribution scan.
5. Preserve README/docs and existing project-local licenses; document license-file absence instead of inventing metadata.
6. Check for generated artifacts, caches, secrets and migration-only files.
7. Freeze the umbrella base and source SHA before import.

## History-preserving import procedure

Use a real Git client and a non-squashed subtree import. Do not download ZIPs or flatten the source into one copy commit.

```bash
git remote add source-project https://github.com/Lei-TzuY/<project>.git
git fetch source-project --tags
git subtree add --prefix=projects/<project> source-project main
```

For every migration candidate:

- require the frozen source SHA to remain an umbrella ancestor;
- require source-root tree and imported subtree equivalence at the freeze point;
- run source-equivalent native gates from `projects/<project>`;
- remove one-shot migration machinery before publication;
- perform final source-head / umbrella-main race checks;
- merge the migration PR with a normal merge commit, never squash/rebase;
- rerun permanent path-scoped CI on exact merged umbrella `main`.

## Verified imports

### mini-elf-toolchain — VERIFIED 2026-09-05

- Source SHA: `3d452a8681bbfb092cd41465dba6f6eb97dfd224`
- One-shot run: `33925540855`
- First verified umbrella main: `baa897d1f5c972b8a1854b0202de67d7c4cd2597`
- Complete reachable-history attribution, non-squashed ancestry, blob equivalence, rustfmt, Clippy `-D warnings` and tests: PASS.

### mini-libc — VERIFIED 2026-09-05

- Source SHA: `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1`
- Source CI: `33842616621`
- One-shot run: `33927869512`
- First verified umbrella main: `26cd23866580026bc9d72644da3fda9e25d18828`
- Permanent umbrella CI: `33927981587`
- GCC/Clang runtime probes, host-libc independence, pinned tiny-C bootstrap and source-style mini-ELF bootstrap: PASS.
- Source has no top-level LICENSE; that absence is preserved.

### mini-debugger — VERIFIED 2026-09-05

- Source SHA: `0ed0d52d0d650e6e7b535bfe49804719cfae2c9a`
- Source CI: `33928883989`
- One-shot run: `33928967178`
- First verified migration branch: `ae6f11bc676546a1e3ff178463f2284918f7e962`
- Post-merge umbrella run: `33929397068`
- Native CMake/CTest and the imported mini-ELF address-level integration: PASS.

The chain is intentionally bounded to sectionless ET_EXEC launch, memory read and numeric breakpoint behavior; `.symtab` interoperability is not claimed.

### tiny-tensor-compiler — VERIFIED / REFRESHED 2026-09-05

- Initial source SHA: `66ff2c6d02a22c621d01579b442af8b6fd43bcc5`
- Refreshed source SHA: `4690df5747a1e7fc0af9b602f8be8d963e72d00f`
- Initial one-shot: `33929543578`
- Refresh one-shot: `33966522648`
- Merged umbrella SHA: `ae03589a39a36d2df8e8220d933471722708cedc`
- Post-merge Ubuntu/Windows × Python 3.11/3.13: `33966708522`, PASS.
- The 26 newly reachable commits were separately attribution-scanned before the non-squashed refresh.
- Source has no top-level LICENSE; that state is preserved.

No direct mini-ELF integration is claimed because the current backend produces host-toolchain shared libraries rather than mini-ELF's static ET_EXEC input contract.

### mini-wasm-runtime — VERIFIED 2026-09-05

- Source SHA: `e923b27a2652aba88d50cdbb75d0fe959d40e457`
- Source core CI: `33892678967`
- Source benchmark: `33892678903`
- Source Wasmtime differential: `33892679049`
- One-shot run: `33966895454`
- Merged umbrella SHA: `e7b1df4734d6c5b4b04c2e6a99de424932c2079f`
- Post-merge permanent seven-job CI: `33967377142`, PASS.
- Stable + Rust 1.81 core gates, Wasmtime differential, deterministic benchmark and deterministic parser/validation fuzz smoke: PASS.
- Source 15-minute coverage-guided fuzzing remains scheduled/manual and is not misrepresented as a PR gate.
- Source LICENSE is preserved.

### Nova — VERIFIED 2026-09-05

- Source SHA: `dcadc2238737b6f1e98887ab8fa658b23413d31b`
- Source CI: `33920772855`
- One-shot run: `33967555408`
- First verified cleaned migration commit: `e725af64a9f05fd37acb51b969b01f635c20c37b`
- PR-head permanent CI: `33968190636`, five jobs PASS.
- Merged umbrella SHA: `7be12c6d2dc8cc10dbf386213064a1b809b7aea3`
- Post-merge permanent CI: `33968264098`, five jobs PASS.
- Full history scan, non-squashed ancestry, blob equivalence, rustfmt, Clippy, tests, build/rustdoc and Rust 1.85 MSRV: PASS.
- Source has no top-level LICENSE; that state is preserved.

### mini-language-server — VERIFIED MIGRATION CANDIDATE 2026-09-05

- Source SHA: `f8a4d642eaa721741ab3cea7eb02d2f261dbad01`
- Exact source CI: `33967371120`, Ubuntu/Windows/macOS × Python 3.11/3.13 PASS.
- Source open implementation PRs at freeze: `0`.
- One-shot run: `33968415924`, PASS.
- First cleaned migration-branch commit: `27c6d64d32308ec7edb7d806e0ff7fe5989e2ba7`.
- Complete reachable-history attribution scan: PASS.
- Non-squashed source ancestry: PASS.
- Blob-for-blob source tree vs imported subtree: PASS.
- Python 3.13 editable install, `ruff check .` and `pytest`: PASS.
- Clean tree, one-shot self-removal and final source/main race check: PASS.
- Frozen source has README/docs/pyproject/src/tests but no top-level LICENSE; that state is preserved.
- Permanent `.github/workflows/mini-language-server.yml` mirrors the source six-way OS/Python matrix.

#### Discovered Nova integration gap

The frozen language-server source genuinely contains `NovaFunctionAdapter`, local/parameter/reference semantics and deterministic `nova.unresolved-name` diagnostics. However, imported Nova's normative grammar requires typed parameters and an explicit return type:

```text
fn name(parameter: Type) -> ReturnType { ... }
```

The frozen adapter's function-declaration pattern currently recognizes only `fn name(params) { ... }`, and its parameter parser only treats bare identifiers as parameters. Therefore the umbrella does **not** claim that legal Nova programs currently flow through the adapter. The first post-migration integration slice is to close this exact grammar mismatch and prove shared-input function/local/reference/unresolved-name behavior without weakening snapshot/stale-result semantics.

## Seven-project checkpoint plan

The migration side of the seven-project checkpoint is complete once mini-language-server PR-head and exact-main six-way CI are green. The architectural checkpoint additionally requires the bounded Nova grammar/semantic integration slice above.

After both are complete:

- verify zero open migration/integration PRs;
- verify applicable path-scoped workflows on exact umbrella `main`;
- freeze README/manifest/ledger evidence;
- retain `tiny-c-compiler` and `sic-xe-assembler` as explicit exceptions if their blockers remain;
- begin the separate `systems-lab` consolidation.

## Source repository retirement policy

After a project reaches **IMPORTED / VERIFIED**:

- keep the original repository available while external links, issues, PRs and releases still matter;
- add a canonical-path notice only after umbrella import and ongoing CI are stable;
- archive only after the umbrella version is stable and the redirect is clear;
- do not delete the original repository as routine portfolio consolidation.
