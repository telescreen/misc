;;; Just to understand shr/sar/shl/sal
;;; compile:
;;; yasm -a x86 -f macho binrepr.asm
;;; gcc -m32 -o shift shift.o binrepr.o
;;; Change shift instruction / number of shift to see the different
;;; Tested on MacOSX. Perhaps run on Linux too.

section .data
		inform_str	db		"Input number: ",0
		count_str	db		"Number of shift: ",0
		int_fmt_str db		"%d",0
		number		dd		0
		count		dd		0
		sar_fmt_str	db		"shift right: %d",0xa,0
		carry_fmt	db		"carry flag ",0,0xa,0 ; 11th byte is 0 or 1

section .text
		global _main
		extern _printf
		extern _scanf
		extern _getchar
		extern _binrepr

_main:
		push ebp
		mov ebp, esp
		sub esp, 0x4
		
		;; Input number
		push inform_str
		call _printf
		mov [esp], dword int_fmt_str
		mov [esp + 4], dword number
		call _scanf

		;; remove annoying 0xa
		call _getchar

		;; Input count		
		mov [esp], dword count_str
		call _printf
		mov [esp], dword int_fmt_str
		mov [esp + 4], dword count
		call _scanf
		
		mov eax, [count]
		mov ebx, [number]		
__shift_loop:
		cmp eax, 0
		je __shift_loop_end
		mov [esp], dword sar_fmt_str
		mov [esp + 4], eax
		call _printf
		
		shl ebx, 1
		
		mov [esp], ebx
		mov esi, ebx
		call _binrepr
		mov ebx, esi
		jc __if_carry
		mov byte [carry_fmt + 11], 0x30 ; '0'
		jmp __end_if_carry
__if_carry:
		mov byte [carry_fmt + 11], 0x31 ; '1'
__end_if_carry:
		mov [esp], dword carry_fmt
		call _printf
		mov eax, [count]
		dec eax
		mov dword [count], eax
		jmp __shift_loop
__shift_loop_end:		
		add esp, 8		
		pop ebp
		ret
