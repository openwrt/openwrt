/*
 * arch/ubicom32/crypto/sha1_ubicom32_asm.S
 *	SHA1 hash support for Ubicom32 architecture V3.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#define __ASM__
#include <asm/ip5000.h>

#ifndef RP
#define RP A5
#endif

;*****************************************************************************************
; The function prototype
;*****************************************************************************************
; void sha1_ip5k_init(void)
; void sha1_ip5k_transform(u32_t *data_input)
; void sha1_ip5k_output(u32_t *digest)

;*****************************************************************************************
; Inputs
;*****************************************************************************************
; data_input is the pointer to the block of data over which the digest will be calculated.
;	It should be word aligned.
;
; digest is the pointer to the block of data into which the digest (the output) will be written.
;	It should be word aligned.
;

;*****************************************************************************************
; Outputs
;*****************************************************************************************
; None

;*****************************************************************************************
; Hash Constants
;*****************************************************************************************
#define HASH_SHA1_IN0 0x67452301
#define HASH_SHA1_IN1 0xefcdab89
#define HASH_SHA1_IN2 0x98badcfe
#define HASH_SHA1_IN3 0x10325476
#define HASH_SHA1_IN4 0xc3d2e1f0

#define HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION 2
#define HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1 ((1 << 5) | HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION)

;*****************************************************************************************
; An: Address Registers
;*****************************************************************************************
#define an_digest a4
#define an_data_input a4
#define an_security_block a3

;*****************************************************************************************
; Hash related defines
;*****************************************************************************************
#define hash_control 0x00(an_security_block)
#define hash_control_low 0x02(an_security_block)
#define hash_status 0x04(an_security_block)

#define hash_input_0 0x30(an_security_block)
#define hash_input_1 0x34(an_security_block)
#define hash_input_2 0x38(an_security_block)
#define hash_input_3 0x3c(an_security_block)
#define hash_input_4 0x40(an_security_block)

#define hash_output_0 0x70(an_security_block)
#define hash_output_0_low 0x72(an_security_block)
#define hash_output_1 0x74(an_security_block)
#define hash_output_1_low 0x76(an_security_block)
#define hash_output_2 0x78(an_security_block)
#define hash_output_2_low 0x7a(an_security_block)
#define hash_output_3 0x7c(an_security_block)
#define hash_output_3_low 0x7e(an_security_block)
#define hash_output_4 0x80(an_security_block)
#define hash_output_4_low 0x82(an_security_block)

;*****************************************************************************************
; Assembly macros
;*****************************************************************************************
	; C compiler reserves RP (A5) for return address during subroutine call.
	; Use RP to return to caller
.macro	call_return_macro
	calli	RP, 0(RP)
.endm

;*****************************************************************************************
;	void sha1_ip5k_init(void)
;		initialize the output registers of the hash module

	;.section .text.sha1_ip5k_init,"ax",@progbits
	.section .ocm_text,"ax",@progbits
	.global	_sha1_ip5k_init
	.func sha1_ip5k_init, _sha1_ip5k_init

_sha1_ip5k_init:
	moveai an_security_block, #SECURITY_BASE_EFFECTIVE_ADDRESS

	movei hash_control, #%hi(HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1)
	movei hash_control_low, #%lo(HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1)

	movei hash_output_0, #%hi(HASH_SHA1_IN0)
	movei hash_output_0_low, #%lo(HASH_SHA1_IN0)

	movei hash_output_1, #%hi(HASH_SHA1_IN1)
	movei hash_output_1_low, #%lo(HASH_SHA1_IN1)

	movei hash_output_2, #%hi(HASH_SHA1_IN2)
	movei hash_output_2_low, #%lo(HASH_SHA1_IN2)

	movei hash_output_3, #%hi(HASH_SHA1_IN3)
	movei hash_output_3_low, #%lo(HASH_SHA1_IN3)

	movei hash_output_4, #%hi(HASH_SHA1_IN4)
	movei hash_output_4_low, #%lo(HASH_SHA1_IN4)

	call_return_macro
	.endfunc

;*****************************************************************************************
;	void sha1_ip5k_init_digest(u32_t *hash_input)
;		initialize the output registers of the hash module

	;.section .text.sha1_ip5k_init_digest,"ax",@progbits
	.section .ocm_text,"ax",@progbits
	.global	_sha1_ip5k_init_digest
	.func sha1_ip5k_init_digest, _sha1_ip5k_init_digest

_sha1_ip5k_init_digest:
	movea an_data_input, D0

	moveai an_security_block, #SECURITY_BASE_EFFECTIVE_ADDRESS

	movei hash_control, #%hi(HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1)
	movei hash_control_low, #%lo(HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1)

	move.4 hash_output_0, (an_data_input)4++
	move.4 hash_output_1, (an_data_input)4++
	move.4 hash_output_2, (an_data_input)4++
	move.4 hash_output_3, (an_data_input)4++
	move.4 hash_output_4, (an_data_input)4++

	call_return_macro
	.endfunc

;*****************************************************************************************
;	void sha1_ip5k_transform(u32_t *data_input)
;		performs intermediate transformation step for the hash calculation

	;.section .text.sha1_ip5k_transform,"ax",@progbits
	.section .ocm_text,"ax",@progbits
	.global	_sha1_ip5k_transform
	.func sha1_ip5k_transform, _sha1_ip5k_transform

_sha1_ip5k_transform:
	movea an_data_input, D0

	moveai an_security_block, #SECURITY_BASE_EFFECTIVE_ADDRESS

	; Write the first 128bits (16 bytes)
	move.4 hash_input_0, (an_data_input)4++
	move.4 hash_input_1, (an_data_input)4++
	move.4 hash_input_2, (an_data_input)4++
	move.4 hash_input_3, (an_data_input)4++
	move.4 hash_input_4, D0

	move.4 hash_input_0, (an_data_input)4++
	move.4 hash_input_1, (an_data_input)4++
	move.4 hash_input_2, (an_data_input)4++
	move.4 hash_input_3, (an_data_input)4++
	move.4 hash_input_4, D0

	move.4 hash_input_0, (an_data_input)4++
	move.4 hash_input_1, (an_data_input)4++
	move.4 hash_input_2, (an_data_input)4++
	move.4 hash_input_3, (an_data_input)4++
	move.4 hash_input_4, D0

	move.4 hash_input_0, (an_data_input)4++
	move.4 hash_input_1, (an_data_input)4++
	move.4 hash_input_2, (an_data_input)4++
	move.4 hash_input_3, (an_data_input)4++
	move.4 hash_input_4, D0

	pipe_flush 0

sha1_ip5k_transform_wait:
	; wait for the module to calculate the output hash
	btst hash_status, #0
	jmpne.f sha1_ip5k_transform_wait

	call_return_macro
	.endfunc

;*****************************************************************************************
;	void sha1_ip5k_output(u32_t *digest)
;		Return the hash of the input data

	;.section .text.sha1_ip5k_output,"ax",@progbits
	.section .ocm_text,"ax",@progbits
	.global	_sha1_ip5k_output
	.func sha1_ip5k_output, _sha1_ip5k_output

_sha1_ip5k_output:
	movea an_digest, D0

	moveai an_security_block, #SECURITY_BASE_EFFECTIVE_ADDRESS

	; we have finished
	move.4 0(an_digest), hash_output_0
	move.4 4(an_digest), hash_output_1
	move.4 8(an_digest), hash_output_2
	move.4 12(an_digest), hash_output_3
	move.4 16(an_digest), hash_output_4

	call_return_macro
	.endfunc

;*****************************************************************************************
;END			;End of program code
;*****************************************************************************************
