/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <asm/ifxmips/ifxmips.h>

void
ifxmips_pmu_enable(unsigned int module)
{
	int err = 1000000;

	ifxmips_w32(ifxmips_r32(IFXMIPS_PMU_PWDCR) & ~module, IFXMIPS_PMU_PWDCR);
	while (--err && (ifxmips_r32(IFXMIPS_PMU_PWDSR) & module)) {}

	if (!err)
		panic("activating PMU module failed!");
}
EXPORT_SYMBOL(ifxmips_pmu_enable);

void
ifxmips_pmu_disable(unsigned int module)
{
	ifxmips_w32(ifxmips_r32(IFXMIPS_PMU_PWDCR) | module, IFXMIPS_PMU_PWDCR);
}
EXPORT_SYMBOL(ifxmips_pmu_disable);
