# mini-elf-toolchain

A correctness-focused ELF64 x86-64 toolchain/linker laboratory. The project is intentionally staged so each layer is validated before the next one depends on it.

## Current stage

The repository currently implements a bounded ELF64 x86-64 header parser. It validates ELF identity/version/machine fields, canonical ELF64 header sizes, and program/section header table spans with checked arithmetic before any table access.

## Core roadmap

1. ELF64 header and table-bound validation
2. Validated section and symbol object model
3. RELA parsing and x86-64 relocation validation
4. Symbol resolution
5. Section layout
6. ELF executable emission
7. CLI and link map
8. Archive lazy extraction
9. Reproducibility and GNU/LLVM semantic differential harness

Each new capability should include focused malformed-input tests. Offsets, sizes, addresses, alignment, and relocation arithmetic must use checked operations where overflow can make an input invalid.

## Development

```sh
cargo fmt --all -- --check
cargo clippy --all-targets --all-features -- -D warnings
cargo test --all-targets --all-features
```
