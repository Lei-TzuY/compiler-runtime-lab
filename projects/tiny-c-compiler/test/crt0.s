  .globl _start
_start:
  call main
  mov %rax, %rdi
  mov $60, %rax
  syscall
  .section .note.GNU-stack,"",@progbits
