# mini-debugger

A compact x86-64 Linux debugger built directly on `ptrace(2)` with a small in-tree ELF64 symbol parser.

Implemented now:

- launch under `PTRACE_TRACEME` and explicit process lifecycle states
- x86-64 general-purpose register inspection
- errno-correct memory reads
- software breakpoints with saved-byte ownership, RIP repair, displaced-instruction single-step, and reinsertion
- explicit signal suppression/forwarding policy
- ELF64 `.symtab` / `.dynsym` parsing
- PIE vs non-PIE load-bias resolution through `/proc/<pid>/maps`
- symbol -> runtime address and runtime address -> symbol resolution
- CLI breakpoints by numeric address or symbol (`break main`)
- deterministic PIE/non-PIE/stripped fixture coverage

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Requirements: Linux, x86-64, CMake 3.20+, and a C++17 compiler. The test environment must permit `PTRACE_TRACEME`.

## CLI

```text
$ ./build/mdbg ./hello
stopped after exec
(mdbg) break main
Breakpoint 1 at 0x55... (main)
(mdbg) continue
breakpoint at 0x55... (main)
(mdbg) regs
...
(mdbg) stepi
single-step trap
```

Commands currently implemented: `continue`, `stepi`, `regs`, `reg <name>`, `x <address|symbol> [length]`, `break <address|symbol>`, `delete <id>`, `info breakpoints`, `symbols [filter]`, and `quit`.

## Breakpoint invariant

```text
save original byte -> write 0xCC -> continue -> SIGTRAP
-> RIP -= 1 -> restore original byte -> expose breakpoint stop
-> single-step original instruction -> reinsert 0xCC -> continue/stop
```

The breakpoint table owns the saved byte. A breakpoint being stepped over is explicit debugger state, not a CLI convention.

## Current limits

One traced process/thread only. No attach, DWARF source mapping, source-level `next/step/finish`, unwinding/backtraces, or hardware watchpoints yet. ELF extended section numbering and non-x86-64/little-endian ELF are intentionally unsupported for now.
