/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#ifndef _DT_BINDINGS_RESET_ZTE_ZX279128S_RESET_H
#define _DT_BINDINGS_RESET_ZTE_ZX279128S_RESET_H

/*
 * Top CRM logical reset lines. Mapped by drivers/clk/zte/clk-zx279128s.c
 * to hardware register offsets and bit shifts.
 */
#define ZX279128S_RESET_USB_B9		0
#define ZX279128S_RESET_USB_B10		1
#define ZX279128S_RESET_USB_B11		2
#define ZX279128S_RESET_USB_B14		3
#define ZX279128S_RESET_COUNT		4

/*
 * Top CRM 2 logical reset lines.
 */
#define ZX279128S_CRM2_RESET_PCIE0	0
#define ZX279128S_CRM2_RESET_PCIE1	1
#define ZX279128S_CRM2_RESET_COUNT	2

#endif
