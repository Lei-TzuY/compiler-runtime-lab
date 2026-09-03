# mini-debugger

A compact x86-64 Linux debugger built directly on `ptrace(2)`.

The current milestone is intentionally narrow but real: it can launch a tracee, classify stop/exit states, inspect registers and memory, install/remove software breakpoints, repair RIP after `INT3`, single-step the displaced instruction, reinsert breakpoints, and explicitly suppress or forward signals.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Requirements: Linux, x86-64, CMake 3.20+, and a C++17 compiler. The test environment must permit `PTRACE_TRACEME`.

## CLI

```sh
./build/mdbg ./path/to/program
```

The first milestone launches immediately and stops after `exec`. Available commands are:

- `continue` / `c`
- `stepi` / `si`
- `regs`
- `reg <name>`
- `x <address> [length]`
- `break <address>` / `b <address>`
- `delete <id>`
- `info breakpoints`
- `quit`

Breakpoints are numeric runtime addresses in this milestone. ELF symbol lookup, PIE load-bias resolution, DWARF source mapping, source-level stepping, unwinding, and watchpoints are future milestones and are not claimed as implemented.

## Breakpoint invariant

For a managed software breakpoint, the debugger follows this sequence:

```text
save original byte -> write 0xCC -> continue -> SIGTRAP
-> RIP -= 1 -> restore original byte -> expose breakpoint stop
-> single-step original instruction -> reinsert 0xCC -> continue/stop
```

The breakpoint table owns the saved byte. A breakpoint that is currently being stepped over is explicit debugger state rather than an implicit CLI convention.
