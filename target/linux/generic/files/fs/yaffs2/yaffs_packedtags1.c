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

#include "yaffs_packedtags1.h"
#include "yportenv.h"

void yaffs_PackTags1(yaffs_PackedTags1 *pt, const yaffs_ext_tags *t)
{
	pt->chunk_id = t->chunk_id;
	pt->serial_number = t->serial_number;
	pt->n_bytes = t->n_bytes;
	pt->obj_id = t->obj_id;
	pt->ecc = 0;
	pt->deleted = (t->is_deleted) ? 0 : 1;
	pt->unusedStuff = 0;
	pt->shouldBeFF = 0xFFFFFFFF;

}

void yaffs_unpack_tags1(yaffs_ext_tags *t, const yaffs_PackedTags1 *pt)
{
	static const __u8 allFF[] =
	    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff };

	if (memcmp(allFF, pt, sizeof(yaffs_PackedTags1))) {
		t->block_bad = 0;
		if (pt->shouldBeFF != 0xFFFFFFFF)
			t->block_bad = 1;
		t->chunk_used = 1;
		t->obj_id = pt->obj_id;
		t->chunk_id = pt->chunk_id;
		t->n_bytes = pt->n_bytes;
		t->ecc_result = YAFFS_ECC_RESULT_NO_ERROR;
		t->is_deleted = (pt->deleted) ? 0 : 1;
		t->serial_number = pt->serial_number;
	} else {
		memset(t, 0, sizeof(yaffs_ext_tags));
	}
}
