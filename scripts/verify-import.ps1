param(
    [Parameter(Mandatory = $true)]
    [string]$Project,

    [string]$UmbrellaRef = 'HEAD'
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Fail([string]$Message) {
    Write-Error $Message
    exit 1
}

function Normalize-TreeLines([string[]]$Lines, [string]$Prefix = '') {
    $normalized = New-Object System.Collections.Generic.List[string]
    foreach ($line in $Lines) {
        if ($line -notmatch '^(?<mode>\d+)\s+(?<type>\S+)\s+(?<sha>[0-9a-f]+)\t(?<path>.+)$') {
            Fail "Could not parse git ls-tree line: $line"
        }
        $path = $Matches.path
        if ($Prefix) {
            $expectedPrefix = $Prefix.TrimEnd('/') + '/'
            if (-not $path.StartsWith($expectedPrefix, [System.StringComparison]::Ordinal)) {
                Fail "Imported path '$path' is outside expected prefix '$Prefix'."
            }
            $path = $path.Substring($expectedPrefix.Length)
        }
        $normalized.Add("$($Matches.mode)`t$($Matches.type)`t$($Matches.sha)`t$path")
    }
    return @($normalized | Sort-Object)
}

$repoRoot = (git rev-parse --show-toplevel 2>$null)
if (-not $repoRoot) { Fail 'Run this script from inside the compiler-runtime-lab clone.' }
Set-Location $repoRoot

$origin = (git remote get-url origin 2>$null)
if ($origin -notmatch 'Lei-TzuY/compiler-runtime-lab(?:\.git)?$') {
    Fail "Refusing to run: origin is '$origin', not Lei-TzuY/compiler-runtime-lab."
}

$manifestPath = Join-Path $repoRoot 'projects/manifest.json'
if (-not (Test-Path $manifestPath)) { Fail 'projects/manifest.json is missing.' }
$manifest = Get-Content $manifestPath -Raw | ConvertFrom-Json
$entries = @($manifest.projects | Where-Object { $_.name -eq $Project })
if ($entries.Count -ne 1) { Fail "Expected exactly one manifest entry for '$Project'." }
$entry = $entries[0]

$sourceSha = [string]$entry.observed_main_sha
$targetPath = [string]$entry.target_path
if (-not (Test-Path $targetPath)) { Fail "Imported target '$targetPath' does not exist in the working tree." }

# A non-squashed subtree import must leave the source commit reachable from the
# umbrella history. This is an important distinction from a current-tree copy.
git merge-base --is-ancestor $sourceSha $UmbrellaRef 2>$null
if ($LASTEXITCODE -ne 0) {
    Fail "Source commit $sourceSha is not an ancestor of $UmbrellaRef. The import may have lost source history or used a squashed/copy-only path."
}

$sourceLines = @(git ls-tree -r $sourceSha)
if ($LASTEXITCODE -ne 0) { Fail "Could not enumerate source tree $sourceSha." }
$importLines = @(git ls-tree -r $UmbrellaRef -- $targetPath)
if ($LASTEXITCODE -ne 0) { Fail "Could not enumerate imported subtree '$targetPath'." }

$sourceTree = Normalize-TreeLines -Lines $sourceLines
$importTree = Normalize-TreeLines -Lines $importLines -Prefix $targetPath

$diff = @(Compare-Object -ReferenceObject $sourceTree -DifferenceObject $importTree)
if ($diff.Count -gt 0) {
    Write-Host 'Tree mismatch detected:' -ForegroundColor Red
    $diff | Format-Table -AutoSize | Out-String | Write-Host
    Fail 'Imported subtree is not blob-for-blob equivalent to the frozen source tree.'
}

Write-Host "History reachability: PASS ($sourceSha is reachable from $UmbrellaRef)" -ForegroundColor Green
Write-Host "Tree equivalence:     PASS ($($sourceTree.Count) entries)" -ForegroundColor Green
Write-Host "Imported path:        $targetPath"
Write-Host ''
Write-Host 'Structural verification passed. Run the project-native formatter/lint/tests/build gates before pushing and before changing the original repository.' -ForegroundColor Green
