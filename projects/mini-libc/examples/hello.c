#include <mini/syscall.h>

int main(int argc, char **argv, char **envp)
{
    static const char message[] = "hello from mini-libc\n";

    (void)argc;
    (void)argv;
    (void)envp;
    return mini_sys_write(1, message, sizeof(message) - 1) ==
                   (long)(sizeof(message) - 1)
               ? 0
               : 1;
}
