# Architecture

`lowlevel::ptrace` is the syscall boundary. It owns errno-sensitive `PTRACE_PEEKDATA`, register access, resume operations, and byte patching.

`Process` owns the traced PID and lifecycle (`Running`, `Stopped`, `Exited`, `Signaled`). Launch uses `PTRACE_TRACEME`, waits for the post-`exec` `SIGTRAP`, then enables `PTRACE_O_EXITKILL`.

`Debugger` owns semantic execution state: last stop reason, breakpoint table, original instruction bytes, IDs, and the optional pending breakpoint step-over address. Non-`SIGTRAP` stops are represented as signals and the caller explicitly chooses suppression or forwarding.

`ElfFile` is an independent parser/resolver. It validates ELF64/x86-64/little-endian headers, reads `SHT_SYMTAB` and `SHT_DYNSYM` directly, and resolves symbols without libbfd/libelf/debugger libraries. For ET_DYN PIE executables it derives load bias from the tracee's offset-zero executable mapping and the ELF's zero-offset PT_LOAD virtual address. ET_EXEC symbols use their linked virtual addresses directly.

On a managed `SIGTRAP`, `Debugger` checks `RIP - 1` against installed breakpoints, rewinds RIP, restores the original byte, and marks the breakpoint temporarily uninstalled. The next `continue` or `stepi` executes exactly one original instruction and reinstalls `INT3` only if the breakpoint still exists.

Current limits: one traced process/thread, x86-64 Linux only, no attach, no DWARF parser, no unwinder, and no hardware watchpoints.
