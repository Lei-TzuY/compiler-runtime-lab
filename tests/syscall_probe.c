#include <mini/syscall.h>

#define MINI_PROT_READ 0x1
#define MINI_PROT_WRITE 0x2
#define MINI_MAP_PRIVATE 0x02
#define MINI_MAP_ANONYMOUS 0x20
#define MINI_EBADF 9

int main(int argc, char **argv, char **envp)
{
    static const char ok[] = "syscall-ok\n";
    const unsigned long page_size = 4096;
    long mapping;
    char *memory;

    (void)argc;
    (void)argv;
    (void)envp;

    if (mini_sys_read(-1, (void *)0, 0) != -MINI_EBADF ||
        mini_sys_close(-1) != -MINI_EBADF ||
        mini_sys_lseek(-1, 0, 0) != -MINI_EBADF) {
        return 20;
    }

    if (mini_sys_brk((void *)0) <= 0) {
        return 21;
    }

    mapping = mini_sys_mmap((void *)0, page_size,
                            MINI_PROT_READ | MINI_PROT_WRITE,
                            MINI_MAP_PRIVATE | MINI_MAP_ANONYMOUS, -1, 0);
    if (mapping < 0) {
        return 22;
    }

    memory = (char *)mapping;
    memory[0] = 'm';
    memory[page_size - 1] = 'l';
    if (memory[0] != 'm' || memory[page_size - 1] != 'l') {
        return 23;
    }
    if (mini_sys_munmap(memory, page_size) != 0) {
        return 24;
    }

    if (mini_sys_write(1, ok, sizeof(ok) - 1) != (long)(sizeof(ok) - 1)) {
        return 25;
    }
    return 0;
}
