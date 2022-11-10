/*
 * rtk_chip.h
 *
 * Copyright (c) 2017 Realtek Semiconductor Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 */

#ifndef _RTK_CHIP_H_INCLUDED_
#define _RTK_CHIP_H_INCLUDED_

#include <linux/sys_soc.h>

enum rtd_chip_id {
	CHIP_ID_RTD1293 = 0x1293,
	CHIP_ID_RTD1295 = 0x1295,
	CHIP_ID_RTD1296 = 0x1296,
	CHIP_ID_RTD1319 = 0x1319,
	CHIP_ID_RTD1619B = 0x1619B,
	CHIP_ID_UNKNOWN = 0xFFFFF,
};

static inline int get_rtd_chip_id(void)
{
	const struct soc_device_attribute *soc_att_match = NULL;
	struct soc_device_attribute rtk_soc[] = {
		{
			.family = "Realtek Phoenix",
			.data = (void *)CHIP_ID_RTD1195,
		},
		{
			.family = "Realtek Kylin",
			.soc_id = "RTD1293",
			.data = (void *)CHIP_ID_RTD1293,
		},
		{
			.family = "Realtek Kylin",
			.soc_id = "RTD1295",
			.data = (void *)CHIP_ID_RTD1295,
		},
		{
			.family = "Realtek Kylin",
			.soc_id = "RTD1296",
			.data = (void *)CHIP_ID_RTD1296,
		},
		{
			.family = "Realtek Hank",
			.soc_id = "RTD1319",
			.data = (void *)CHIP_ID_RTD1319,
		},
		{
			.family = "Realtek Stark",
			.soc_id = "RTD1619B",
			.data = (void *)CHIP_ID_RTD1619B,
		},
		{
		/* empty */
		}
	};

	soc_att_match = soc_device_match(rtk_soc);
	if (soc_att_match) {
		pr_info("%s get chip id %x\n", __func__,
			    (int)(uintptr_t)soc_att_match->data);
		return (int)(uintptr_t)soc_att_match->data;
	} else {
		pr_err("%s ERROR: no match chip_id\n", __func__);
		return CHIP_ID_UNKNOWN;
	}
}

enum rtd_chip_revision {
	RTD_CHIP_A00 = 0xA00,
	RTD_CHIP_A01 = 0xA01,
	RTD_CHIP_A02 = 0xA02,
	RTD_CHIP_B00 = 0xB00,
	RTD_CHIP_B01 = 0xB01,
	RTD_CHIP_B02 = 0xB02,
	RTD_CHIP_UNKNOWN_REV = 0xFFF,
};

static inline int get_rtd_chip_revision(void)
{
	const struct soc_device_attribute *soc_att_match = NULL;
	struct soc_device_attribute rtk_soc[] = {
		{
			.revision = "A00",
			.data = (void *)RTD_CHIP_A00,
		},
		{
			.revision = "A01",
			.data = (void *)RTD_CHIP_A01,
		},
		{
			.revision = "A02",
			.data = (void *)RTD_CHIP_A02,
		},
		{
			.revision = "B00",
			.data = (void *)RTD_CHIP_B00,
		},
		{
			.revision = "B01",
			.data = (void *)RTD_CHIP_B01,
		},
		{
			.revision = "B02",
			.data = (void *)RTD_CHIP_B02,
		},
		{
		/* empty */
		}
	};

	soc_att_match = soc_device_match(rtk_soc);
	if (soc_att_match) {
		pr_info("%s get chip revision %x\n", __func__,
			    (int)(uintptr_t)soc_att_match->data);
		return (int)(uintptr_t)soc_att_match->data;
	} else {
		pr_err("%s ERROR: no match chip revision\n", __func__);
		return RTD_CHIP_UNKNOWN_REV;
	}

}
#endif //_RTK_CHIP_H_INCLUDED_
