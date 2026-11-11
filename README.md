# Cargo Dependency Graph

Command-line dependency graph analyzer for Rust packages.

## Stage 2

The application downloads package metadata from a Cargo sparse registry
and prints direct dependency names.

The highest stable, non-yanked version in the downloaded index is selected.
Normal, development, build and optional dependency declarations are included.
Renamed dependencies use the actual package name from the package field.

The downloaded index is stored in build/index.jsonl.
HTTP downloads require curl.exe in PATH.

## Metadata functions

| Function | Purpose |
| --- | --- |
| registry_url | Construct the sparse-index URL from the crate name. |
| download_index | Download the index through curl and check its exit status. |
| index_read | Read the downloaded file into an allocated string. |
| spaces, string_end, skip_value | Scan whitespace, strings and nested JSON values. |
| field | Locate a field in a JSON object. |
| stable, newer | Filter prereleases and compare version components. |
| metadata_print | Select a release and print its direct dependencies. |

## Stage 2 checks

Run from the project root in PowerShell:

```powershell
make
.\depgraph.exe --package csv-core
.\depgraph.exe --package itoa
.\depgraph.exe --package definitely-missing-crate-987654321
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
