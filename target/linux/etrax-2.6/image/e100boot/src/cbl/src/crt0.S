	.global crt0    ; Needed because of a bug in binutils
	.global _target_address 
	.global _bytes_to_read

	.section .startup, "ax"

	nop
	nop

	
crt0:
	move.d 0x38001f00, sp
	jump _crt1
	
_bytes_to_read:	 .dword 0x12345678
_target_address: .dword 0x87654321

