# mini-libc

A small, correctness-focused C runtime and libc laboratory for x86-64 Linux.
The long-term goal is to provide a progressively usable userspace runtime for
`tiny-c-compiler`, `mini-elf-toolchain`, and eventually `minios-x86`, without
trying to recreate glibc.

## Current milestone: freestanding runtime

The repository already builds real ELF executables through this path:

```text
Linux process entry
  -> _start (mini-libc crt0)
  -> decode argc / argv / envp
  -> main(argc, argv, envp)
  -> mini_sys_exit(main_status)
  -> SYS_exit
```

`examples/hello.c` writes through `mini_sys_write`, which executes the Linux
x86-64 `write` syscall directly. No host CRT or host libc is linked into the
resulting executable.

The raw syscall layer currently implements `read`, `write`, `close`, `lseek`,
`brk`, `mmap`, `munmap`, and `exit`. Its API is intentionally named
`mini_sys_*`: failures return negative kernel errno values directly. Standard
POSIX wrappers and `errno` will be added only when their semantics can be
implemented completely.

## Build and verify

Requirements: an x86-64 Linux environment with a C compiler, GNU-compatible
`ld`/`ar`, `readelf`, `nm`, and POSIX shell utilities.

```sh
make
make test
make inspect
./build/hello
```

`make test` verifies process-stack decoding, propagation of `main`'s return
status, direct syscall behavior, and mmap/munmap. `make inspect` rejects a
`PT_INTERP`, dynamic `NEEDED` entries, or unresolved symbols in milestone
executables.

## Layout

```text
include/mini/       implemented project-specific public APIs
src/crt/            process entry and startup
src/syscall/        Linux x86-64 syscall boundary
tests/              runtime/syscall probes and ELF independence checks
examples/           freestanding sample programs
docs/               ABI contracts and design notes
```

Standard headers such as `string.h`, `stdlib.h`, and `stdio.h` are deliberately
absent until the corresponding APIs actually exist.

See [`docs/abi.md`](docs/abi.md) for the exact ABI assumptions and raw syscall
contract.

## Next

The next high-value layer is the C memory/string core (`memcpy`, `memmove`,
`memset`, `memcmp`, `strlen`, comparisons and copy/search routines) with
edge-case and differential tests. Cross-repository integration will wait until
mini-libc is stable on the system assembler/linker bootstrap path.
