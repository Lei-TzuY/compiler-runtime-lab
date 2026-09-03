#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile uint64_t fixture_value = 0x1122334455667788ULL;

__attribute__((noinline)) void breakpoint_one(void) {
  __asm__ volatile("nop" ::: "memory");
  fixture_value += 1;
}

__attribute__((noinline)) void breakpoint_two(void) {
  __asm__ volatile("nop" ::: "memory");
  fixture_value += 1;
}

static void publish_addresses(const char* path) {
  FILE* file = fopen(path, "w");
  if (file == NULL) _Exit(80);
  fprintf(file, "%p %p %p\n", (void*)&breakpoint_one, (void*)&breakpoint_two,
          (void*)&fixture_value);
  if (fclose(file) != 0) _Exit(81);
}

int main(int argc, char** argv) {
  if (argc < 3) return 82;
  publish_addresses(argv[1]);
  raise(SIGSTOP);

  if (strcmp(argv[2], "exit") == 0) return 0;
  if (strcmp(argv[2], "trap") == 0) {
    raise(SIGTRAP);
    return 0;
  }
  if (strcmp(argv[2], "signal") == 0) {
    raise(SIGUSR1);
    return 0;
  }
  if (strcmp(argv[2], "terminate") == 0) {
    raise(SIGTERM);
    return 83;
  }

  breakpoint_one();
  breakpoint_two();
  breakpoint_one();
  return fixture_value == 0x112233445566778bULL ? 0 : 84;
}
