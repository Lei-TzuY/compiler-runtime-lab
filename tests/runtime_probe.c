#include <mini/syscall.h>

static int equals(const char *left, const char *right)
{
    while (*left != '\0' && *left == *right) {
        ++left;
        ++right;
    }
    return *left == *right;
}

int main(int argc, char **argv, char **envp)
{
    static const char ok[] = "runtime-ok\n";
    int saw_sentinel = 0;
    char **entry;

    if (argc != 3 || !equals(argv[0], "./build/runtime_probe") ||
        !equals(argv[1], "alpha") || !equals(argv[2], "beta") ||
        argv[3] != (char *)0) {
        return 10;
    }

    for (entry = envp; *entry != (char *)0; ++entry) {
        if (equals(*entry, "MINI_LIBC_SENTINEL=present")) {
            saw_sentinel = 1;
            break;
        }
    }
    if (!saw_sentinel) {
        return 11;
    }

    if (mini_sys_write(1, ok, sizeof(ok) - 1) != (long)(sizeof(ok) - 1)) {
        return 12;
    }
    return 37;
}
