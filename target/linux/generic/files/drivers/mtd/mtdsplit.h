/*
 * Copyright (C) 2009-2013 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2009-2013 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2012 Jonas Gorski <jogo@openwrt.org>
 * Copyright (C) 2013 Hauke Mehrtens <hauke@hauke-m.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#ifndef _MTDSPLIT_H
#define _MTDSPLIT_H

#define ROOTFS_SPLIT_NAME	"rootfs_data"

#ifdef CONFIG_MTD_SPLIT
int mtd_get_squashfs_len(struct mtd_info *master,
			 size_t offset,
			 size_t *squashfs_len);
#else
static inline int mtd_get_squashfs_len(struct mtd_info *master,
				       size_t offset,
				       size_t *squashfs_len)
{
	return -ENODEV;
}
#endif

#endif /* _MTDSPLIT_H */
