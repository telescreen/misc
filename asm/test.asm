section .data
		msg		db		"Hello world!",0x0a

section .text
		global start
start:
		mov rax, 0x2000004
		mov rdi, 1
		mov rsi, qword msg
		mov rdx, 14
		syscall
		mov rax, 0x2000001
		mov rdi, 0
		syscall
