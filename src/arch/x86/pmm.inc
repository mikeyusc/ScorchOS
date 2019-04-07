
global _pmmngr_paging_enable
global _pmmngr_is_paging
global _pmmngr_load_PDBR
global _pmmngr_get_PDBR
global _pmmngr_test

_pmmngr_get_PDBR:
	mov eax, cr3
	ret

_pmmngr_test:
	push ebp
	mov ebp,esp
	mov eax,[ebp+8]
	pop ebp
	ret

_pmmngr_load_PDBR:
	push ebp
	mov ebp,esp
	mov eax,[ebp+8]
	mov cr3, eax
	pop ebp
	ret


_pmmngr_is_paging:
	mov eax,cr0
	and eax,0x80000000
	jz nopage
	mov eax,1
	ret
nopage:
	mov eax,0

	ret




_pmmngr_paging_enable:
	push ebp
	mov ebp,esp
	mov ebx,[ebp+8]
	mov eax,cr0
	cmp ebx,1
	je enable
	jmp disable

enable:
	or eax, 0x80000000		
	mov	cr0, eax
	jmp done
disable:
	and eax, 0x7FFFFFFF		
	mov	cr0, eax

done:
	

	pop ebp

    ret


