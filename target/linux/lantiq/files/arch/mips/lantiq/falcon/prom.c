/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2011 Thomas Langer <thomas.langer@lantiq.com>
 * Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 */

#include <lantiq_soc.h>

#include "devices.h"

#include "../prom.h"

#define SOC_FALCON		"Falcon"
#define SOC_FALCON_D		"Falcon-D"
#define SOC_FALCON_V		"Falcon-V"
#define SOC_FALCON_M		"Falcon-M"

#define PART_SHIFT	12
#define PART_MASK	0x0FFFF000
#define REV_SHIFT	28
#define REV_MASK	0xF0000000
#define SREV_SHIFT	22
#define SREV_MASK	0x03C00000
#define TYPE_SHIFT	26
#define TYPE_MASK	0x3C000000

/* this parameter allows us enable/disable asc1 via commandline */
static int register_asc1;
static int __init
ltq_parse_asc1(char *p)
{
	register_asc1 = 1;
	return 0;
}
__setup("use_asc1", ltq_parse_asc1);

void __init
ltq_soc_setup(void)
{
	ltq_register_asc(0);
	ltq_register_wdt();
	falcon_register_gpio();
	if (register_asc1)
		ltq_register_asc(1);
}

void __init
ltq_soc_detect(struct ltq_soc_info *i)
{
	u32 type;
	i->partnum = (ltq_r32(LTQ_FALCON_CHIPID) & PART_MASK) >> PART_SHIFT;
	i->rev = (ltq_r32(LTQ_FALCON_CHIPID) & REV_MASK) >> REV_SHIFT;
	i->srev = ((ltq_r32(LTQ_FALCON_CHIPCONF) & SREV_MASK) >> SREV_SHIFT);
	sprintf(i->rev_type, "%c%d%d", (i->srev & 0x4) ? ('B') : ('A'),
		i->rev & 0x7, (i->srev & 0x3) + 1);

	switch (i->partnum) {
	case SOC_ID_FALCON:
		type = (ltq_r32(LTQ_FALCON_CHIPTYPE) & TYPE_MASK) >> TYPE_SHIFT;
		switch (type) {
		case 0:
			i->name = SOC_FALCON_D;
			break;
		case 1:
			i->name = SOC_FALCON_V;
			break;
		case 2:
			i->name = SOC_FALCON_M;
			break;
		default:
			i->name = SOC_FALCON;
			break;
		}
		i->type = SOC_TYPE_FALCON;
		break;

	default:
		unreachable();
		break;
	}
}
