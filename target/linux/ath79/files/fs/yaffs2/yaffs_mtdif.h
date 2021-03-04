/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2018 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __YAFFS_MTDIF_H__
#define __YAFFS_MTDIF_H__

#include "yaffs_guts.h"

void yaffs_mtd_drv_install(struct yaffs_dev *dev);
struct mtd_info * yaffs_get_mtd_device(dev_t sdev);
void yaffs_put_mtd_device(struct mtd_info *mtd);
int yaffs_verify_mtd(struct mtd_info *mtd, int yaffs_version, int inband_tags);
#endif
