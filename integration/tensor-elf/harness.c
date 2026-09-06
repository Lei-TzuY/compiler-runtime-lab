#include <stdio.h>

/* The integration target is x86-64 Linux, where C int is the 32-bit ABI type
 * emitted by tiny-tensor-compiler for dtype=int32. Keep this freestanding
 * harness within mini-libc's existing header surface instead of inventing a
 * stdint.h dependency solely for the test. */
void tiny_tensor_run(int *out, const int *input0, const int *input1);

int main(void)
{
    int lhs[5] = {1, -2, 3, 1000000, -2000000000};
    int rhs[5] = {9, 12, -3, 234, 1};
    int expected[5] = {10, 10, 0, 1000234, -1999999999};
    int out[5] = {0, 0, 0, 0, 0};
    int i;

    tiny_tensor_run(out, lhs, rhs);
    for (i = 0; i < 5; ++i) {
        if (out[i] != expected[i]) {
            return 10 + i;
        }
    }

    if (puts("tensor-elf-ok") == EOF) {
        return 30;
    }
    return 0;
}
