
.globl rsi_rdx_stack_swap

rsi_rdx_stack_swap:
    pushq %rsi
    pushq %rdx
    popq %rsi
    popq %rdx
    retq

