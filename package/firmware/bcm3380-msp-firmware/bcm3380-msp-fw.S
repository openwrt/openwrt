	.set	noreorder
	.set	noat
	.text
	.globl	_start

_start:
	lui	$16, 0xe000
	ori	$8, $16, 0x1000
	lui	$9, 0x4b45
	ori	$9, $9, 0x4f4b
	sw	$9, 0x28($8)

	lui	$9, 0x1000
	sw	$9, 0x1800($16)

	mfc0	$8, $12
	lui	$9, 0x4000
	or	$8, $8, $9
	mtc0	$8, $12
	nop
	nop

	ori	$17, $16, 0x1204
	ori	$18, $16, 0x1240
	ori	$19, $16, 0x1a0c
	ori	$20, $16, 0x1c00
	ori	$21, $16, 0x1820
	ori	$24, $16, 0x102c
	lui	$25, 0x0420
	ori	$25, $25, 0x8000

loop:
	lw	$8, 0($21)
	andi	$9, $8, 0x0002
	bnez	$9, tx_high
	nop

	andi	$9, $8, 0x0004
	bnez	$9, tx_low
	nop

rx_check:
	lw	$8, 0($19)
	andi	$8, $8, 0x3fff
	beqz	$8, loop
	nop

	lw	$8, 0($17)
	bgez	$8, loop
	nop
	lw	$9, 0($18)

wait_second:
	lw	$8, 0($17)
	bgez	$8, wait_second
	nop
	lw	$10, 0($18)

	srl	$11, $9, 26
	bnez	$11, loop
	nop

	sw	$10, 0($20)
	b	loop
	nop

tx_high:
	addiu	$8, $0, 1
	mtc2	$8, $1
	nop
	mfc2	$10, $27
	nop
	b	tx_send
	nop

tx_low:
	addiu	$8, $0, 2
	mtc2	$8, $1
	nop
	mfc2	$10, $27
	nop

tx_send:
	lw	$11, 0($24)
	mtc2	$25, $31, 1
	mtc2	$10, $31, 1
	mtc2	$11, $31, 1
	b	loop
	nop
