    .text
    .globl _start
    .type _start, @function
_start:
    xorl %ebp, %ebp
    movq %rsp, %rdi
    andq $-16, %rsp
    call __mini_start
    ud2
    .size _start, .-_start

    .section .note.GNU-stack,"",@progbits
