param(
    [Parameter(Mandatory = $true)]
    [string]$Project,

    [switch]$Execute
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Fail([string]$Message) {
    Write-Error $Message
    exit 1
}

function Require-Command([string]$Name) {
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        Fail "Required command '$Name' was not found."
    }
}

Require-Command git

$repoRoot = (git rev-parse --show-toplevel 2>$null)
if (-not $repoRoot) { Fail 'Run this script from inside the compiler-runtime-lab clone.' }
Set-Location $repoRoot

$origin = (git remote get-url origin 2>$null)
if ($origin -notmatch 'Lei-TzuY/compiler-runtime-lab(?:\.git)?$') {
    Fail "Refusing to run: origin is '$origin', not Lei-TzuY/compiler-runtime-lab."
}

if (git status --porcelain) {
    Fail 'Working tree is not clean. Commit/stash changes before migration.'
}

$manifestPath = Join-Path $repoRoot 'projects/manifest.json'
if (-not (Test-Path $manifestPath)) { Fail 'projects/manifest.json is missing.' }
$manifest = Get-Content $manifestPath -Raw | ConvertFrom-Json
$entry = $manifest.projects | Where-Object { $_.name -eq $Project }
if (-not $entry) { Fail "Unknown project '$Project'." }
if (@($entry).Count -ne 1) { Fail "Manifest contains duplicate project name '$Project'." }

if ($entry.status -ne 'ready-for-import-prep') {
    Fail "Project '$Project' is not migration-ready (status: $($entry.status); blocker: $($entry.blocker))."
}

$targetPath = [string]$entry.target_path
if (Test-Path $targetPath) {
    Fail "Target path '$targetPath' already exists. Refusing to overwrite an existing import."
}

$sourceRepo = [string]$entry.source_repository
$sourceUrl = "https://github.com/$sourceRepo.git"
$remoteName = 'source-' + (($Project -replace '[^A-Za-z0-9._-]', '-') .ToLowerInvariant())

Write-Host "== compiler-runtime-lab migration preflight =="
Write-Host "Project:      $Project"
Write-Host "Source:       $sourceRepo"
Write-Host "Target:       $targetPath"
Write-Host "Ledger SHA:   $($entry.observed_main_sha)"
Write-Host "Mode:         $($(if ($Execute) { 'EXECUTE' } else { 'DRY RUN' }))"

# Refresh umbrella first. Do not silently merge/rebase local work.
git fetch origin --prune
$localHead = (git rev-parse HEAD).Trim()
$originMain = (git rev-parse origin/main).Trim()
if ($localHead -ne $originMain) {
    Fail "Local HEAD ($localHead) is not exact origin/main ($originMain). Refresh the umbrella clone first."
}

# Use a dedicated source remote and fetch exact source history.
$existingRemote = git remote 2>$null | Where-Object { $_ -eq $remoteName }
if ($existingRemote) {
    git remote set-url $remoteName $sourceUrl
} else {
    git remote add $remoteName $sourceUrl
}
git fetch $remoteName --tags --prune

$sourceHead = (git rev-parse "$remoteName/main").Trim()
Write-Host "Live source:  $sourceHead"

if ($sourceHead -ne [string]$entry.observed_main_sha) {
    Fail "Source main moved since the ledger snapshot. Refresh GitHub PR/CI state and update the manifest before importing."
}

# Full reachable-history attribution scan. This is intentionally broad and
# fails closed: matches must be reviewed, not silently rewritten.
$patterns = 'co-authored-by|generated-by|assisted-by|signed-off-by|anthropic|claude|openai'
$matches = git log "$remoteName/main" --format='%H%x09%an%x09%ae%x09%B%x00' | Select-String -Pattern $patterns -CaseSensitive:$false
if ($matches) {
    Write-Host ''
    Write-Host 'Attribution/history matches detected:' -ForegroundColor Yellow
    $matches | ForEach-Object { Write-Host $_.Line }
    Fail 'History attribution review is required before import. Do not delete or rewrite genuine authorship blindly.'
}

Write-Host ''
Write-Host 'Preflight passed: source head is frozen and no configured attribution patterns were found.' -ForegroundColor Green
Write-Host "Planned command: git subtree add --prefix=$targetPath $remoteName main"

if (-not $Execute) {
    Write-Host 'Dry run only. Re-run with -Execute after confirming GitHub open PRs and exact source CI are still clean.'
    exit 0
}

# This command preserves the source history by merging it into the umbrella.
# Do NOT add --squash: the purpose of this umbrella is to retain evidence.
git subtree add --prefix="$targetPath" $remoteName main

Write-Host ''
Write-Host "Imported $Project into $targetPath with source history preserved." -ForegroundColor Green
Write-Host 'Next: run scripts/verify-import.ps1 before pushing or changing the original repository.'
