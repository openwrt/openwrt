/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2018 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * This simple implementation of a name-value store assumes a small number of
* values and fits into a small finite buffer.
 *
 * Each attribute is stored as a record:
 *  sizeof(size) bytes   record size.
 *  strnlen+1 bytes name null terminated.
 *  nbytes    value.
 *  ----------
 *  total size  stored in record size
 *
 * This code has not been tested with unicode yet.
 */

#include "yaffs_nameval.h"
#include "yaffs_guts.h"
#include "yportenv.h"
#include "yaffs_endian.h"

static int nval_find(struct yaffs_dev *dev,
		     const char *xb, int xb_size, const YCHAR *name,
		     int *exist_size)
{
	int pos = 0;
	s32 size;

	memcpy(&size, xb, sizeof(size));
	yaffs_do_endian_s32(dev, &size);

	while (size > 0 && (size < xb_size) && (pos + size < xb_size)) {
		if (!strncmp((YCHAR *) (xb + pos + sizeof(size)),
				name, size)) {
			if (exist_size)
				*exist_size = size;
			return pos;
		}
		pos += size;
		if (pos < (int)(xb_size - sizeof(size))) {
			memcpy(&size, xb + pos, sizeof(size));
			yaffs_do_endian_s32(dev, &size);

		} else
			size = 0;
	}
	if (exist_size)
		*exist_size = 0;
	return -ENODATA;
}

static int nval_used(struct yaffs_dev *dev, const char *xb, int xb_size)
{
	int pos = 0;
	s32 size;

	memcpy(&size, xb + pos, sizeof(size));
	yaffs_do_endian_s32(dev, &size);

	while (size > 0 && (size < xb_size) && (pos + size < xb_size)) {
		pos += size;
		if (pos < (int)(xb_size - sizeof(size))) {
			memcpy(&size, xb + pos, sizeof(size));
			yaffs_do_endian_s32(dev, &size);
		} else
			size = 0;
	}
	return pos;
}

int nval_del(struct yaffs_dev *dev, char *xb, int xb_size, const YCHAR *name)
{
	int pos = nval_find(dev, xb, xb_size, name, NULL);
	s32 size;

	if (pos < 0 || pos >= xb_size)
		return -ENODATA;

	/* Find size, shift rest over this record,
	 * then zero out the rest of buffer */
	memcpy(&size, xb + pos, sizeof(size));
	yaffs_do_endian_s32(dev, &size);

	memcpy(xb + pos, xb + pos + size, xb_size - (pos + size));
	memset(xb + (xb_size - size), 0, size);
	return 0;
}

int nval_set(struct yaffs_dev *dev,
	     char *xb, int xb_size, const YCHAR *name, const char *buf,
	     int bsize, int flags)
{
	int pos;
	int namelen = strnlen(name, xb_size);
	int size_exist = 0;
	int space;
	int start;
	s32 reclen;
	s32 reclen_endianised;

	pos = nval_find(dev, xb, xb_size, name, &size_exist);

	if (flags & XATTR_CREATE && pos >= 0)
		return -EEXIST;
	if (flags & XATTR_REPLACE && pos < 0)
		return -ENODATA;

	start = nval_used(dev, xb, xb_size);
	space = xb_size - start + size_exist;

	reclen = (sizeof(reclen) + namelen + 1 + bsize);

	if (reclen > space)
		return -ENOSPC;

	if (pos >= 0) {
		/* Exists, so delete it. */
		nval_del(dev, xb, xb_size, name);
		start = nval_used(dev, xb, xb_size);
	}

	pos = start;

	reclen_endianised = reclen;
	yaffs_do_endian_s32(dev, &reclen_endianised);
	memcpy(xb + pos, &reclen_endianised, sizeof(reclen_endianised));
	pos += sizeof(reclen_endianised);
	strncpy((YCHAR *) (xb + pos), name, reclen);
	pos += (namelen + 1);
	memcpy(xb + pos, buf, bsize);
	return 0;
}

int nval_get(struct yaffs_dev *dev,
	     const char *xb, int xb_size, const YCHAR * name, char *buf,
	     int bsize)
{
	int pos = nval_find(dev, xb, xb_size, name, NULL);
	s32 size;

	if (pos >= 0 && pos < xb_size) {

		memcpy(&size, xb + pos, sizeof(size));
		yaffs_do_endian_s32(dev, &size);
		pos += sizeof(size);	/* advance past record length */
		size -= sizeof(size);

		/* Advance over name string */
		while (xb[pos] && size > 0 && pos < xb_size) {
			pos++;
			size--;
		}
		/*Advance over NUL */
		pos++;
		size--;

		/* If bsize is zero then this is a size query.
		 * Return the size, but don't copy.
		 */
		if (!bsize)
			return size;

		if (size <= bsize) {
			memcpy(buf, xb + pos, size);
			return size;
		}
	}
	if (pos >= 0)
		return -ERANGE;

	return -ENODATA;
}

int nval_list(struct yaffs_dev *dev, const char *xb, int xb_size, char *buf, int bsize)
{
	int pos = 0;
	s32 size;
	int name_len;
	int ncopied = 0;
	int filled = 0;

	memcpy(&size, xb + pos, sizeof(size));
	yaffs_do_endian_s32(dev, &size);

	while (size > (int)(sizeof(size)) &&
		size <= xb_size &&
		(pos + size) < xb_size &&
		!filled) {
		pos += sizeof(size);
		size -= sizeof(size);
		name_len = strnlen((YCHAR *) (xb + pos), size);
		if (ncopied + name_len + 1 < bsize) {
			memcpy(buf, xb + pos, name_len * sizeof(YCHAR));
			buf += name_len;
			*buf = '\0';
			buf++;
			if (sizeof(YCHAR) > 1) {
				*buf = '\0';
				buf++;
			}
			ncopied += (name_len + 1);
		} else {
			filled = 1;
		}
		pos += size;
		if (pos < (int)(xb_size - sizeof(size))) {
			memcpy(&size, xb + pos, sizeof(size));
			yaffs_do_endian_s32(dev, &size);
		}
		else
			size = 0;
	}
	return ncopied;
}

int nval_hasvalues(struct yaffs_dev *dev, const char *xb, int xb_size)
{
	return nval_used(dev, xb, xb_size) > 0;
}
