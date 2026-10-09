/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include <mach/al_hal_common.h>
#include <mach/al_hal_reg_utils.h>
#include "al_hal_nb_regs.h"
#include "al_init_sys_fabric.h"
#include "al_init_ccu_regs.h"
#include "al_init_sys_fabric_offsets.h"

/* definition currently missing from nb_regs */
#define AL_NB_ACF_MISC_READ_BYPASS (1 << 30)

/* initialization of different units */
void al_nbservice_init(void __iomem *nb_regs_address,
		al_bool dev_ord_relax)
{
	struct al_nb_regs __iomem *nb_regs = nb_regs_address;

	/* allow reads to bypass writes to different addresses */
	al_reg_write32_masked(
			&(nb_regs->global.acf_misc),
			AL_NB_ACF_MISC_READ_BYPASS,
			(dev_ord_relax) ? AL_NB_ACF_MISC_READ_BYPASS : 0);
}

void al_ccu_init(void __iomem *ccu_address, al_bool iocc)
{
	/* enable snoop */
	if (iocc) {
		al_reg_write32(ccu_address + AL_CCU_SNOOP_CONTROL_IOFAB_0_OFFSET
									, 1);
		al_reg_write32(ccu_address + AL_CCU_SNOOP_CONTROL_IOFAB_1_OFFSET
									, 1);
	}
	/* disable speculative fetches from masters */
	al_reg_write32(ccu_address + AL_CCU_SPECULATION_CONTROL_OFFSET, 7);
}

void al_nbservice_clear_settings(void __iomem *nb_regs_address)
{
	struct al_nb_regs __iomem *nb_regs = nb_regs_address;

	al_reg_write32_masked(
		&(nb_regs->global.acf_misc),
		AL_NB_ACF_MISC_READ_BYPASS,
		0);
}

void al_ccu_clear_settings(void __iomem *ccu_address)
{
	al_reg_write32(ccu_address + AL_CCU_SNOOP_CONTROL_IOFAB_0_OFFSET, 0);
	al_reg_write32(ccu_address + AL_CCU_SNOOP_CONTROL_IOFAB_1_OFFSET, 0);
}
