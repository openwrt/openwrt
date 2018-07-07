/*
 * LZMA compressed kernel loader for Realtek SoCs based boards
 *
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#include "cacheops.h"

#define LEXRA_CCTL_BARRIER						\
	do {								\
		__asm__ __volatile__ ( "" : : : "memory");		\
	} while (0)

void flush_cache(unsigned long start_addr, unsigned long size)
{
	write_c0_cctl(0);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(CCTL_DWB);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(0);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(CCTL_DInval);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(0);
	LEXRA_CCTL_BARRIER;
	write_c0_cctl(CCTL_IInval);
}

