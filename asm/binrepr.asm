section .text
		extern _puts

		global _snoob
		global _habs
		global _hsign		
		
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
		
_habs:
		mov eax, [esp + 4]
		mov edx, eax
		sar edx, 31
		xor eax, edx
		sub eax, edx
		ret
		
;;; sign(x) =
;;; -1 if x < 0
;;; 0 if x = 0
;;; 1 if x > 0
_hsign:
		mov eax, [esp + 4]
		mov ebx, eax
		neg ebx
		sar eax, 31
		sar ebx, 31
		or eax, ebx
		ret
