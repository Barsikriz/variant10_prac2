$ErrorActionPreference = 'Stop'

$cases = @(
    @{ Args = @('--package', 'csv'); Valid = $true }
    @{ Args = @('--package', 'A', '--test', '--repo', 'graph.txt'); Valid = $true }
    @{ Args = @('--package', 'a/b'); Valid = $false }
    @{ Args = @('--package', 'A', '--test'); Valid = $false }
    @{ Args = @('--package', 'csv', '--repo', 'ftp://example.com'); Valid = $false }
    @{ Args = @('--package', 'csv', '--out', 'result.png'); Valid = $false }
    @{ Args = @('--package', 'csv', '--ascii', '--ascii'); Valid = $false }
    @{ Args = @('--package', 'csv', '--out'); Valid = $false }
)

foreach ($case in $cases) {
    $arguments = $case.Args
    & "$PSScriptRoot\..\depgraph.exe" @arguments

    if (($LASTEXITCODE -eq 0) -ne $case.Valid) {
        throw "Unexpected result: $($arguments -join ' ')"
    }
}

Write-Host 'All stage 1 checks passed.'
