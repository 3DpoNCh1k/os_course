.globl my_pow

my_pow:
    movq $0, %rax
    cmpq $0, %rdi
    jz ret
    movq $1, %rax
cycle:
    cmpq $0, %rsi
    jz ret
    decq %rsi
    mulq %rdi
    jmp cycle
ret:
    retq