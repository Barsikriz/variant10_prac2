# Cargo Dependency Graph

C11 CLI tool for Rust dependency graphs from a Cargo sparse index or a local test repository.
Edges point from a package to its dependency. Reverse mode uses dependency-to-consumer edges.

## Requirements and build

Windows, GCC, GNU Make, curl.exe and Graphviz dot.exe on PATH.
Python 3.8+ is used by the offline demonstration. Cargo is used only for reference comparisons.

```powershell
make
make test-stage5
make demo
Invoke-Item .\build\demo\csv.svg
```

The C application uses the standard C library and Windows CRT process functions.
The analyzer parses index records itself. curl transfers bytes and dot renders the generated DOT source.

## Options

| Option | Description |
| --- | --- |
| --package NAME | Required root package. |
| --repo URL_OR_FILE | HTTP(S) sparse-index base URL or local graph path in test mode. |
| --test | Load a local repository. Requires an explicit --repo path. |
| --out FILE.svg | SVG output filename. Default graph.svg. A sibling .dot file is also written. |
| --ascii | Print the dependency graph as an ASCII tree with reference markers. |
| --reverse | Find direct and transitive consumers in the complete local test repository. Requires --test. |

The default registry is https://index.crates.io. Duplicate options and missing values are errors.
Output directories must exist. File paths with spaces must be quoted in PowerShell.
Without --ascii, reachable names are printed in BFS order. Reverse lists exclude the selected root.
An ASCII tree includes the root. A reference marker [ref] identifies a repeated vertex, including cycle edges.

```powershell
.\depgraph.exe --package csv-core --ascii --out csv-core.svg
.\depgraph.exe --test --repo tests/graphs/diamond.txt --package A --ascii --out diamond.svg
.\depgraph.exe --test --repo tests/graphs/diamond.txt --package D --reverse --ascii --out reverse.svg
```

## Data and traversal

Each local graph line has the form NAME: DEP DEP. Names contain uppercase ASCII letters.
Blank lines and comments starting with # are ignored. Repeated declarations merge and duplicate edges collapse.
Dependencies mentioned only on the right-hand side are created as leaf vertices.

```text
A: B C
B: D
C: D
D:
X: B
```

Registry mode selects the highest stable non-yanked version of each discovered crate independently.
It traverses mandatory normal dependencies. Dev, build and optional declarations are excluded.
Target predicates, feature activation and dependency version constraints are not resolved.
Dependency aliases use their actual package name. Downloads are stored temporarily in build/index.jsonl.

BFS uses a growing queue. A vertex is marked when enqueued, so cycles do not cause repeated expansion.
All edges remain in adjacency lists. The ASCII renderer reconstructs BFS parents and prints using an explicit stack.
DOT and SVG retain all reachable edges, including self-loops and references.

## Functions

| Functions | Purpose |
| --- | --- |
| main | Parse and validate configuration, select a source, traverse, render and release memory. |
| registry_url | Map a crate name to its Cargo sparse-index URL. |
| download_index | Run curl and check the download result. |
| index_read | Read a text file into an allocated buffer. |
| spaces, string_end, skip_value, field | Scan JSON records and locate metadata fields. |
| stable, newer | Select stable releases and compare version components. |
| metadata_each | Pass dependency names to a callback under the requested dependency profile. |
| metadata_print, print_dependency | Preserve direct-dependency printing from stage 2. |
| graph_find, graph_node, graph_edge | Find/create vertices and insert distinct directed edges. |
| trim, valid_name, parse_line, graph_load | Parse the complete local test repository. |
| enqueue, graph_bfs, graph_bfs_expand | Traverse a preloaded or dynamically expanded graph. |
| registry_expand, add_dependency | Fetch a crate and add its outgoing dependencies. |
| graph_reverse | Construct a temporary graph with all edges reversed and replace the original on success. |
| graph_free | Release graph names, adjacency arrays and vertex storage. |
| write_label, graph_write_dot | Escape labels and write the reachable graph as DOT. |
| quote_path, graph_render_svg | Prepare Windows arguments and run Graphviz for SVG output. |
| graph_print_tree | Reconstruct BFS parents and print an ASCII tree with repeated-node markers. |

## Tests and demonstrations

```powershell
make test-graph
make test-expand
make test-stage3
make test-stage4
make test-stage5
make demo
```

Tests cover graph storage, dynamic BFS, cycles, shared dependencies, isolated vertices, reverse traversal,
ASCII reference markers, DOT node/edge counts and SVG XML output. The final target includes earlier stage tests.

make demo serves the bundled real Cargo index snapshot on loopback, runs the same C download/parser path,
and generates itoa.svg, csv-core.svg and csv.svg under build/demo without external network access.
Use make PYTHON=py demo if the Python launcher is named py on the system.

```powershell
make compare
```

The comparison additionally requires Cargo and rustc on PATH. Cargo may download its reference dependencies.
A locked reference manifest and captured Cargo trees are provided. Results and differences are described in
[docs/comparison.md](docs/comparison.md).

Successful execution returns zero. Errors are written to stderr and return nonzero.
Generated executables, caches, DOT and SVG files are excluded through .gitignore.
