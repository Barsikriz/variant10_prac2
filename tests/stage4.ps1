$ErrorActionPreference = 'Stop'
$project = Split-Path $PSScriptRoot -Parent

$cases = @(
    @{ File = 'diamond.txt'; Package = 'D'; Expected = 'B C A X' }
    @{ File = 'diamond.txt'; Package = 'B'; Expected = 'A X' }
    @{ File = 'diamond.txt'; Package = 'A'; Expected = '' }
    @{ File = 'cycle.txt'; Package = 'A'; Expected = 'C B' }
    @{ File = 'solo.txt'; Package = 'A'; Expected = '' }
)

foreach ($case in $cases) {
    $path = Join-Path $PSScriptRoot ('graphs/' + $case.File)

    $lines = & "$project/depgraph.exe" `
        --test --repo $path --package $case.Package --reverse

    if ($LASTEXITCODE -ne 0 -or ($lines -join ' ') -ne $case.Expected) {
        throw "Unexpected reverse result for $($case.File), package $($case.Package): $lines"
    }
}

Write-Host 'Stage 4 checks passed.'
