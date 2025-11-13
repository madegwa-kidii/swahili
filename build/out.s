section .data
fmt db "%d",10,0

section .text
global main
extern printf

main:
    mov rdi, fmt
    mov rsi, 7
    xor rax, rax
    call printf
    ret
