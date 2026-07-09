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

#include <mach/al_hal_iofic_regs.h>
#include "al_hal_pcie_interrupts.h"
#include "al_hal_pcie_regs.h"

/* Enable PCIe controller interrupts */
int al_pcie_ints_config(struct al_pcie_port *pcie_port)
{
	al_iofic_config(pcie_port->app_int_grp_a_base, 0,
			INT_CONTROL_GRP_SET_ON_POSEDGE);
	al_iofic_config(pcie_port->app_int_grp_b_base, 0, 0);
	al_iofic_config(pcie_port->axi_int_grp_a_base, 0, 0);

	return 0;
}

void al_pcie_app_int_grp_a_unmask(struct al_pcie_port *pcie_port,
		uint32_t int_mask)
{
	al_iofic_unmask(pcie_port->app_int_grp_a_base, 0, int_mask);
}

void al_pcie_app_int_grp_a_mask(struct al_pcie_port *pcie_port,
		uint32_t int_mask)
{
	al_iofic_mask(pcie_port->app_int_grp_a_base, 0, int_mask);
}

void al_pcie_app_int_grp_b_unmask(struct al_pcie_port *pcie_port,
		uint32_t int_mask)
{
	al_iofic_unmask(pcie_port->app_int_grp_b_base, 0, int_mask);
}

void al_pcie_app_int_grp_b_mask(struct al_pcie_port *pcie_port,
		uint32_t int_mask)
{
	al_iofic_mask(pcie_port->app_int_grp_b_base, 0, int_mask);
}
