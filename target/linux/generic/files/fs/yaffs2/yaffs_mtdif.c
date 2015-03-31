/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "yportenv.h"

#include "yaffs_mtdif.h"

#include "linux/mtd/mtd.h"
#include "linux/types.h"
#include "linux/time.h"
#include "linux/mtd/nand.h"
#include "linux/kernel.h"
#include "linux/version.h"
#include "linux/types.h"
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
#include "uapi/linux/major.h"
#endif

#include "yaffs_trace.h"
#include "yaffs_guts.h"
#include "yaffs_linux.h"

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0))
#define MTD_OPS_AUTO_OOB MTD_OOB_AUTO
#endif


#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0))
#define mtd_erase(m, ei) (m)->erase(m, ei)
#define mtd_write_oob(m, addr, pops) (m)->write_oob(m, addr, pops)
#define mtd_read_oob(m, addr, pops) (m)->read_oob(m, addr, pops)
#define mtd_block_isbad(m, offs) (m)->block_isbad(m, offs)
#define mtd_block_markbad(m, offs) (m)->block_markbad(m, offs)
#endif



int nandmtd_erase_block(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	u32 addr =
	    ((loff_t) block_no) * dev->param.total_bytes_per_chunk *
	    dev->param.chunks_per_block;
	struct erase_info ei;
	int retval = 0;

	ei.mtd = mtd;
	ei.addr = addr;
	ei.len = dev->param.total_bytes_per_chunk * dev->param.chunks_per_block;
	ei.time = 1000;
	ei.retries = 2;
	ei.callback = NULL;
	ei.priv = (u_long) dev;

	retval = mtd_erase(mtd, &ei);

	if (retval == 0)
		return YAFFS_OK;

	return YAFFS_FAIL;
}


static 	int yaffs_mtd_write(struct yaffs_dev *dev, int nand_chunk,
				   const u8 *data, int data_len,
				   const u8 *oob, int oob_len)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	loff_t addr;
	struct mtd_oob_ops ops;
	int retval;

	yaffs_trace(YAFFS_TRACE_MTD,
			"yaffs_mtd_write(%p, %d, %p, %d, %p, %d)\n",
			dev, nand_chunk, data, data_len, oob, oob_len);

	if (!data || !data_len) {
		data = NULL;
		data_len = 0;
	}

	if (!oob || !oob_len) {
		oob = NULL;
		oob_len = 0;
	}

	addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;
	memset(&ops, 0, sizeof(ops));
	ops.mode = MTD_OPS_AUTO_OOB;
	ops.len = (data) ? data_len : 0;
	ops.ooblen = oob_len;
	ops.datbuf = (u8 *)data;
	ops.oobbuf = (u8 *)oob;

	retval = mtd_write_oob(mtd, addr, &ops);
	if (retval) {
		yaffs_trace(YAFFS_TRACE_MTD,
			"write_oob failed, chunk %d, mtd error %d",
			nand_chunk, retval);
	}
	return retval ? YAFFS_FAIL : YAFFS_OK;
}

static int yaffs_mtd_read(struct yaffs_dev *dev, int nand_chunk,
				   u8 *data, int data_len,
				   u8 *oob, int oob_len,
				   enum yaffs_ecc_result *ecc_result)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	loff_t addr;
	struct mtd_oob_ops ops;
	int retval;

	addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;
	memset(&ops, 0, sizeof(ops));
	ops.mode = MTD_OPS_AUTO_OOB;
	ops.len = (data) ? data_len : 0;
	ops.ooblen = oob_len;
	ops.datbuf = data;
	ops.oobbuf = oob;

#if (MTD_VERSION_CODE < MTD_VERSION(2, 6, 20))
	/* In MTD 2.6.18 to 2.6.19 nand_base.c:nand_do_read_oob() has a bug;
	 * help it out with ops.len = ops.ooblen when ops.datbuf == NULL.
	 */
	ops.len = (ops.datbuf) ? ops.len : ops.ooblen;
#endif
	/* Read page and oob using MTD.
	 * Check status and determine ECC result.
	 */
	retval = mtd_read_oob(mtd, addr, &ops);
	if (retval)
		yaffs_trace(YAFFS_TRACE_MTD,
			"read_oob failed, chunk %d, mtd error %d",
			nand_chunk, retval);

	switch (retval) {
	case 0:
		/* no error */
		if(ecc_result)
			*ecc_result = YAFFS_ECC_RESULT_NO_ERROR;
		break;

	case -EUCLEAN:
		/* MTD's ECC fixed the data */
		if(ecc_result)
			*ecc_result = YAFFS_ECC_RESULT_FIXED;
		dev->n_ecc_fixed++;
		break;

	case -EBADMSG:
	default:
		/* MTD's ECC could not fix the data */
		dev->n_ecc_unfixed++;
		if(ecc_result)
			*ecc_result = YAFFS_ECC_RESULT_UNFIXED;
		return YAFFS_FAIL;
	}

	return YAFFS_OK;
}

static 	int yaffs_mtd_erase(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);

	loff_t addr;
	struct erase_info ei;
	int retval = 0;
	u32 block_size;

	block_size = dev->param.total_bytes_per_chunk *
		     dev->param.chunks_per_block;
	addr = ((loff_t) block_no) * block_size;

	ei.mtd = mtd;
	ei.addr = addr;
	ei.len = block_size;
	ei.time = 1000;
	ei.retries = 2;
	ei.callback = NULL;
	ei.priv = (u_long) dev;

	retval = mtd_erase(mtd, &ei);

	if (retval == 0)
		return YAFFS_OK;

	return YAFFS_FAIL;
}

static int yaffs_mtd_mark_bad(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	int blocksize = dev->param.chunks_per_block * dev->param.total_bytes_per_chunk;
	int retval;

	yaffs_trace(YAFFS_TRACE_BAD_BLOCKS, "marking block %d bad", block_no);

	retval = mtd_block_markbad(mtd, (loff_t) blocksize * block_no);
	return (retval) ? YAFFS_FAIL : YAFFS_OK;
}

static int yaffs_mtd_check_bad(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	int blocksize = dev->param.chunks_per_block * dev->param.total_bytes_per_chunk;
	int retval;

	yaffs_trace(YAFFS_TRACE_MTD, "checking block %d bad", block_no);

	retval = mtd_block_isbad(mtd, (loff_t) blocksize * block_no);
	return (retval) ? YAFFS_FAIL : YAFFS_OK;
}

static int yaffs_mtd_initialise(struct yaffs_dev *dev)
{
	return YAFFS_OK;
}

static int yaffs_mtd_deinitialise(struct yaffs_dev *dev)
{
	return YAFFS_OK;
}


void yaffs_mtd_drv_install(struct yaffs_dev *dev)
{
	struct yaffs_driver *drv = &dev->drv;

	drv->drv_write_chunk_fn = yaffs_mtd_write;
	drv->drv_read_chunk_fn = yaffs_mtd_read;
	drv->drv_erase_fn = yaffs_mtd_erase;
	drv->drv_mark_bad_fn = yaffs_mtd_mark_bad;
	drv->drv_check_bad_fn = yaffs_mtd_check_bad;
	drv->drv_initialise_fn = yaffs_mtd_initialise;
	drv->drv_deinitialise_fn = yaffs_mtd_deinitialise;
}


struct mtd_info * yaffs_get_mtd_device(dev_t sdev)
{
	struct mtd_info *mtd;

	mtd = yaffs_get_mtd_device(sdev);

	/* Check it's an mtd device..... */
	if (MAJOR(sdev) != MTD_BLOCK_MAJOR)
		return NULL;	/* This isn't an mtd device */

	/* Check it's NAND */
	if (mtd->type != MTD_NANDFLASH) {
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"yaffs: MTD device is not NAND it's type %d",
			mtd->type);
		return NULL;
	}

	yaffs_trace(YAFFS_TRACE_OS, " %s %d", WRITE_SIZE_STR, WRITE_SIZE(mtd));
	yaffs_trace(YAFFS_TRACE_OS, " oobsize %d", mtd->oobsize);
	yaffs_trace(YAFFS_TRACE_OS, " erasesize %d", mtd->erasesize);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
	yaffs_trace(YAFFS_TRACE_OS, " size %u", mtd->size);
#else
	yaffs_trace(YAFFS_TRACE_OS, " size %lld", mtd->size);
#endif

	return mtd;
}

int yaffs_verify_mtd(struct mtd_info *mtd, int yaffs_version, int inband_tags)
{
	if (yaffs_version == 2) {
		if ((WRITE_SIZE(mtd) < YAFFS_MIN_YAFFS2_CHUNK_SIZE ||
		     mtd->oobsize < YAFFS_MIN_YAFFS2_SPARE_SIZE) &&
		    !inband_tags) {
			yaffs_trace(YAFFS_TRACE_ALWAYS,
				"MTD device does not have the right page sizes"
			);
			return -1;
		}
	} else {
		if (WRITE_SIZE(mtd) < YAFFS_BYTES_PER_CHUNK ||
		    mtd->oobsize != YAFFS_BYTES_PER_SPARE) {
			yaffs_trace(YAFFS_TRACE_ALWAYS,
				"MTD device does not support have the right page sizes"
			);
			return -1;
		}
	}

	return 0;
}


void yaffs_put_mtd_device(struct mtd_info *mtd)
{
	if(mtd)
		put_mtd_device(mtd);
}
