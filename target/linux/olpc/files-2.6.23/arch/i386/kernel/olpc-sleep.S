.text

ENTRY(olpc_sleep_asm)
olpc_sleep:
	;; Get the value of PM1_CNT and store it off
	
	add 08h, ax
	mov bx,dx
	in dx,eax
	or 2000h, ax
	mov ax,di
	
	;; flush the cache
	wbinvd

	;; GX2 must disable refresh before going into self-refresh 
	mov 2000000180xh, ecx
	rdmsr
	mov eax, esi
	and 0FF0000FFh, eax
	wrmsr

	;; Now, put  the memory into self refresh
	mov 2004, cx
	xor edx, edx
	xor eax, eax
	mov 04h, al
	wrmsr

	;; Thats all she wrote - time to go to sleep
	
	mov bx, dx
	movzx di, eax
	out eax, dx

	;;  
	
	
	
