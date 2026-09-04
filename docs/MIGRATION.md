# Migration Protocol & Preflight Ledger

This document is the durable migration ledger for `compiler-runtime-lab`.

## Status vocabulary

- **READY FOR IMPORT PREP** — first-pass source audit is healthy, but the exact source head and all gates must be refreshed immediately before import.
- **ATTRIBUTION REVIEW** — source code may be healthy, but reachable history contains attribution metadata that needs an explicit preservation/rewrite decision before import.
- **HOLD** — do not import while the stated blocker is active.
- **IMPORTED / VERIFIED** — reserved for a history-preserving import whose selected source tree and umbrella tree have been verified and whose applicable tests pass from the umbrella layout.

## First-pass preflight — 2026-09-05 (Asia/Taipei)

| Project | Observed source `main` | Open implementation PR | Latest observed main workflow | Attribution search | Status |
| --- | --- | --- | --- | --- | --- |
| Nova | `dcadc2238737b6f1e98887ab8fa658b23413d31b` | none | CI completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP |
| tiny-c-compiler | `5607c3152d319353c42f05ed44ff53479272a74f` | none | Tests completed / success | reachable commits include `Co-authored-by: github-actions[bot]` | ATTRIBUTION REVIEW |
| sic-xe-assembler | `a58f4b5c7f34675fddf437d4af596cd81b5d891f` | **#30 open** — cross-domain memory/register CFG fixed point | Tests completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | **HOLD** |
| mini-elf-toolchain | `3d452a8681bbfb092cd41465dba6f6eb97dfd224` | none | CI completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP |
| mini-language-server | `74ecddce7061abc6cae467728d12599f3403c28c` | none | CI completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP |
| mini-debugger | `f35d4f70075cbd95230dd37592c5d5a1a90a40a2` | none | CI completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP |
| mini-libc | `a9d2a1d9fb1ead44d45d679da5a8586d6f8007a1` | none | CI completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP |
| mini-wasm-runtime | `e923b27a2652aba88d50cdbb75d0fe959d40e457` | none | latest observed main workflow (`Differential reference smoke`) completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP; refresh all CI/fuzz/differential gates before freeze |
| tiny-tensor-compiler | `8ebab53570adcdea8c483e301f07aab05e679426` | none | CI completed / success | no `Claude` / `Anthropic` commit-message hits in candidate set search | READY FOR IMPORT PREP |

### Important limitations of this first-pass attribution search

The GitHub commit search used here is evidence, not a cryptographic proof of absence. Before an actual history-preserving import, run a complete local reachable-history scan over the exact source clone, including at least:

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
5. Run a complete reachable-history attribution scan locally.
6. Confirm README and project-local license/documentation are present and will remain inside the imported subtree.
7. Check for generated binaries, huge artifacts, secrets, local caches, vendored dependency accidents, and temporary migration workflows/scripts.
8. Freeze the source SHA in this ledger before performing the import.

## Recommended history-preserving import procedure

Use a real Git client. Do **not** download ZIPs or copy only the current tree.

Example for `mini-elf-toolchain`:

```bash
git clone https://github.com/Lei-TzuY/compiler-runtime-lab.git
cd compiler-runtime-lab

git remote add source-mini-elf https://github.com/Lei-TzuY/mini-elf-toolchain.git
git fetch source-mini-elf --tags

# Re-check source/main SHA here against the frozen ledger entry.

git subtree add \
  --prefix=projects/mini-elf-toolchain \
  source-mini-elf main
```

Do not use `--squash` when the purpose is to preserve source history.

Alternative: use `git filter-repo --to-subdirectory-filter projects/<project>` in a temporary clone, then merge the rewritten history into the umbrella. This gives more explicit control over path rewriting and is preferred when attribution/history surgery is intentionally required.

## Verification after each import

For the selected source commit `SOURCE_SHA` and imported umbrella commit `UMBRELLA_SHA`:

1. Export/hash the source tree and imported subtree independently.
2. Exclude only migration-intentional path-prefix differences.
3. Require content equivalence for project files.
4. Run the project's native test/check/build gates from `projects/<project>`.
5. Run any cross-project integration gates that apply.
6. Record source SHA, umbrella SHA, verification command, CI run and outcome below.

## Verified imports

None yet. This is intentional: bootstrap/preflight has completed, but no project history has been copied or rewritten through the GitHub web/connector surface.

## Source repository retirement policy

After a project reaches **IMPORTED / VERIFIED**:

- keep the original repository available while external links, issues, PRs and releases still matter;
- add a prominent canonical-path notice to the original README;
- archive only after the umbrella version is stable and the redirect is clear;
- do not delete the original repository as part of routine portfolio consolidation.
