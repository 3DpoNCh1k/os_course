
.globl swap

// rip is saved (pushed on stack) by `call` instruction from caller
// rbp could be not saved by our callee-function `swap`

swap:
    pushq %rbp
    movq %rsp, %rbp
    pushq (%rdi)
    pushq (%rsi)
    popq (%rdi)
    popq (%rsi)
    popq %rbp
    retq