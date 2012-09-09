/*
 *  RouterBoot definitions
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _ATH79_ROUTERBOOT_H_
#define _ATH79_ROUTERBOOT_H_

#ifdef CONFIG_ATH79_ROUTERBOOT
int routerboot_find_tag(u8 *buf, unsigned int buflen, u16 tag_id,
			u8 **tag_data, u16 *tag_len);
#else
static inline int
routerboot_find_tag(u8 *buf, unsigned int buflen, u16 tag_id,
		    u8 **tag_data, u16 *tag_len)
{
	return -ENOENT;
}
#endif

#endif /* _ATH79_ROUTERBOOT_H_ */
