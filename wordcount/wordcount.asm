sys_write:      equ             1
sys_exit:       equ             60

stdout_fileno:  equ             1
stderr_fileno:  equ             2

                section         .text

                global          _start

_start:
                xor             ebx, ebx            ; счётчик числа слов
                xor             r8, r8              ; показатель нового слова
                xor             r9, r9              ; текущий символ

.read_next_buf:
                xor             eax, eax            ; sys_read
                xor             edi, edi            ; stdin_fileno
                mov             rsi, buf
                mov             rdx, buf_size
                syscall
                test            rax, rax
                js              read_error
                jz              .eof

                xor             ecx, ecx
.next_byte:
                cmp             rcx, rax
                je              .read_next_buf
                
                mov             r9b, byte [rsi + rcx]
                inc             rcx
                
                cmp             r9b, 0x20
                je              .space

                cmp             r9b, 0x0d
                jg              .word
                cmp             r9b, 0x09
                jl              .word
                
                jmp             .space
                
.space:
                xor             r8, r8
                jmp             .next_byte
                
.word:
                test            r8, r8
                jnz              .next_byte
                
                inc             r8
                inc             rbx
                jmp		 .next_byte

.eof:
                call            print_int

                mov             rax, sys_exit
                xor             edi, edi
                syscall

print_int:
                mov             rax, rbx
                mov             ecx, 10
                
                mov             rbx, rsp
                mov             rsi, rsp
                sub             rsp, 24

                dec             rsi
                mov             byte [rsi], 0x0a

.next_char:
                xor             edx, edx
                div             rcx
                dec             rsi
                add             dl, '0'
                mov             [rsi], dl
                test            rax, rax
                jnz             .next_char
                
                mov             rax, sys_write
                mov             rdi, stdout_fileno
                mov             rdx, rbx
                sub             rdx, rsi
                syscall

                add             rsp, 24
                ret

read_error:
                mov             rax, sys_write
                mov             rdi, stderr_fileno
                mov             rsi, read_error_msg
                mov             rdx, read_error_msg_size
                syscall

                mov             rax, sys_exit
                mov             edi, 1
                syscall
                
                section         .bss

buf_size:       equ             8 * 1024
buf:            resb            buf_size

                section         .rodata

read_error_msg: db              "read failure",0x0a
read_error_msg_size: equ        $ - read_error_msg
