# Cargo Dependency Graph

Command-line dependency graph analyzer for Rust packages.

## Behavior

The application builds a transitive dependency graph using iterative BFS
and prints reachable package names in discovery order, including the root.

Registry traversal includes mandatory normal dependencies.
Development, build and optional dependencies are excluded.
Target predicates are not evaluated. Each package uses its highest stable,
non-yanked version independently of dependency version requirements.

Local test repositories use uppercase package names and adjacency lists.
Cycle edges and self-loops are preserved. Each vertex is enqueued once.

## Stage 3

The application builds a transitive dependency graph using iterative BFS.
Each package is enqueued once. Cycle edges and self-loops remain in the graph.

Local test repositories use an adjacency-list format:

```text
A: B C
B: D
C: D
D:

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
