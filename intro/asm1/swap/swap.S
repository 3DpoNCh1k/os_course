
.globl rsi_rdx_swap
.globl get_rsi
.globl get_rdx

rsi_rdx_swap:
    movq %rsi, %rax
    movq %rdx, %rsi
    movq %rax, %rdx
    retq

get_rsi:
    movq %rsi, %rax
    retq

get_rdx:
    movq %rdx, %rax
    retq