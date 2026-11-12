# Comparison with Cargo

The checked-in index snapshot and Cargo.lock pin the inputs used for this comparison.
The reference command used Cargo 1.90.0 on the x86_64-unknown-linux-gnu target.
Each count includes the root and counts distinct crate names.

| Root | Analyzer nodes | Cargo nodes | Result |
| --- | ---: | ---: | --- |
| itoa 1.0.19 | 1 | 1 | Both report a leaf. |
| csv-core 0.1.13 | 2 | 2 | Both report memchr 2.8.3 as the only child. |
| csv 1.4.0 | 11 | 6 | Both report csv-core, itoa, ryu and serde_core as direct children. |

For csv, both tools also report csv-core -> memchr. The first differing edge is
serde_core 1.0.229 -> serde_derive 1.0.229. The index declares this normal dependency with
`target = "cfg(any())"`. The empty `any()` predicate is false. Cargo prunes this edge.
The analyzer retains target-conditioned declarations. Following that edge adds serde_derive,
proc-macro2, quote, syn and unicode-ident to the analyzer's graph.

Run `make compare` to regenerate the three ASCII trees, DOT files, SVGs and Cargo reference trees
under `build/demo`. Cargo may need Internet access on the first run. The reference manifest is
only an input to the comparison. The analyzer reads the sparse index independently.

Reference command for one root:

```sh
cargo tree --locked -e normal -p csv --target x86_64-unknown-linux-gnu --manifest-path examples/reference/Cargo.toml
```

Captured reference output:

```text
csv v1.4.0
├── csv-core v0.1.13
│   └── memchr v2.8.3
├── itoa v1.0.19
├── ryu v1.0.23
└── serde_core v1.0.229
```

The snapshot retains name, vers, yanked and the original deps array for each selected record.
`make demo` serves those records over loopback and exercises the same C download/parser path as a live registry.
Live queries can select later releases as the registry changes.

The current C frontend prints crate names in its ASCII tree. Version numbers in this table
identify the bundled records, independently of that display format.
