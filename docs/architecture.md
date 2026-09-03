# Milestone 1 architecture

`lowlevel::ptrace` is the syscall boundary. It owns errno-sensitive `PTRACE_PEEKDATA`, register access, resume operations, and byte patching.

`Process` owns the traced PID and lifecycle (`Running`, `Stopped`, `Exited`, `Signaled`). Launch uses `PTRACE_TRACEME`, waits for the post-`exec` `SIGTRAP`, then enables `PTRACE_O_EXITKILL`.

`Debugger` owns semantic state: the last stop reason, software-breakpoint table, original instruction bytes, IDs, and the optional pending breakpoint step-over address. Non-`SIGTRAP` stops are represented as signals and the caller must explicitly choose suppression or forwarding on resume.

On a managed `SIGTRAP`, `Debugger` checks `RIP - 1` against installed breakpoints, rewinds RIP, restores the original byte, and marks the breakpoint temporarily uninstalled. The next `continue` or `stepi` executes exactly one original instruction and reinstalls `INT3` only if the breakpoint still exists.

Current limitations are deliberate: one traced thread/process, x86-64 Linux only, no attach, no ELF/DWARF parser, no symbolized breakpoints, no unwinder, and no hardware watchpoints yet.
