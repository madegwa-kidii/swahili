section .data
fmt db "%d",10,0

section .text
global main
extern printf

main:
    mov rcx, fmt
    mov rdx, 7
    call printf
    ret
