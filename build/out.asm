section .data
DataPointer dw 0
Data db 0
section .text
global _start
_start:
    mov rbx, [DataPointer]
    mov al, [Data+rbx]
    inc al
    mov [Data+rbx], al
    mov rbx, [DataPointer]
    mov al, [Data+rbx]
    inc al
    mov [Data+rbx], al
    mov rbx, [DataPointer]
    mov al, [Data+rbx]
    inc al
    mov [Data+rbx], al
    mov rbx, [DataPointer]
    mov al, [Data+rbx]
    inc al
    mov [Data+rbx], al
    mov rbx, [DataPointer]
    mov al, [Data+rbx]
    inc al
    mov [Data+rbx], al
    mov rbx, [DataPointer]
    mov al, [Data+rbx]
    inc al
    mov [Data+rbx], al
    mov rbx, [DataPointer]
    mov al, [Data+rbx]
    inc al
    mov [Data+rbx], al
    mov rbx, [DataPointer]
    mov al, [Data+rbx]
    push rax
    mov rax, 1
    mov edi, 1
    mov rsi, rsp
    mov rdx, 1
    syscall
    add rsp, 8
