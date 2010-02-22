/*
 * arch/ubicom32/crypto/crypto_ubicom32.h
 *   Support for Ubicom32 cryptographic instructions.
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
#ifndef _CRYPTO_ARCH_UBICOM32_CRYPT_H
#define _CRYPTO_ARCH_UBICOM32_CRYPT_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/spinlock.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/ip5000.h>

#define CRYPTO_UBICOM32_LOOP_ASM 1
#define CRYPTO_UBICOM32_ALIGNMENT 4
#define SEC_ALIGNED(p) (((u32)p & 3) == 0)

#define SEC_BASE		SECURITY_BASE
#define SEC_KEY_OFFSET		SECURITY_KEY_VALUE(0)
#define SEC_INPUT_OFFSET	SECURITY_KEY_IN(0)
#define SEC_OUTPUT_OFFSET	SECURITY_KEY_OUT(0)
#define SEC_HASH_OFFSET		SECURITY_KEY_HASH(0)

#define SEC_KEY_128_BITS	SECURITY_CTRL_KEY_SIZE(0)
#define SEC_KEY_192_BITS	SECURITY_CTRL_KEY_SIZE(1)
#define SEC_KEY_256_BITS	SECURITY_CTRL_KEY_SIZE(2)

#define SEC_HASH_NONE		SECURITY_CTRL_HASH_ALG_NONE
#define SEC_HASH_MD5		SECURITY_CTRL_HASH_ALG_MD5
#define SEC_HASH_SHA1		SECURITY_CTRL_HASH_ALG_SHA1

#define SEC_CBC_SET		SECURITY_CTRL_CBC
#define SEC_CBC_NONE		0

#define SEC_ALG_AES		SECURITY_CTRL_CIPHER_ALG_AES
#define SEC_ALG_NONE		SECURITY_CTRL_CIPHER_ALG_NONE
#define SEC_ALG_DES		SECURITY_CTRL_CIPHER_ALG_DES
#define SEC_ALG_3DES		SECURITY_CTRL_CIPHER_ALG_3DES

#define SEC_DIR_ENCRYPT		SECURITY_CTRL_ENCIPHER
#define SEC_DIR_DECRYPT		0

#define CRYPTO_UBICOM32_PRIORITY 300
#define CRYPTO_UBICOM32_COMPOSITE_PRIORITY 400

#define HW_CRYPTO_PS_MAX_IDLE_MS 100    /* idle time (ms) before shuting down sm */

extern spinlock_t crypto_ubicom32_lock;
extern bool crypto_ubicom32_inited;
extern volatile bool crypto_ubicom32_on;
extern volatile unsigned long crypto_ubicom32_last_use;
extern struct timer_list crypto_ubicom32_ps_timer;
extern void crypto_ubicom32_ps_check(unsigned long data);

#define SEC_COPY_2W(t, s)				\
	asm volatile (					\
	"	move.4 0(%0), 0(%1)		\n\t"	\
	"	move.4 4(%0), 4(%1)		\n\t"	\
							\
		:					\
		: "a" (t), "a" (s)			\
	)

#define SEC_COPY_4W(t, s)				\
	asm volatile (					\
	"	move.4 0(%0), 0(%1)		\n\t"	\
	"	move.4 4(%0), 4(%1)		\n\t"	\
	"	move.4 8(%0), 8(%1)		\n\t"	\
	"	move.4 12(%0), 12(%1)		\n\t"	\
		:					\
		: "a" (t), "a" (s)			\
	)

#define SEC_COPY_5W(t, s)				\
	asm volatile (					\
	"	move.4 0(%0), 0(%1)		\n\t"	\
	"	move.4 4(%0), 4(%1)		\n\t"	\
	"	move.4 8(%0), 8(%1)		\n\t"	\
	"	move.4 12(%0), 12(%1)		\n\t"	\
	"	move.4 16(%0), 16(%1)		\n\t"	\
		:					\
		: "a" (t), "a" (s)			\
	)

#define SEC_SET_KEY_2W(x)				\
	asm volatile (					\
	"	; write key to Security Keyblock \n\t"	\
	"	move.4 0x10(%0), 0(%1)		\n\t"	\
	"	move.4 0x14(%0), 4(%1)		\n\t"	\
		:					\
		: "a" (SECURITY_BASE), "a" (x)		\
	)

#define SEC_SET_KEY_4W(x) \
	asm volatile ( \
	"	; write key to Security Keyblock \n\t"	\
	"	move.4 0x10(%0), 0(%1)		\n\t"	\
	"	move.4 0x14(%0), 4(%1)		\n\t"	\
	"	move.4 0x18(%0), 8(%1)		\n\t"	\
	"	move.4 0x1c(%0), 12(%1)		\n\t"	\
		:					\
		: "a"(SECURITY_BASE), "a"(x)		\
	)

#define SEC_SET_KEY_6W(x)				\
	asm volatile (					\
	"	; write key to Security Keyblock \n\t"	\
	"	move.4 0x10(%0), 0(%1)		\n\t"	\
	"	move.4 0x14(%0), 4(%1)		\n\t"	\
	"	move.4 0x18(%0), 8(%1)		\n\t"	\
	"	move.4 0x1c(%0), 12(%1)		\n\t"	\
	"	move.4 0x20(%0), 16(%1)		\n\t"	\
	"	move.4 0x24(%0), 20(%1)		\n\t"	\
		:					\
		: "a" (SECURITY_BASE), "a" (x)		\
	)

#define SEC_SET_KEY_8W(x)				\
	asm volatile (					\
	"	; write key to Security Keyblock \n\t"	\
	"	move.4 0x10(%0), 0(%1)		\n\t"	\
	"	move.4 0x14(%0), 4(%1)		\n\t"	\
	"	move.4 0x18(%0), 8(%1)		\n\t"	\
	"	move.4 0x1c(%0), 12(%1)		\n\t"	\
	"	move.4 0x20(%0), 16(%1)		\n\t"	\
	"	move.4 0x24(%0), 20(%1)		\n\t"	\
	"	move.4 0x28(%0), 24(%1)		\n\t"	\
	"	move.4 0x2c(%0), 28(%1)		\n\t"	\
		:					\
		: "a" (SECURITY_BASE), "a" (x)		\
	)

#define SEC_SET_KEY_64(k)	SEC_SET_KEY_2W(k)
#define SEC_SET_KEY_128(k)	SEC_SET_KEY_4W(k)
#define SEC_SET_KEY_192(k)	SEC_SET_KEY_6W(k)
#define SEC_SET_KEY_256(k)	SEC_SET_KEY_8W(k)

#define DES_SET_KEY(x) 		SEC_SET_KEY_64(x)
#define DES3_SET_KEY(x) 	SEC_SET_KEY_192(x)

#define SEC_SET_INPUT_2W(x)				\
	asm volatile (					\
	"	; write key to Security Keyblock \n\t"	\
	"	move.4 0x30(%0), 0(%1)		\n\t"	\
	"	move.4 0x34(%0), 4(%1)		\n\t"	\
		:					\
		: "a" (SECURITY_BASE), "a" (x)		\
	)

#define SEC_GET_OUTPUT_2W(x)				\
	asm volatile (					\
	"	; write key to Security Keyblock \n\t"	\
	"	move.4 0(%1), 0x50(%0)		\n\t"	\
	"	move.4 4(%1), 0x54(%0)		\n\t"	\
		:					\
		: "a" (SECURITY_BASE), "a" (x)		\
	)

#define SEC_SET_INPUT_4W(x) \
	asm volatile ( \
	"	; write key to Security Keyblock \n\t"	\
	"	move.4 0x30(%0), 0(%1)		\n\t"	\
	"	move.4 0x34(%0), 4(%1)		\n\t"	\
	"	move.4 0x38(%0), 8(%1)		\n\t"	\
	"	move.4 0x3c(%0), 12(%1)		\n\t"	\
		:					\
		: "a" (SECURITY_BASE), "a" (x)		\
	)

#define SEC_GET_OUTPUT_4W(x)				\
	asm volatile (					\
	"	; read output from Security Keyblock \n\t" \
	"	move.4 0(%1), 0x50(%0)		\n\t"	\
	"	move.4 4(%1), 0x54(%0)		\n\t"	\
	"	move.4 8(%1), 0x58(%0)		\n\t"	\
	"	move.4 12(%1), 0x5c(%0)		\n\t"	\
		:					\
		: "a" (SECURITY_BASE), "a" (x)		\
	)

#define SEC_SET_IV_4W(x)				\
	asm volatile (					\
	"	; write IV to Security Keyblock  \n\t"	\
	"	move.4 0x50(%0), 0(%1)		\n\t"	\
	"	move.4 0x54(%0), 4(%1)		\n\t"	\
	"	move.4 0x58(%0), 8(%1)		\n\t"	\
	"	move.4 0x5c(%0), 12(%1)		\n\t"	\
		:					\
		: "a" (SECURITY_BASE), "a" (x)		\
	)

#define SEC_PIPE_FLUSH() asm volatile ( " pipe_flush 0 \n\t" )

static inline void hw_crypto_set_ctrl(uint32_t c)
{
	asm volatile (
	"	move.4	0(%0), %1		\n\t"
		:
		: "a" (SECURITY_BASE + SECURITY_CTRL), "d" (c)
	);
}

static inline void hw_crypto_ps_start(void)
{
	crypto_ubicom32_ps_timer.expires = jiffies + msecs_to_jiffies(HW_CRYPTO_PS_MAX_IDLE_MS >> 1);
	add_timer(&crypto_ubicom32_ps_timer);
}

static inline void hw_crypto_turn_on(void)
{
	asm volatile (
	"	moveai  A4, %0			\n\t"
	"	bset    0x0(A4), 0x0(A4), %1	\n\t"
	"	cycles 11			\n\t"
		:
		: "i" (OCP_BASE >> 7), "i" (GEN_CLK_PLL_SECURITY_BIT_NO)
		: "a4", "cc"
	);
	crypto_ubicom32_on = true;
}

static inline void hw_crypto_turn_off(void)
{
	asm volatile (
	"	moveai  A4, %0			\n\t"
	"	bclr    0x0(A4), 0x0(A4), %1	\n\t"
		:
		: "i" (OCP_BASE >> 7), "i" (GEN_CLK_PLL_SECURITY_BIT_NO)
		: "a4", "cc"
	);
	crypto_ubicom32_on = false;
}

/*
 * hw_crypto_check
 * 	Most probably hw crypto is called in clusters and it makes no sense to turn it off
 * 	and on and waster 13 cycles every time.
 */
static inline void hw_crypto_check(void)
{
	if (likely(crypto_ubicom32_on)) {
		return;
	}
	crypto_ubicom32_last_use = jiffies;
	hw_crypto_turn_on();
	hw_crypto_ps_start();
}

/*
 * hw_crypto_ps_init
 * 	Init power save timer
 */
static inline void hw_crypto_ps_init(void)
{
	init_timer_deferrable(&crypto_ubicom32_ps_timer);
	crypto_ubicom32_ps_timer.function = crypto_ubicom32_ps_check;
	crypto_ubicom32_ps_timer.data = 0;
}

/*
 * hw_crypto_init()
 *      Initialize OCP security module lock and disables its clock.
 */
static inline void hw_crypto_init(void)
{
	if (!crypto_ubicom32_inited) {
		crypto_ubicom32_inited = true;
		spin_lock_init(&crypto_ubicom32_lock);
		hw_crypto_ps_init();
		hw_crypto_turn_off();
	}
}

/*
 * hw_crypto_lock()
 *      Locks the OCP security module and enables its clock.
 */
static inline void hw_crypto_lock(void)
{
	spin_lock_bh(&crypto_ubicom32_lock);
}

/*
 * hw_crypto_unlock()
 *      Unlocks the OCP security module and disables its clock.
 */
static inline void hw_crypto_unlock(void)
{
	crypto_ubicom32_last_use = jiffies;
	spin_unlock_bh(&crypto_ubicom32_lock);
}

#define CONFIG_CRYPTO_UBICOM32_DEBUG 1

#ifdef CONFIG_CRYPTO_UBICOM32_DEBUG
static inline void hex_dump(void *buf, int b_size, const char *msg)
{
	u8 *b = (u8 *)buf;
	int i;
	if (msg) {
		printk("%s:\t", msg);
	}

	for (i=0; i < b_size; i++) {
		printk("%02x ", b[i]);
		if ((i & 3) == 3) {
			printk(" ");
		}
		if ((i & 31) == 31) {
			printk("\n");
		}
	}
	printk("\n");
}
#define UBICOM32_SEC_DUMP(a, b, c) hex_dump(a, b, c)
#else
#define UBICOM32_SEC_DUMP(a, b, c)
#endif

#endif	/* _CRYPTO_ARCH_UBICOM32_CRYPT_H */
