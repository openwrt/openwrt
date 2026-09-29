.syntax unified
.arm
.global _start
.type _start, %function

_start:
	ldr r0, =device
	mov r1, #0
	mov r2, #0
	mov r7, #322
	svc #0
	mov r4, r0

	mov r0, #0
	mov r1, #0x1000
	mov r2, #1
	mov r3, #1
	ldr r5, =0x94406
	mov r7, #192
	svc #0
	mov r6, r0

	ldr r8, =1000
	mov r10, r6

sample:
	mov r0, #1
	mov r1, r10
	mov r2, #0x40
	mov r7, #4
	svc #0
	subs r8, r8, #1
	bne sample

	mov r0, #0
	mov r7, #1
	svc #0

.align 2
device:
	.asciz "/dev/mem"