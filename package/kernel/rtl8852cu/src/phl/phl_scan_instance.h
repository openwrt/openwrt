/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef _PHL_SCAN_INSTANCE_H_
#define _PHL_SCAN_INSTANCE_H_

#define MAX_SCAN_INSTANCE 160

enum order_strategy {
	ORDER_5GHZ_PRIOR = BIT(0),
	ORDER_ACTIVE_PRIOR = BIT(1)
};

enum skip_strategy {
	SKIP_2GHZ = BIT(0),
	SKIP_5GHZ = BIT(1),
	SKIP_PASSIVE = BIT(2),
	SKIP_DFS = BIT(3),
	SKIP_6GHZ = BIT(4),
};

enum period_strategy {
	PERIOD_ALL_MAX = BIT(0),
	PERIOD_ALL_MIN = BIT(1),
	PERIOD_MIN_DFS = BIT(2)
};

struct instance_strategy {
	u8 order;
	u8 skip;
	u8 period;
};

struct instance_channel {
	enum band_type band;
	u8 channel;
	u8 property;
	u8 active;
	u8 period;
	u8 mode;
	u8 bw;
	u8 offset;
};

struct instance {
	u32 cnt; /* channel cnt */
	struct instance_channel ch[MAX_SCAN_INSTANCE];
};


#endif /* _PHL_SCAN_INSTANCE_H_ */
