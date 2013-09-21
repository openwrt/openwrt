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

int mtd_check_rootfs_magic(struct mtd_info *mtd, size_t offset);

int mtd_find_rootfs_from(struct mtd_info *mtd,
			 size_t from,
			 size_t limit,
			 size_t *ret_offset);

#else
static inline int mtd_get_squashfs_len(struct mtd_info *master,
				       size_t offset,
				       size_t *squashfs_len)
{
	return -ENODEV;
}

static inline int mtd_check_rootfs_magic(struct mtd_info *mtd, size_t offset)
{
	return -EINVAL;
}

static inline int mtd_find_rootfs_from(struct mtd_info *mtd,
				       size_t from,
				       size_t limit,
				       size_t *ret_offset)
{
	return -ENODEV;
}
#endif /* CONFIG_MTD_SPLIT */

#endif /* _MTDSPLIT_H */
