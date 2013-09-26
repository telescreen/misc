section .data
		hex_char	db	'0123456789ABCDEF'
		hexbuf		times 8 db 0x30
					db 0
		binbuf		times 32 db 0x30
					db 0
		
section .text
		extern _puts

		global _binrepr
		global _hexrepr
		global _snoob
		global _ha_abs
		global _ha_sign
		
;;; Convert a decimal to binary
_binrepr:
		push ebp
		mov ebp, esp
		mov eax, [ebp + 8]
		mov ebx, binbuf
		mov ecx, 32
__convert2:
		mov dl, 1
		and edx, eax
		add dl, 0x30
		mov byte [ebx + ecx - 1], dl
		shr eax, 1
		loop __convert2
		;; MacOS requires 16 bytes aligned
		sub esp, 4
		push binbuf
		call _puts
		add esp, 8		
		pop ebp
		ret

;;; Print a decimal number as hexa
_hexrepr:
		push ebp
		mov ebp, esp
		mov eax, [ebp + 8]
		mov ebx, hex_char
		mov ecx, 16
		mov esi, 8
__converthex:
		cmp eax, 0
		je __printhex
		xor edx, edx
		div ecx
		mov edi, edx
		mov dl, [ebx + edi]
		mov byte [hexbuf + esi - 1], dl
		dec esi
		jmp __converthex
__printhex:		
		;; MacOS requires 16 bytes aligned
		sub esp, 4
		push hexbuf
		call _puts
		add esp, 8
__endhexrepr:
		pop ebp
		ret		
		
;;; HAK Item 175
_snoob:
		push ebp
		mov ebp, esp
		mov ecx, [ebp + 8]
		mov ebx, ecx
		mov eax, ecx
		neg ebx
		and ebx, ecx
		add eax, ebx
		mov edi, eax
		xor eax, ecx
		shr eax, 2
		xor edx, edx
		div ebx
		or eax, edi
		pop ebp
		ret
		
_ha_abs:
		mov eax, [esp + 4]
		mov edx, eax
		sar edx, 21
		xor eax, edx
		sub eax, edx
		ret
		
;;; sign(x) =
;;; -1 if x < 0
;;; 0 if x = 0
;;; 1 if x > 0
_ha_sign:
		mov eax, [esp + 4]
		mov ebx, eax
		neg ebx
		sar eax, 31
		sar ebx, 31
		or eax, ebx
		ret
