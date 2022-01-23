
.globl fahrenheit_to_celcius

fahrenheit_to_celcius:
    sub $32, %rsi
    movq $5, %rax
    mulq %rsi
    movq $9, %rsi
    divq %rsi
    movq %rax, %rsi
    retq
