# Cargo Dependency Graph

Command-line dependency graph analyzer for Rust packages.

## Stage 1

The current prototype parses and validates configuration options.
It prints the resulting configuration as key-value pairs.

## Configuration

| Option | Description | Default |
| --- | --- | --- |
| `--package NAME` | Required package name. | Required |
| `--repo URL_OR_FILE` | Registry URL or test repository file path. | `https://index.crates.io` |
| `--test` | Enable local test repository mode. | Disabled |
| `--out FILE.svg` | Output image filename. | `graph.svg` |
| `--ascii` | Enable ASCII dependency tree output. | Disabled |

Test mode requires an explicit repository file path and uppercase ASCII package names.
Registry mode requires an HTTP(S) URL.

Unknown options, duplicate options, missing values, invalid package names,
invalid repository modes and unsupported output extensions cause an error.

## Functions

`main` parses arguments, validates configuration, applies defaults and prints the settings.
A successful execution returns zero. Invalid configuration returns a nonzero exit code.
Error messages are written to stderr.

## Build

Requires GCC. Run from the project root in PowerShell:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -g src/main.c -o depgraph.exe
```

## Usage

```powershell
.\depgraph.exe --package csv
.\depgraph.exe --package A --test --repo graph.txt --out result.svg --ascii
```

## Manual checks

```powershell
.\depgraph.exe --package csv --repo ftp://example.com
.\depgraph.exe --package A --test
.\depgraph.exe --package csv --out result.png
.\depgraph.exe --package csv --ascii --ascii
```

```powershell
powershell -ExecutionPolicy Bypass -File .\tests\stage1.ps1
```

Each command in this section must report an error and return a nonzero exit code.
