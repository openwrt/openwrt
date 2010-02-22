/*
 * arch/ubicom32/crypto/md5_ubicom32_asm.S
 *	MD5 (Message Digest 5) support for Ubicom32 v3 architecture
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
; The function prototypes
;*****************************************************************************************
; void md5_ip5k_init(void)
; void md5_ip5k_transform(u32_t *data_input)
; void md5_get_digest(u32_t *digest)

;*****************************************************************************************
; Inputs
;*****************************************************************************************;
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
; An: Address Registers
;*****************************************************************************************
#define an_digest A3
#define an_data_input A3
#define an_security_block A4

;*****************************************************************************************
; Hash Constants
;*****************************************************************************************
#define HASH_MD5_IN0 0x01234567
#define HASH_MD5_IN1 0x89abcdef
#define HASH_MD5_IN2 0xfedcba98
#define HASH_MD5_IN3 0x76543210

#define HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION 2
#define HASH_SECURITY_BLOCK_CONTROL_INIT_MD5 ((1 << 4) | HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION)

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

;*****************************************************************************************
; Assembly macros
;*****************************************************************************************
	; C compiler reserves RP (A5) for return address during subroutine call.
	; Use RP to return to caller
.macro	call_return_macro
	calli	RP, 0(RP)
.endm

#if 0
;*****************************************************************************************
;	void md5_ip5k_init(void)
;		initialize the output registers of the hash module
;
	;.section .text.md5_ip5k_init,"ax",@progbits
	.section .text
	.global	_md5_ip5k_init
	.func md5_ip5k_init, _md5_ip5k_init

_md5_ip5k_init:
	moveai an_security_block, #SECURITY_BASE_EFFECTIVE_ADDRESS

	movei hash_control, #%hi(HASH_SECURITY_BLOCK_CONTROL_INIT_MD5)
	movei hash_control_low, #%lo(HASH_SECURITY_BLOCK_CONTROL_INIT_MD5)

	movei hash_output_0, #%hi(HASH_MD5_IN0)
	movei hash_output_0_low, #%lo(HASH_MD5_IN0)

	movei hash_output_1, #%hi(HASH_MD5_IN1)
	movei hash_output_1_low, #%lo(HASH_MD5_IN1)

	movei hash_output_2, #%hi(HASH_MD5_IN2)
	movei hash_output_2_low, #%lo(HASH_MD5_IN2)

	movei hash_output_3, #%hi(HASH_MD5_IN3)
	movei hash_output_3_low, #%lo(HASH_MD5_IN3)

	call_return_macro
	.endfunc
#endif

;*****************************************************************************************
;	void md5_ip5k_init_digest(u32_t *hash_input)
;		initialize the output registers of the hash module

	;.section .text.md5_ip5k_init_digest,"ax",@progbits
	.section .text
	.global	_md5_ip5k_init_digest
	.func md5_ip5k_init_digest, _md5_ip5k_init_digest

_md5_ip5k_init_digest:
	movea an_data_input, D0

	moveai an_security_block, #SECURITY_BASE_EFFECTIVE_ADDRESS

	movei hash_control, #%hi(HASH_SECURITY_BLOCK_CONTROL_INIT_MD5)
	movei hash_control_low, #%lo(HASH_SECURITY_BLOCK_CONTROL_INIT_MD5)

	move.4 hash_output_0, (an_data_input)4++
	move.4 hash_output_1, (an_data_input)4++
	move.4 hash_output_2, (an_data_input)4++
	move.4 hash_output_3, (an_data_input)4++

	call_return_macro
	.endfunc

;*****************************************************************************************
;	void md5_ip5k_transform(u32_t *data_input)
;		performs intermediate transformation step for the hash calculation
;
	;.sect .text.md5_ip5k_transform,"ax",@progbits
	.section .text
	.global	_md5_ip5k_transform
	.func md5_ip5k_transform, _md5_ip5k_transform

_md5_ip5k_transform:
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

md5_ip5k_transform_wait:
	; wait for the module to calculate the output hash
	btst hash_status, #0
	jmpne.f md5_ip5k_transform_wait

	call_return_macro
	.endfunc

;*****************************************************************************************
;	void md5_ip5k_get_digest(u32_t *digest)
;		Return the hash of the input data
;
	;.sect .text.md5_get_digest,"ax",@progbits
	.section .text
	.global	_md5_ip5k_get_digest
	.func md5_ip5k_get_digest, _md5_ip5k_get_digest

_md5_ip5k_get_digest:
	movea an_digest, D0

	moveai an_security_block, #SECURITY_BASE_EFFECTIVE_ADDRESS

	; we have finished
	move.4 0(an_digest), hash_output_0
	move.4 4(an_digest), hash_output_1
	move.4 8(an_digest), hash_output_2
	move.4 12(an_digest), hash_output_3

	call_return_macro
	.endfunc
