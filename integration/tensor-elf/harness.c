#include <stdint.h>
#include <stdio.h>

void tiny_tensor_run(int32_t *out, const int32_t *input0, const int32_t *input1);

int main(void)
{
    int32_t lhs[5] = {1, -2, 3, 1000000, -2000000000};
    int32_t rhs[5] = {9, 12, -3, 234, 1};
    int32_t expected[5] = {10, 10, 0, 1000234, -1999999999};
    int32_t out[5] = {0, 0, 0, 0, 0};
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
