/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __YAFFS_VERIFY_H__
#define __YAFFS_VERIFY_H__

#include "yaffs_guts.h"

void yaffs_verify_blk(yaffs_dev_t *dev, yaffs_block_info_t *bi, int n);
void yaffs_verify_collected_blk(yaffs_dev_t *dev, yaffs_block_info_t *bi, int n);
void yaffs_verify_blocks(yaffs_dev_t *dev);

void yaffs_verify_oh(yaffs_obj_t *obj, yaffs_obj_header *oh, yaffs_ext_tags *tags, int parentCheck);
void yaffs_verify_file(yaffs_obj_t *obj);
void yaffs_verify_link(yaffs_obj_t *obj);
void yaffs_verify_symlink(yaffs_obj_t *obj);
void yaffs_verify_special(yaffs_obj_t *obj);
void yaffs_verify_obj(yaffs_obj_t *obj);
void yaffs_verify_objects(yaffs_dev_t *dev);
void yaffs_verify_obj_in_dir(yaffs_obj_t *obj);
void yaffs_verify_dir(yaffs_obj_t *directory);
void yaffs_verify_free_chunks(yaffs_dev_t *dev);

int yaffs_verify_file_sane(yaffs_obj_t *obj);

int yaffs_skip_verification(yaffs_dev_t *dev);

#endif

