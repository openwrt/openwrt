/*
 * plio.c
 *	PLIO state machine support functions
 *
 * Copyright © 2009 Ubicom Inc. <www.ubicom.com>.  All rights reserved.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 2 of the License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */

#include <linux/types.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <asm/plio.h>

/*
 * plio_reset
 * 	Select and reset PLIO function
 */
static void plio_reset(const plio_fctl_t *plio_fctl) {
	plio_io_function_t plio_function = {
		.fn_sel		= PLIO_FN,
		.fn_reset	= 1,
	};

	/*
	 * enable extension port
	 */
	PEXT_NBR->function = plio_function;

	/*
	 * program clock dividers
	 */
	PLIO_NBR->fctl2 = plio_fctl->fctl2;

	/*
	 * select plio function and assert function reset
	 */
	plio_function.br_thread = thread_get_self();
	plio_function.fn_reset = 1;
	PLIO_NBR->function = plio_function;

	/*
	 * program plio controls
	 */
	PLIO_NBR->fctl0 = plio_fctl->fctl0;
	PLIO_NBR->fctl1 = plio_fctl->fctl1;

	/*
	 * deassert function reset
	 */
	plio_function.fn_reset = 0;
	PLIO_NBR->function = plio_function;
}

/*
 * plio_init
 *	configure and initialize PLIO.
 */
void plio_init(const plio_fctl_t *plio_fctl, const plio_config_t *plio_config, const plio_sram_t plio_sram_cfg[], int sram_cfg_size){
	/*
	 * first reset to start plio clock
	 */
	plio_reset(plio_fctl);

	udelay(1);

	/*
	 * configure pfsm
	 */
	PLIO_NBR->fctl0.pfsm_prog = 1;
	memcpy(PLIO_BR->pfsm_sram, plio_sram_cfg, sram_cfg_size);
	PLIO_NBR->fctl0.pfsm_prog = 0;

	/*
	 * program rest of plio
	 */
	memcpy(&PLIO_BR->config, plio_config, sizeof(plio_config_t));
}
