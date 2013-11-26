section .bss
		array 	resd 	5
		sum		resq 	1

section .data
		input_fmt_str	db	"%f"
		inform_str		db	"Input 5 floats",0xa,0
		result_str		db	"Sum of 5 floats: %f",0		

section .text
		global _main
		extern _printf
		extern _scanf
_main:
		push ebp
		mov ebp, esp
		sub esp, 4
		push inform_str
		call _printf

		mov esi, array
		mov ecx, 5
__input_loop:
		mov edi, ecx
		mov [esp], dword input_fmt_str
		mov [esp+4], esi
		call _scanf
		add esi, 4
		mov ecx, edi
		loop __input_loop

		mov esi, array
		mov ecx, 5
		fldz
__sum_loop:		
		fadd qword [esi]
		add esi, 4
		loop __sum_loop
		fstp qword [sum]

		mov [esp], dword result_str
		mov [esp+4], dword sum		
		call _printf

		add esp, 8
		
		pop ebp
		ret
