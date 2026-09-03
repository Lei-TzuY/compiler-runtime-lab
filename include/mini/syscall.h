#ifndef MINI_LIBC_MINI_SYSCALL_H
#define MINI_LIBC_MINI_SYSCALL_H

/*
 * Raw Linux x86-64 syscall boundary.
 *
 * These functions deliberately expose kernel return values directly:
 * successful calls return their normal non-negative result, while failures
 * return -errno in the inclusive range [-4095, -1].  They are not POSIX libc
 * wrappers and do not set errno.
 */

long mini_sys_read(int fd, void *buf, unsigned long count);
long mini_sys_write(int fd, const void *buf, unsigned long count);
long mini_sys_close(int fd);
long mini_sys_lseek(int fd, long offset, int whence);
long mini_sys_brk(void *addr);
long mini_sys_mmap(void *addr, unsigned long length, int prot, int flags,
                   int fd, long offset);
long mini_sys_munmap(void *addr, unsigned long length);
__attribute__((noreturn)) void mini_sys_exit(int status);

#endif
