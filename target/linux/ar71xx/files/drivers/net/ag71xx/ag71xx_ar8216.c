/*
 *  Atheros AR71xx built-in ethernet mac driver
 *  Special support for the Atheros ar8216 switch chip
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  Based on Atheros' AG7100 driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "ag71xx.h"

#define AR8216_PACKET_TYPE_MASK		0xf
#define AR8216_PACKET_TYPE_NORMAL	0

#define AR8216_HEADER_LEN	2

void ag71xx_add_ar8216_header(struct ag71xx *ag, struct sk_buff *skb)
{
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);

	if (!pdata->has_ar8216)
		return;

        skb_push(skb, AR8216_HEADER_LEN);
        skb->data[0] = 0x10;
        skb->data[1] = 0x80;
}

int ag71xx_remove_ar8216_header(struct ag71xx *ag,
				struct sk_buff *skb)
{
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
	u8 type;

	if (!pdata->has_ar8216)
		return 0;

	type = skb->data[1] & AR8216_PACKET_TYPE_MASK;

	switch (type) {
	case AR8216_PACKET_TYPE_NORMAL:
		skb_pull(skb, AR8216_HEADER_LEN);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
