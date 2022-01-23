.globl min

min:
    movq %rdi, %rax
    cmpq %rdi, %rsi
    ja swap
    movq %rsi, %rax
swap:
    retq
// Put your code here