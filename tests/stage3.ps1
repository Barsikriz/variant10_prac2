$ErrorActionPreference = 'Stop'
$project = Split-Path $PSScriptRoot -Parent
$cases = @(
    @{ File = 'diamond.txt'; Expected = 'A B C D' }
    @{ File = 'cycle.txt'; Expected = 'A B C' }
    @{ File = 'solo.txt'; Expected = 'A' }
)
foreach ($case in $cases) {
    $path = Join-Path $PSScriptRoot ('graphs/' + $case.File)
    $lines = & "$project/depgraph.exe" --test --repo $path --package A
    if ($LASTEXITCODE -ne 0 -or ($lines -join ' ') -ne $case.Expected) {
        throw "Unexpected BFS result for $($case.File): $lines"
    }
}
$path = Join-Path $PSScriptRoot 'graphs/solo.txt'
& "$project/depgraph.exe" --test --repo $path --package Z
if ($LASTEXITCODE -eq 0) { throw 'Unknown root was accepted.' }
Write-Host 'Stage 3 CLI checks passed.'
