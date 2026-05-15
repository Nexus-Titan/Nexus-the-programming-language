; Nexus Fast Math (ASM x86_64)
section .text
global _add_fast
_add_fast:
    mov rax, rdi
    add rax, rsi
    ret
; Nexus Bit Hash (ASM)
global _bit_hash
_bit_hash:
    mov rax, rdi
    rol rax, 7
    xor rax, 0x55AA55AA
    ret
