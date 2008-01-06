/*
 * Broadcom SiliconBackplane chipcommon serial flash interface
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef _sflash_h_
#define _sflash_h_

#include <typedefs.h>
#include <sbchipc.h>
#include <sbutils.h>

struct sflash {
	uint blocksize;		/* Block size */
	uint numblocks;		/* Number of blocks */
	uint32 type;		/* Type */
	uint size;		/* Total size in bytes */
};

/* Utility functions */
extern int sflash_poll(sb_t *sbh, chipcregs_t *cc, uint offset);
extern int sflash_read(sb_t *sbh, chipcregs_t *cc,
                       uint offset, uint len, uchar *buf);
extern int sflash_write(sb_t *sbh, chipcregs_t *cc,
                        uint offset, uint len, const uchar *buf);
extern int sflash_erase(sb_t *sbh, chipcregs_t *cc, uint offset);
extern int sflash_commit(sb_t *sbh, chipcregs_t *cc,
                         uint offset, uint len, const uchar *buf);
extern struct sflash *sflash_init(sb_t *sbh, chipcregs_t *cc);

#endif /* _sflash_h_ */
