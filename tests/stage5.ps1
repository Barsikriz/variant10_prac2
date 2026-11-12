$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Push-Location $root
try {
    New-Item -ItemType Directory -Force 'build/stage5' | Out-Null
    $cases = @(
        @{ File = 'diamond.txt'; Root = 'A'; Nodes = 4; Edges = 4; Ref = $true }
        @{ File = 'cycle.txt'; Root = 'A'; Nodes = 3; Edges = 4; Ref = $true }
        @{ File = 'solo.txt'; Root = 'A'; Nodes = 1; Edges = 0; Ref = $false }
    )
    foreach ($case in $cases) {
        $svg = "build/stage5/$($case.File).svg"
        $inputPath = "tests/graphs/$($case.File)"
        $lines = & .\depgraph.exe --test --repo $inputPath --package $case.Root --out $svg --ascii
        if ($LASTEXITCODE -ne 0) { throw "Failed to render $($case.File)" }
        if (($lines -join "`n").Contains('[ref]') -ne $case.Ref) {
            throw "Unexpected reference markers for $($case.File)"
        }
        [xml]$image = Get-Content -Raw $svg
        if ($image.DocumentElement.LocalName -ne 'svg') { throw 'Invalid SVG document' }
        $dot = Get-Content -Raw ([IO.Path]::ChangeExtension($svg, '.dot'))
        if ([regex]::Matches($dot, '\[label=').Count -ne $case.Nodes) { throw 'Wrong node count' }
        if ([regex]::Matches($dot, ' -> ').Count -ne $case.Edges) { throw 'Wrong edge count' }
    }
    $lines = & .\depgraph.exe --test --repo tests/graphs/diamond.txt --package D `
        --reverse --ascii --out 'build/stage5/reverse graph.svg'
    if ($LASTEXITCODE -ne 0 -or $lines[0] -ne 'D') { throw 'Reverse tree failed' }
    if (-not (Test-Path 'build/stage5/reverse graph.svg')) { throw 'Missing SVG with spaces' }
    Write-Host 'Stage 5 checks passed.'
} finally { Pop-Location }
