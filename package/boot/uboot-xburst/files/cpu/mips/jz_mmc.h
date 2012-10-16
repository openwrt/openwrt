/*
 *  linux/drivers/mmc/jz_mmc.h
 *
 *  Author: Vladimir Shebordaev, Igor Oblakov
 *  Copyright:  MontaVista Software Inc.
 *
 *  $Id: jz_mmc.h,v 1.3 2007-06-15 08:04:20 jlwei Exp $
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
#ifndef __MMC_JZMMC_H__
#define __MMC_JZMMC_H__

#include "mmc_protocol.h"

#define MMC_DEBUG_LEVEL		0		/* Enable Debug: 0 - no debug */

#define MMC_BLOCK_SIZE		512		/* MMC/SD Block Size */

#define ID_TO_RCA(x) ((x)+1)

#define MMC_OCR_ARG		0x00ff8000	/* Argument of OCR */

enum mmc_result_t {
	MMC_NO_RESPONSE        = -1,
	MMC_NO_ERROR           = 0,
	MMC_ERROR_OUT_OF_RANGE,
	MMC_ERROR_ADDRESS,
	MMC_ERROR_BLOCK_LEN,
	MMC_ERROR_ERASE_SEQ,
	MMC_ERROR_ERASE_PARAM,
	MMC_ERROR_WP_VIOLATION,
	MMC_ERROR_CARD_IS_LOCKED,
	MMC_ERROR_LOCK_UNLOCK_FAILED,
	MMC_ERROR_COM_CRC,
	MMC_ERROR_ILLEGAL_COMMAND,
	MMC_ERROR_CARD_ECC_FAILED,
	MMC_ERROR_CC,
	MMC_ERROR_GENERAL,
	MMC_ERROR_UNDERRUN,
	MMC_ERROR_OVERRUN,
	MMC_ERROR_CID_CSD_OVERWRITE,
	MMC_ERROR_STATE_MISMATCH,
	MMC_ERROR_HEADER_MISMATCH,
	MMC_ERROR_TIMEOUT,
	MMC_ERROR_CRC,
	MMC_ERROR_DRIVER_FAILURE,
};

/* the information structure of MMC/SD Card */
typedef struct MMC_INFO
{
	int             id;     /* Card index */
        int             sd;     /* MMC or SD card */
        int             rca;    /* RCA */
        u32             scr;    /* SCR 63:32*/
	int             flags;  /* Ejected, inserted */
	enum card_state state;  /* empty, ident, ready, whatever */

	/* Card specific information */
	struct mmc_cid  cid;
	struct mmc_csd  csd;
	u32             block_num;
	u32             block_len;
	u32             erase_unit;
} mmc_info;

mmc_info mmcinfo;

struct mmc_request {
	int               index;      /* Slot index - used for CS lines */
	int               cmd;        /* Command to send */
	u32               arg;        /* Argument to send */
	enum mmc_rsp_t    rtype;      /* Response type expected */

	/* Data transfer (these may be modified at the low level) */
	u16               nob;        /* Number of blocks to transfer*/
	u16               block_len;  /* Block length */
	u8               *buffer;     /* Data buffer */
	u32               cnt;        /* Data length, for PIO */

	/* Results */
	u8                response[18]; /* Buffer to store response - CRC is optional */
	enum mmc_result_t result;
};

char * mmc_result_to_string(int);
int    mmc_unpack_csd(struct mmc_request *request, struct mmc_csd *csd);
int    mmc_unpack_r1(struct mmc_request *request, struct mmc_response_r1 *r1, enum card_state state);
int    mmc_unpack_r6(struct mmc_request *request, struct mmc_response_r1 *r1, enum card_state state, int *rca);
int    mmc_unpack_scr(struct mmc_request *request, struct mmc_response_r1 *r1, enum card_state state, u32 *scr);
int    mmc_unpack_cid(struct mmc_request *request, struct mmc_cid *cid);
int    mmc_unpack_r3(struct mmc_request *request, struct mmc_response_r3 *r3);

void   mmc_send_cmd(struct mmc_request *request, int cmd, u32 arg, 
		     u16 nob, u16 block_len, enum mmc_rsp_t rtype, u8 *buffer);
u32    mmc_tran_speed(u8 ts);
void   jz_mmc_set_clock(int sd, u32 rate);
void   jz_mmc_hardware_init(void);

static inline void mmc_simple_cmd(struct mmc_request *request, int cmd, u32 arg, enum mmc_rsp_t rtype)
{
	mmc_send_cmd( request, cmd, arg, 0, 0, rtype, 0);
}

int mmc_legacy_init(int verbose);
int mmc_read(ulong src, uchar *dst, int size);
int mmc_write(uchar *src, ulong dst, int size);
int mmc2info(ulong addr);

#endif /* __MMC_JZMMC_H__ */
