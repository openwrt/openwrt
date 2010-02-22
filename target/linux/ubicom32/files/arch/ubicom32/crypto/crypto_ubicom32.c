/*
 * arch/ubicom32/crypto/crypto_ubicom32.c
 *   Generic code to support ubicom32 hardware crypto accelerator
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
#include "crypto_ubicom32.h"

spinlock_t crypto_ubicom32_lock;
bool crypto_ubicom32_inited = false;
volatile bool crypto_ubicom32_on = false;
volatile unsigned long crypto_ubicom32_last_use;

struct timer_list crypto_ubicom32_ps_timer;
void crypto_ubicom32_ps_check(unsigned long data)
{
	unsigned long idle_time = msecs_to_jiffies(HW_CRYPTO_PS_MAX_IDLE_MS);

	BUG_ON(!crypto_ubicom32_on);

	if (((jiffies - crypto_ubicom32_last_use) > idle_time) && spin_trylock_bh(&crypto_ubicom32_lock)) {
                hw_crypto_turn_off();
                spin_unlock_bh(&crypto_ubicom32_lock);
		return;
	}

	/* keep monitoring */
	hw_crypto_ps_start();
}
