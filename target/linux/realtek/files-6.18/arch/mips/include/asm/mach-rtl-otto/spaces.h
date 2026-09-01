/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __ASM_MACH_RTL_OTTO_SPACES_H_
#define __ASM_MACH_RTL_OTTO_SPACES_H_

#ifdef CONFIG_RTL960X

/* Double space for RTL9607C - for two PCIe ports */

#define PCI_IOSIZE	(SZ_64K * 2)
#define IO_SPACE_LIMIT	(PCI_IOSIZE - 1)
#endif /* CONFIG_RTL960X */

#include <asm/mach-generic/spaces.h>

#endif /* __ASM_MACH_RTL_OTTO_SPACES_H */
