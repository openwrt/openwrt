/*
 * (C) Copyright 2003
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <part.h>

#if defined CONFIG_JZ4740
#include <asm-mips/jz4740.h>

#include "jz_mmc.h"

#define CFG_MMC_BASE		0x80600000
static int sd2_0 = 0;

/*
 * GPIO definition
 */
#if defined(CONFIG_SAKC)

#define __msc_init_io()				\
do {						\
	__gpio_as_input(GPIO_SD_CD_N);		\
} while (0)

#else
#define __msc_init_io()				\
do {						\
	__gpio_as_output(GPIO_SD_VCC_EN_N);	\
	__gpio_as_input(GPIO_SD_CD_N);		\
} while (0)

#define __msc_enable_power()			\
do {						\
	__gpio_clear_pin(GPIO_SD_VCC_EN_N);	\
} while (0)

#define __msc_disable_power()			\
do {						\
	__gpio_set_pin(GPIO_SD_VCC_EN_N);	\
} while (0)
	
#endif /* CONFIG_SAKE */

#define __msc_card_detected()			\
({						\
	int detected = 1;			\
	__gpio_as_input(GPIO_SD_CD_N);		\
	__gpio_disable_pull(GPIO_SD_CD_N);	\
	if (!__gpio_get_pin(GPIO_SD_CD_N))	\
		detected = 0;			\
	detected;				\
})

/*
 * Local functions
 */

#ifdef CONFIG_MMC
extern int
fat_register_device(block_dev_desc_t *dev_desc, int part_no);

static block_dev_desc_t mmc_dev;

block_dev_desc_t * mmc_get_dev(int dev)
{
	return ((block_dev_desc_t *)&mmc_dev);
}

/*
 * FIXME needs to read cid and csd info to determine block size
 * and other parameters
 */
static uchar mmc_buf[MMC_BLOCK_SIZE];
static int mmc_ready = 0;
static mmc_csd_t mmc_csd;
static int use_4bit;                    /* Use 4-bit data bus */
/*
 *  MMC Events
 */
#define MMC_EVENT_NONE	        0x00	/* No events */
#define MMC_EVENT_RX_DATA_DONE	0x01	/* Rx data done */
#define MMC_EVENT_TX_DATA_DONE	0x02	/* Tx data done */
#define MMC_EVENT_PROG_DONE	0x04	/* Programming is done */


#define MMC_IRQ_MASK()				\
do {						\
      	REG_MSC_IMASK = 0xffff;			\
      	REG_MSC_IREG = 0xffff;			\
} while (0)

/* Stop the MMC clock and wait while it happens */
static inline int jz_mmc_stop_clock(void)
{
	int timeout = 1000;

	REG_MSC_STRPCL = MSC_STRPCL_CLOCK_CONTROL_STOP;

	while (timeout && (REG_MSC_STAT & MSC_STAT_CLK_EN)) {
		timeout--;
		if (timeout == 0) {
			return MMC_ERROR_TIMEOUT;
		}
		udelay(1);
	}
        return MMC_NO_ERROR;
}

/* Start the MMC clock and operation */
static inline int jz_mmc_start_clock(void)
{
	REG_MSC_STRPCL = MSC_STRPCL_CLOCK_CONTROL_START | MSC_STRPCL_START_OP;
	return MMC_NO_ERROR;
}

static inline u32 jz_mmc_calc_clkrt(int is_sd, u32 rate)
{
	u32 clkrt;
	u32 clk_src = is_sd ? 24000000: 16000000;

	clkrt = 0;
  	while (rate < clk_src)
    	{
      		clkrt ++;
      		clk_src >>= 1;
    	}
	return clkrt;
}

/* Set the MMC clock frequency */
void jz_mmc_set_clock(int sd, u32 rate)
{
	jz_mmc_stop_clock();

	/* Select clock source of MSC */
	__cpm_select_msc_clk(sd);

	/* Set clock dividor of MSC */
	REG_MSC_CLKRT = jz_mmc_calc_clkrt(sd, rate);
}

static int jz_mmc_check_status(struct mmc_request *request)
{
	u32 status = REG_MSC_STAT;

	/* Checking for response or data timeout */
	if (status & (MSC_STAT_TIME_OUT_RES | MSC_STAT_TIME_OUT_READ)) {
		printf("MMC/SD timeout, MMC_STAT 0x%x CMD %d\n", status, request->cmd);
		return MMC_ERROR_TIMEOUT;
	}

	/* Checking for CRC error */
	if (status & (MSC_STAT_CRC_READ_ERROR | MSC_STAT_CRC_WRITE_ERROR | MSC_STAT_CRC_RES_ERR)) {
		printf("MMC/CD CRC error, MMC_STAT 0x%x\n", status);
		return MMC_ERROR_CRC;
	}

	return MMC_NO_ERROR;
}

/* Obtain response to the command and store it to response buffer */
static void jz_mmc_get_response(struct mmc_request *request)
{
	int i;
	u8 *buf;
	u32 data;

	DEBUG(3, "fetch response for request %d, cmd %d\n", request->rtype, request->cmd);

	buf = request->response;
	request->result = MMC_NO_ERROR;

	switch (request->rtype) {
	case RESPONSE_R1: case RESPONSE_R1B: case RESPONSE_R6:
	case RESPONSE_R3: case RESPONSE_R4: case RESPONSE_R5:
	{
		data = REG_MSC_RES;
		buf[0] = (data >> 8) & 0xff;
		buf[1] = data & 0xff;
		data = REG_MSC_RES;
		buf[2] = (data >> 8) & 0xff;
		buf[3] = data & 0xff;
		data = REG_MSC_RES;
		buf[4] = data & 0xff;

		DEBUG(3, "request %d, response [%02x %02x %02x %02x %02x]\n",
		      request->rtype, buf[0], buf[1], buf[2], buf[3], buf[4]);
		break;
	}
	case RESPONSE_R2_CID: case RESPONSE_R2_CSD:
	{
		for (i = 0; i < 16; i += 2) {
			data = REG_MSC_RES;
			buf[i] = (data >> 8) & 0xff;
			buf[i+1] = data & 0xff;
		}
		DEBUG(3, "request %d, response [", request->rtype);
#if CONFIG_MMC_DEBUG_VERBOSE > 2
		if (g_mmc_debug >= 3) {
			int n;
			for (n = 0; n < 17; n++)
				printk("%02x ", buf[n]);
			printk("]\n");
		}
#endif
		break;
	}
	case RESPONSE_NONE:
		DEBUG(3, "No response\n");
		break;

	default:
		DEBUG(3, "unhandled response type for request %d\n", request->rtype);
		break;
	}
}

static int jz_mmc_receive_data(struct mmc_request *req)
{
	u32  stat, timeout, data, cnt;
	u8 *buf = req->buffer;
	u32 wblocklen = (u32)(req->block_len + 3) >> 2; /* length in word */

	timeout = 0x3ffffff;

	while (timeout) {
		timeout--;
		stat = REG_MSC_STAT;

		if (stat & MSC_STAT_TIME_OUT_READ)
			return MMC_ERROR_TIMEOUT;
		else if (stat & MSC_STAT_CRC_READ_ERROR)
			return MMC_ERROR_CRC;
		else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)
			 || (stat & MSC_STAT_DATA_FIFO_AFULL)) {
			/* Ready to read data */
			break;
		}
		udelay(1);
	}
	if (!timeout)
		return MMC_ERROR_TIMEOUT;

	/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
	cnt = wblocklen;
	while (cnt) {
		data = REG_MSC_RXFIFO;
		{
			*buf++ = (u8)(data >> 0);
			*buf++ = (u8)(data >> 8);
			*buf++ = (u8)(data >> 16);
			*buf++ = (u8)(data >> 24);
		}
		cnt --;
		while (cnt && (REG_MSC_STAT & MSC_STAT_DATA_FIFO_EMPTY))
			;
	}
	return MMC_NO_ERROR;
}

static int jz_mmc_transmit_data(struct mmc_request *req)
{
#if 0
	u32 nob = req->nob;
	u32 wblocklen = (u32)(req->block_len + 3) >> 2; /* length in word */
	u8 *buf = req->buffer;
	u32 *wbuf = (u32 *)buf;
	u32 waligned = (((u32)buf & 0x3) == 0); /* word aligned ? */
	u32 stat, timeout, data, cnt;

	for (nob; nob >= 1; nob--) {
		timeout = 0x3FFFFFF;

		while (timeout) {
			timeout--;
			stat = REG_MSC_STAT;

			if (stat & (MSC_STAT_CRC_WRITE_ERROR | MSC_STAT_CRC_WRITE_ERROR_NOSTS))
				return MMC_ERROR_CRC;
			else if (!(stat & MSC_STAT_DATA_FIFO_FULL)) {
				/* Ready to write data */
				break;
			}

			udelay(1);
		}

		if (!timeout)
			return MMC_ERROR_TIMEOUT;

		/* Write data to TXFIFO */
		cnt = wblocklen;
		while (cnt) {
			while (REG_MSC_STAT & MSC_STAT_DATA_FIFO_FULL)
				;

			if (waligned) {
				REG_MSC_TXFIFO = *wbuf++;
			}
			else {
				data = *buf++ | (*buf++ << 8) | (*buf++ << 16) | (*buf++ << 24);
				REG_MSC_TXFIFO = data;
			}

			cnt--;
		}
	}
#endif
	return MMC_NO_ERROR;
}


/*
 * Name:	  int jz_mmc_exec_cmd()
 * Function:      send command to the card, and get a response
 * Input:	  struct mmc_request *req	: MMC/SD request
 * Output:	  0:  right		>0:  error code
 */
int jz_mmc_exec_cmd(struct mmc_request *request)
{
	u32 cmdat = 0, events = 0;
	int retval, timeout = 0x3fffff;

	/* Indicate we have no result yet */
	request->result = MMC_NO_RESPONSE;
	if (request->cmd == MMC_CIM_RESET) {
		/* On reset, 1-bit bus width */
		use_4bit = 0;

		/* Reset MMC/SD controller */
		__msc_reset();

		/* On reset, drop MMC clock down */
		jz_mmc_set_clock(0, MMC_CLOCK_SLOW);

		/* On reset, stop MMC clock */
		jz_mmc_stop_clock();
	}
	if (request->cmd == MMC_SEND_OP_COND) {
		DEBUG(3, "Have an MMC card\n");
		/* always use 1bit for MMC */
		use_4bit = 0;
	}
	if (request->cmd == SET_BUS_WIDTH) {
		if (request->arg == 0x2) {
			printf("Use 4-bit bus width\n");
			use_4bit = 1;
		}
		else {
			printf("Use 1-bit bus width\n");			
			use_4bit = 0;
		}
	}

	/* stop clock */
	jz_mmc_stop_clock();

	/* mask all interrupts */
	REG_MSC_IMASK = 0xffff;

	/* clear status */
	REG_MSC_IREG = 0xffff;

	/* use 4-bit bus width when possible */
	if (use_4bit)
		cmdat |= MSC_CMDAT_BUS_WIDTH_4BIT;

        /* Set command type and events */
	switch (request->cmd) {
	/* MMC core extra command */
	case MMC_CIM_RESET:
		cmdat |= MSC_CMDAT_INIT; /* Initialization sequence sent prior to command */
		break;

	/* bc - broadcast - no response */
	case MMC_GO_IDLE_STATE:
	case MMC_SET_DSR:
		break;

	/* bcr - broadcast with response */
	case MMC_SEND_OP_COND:
	case MMC_ALL_SEND_CID:
	case MMC_GO_IRQ_STATE:
		break;

	/* adtc - addressed with data transfer */
	case MMC_READ_DAT_UNTIL_STOP:
	case MMC_READ_SINGLE_BLOCK:
	case MMC_READ_MULTIPLE_BLOCK:
	case SEND_SCR:
		cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_READ;
		events = MMC_EVENT_RX_DATA_DONE;
		break;

	case MMC_WRITE_DAT_UNTIL_STOP:
	case MMC_WRITE_BLOCK:
	case MMC_WRITE_MULTIPLE_BLOCK:
	case MMC_PROGRAM_CID:
	case MMC_PROGRAM_CSD:
	case MMC_SEND_WRITE_PROT:
	case MMC_GEN_CMD:
	case MMC_LOCK_UNLOCK:
		cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_WRITE;
		events = MMC_EVENT_TX_DATA_DONE | MMC_EVENT_PROG_DONE;

		break;

	case MMC_STOP_TRANSMISSION:
		events = MMC_EVENT_PROG_DONE;
		break;

	/* ac - no data transfer */
	default:
		break;
	}

	/* Set response type */
	switch (request->rtype) {
	case RESPONSE_NONE:
		break;

	case RESPONSE_R1B:
		cmdat |= MSC_CMDAT_BUSY;
		/*FALLTHRU*/
	case RESPONSE_R1:
		cmdat |= MSC_CMDAT_RESPONSE_R1;
		break;
	case RESPONSE_R2_CID:
	case RESPONSE_R2_CSD:
		cmdat |= MSC_CMDAT_RESPONSE_R2;
		break;
	case RESPONSE_R3:
		cmdat |= MSC_CMDAT_RESPONSE_R3;
		break;
	case RESPONSE_R4:
		cmdat |= MSC_CMDAT_RESPONSE_R4;
		break;
	case RESPONSE_R5:
		cmdat |= MSC_CMDAT_RESPONSE_R5;
		break;
	case RESPONSE_R6:
		cmdat |= MSC_CMDAT_RESPONSE_R6;
		break;
	default:
		break;
	}

	/* Set command index */
	if (request->cmd == MMC_CIM_RESET) {
		REG_MSC_CMD = MMC_GO_IDLE_STATE;
	} else {
		REG_MSC_CMD = request->cmd;
	}

        /* Set argument */
	REG_MSC_ARG = request->arg;

	/* Set block length and nob */
	if (request->cmd == SEND_SCR) { /* get SCR from DataFIFO */
		REG_MSC_BLKLEN = 8;
		REG_MSC_NOB = 1;
	} else {
		REG_MSC_BLKLEN = request->block_len;
		REG_MSC_NOB = request->nob;
	}

	/* Set command */
	REG_MSC_CMDAT = cmdat;

	DEBUG(1, "Send cmd %d cmdat: %x arg: %x resp %d\n", request->cmd,
	      cmdat, request->arg, request->rtype);

        /* Start MMC/SD clock and send command to card */
	jz_mmc_start_clock();

	/* Wait for command completion */
	while (timeout-- && !(REG_MSC_STAT & MSC_STAT_END_CMD_RES))
		;

	if (timeout == 0)
		return MMC_ERROR_TIMEOUT;

	REG_MSC_IREG = MSC_IREG_END_CMD_RES; /* clear flag */

	/* Check for status */
	retval = jz_mmc_check_status(request);
	if (retval) {
		return retval;
	}

	/* Complete command with no response */
	if (request->rtype == RESPONSE_NONE) {
		return MMC_NO_ERROR;
	}

	/* Get response */
	jz_mmc_get_response(request);

	/* Start data operation */
	if (events & (MMC_EVENT_RX_DATA_DONE | MMC_EVENT_TX_DATA_DONE)) {
		if (events & MMC_EVENT_RX_DATA_DONE) {
			if (request->cmd == SEND_SCR) {
				/* SD card returns SCR register as data.
				   MMC core expect it in the response buffer,
				   after normal response. */
				request->buffer = (u8 *)((u32)request->response + 5);
			}
			jz_mmc_receive_data(request);
		}

		if (events & MMC_EVENT_TX_DATA_DONE) {
			jz_mmc_transmit_data(request);
		}

		/* Wait for Data Done */
		while (!(REG_MSC_IREG & MSC_IREG_DATA_TRAN_DONE))
			;
		REG_MSC_IREG = MSC_IREG_DATA_TRAN_DONE; /* clear status */
	}

	/* Wait for Prog Done event */
	if (events & MMC_EVENT_PROG_DONE) {
		while (!(REG_MSC_IREG & MSC_IREG_PRG_DONE))
			;
		REG_MSC_IREG = MSC_IREG_PRG_DONE; /* clear status */
	}

	/* Command completed */

	return MMC_NO_ERROR;			 /* return successfully */
}

int mmc_block_read(u8 *dst, ulong src, ulong len)
{

	struct mmc_request request;
	struct mmc_response_r1 r1;
	int retval;

	if (len == 0) {
		return 0;
	}
	mmc_simple_cmd(&request, MMC_SEND_STATUS, mmcinfo.rca, RESPONSE_R1);
	retval = mmc_unpack_r1(&request, &r1, 0);
	if (retval && (retval != MMC_ERROR_STATE_MISMATCH)) {
		return retval;
	}

	mmc_simple_cmd(&request, MMC_SET_BLOCKLEN, len, RESPONSE_R1);
	if ((retval = mmc_unpack_r1(&request, &r1, 0))) {
		return retval;
	}

	if (sd2_0)
		src /= len;

	mmc_send_cmd(&request, MMC_READ_SINGLE_BLOCK, src, 1,len, RESPONSE_R1, dst);
	if ((retval = mmc_unpack_r1(&request, &r1, 0))) {
		return retval;
	}
	return retval;
}

int mmc_block_write(ulong dst, uchar *src, int len)
{
	return 0;
}

int mmc_read(ulong src, uchar *dst, int size)
{
	ulong end, part_start, part_end, part_len, aligned_start, aligned_end;
	ulong mmc_block_size, mmc_block_address;

	if (size == 0) {
		return 0;
	}

	if (!mmc_ready) {
		printf("MMC card is not ready\n");
		return -1;
	}

	mmc_block_size = MMC_BLOCK_SIZE;
	mmc_block_address = ~(mmc_block_size - 1);

	src -= CFG_MMC_BASE;
	end = src + size;
	part_start = ~mmc_block_address & src;
	part_end = ~mmc_block_address & end;
	aligned_start = mmc_block_address & src;
	aligned_end = mmc_block_address & end;
	/* all block aligned accesses */
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	if (part_start) {
		part_len = mmc_block_size - part_start;
		debug("ps src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_read(mmc_buf, aligned_start, mmc_block_size)) < 0) {
			return -1;
		}
		memcpy(dst, mmc_buf+part_start, part_len);
		dst += part_len;
		src += part_len;
	}
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	for (; src < aligned_end; src += mmc_block_size, dst += mmc_block_size) {
		debug("al src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);

		if ((mmc_block_read((uchar *)(dst), src, mmc_block_size)) < 0) {
			return -1;
		}
	}
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);

	if (part_end && src < end) {
		if ((mmc_block_read(mmc_buf, aligned_end, mmc_block_size)) < 0) {
			return -1;
		}
		memcpy(dst, mmc_buf, part_end);
	}
	return 0;

}

int mmc_write(uchar *src, ulong dst, int size)
{
	ulong end, part_start, part_end, part_len, aligned_start, aligned_end;
	ulong mmc_block_size, mmc_block_address;

	if (size == 0) {
		return 0;
	}

	if (!mmc_ready) {
		printf("MMC card is not ready\n");
		return -1;
	}

	mmc_block_size = MMC_BLOCK_SIZE;
	mmc_block_address = ~(mmc_block_size - 1);

	dst -= CFG_MMC_BASE;
	end = dst + size;
	part_start = ~mmc_block_address & dst;
	part_end = ~mmc_block_address & end;
	aligned_start = mmc_block_address & dst;
	aligned_end = mmc_block_address & end;

	/* all block aligned accesses */
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	if (part_start) {
		part_len = mmc_block_size - part_start;
		debug("ps src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		(ulong)src, dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_read(mmc_buf, aligned_start, mmc_block_size)) < 0) {
			return -1;
		}
		memcpy(mmc_buf+part_start, src, part_len);
		if ((mmc_block_write(aligned_start, mmc_buf, mmc_block_size)) < 0) {
			return -1;
		}
		dst += part_len;
		src += part_len;
	}
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	for (; dst < aligned_end; src += mmc_block_size, dst += mmc_block_size) {
		debug("al src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_write(dst, (uchar *)src, mmc_block_size)) < 0) {
			return -1;
		}
	}
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	if (part_end && dst < end) {
		debug("pe src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_read(mmc_buf, aligned_end, mmc_block_size)) < 0) {
			return -1;
		}
		memcpy(mmc_buf, src, part_end);
		if ((mmc_block_write(aligned_end, mmc_buf, mmc_block_size)) < 0) {
			return -1;
		}
	}
	return 0;
}

ulong mmc_bread(int dev_num, ulong blknr, ulong blkcnt, ulong *dst)
{
	ulong src;
	int mmc_block_size = MMC_BLOCK_SIZE;

	src = blknr * mmc_block_size + CFG_MMC_BASE;
	mmc_read(src, (uchar *)dst, blkcnt*mmc_block_size);
	return blkcnt;
}

int mmc_select_card(void)
{
	struct mmc_request request;
	struct mmc_response_r1 r1;
	int retval;

	mmc_simple_cmd(&request, MMC_SELECT_CARD, mmcinfo.rca, RESPONSE_R1B);
	retval = mmc_unpack_r1(&request, &r1, 0);
	if (retval) {
		return retval;
	}

	if (mmcinfo.sd) {
		mmc_simple_cmd(&request, MMC_APP_CMD,  mmcinfo.rca, RESPONSE_R1);
		retval = mmc_unpack_r1(&request,&r1,0);
		if (retval) {
			return retval;
		}
#if defined(MMC_BUS_WIDTH_1BIT)		
		mmc_simple_cmd(&request, SET_BUS_WIDTH, 1, RESPONSE_R1);
#else
		mmc_simple_cmd(&request, SET_BUS_WIDTH, 2, RESPONSE_R1);
#endif
                retval = mmc_unpack_r1(&request,&r1,0);
                if (retval) {
			return retval;
		}
	}
	return 0;
}

/*
 * Configure card
 */
static void mmc_configure_card(void)
{
	u32 rate;

	/* Get card info */
	if (sd2_0)
		mmcinfo.block_num = (mmcinfo.csd.c_size + 1) << 10;
	else
		mmcinfo.block_num = (mmcinfo.csd.c_size + 1) * (1 << (mmcinfo.csd.c_size_mult + 2));

	mmcinfo.block_len = 1 << mmcinfo.csd.read_bl_len;

	/* Fix the clock rate */
	rate = mmc_tran_speed(mmcinfo.csd.tran_speed);
	if (rate < MMC_CLOCK_SLOW)
		rate = MMC_CLOCK_SLOW;
	if ((mmcinfo.sd == 0) && (rate > MMC_CLOCK_FAST))
		rate = MMC_CLOCK_FAST;
        if ((mmcinfo.sd) && (rate > SD_CLOCK_FAST))
		rate = SD_CLOCK_FAST;

	DEBUG(2,"mmc_configure_card: block_len=%d block_num=%d rate=%d\n", mmcinfo.block_len, mmcinfo.block_num, rate);

	jz_mmc_set_clock(mmcinfo.sd, rate);
}

/*
 * State machine routines to initialize card(s)
 */

/*
  CIM_SINGLE_CARD_ACQ  (frequency at 400 kHz)
  --- Must enter from GO_IDLE_STATE ---
  1. SD_SEND_OP_COND (SD Card) [CMD55] + [CMD41]
  2. SEND_OP_COND (Full Range) [CMD1]   {optional}
  3. SEND_OP_COND (Set Range ) [CMD1]
     If busy, delay and repeat step 2
  4. ALL_SEND_CID              [CMD2]
     If timeout, set an error (no cards found)
  5. SET_RELATIVE_ADDR         [CMD3]
  6. SEND_CSD                  [CMD9]
  7. SET_DSR                   [CMD4]    Only call this if (csd.dsr_imp).
  8. Set clock frequency (check available in csd.tran_speed)
 */

#define MMC_INIT_DOING   0
#define MMC_INIT_PASSED  1
#define MMC_INIT_FAILED  2

static int mmc_init_card_state(struct mmc_request *request)
{
	struct mmc_response_r1 r1;
	struct mmc_response_r3 r3;
	int retval;
	int ocr = 0x40300000;
	int limit_41 = 0;

	DEBUG(2,"mmc_init_card_state\n");

	switch (request->cmd) {
	case MMC_GO_IDLE_STATE: /* No response to parse */
		if (mmcinfo.sd)
			mmc_simple_cmd(request, 8, 0x1aa, RESPONSE_R1);
		else
			mmc_simple_cmd(request, MMC_SEND_OP_COND, MMC_OCR_ARG, RESPONSE_R3);
		break;

	case 8:
        	retval = mmc_unpack_r1(request,&r1,mmcinfo.state);
		mmc_simple_cmd(request, MMC_APP_CMD,  0, RESPONSE_R1);
		break;

        case MMC_APP_CMD:
        	retval = mmc_unpack_r1(request,&r1,mmcinfo.state);
		if (retval & (limit_41 < 100)) {
			DEBUG(0, "mmc_init_card_state: unable to MMC_APP_CMD error=%d (%s)\n", 
			      retval, mmc_result_to_string(retval));
			limit_41++;
			mmc_simple_cmd(request, SD_SEND_OP_COND, ocr, RESPONSE_R3);
		} else if (limit_41 < 100) {
			limit_41++;
			mmc_simple_cmd(request, SD_SEND_OP_COND, ocr, RESPONSE_R3);
		} else{
			/* reset the card to idle*/
			mmc_simple_cmd(request, MMC_GO_IDLE_STATE, 0, RESPONSE_NONE);
			mmcinfo.sd = 0;
		}
		break;

        case SD_SEND_OP_COND:
                retval = mmc_unpack_r3(request, &r3);
                if (retval) {
                  /* Try MMC card */
                    mmc_simple_cmd(request, MMC_SEND_OP_COND, MMC_OCR_ARG, RESPONSE_R3);
                    break;
		}

                DEBUG(2,"mmc_init_card_state: read ocr value = 0x%08x\n", r3.ocr);

		if(!(r3.ocr & MMC_CARD_BUSY || ocr == 0)){
			udelay(10000);
			mmc_simple_cmd(request, MMC_APP_CMD, 0, RESPONSE_R1);
		}
		else {
		  /* Set the data bus width to 4 bits */
                  mmcinfo.sd = 1; /* SD Card ready */
                  mmcinfo.state = CARD_STATE_READY;
		  mmc_simple_cmd(request, MMC_ALL_SEND_CID, 0, RESPONSE_R2_CID);
		}
		break;

	case MMC_SEND_OP_COND:
		retval = mmc_unpack_r3(request, &r3);
		if (retval) {
			DEBUG(0,"mmc_init_card_state: failed SEND_OP_COND error=%d (%s)\n", 
			      retval, mmc_result_to_string(retval));
			return MMC_INIT_FAILED;
		}

		DEBUG(2,"mmc_init_card_state: read ocr value = 0x%08x\n", r3.ocr);
		if (!(r3.ocr & MMC_CARD_BUSY)) {
	                mmc_simple_cmd(request, MMC_SEND_OP_COND, MMC_OCR_ARG, RESPONSE_R3);
		}
		else {
		        mmcinfo.sd = 0; /* MMC Card ready */
			mmcinfo.state = CARD_STATE_READY;
			mmc_simple_cmd(request, MMC_ALL_SEND_CID, 0, RESPONSE_R2_CID);
		}
		break;

	case MMC_ALL_SEND_CID: 
		retval = mmc_unpack_cid( request, &mmcinfo.cid );
		mmc_dev.if_type = IF_TYPE_MMC;
		mmc_dev.part_type = PART_TYPE_DOS;
		mmc_dev.dev = 0;
		mmc_dev.lun = 0;
		mmc_dev.type = 0;
		/* FIXME fill in the correct size (is set to 32MByte) */
		mmc_dev.blksz = 512;
		mmc_dev.lba = 0x10000;
		mmc_dev.removable = 0;

		/*FIXME:ignore CRC error for CMD2/CMD9/CMD10 */
		if ( retval && (retval != MMC_ERROR_CRC)) {
			DEBUG(0,"mmc_init_card_state: unable to ALL_SEND_CID error=%d (%s)\n", 
			      retval, mmc_result_to_string(retval));
			return MMC_INIT_FAILED;
		}
		mmcinfo.state = CARD_STATE_IDENT;
		if(mmcinfo.sd)
			mmc_simple_cmd(request, MMC_SET_RELATIVE_ADDR, 0, RESPONSE_R6);
                else
			mmc_simple_cmd(request, MMC_SET_RELATIVE_ADDR, ID_TO_RCA(mmcinfo.id) << 16, RESPONSE_R1);
		break;

        case MMC_SET_RELATIVE_ADDR:
	        if (mmcinfo.sd)	{
			retval = mmc_unpack_r6(request, &r1, mmcinfo.state, &mmcinfo.rca);
			mmcinfo.rca = mmcinfo.rca << 16; 
			DEBUG(2, "mmc_init_card_state: Get RCA from SD: 0x%04x Status: %x\n", mmcinfo.rca, r1.status);
                } else {
			retval = mmc_unpack_r1(request,&r1,mmcinfo.state);
			mmcinfo.rca = ID_TO_RCA(mmcinfo.id) << 16;
	        }
		if (retval) {
			DEBUG(0, "mmc_init_card_state: unable to SET_RELATIVE_ADDR error=%d (%s)\n", 
			      retval, mmc_result_to_string(retval));
			return MMC_INIT_FAILED;
		}

		mmcinfo.state = CARD_STATE_STBY;
                mmc_simple_cmd(request, MMC_SEND_CSD, mmcinfo.rca, RESPONSE_R2_CSD);

		break;

	case MMC_SEND_CSD:
		retval = mmc_unpack_csd(request, &mmcinfo.csd);
			mmc_csd_t *csd = (mmc_csd_t *)retval;
			memcpy(&mmc_csd, csd, sizeof(csd));
			mmc_ready = 1;

			printf("MMC card is ready\n");
			/* FIXME add verbose printout for csd */

		/*FIXME:ignore CRC error for CMD2/CMD9/CMD10 */
	        if (retval && (retval != MMC_ERROR_CRC)) {
			DEBUG(0, "mmc_init_card_state: unable to SEND_CSD error=%d (%s)\n", 
			      retval, mmc_result_to_string(retval));
			return MMC_INIT_FAILED;
		}
		if (mmcinfo.csd.dsr_imp) {
			DEBUG(0, "mmc_init_card_state: driver doesn't support setting DSR\n");
		}
		mmc_configure_card();
		return MMC_INIT_PASSED;

	default:
		DEBUG(0, "mmc_init_card_state: error!  Illegal last cmd %d\n", request->cmd);
		return MMC_INIT_FAILED;
	}

	return MMC_INIT_DOING;
}

int mmc_init_card(void)
{
	struct mmc_request request;
	int retval;

	mmc_simple_cmd(&request, MMC_CIM_RESET, 0, RESPONSE_NONE); /* reset card */
	mmc_simple_cmd(&request, MMC_GO_IDLE_STATE, 0, RESPONSE_NONE);
	mmcinfo.sd = 1;  /* assuming a SD card */

	while ((retval = mmc_init_card_state(&request)) == MMC_INIT_DOING)
		;

	if (retval == MMC_INIT_PASSED)
		return MMC_NO_ERROR;
	else
		return MMC_NO_RESPONSE;
}

int mmc_legacy_init(int verbose)
{
	if (!__msc_card_detected())
		return 1;

	printf("MMC card found\n");

	/* Step-1: init GPIO */
	__gpio_as_msc();

	__msc_init_io();

	/* Step-2: turn on power of card */
#if !defined(CONFIG_SAKC)
	__msc_enable_power();
#endif

	/* Step-3: Reset MSC Controller. */
	__msc_reset();

	/* Step-3: mask all IRQs. */
	MMC_IRQ_MASK();

	/* Step-4: stop MMC/SD clock */
	jz_mmc_stop_clock();
	mmc_init_card();
	mmc_select_card();

	mmc_dev.block_read = mmc_bread;
	fat_register_device(&mmc_dev,1); /* partitions start counting with 1 */

	return 0;
}

int mmc_ident(block_dev_desc_t *dev)
{
	return 0;
}

int mmc2info(ulong addr)
{
	/* FIXME hard codes to 32 MB device */
	if (addr >= CFG_MMC_BASE && addr < CFG_MMC_BASE + 0x02000000) {
	return 1;
	}
	return 0;
}
/*
 * Debugging functions
 */

static char * mmc_result_strings[] = {
	"NO_RESPONSE",
	"NO_ERROR",
	"ERROR_OUT_OF_RANGE",
	"ERROR_ADDRESS",
	"ERROR_BLOCK_LEN",
	"ERROR_ERASE_SEQ",
	"ERROR_ERASE_PARAM",
	"ERROR_WP_VIOLATION",
	"ERROR_CARD_IS_LOCKED",
	"ERROR_LOCK_UNLOCK_FAILED",
	"ERROR_COM_CRC",
	"ERROR_ILLEGAL_COMMAND",
	"ERROR_CARD_ECC_FAILED",
	"ERROR_CC",
	"ERROR_GENERAL",
	"ERROR_UNDERRUN",
	"ERROR_OVERRUN",
	"ERROR_CID_CSD_OVERWRITE",
	"ERROR_STATE_MISMATCH",
	"ERROR_HEADER_MISMATCH",
	"ERROR_TIMEOUT",
	"ERROR_CRC",
	"ERROR_DRIVER_FAILURE",
};

char * mmc_result_to_string(int i)
{
	return mmc_result_strings[i+1];
}

static char * card_state_strings[] = {
	"empty",
	"idle",
	"ready",
	"ident",
	"stby",
	"tran",
	"data",
	"rcv",
	"prg",
	"dis",
};

static inline char * card_state_to_string(int i)
{
	return card_state_strings[i+1];
}

/*
 * Utility functions
 */

#define PARSE_U32(_buf,_index) \
	(((u32)_buf[_index]) << 24) | (((u32)_buf[_index+1]) << 16) | \
        (((u32)_buf[_index+2]) << 8) | ((u32)_buf[_index+3]);

#define PARSE_U16(_buf,_index) \
	(((u16)_buf[_index]) << 8) | ((u16)_buf[_index+1]);

int mmc_unpack_csd(struct mmc_request *request, struct mmc_csd *csd)
{
	u8 *buf = request->response;
	int num = 0;

	if (request->result)
		return request->result;

	csd->csd_structure      = (buf[1] & 0xc0) >> 6;
	if (csd->csd_structure)
		sd2_0 = 1;
	else
		sd2_0 = 0;

	switch (csd->csd_structure) {
	case 0 :
		csd->taac               = buf[2];
		csd->nsac               = buf[3];
		csd->tran_speed         = buf[4];
		csd->ccc                = (((u16)buf[5]) << 4) | ((buf[6] & 0xf0) >> 4);
		csd->read_bl_len        = buf[6] & 0x0f;
		/* for support 2GB card*/
		if (csd->read_bl_len >= 10)
		{
			num = csd->read_bl_len - 9;
			csd->read_bl_len = 9;
		}

		csd->read_bl_partial    = (buf[7] & 0x80) ? 1 : 0;
		csd->write_blk_misalign = (buf[7] & 0x40) ? 1 : 0;
		csd->read_blk_misalign  = (buf[7] & 0x20) ? 1 : 0;
		csd->dsr_imp            = (buf[7] & 0x10) ? 1 : 0;
		csd->c_size             = ((((u16)buf[7]) & 0x03) << 10) | (((u16)buf[8]) << 2) | (((u16)buf[9]) & 0xc0) >> 6;

		if (num)
			csd->c_size = csd->c_size << num;


		csd->vdd_r_curr_min     = (buf[9] & 0x38) >> 3;
		csd->vdd_r_curr_max     = buf[9] & 0x07;
		csd->vdd_w_curr_min     = (buf[10] & 0xe0) >> 5;
		csd->vdd_w_curr_max     = (buf[10] & 0x1c) >> 2;
		csd->c_size_mult        = ((buf[10] & 0x03) << 1) | ((buf[11] & 0x80) >> 7);
		switch (csd->csd_structure) {
		case CSD_STRUCT_VER_1_0:
		case CSD_STRUCT_VER_1_1:
			csd->erase.v22.sector_size    = (buf[11] & 0x7c) >> 2;
			csd->erase.v22.erase_grp_size = ((buf[11] & 0x03) << 3) | ((buf[12] & 0xe0) >> 5);

			break;
		case CSD_STRUCT_VER_1_2:
		default:
			csd->erase.v31.erase_grp_size = (buf[11] & 0x7c) >> 2;
			csd->erase.v31.erase_grp_mult = ((buf[11] & 0x03) << 3) | ((buf[12] & 0xe0) >> 5);
			break;
		}
		csd->wp_grp_size        = buf[12] & 0x1f;
		csd->wp_grp_enable      = (buf[13] & 0x80) ? 1 : 0;
		csd->default_ecc        = (buf[13] & 0x60) >> 5;
		csd->r2w_factor         = (buf[13] & 0x1c) >> 2;
		csd->write_bl_len       = ((buf[13] & 0x03) << 2) | ((buf[14] & 0xc0) >> 6);
		if (csd->write_bl_len >= 10)
			csd->write_bl_len = 9;

		csd->write_bl_partial   = (buf[14] & 0x20) ? 1 : 0;
		csd->file_format_grp    = (buf[15] & 0x80) ? 1 : 0;
		csd->copy               = (buf[15] & 0x40) ? 1 : 0;
		csd->perm_write_protect = (buf[15] & 0x20) ? 1 : 0;
		csd->tmp_write_protect  = (buf[15] & 0x10) ? 1 : 0;
		csd->file_format        = (buf[15] & 0x0c) >> 2;
		csd->ecc                = buf[15] & 0x03;

		DEBUG(2,"  csd_structure=%d  spec_vers=%d  taac=%02x  nsac=%02x  tran_speed=%02x\n"
		      "  ccc=%04x  read_bl_len=%d  read_bl_partial=%d  write_blk_misalign=%d\n"
		      "  read_blk_misalign=%d  dsr_imp=%d  c_size=%d  vdd_r_curr_min=%d\n"
		      "  vdd_r_curr_max=%d  vdd_w_curr_min=%d  vdd_w_curr_max=%d  c_size_mult=%d\n"
		      "  wp_grp_size=%d  wp_grp_enable=%d  default_ecc=%d  r2w_factor=%d\n"
		      "  write_bl_len=%d  write_bl_partial=%d  file_format_grp=%d  copy=%d\n"
		      "  perm_write_protect=%d  tmp_write_protect=%d  file_format=%d  ecc=%d\n",
		      csd->csd_structure, csd->spec_vers, 
		      csd->taac, csd->nsac, csd->tran_speed,
		      csd->ccc, csd->read_bl_len, 
		      csd->read_bl_partial, csd->write_blk_misalign,
		      csd->read_blk_misalign, csd->dsr_imp, 
		      csd->c_size, csd->vdd_r_curr_min,
		      csd->vdd_r_curr_max, csd->vdd_w_curr_min, 
		      csd->vdd_w_curr_max, csd->c_size_mult,
		      csd->wp_grp_size, csd->wp_grp_enable,
		      csd->default_ecc, csd->r2w_factor, 
		      csd->write_bl_len, csd->write_bl_partial,
		      csd->file_format_grp, csd->copy, 
		      csd->perm_write_protect, csd->tmp_write_protect,
		      csd->file_format, csd->ecc);
		switch (csd->csd_structure) {
		case CSD_STRUCT_VER_1_0:
		case CSD_STRUCT_VER_1_1:
			DEBUG(2," V22 sector_size=%d erase_grp_size=%d\n", 
			      csd->erase.v22.sector_size, 
			      csd->erase.v22.erase_grp_size);
			break;
		case CSD_STRUCT_VER_1_2:
		default:
			DEBUG(2," V31 erase_grp_size=%d erase_grp_mult=%d\n", 
			      csd->erase.v31.erase_grp_size,
			      csd->erase.v31.erase_grp_mult);
			break;

		}
		break;

	case 1 :
		csd->taac               = 0;
		csd->nsac               = 0;
		csd->tran_speed         = buf[4];
		csd->ccc                = (((u16)buf[5]) << 4) | ((buf[6] & 0xf0) >> 4);

		csd->read_bl_len        = 9;
		csd->read_bl_partial    = 0;
		csd->write_blk_misalign = 0;
		csd->read_blk_misalign  = 0;
		csd->dsr_imp            = (buf[7] & 0x10) ? 1 : 0;
		csd->c_size             = ((((u16)buf[8]) & 0x3f) << 16) | (((u16)buf[9]) << 8) | ((u16)buf[10]) ;
		switch (csd->csd_structure) {
		case CSD_STRUCT_VER_1_0:
		case CSD_STRUCT_VER_1_1:
			csd->erase.v22.sector_size    = 0x7f;
			csd->erase.v22.erase_grp_size = 0;
			break;
		case CSD_STRUCT_VER_1_2:
		default:
			csd->erase.v31.erase_grp_size = 0x7f;
			csd->erase.v31.erase_grp_mult = 0;
			break;
		}
		csd->wp_grp_size        = 0;
		csd->wp_grp_enable      = 0;
		csd->default_ecc        = (buf[13] & 0x60) >> 5;
		csd->r2w_factor         = 4;/* Unused */
		csd->write_bl_len       = 9;

		csd->write_bl_partial   = 0;
		csd->file_format_grp    = 0;
		csd->copy               = (buf[15] & 0x40) ? 1 : 0;
		csd->perm_write_protect = (buf[15] & 0x20) ? 1 : 0;
		csd->tmp_write_protect  = (buf[15] & 0x10) ? 1 : 0;
		csd->file_format        = 0;
		csd->ecc                = buf[15] & 0x03;

		DEBUG(2,"  csd_structure=%d  spec_vers=%d  taac=%02x  nsac=%02x  tran_speed=%02x\n"
		      "  ccc=%04x  read_bl_len=%d  read_bl_partial=%d  write_blk_misalign=%d\n"
		      "  read_blk_misalign=%d  dsr_imp=%d  c_size=%d  vdd_r_curr_min=%d\n"
		      "  vdd_r_curr_max=%d  vdd_w_curr_min=%d  vdd_w_curr_max=%d  c_size_mult=%d\n"
		      "  wp_grp_size=%d  wp_grp_enable=%d  default_ecc=%d  r2w_factor=%d\n"
		      "  write_bl_len=%d  write_bl_partial=%d  file_format_grp=%d  copy=%d\n"
		      "  perm_write_protect=%d  tmp_write_protect=%d  file_format=%d  ecc=%d\n",
		      csd->csd_structure, csd->spec_vers, 
		      csd->taac, csd->nsac, csd->tran_speed,
		      csd->ccc, csd->read_bl_len, 
		      csd->read_bl_partial, csd->write_blk_misalign,
		      csd->read_blk_misalign, csd->dsr_imp, 
		      csd->c_size, csd->vdd_r_curr_min,
		      csd->vdd_r_curr_max, csd->vdd_w_curr_min, 
		      csd->vdd_w_curr_max, csd->c_size_mult,
		      csd->wp_grp_size, csd->wp_grp_enable,
		      csd->default_ecc, csd->r2w_factor, 
		      csd->write_bl_len, csd->write_bl_partial,
		      csd->file_format_grp, csd->copy, 
		      csd->perm_write_protect, csd->tmp_write_protect,
		      csd->file_format, csd->ecc);
		switch (csd->csd_structure) {
		case CSD_STRUCT_VER_1_0:
		case CSD_STRUCT_VER_1_1:
			DEBUG(2," V22 sector_size=%d erase_grp_size=%d\n", 
			      csd->erase.v22.sector_size, 
			      csd->erase.v22.erase_grp_size);
			break;
		case CSD_STRUCT_VER_1_2:
		default:
			DEBUG(2," V31 erase_grp_size=%d erase_grp_mult=%d\n", 
			      csd->erase.v31.erase_grp_size,
			      csd->erase.v31.erase_grp_mult);
			break;
		}
	}

	if (buf[0] != 0x3f)  return MMC_ERROR_HEADER_MISMATCH;

	return 0;
}

int mmc_unpack_r1(struct mmc_request *request, struct mmc_response_r1 *r1, enum card_state state)
{
	u8 *buf = request->response;

	if (request->result)        return request->result;

	r1->cmd    = buf[0];
	r1->status = PARSE_U32(buf,1);

	DEBUG(2, "mmc_unpack_r1: cmd=%d status=%08x\n", r1->cmd, r1->status);

	if (R1_STATUS(r1->status)) {
		if (r1->status & R1_OUT_OF_RANGE)       return MMC_ERROR_OUT_OF_RANGE;
		if (r1->status & R1_ADDRESS_ERROR)      return MMC_ERROR_ADDRESS;
		if (r1->status & R1_BLOCK_LEN_ERROR)    return MMC_ERROR_BLOCK_LEN;
		if (r1->status & R1_ERASE_SEQ_ERROR)    return MMC_ERROR_ERASE_SEQ;
		if (r1->status & R1_ERASE_PARAM)        return MMC_ERROR_ERASE_PARAM;
		if (r1->status & R1_WP_VIOLATION)       return MMC_ERROR_WP_VIOLATION;
		/*if (r1->status & R1_CARD_IS_LOCKED)     return MMC_ERROR_CARD_IS_LOCKED; */
		if (r1->status & R1_LOCK_UNLOCK_FAILED) return MMC_ERROR_LOCK_UNLOCK_FAILED;
		if (r1->status & R1_COM_CRC_ERROR)      return MMC_ERROR_COM_CRC;
		if (r1->status & R1_ILLEGAL_COMMAND)    return MMC_ERROR_ILLEGAL_COMMAND;
		if (r1->status & R1_CARD_ECC_FAILED)    return MMC_ERROR_CARD_ECC_FAILED;
		if (r1->status & R1_CC_ERROR)           return MMC_ERROR_CC;
		if (r1->status & R1_ERROR)              return MMC_ERROR_GENERAL;
		if (r1->status & R1_UNDERRUN)           return MMC_ERROR_UNDERRUN;
		if (r1->status & R1_OVERRUN)            return MMC_ERROR_OVERRUN;
		if (r1->status & R1_CID_CSD_OVERWRITE)  return MMC_ERROR_CID_CSD_OVERWRITE;
	}

	if (buf[0] != request->cmd) return MMC_ERROR_HEADER_MISMATCH;

	/* This should be last - it's the least dangerous error */

	return 0;
}

int mmc_unpack_scr(struct mmc_request *request, struct mmc_response_r1 *r1, enum card_state state, u32 *scr)
{
        u8 *buf = request->response;
	if (request->result)        return request->result;

        *scr = PARSE_U32(buf, 5); /* Save SCR returned by the SD Card */
        return mmc_unpack_r1(request, r1, state);

}

int mmc_unpack_r6(struct mmc_request *request, struct mmc_response_r1 *r1, enum card_state state, int *rca)
{
	u8 *buf = request->response;

	if (request->result)        return request->result;

        *rca = PARSE_U16(buf,1);  /* Save RCA returned by the SD Card */

        *(buf+1) = 0;
        *(buf+2) = 0;

        return mmc_unpack_r1(request, r1, state);
}

int mmc_unpack_cid(struct mmc_request *request, struct mmc_cid *cid)
{
	u8 *buf = request->response;
	int i;

	if (request->result) return request->result;

	cid->mid = buf[1];
	cid->oid = PARSE_U16(buf,2);
	for (i = 0 ; i < 6 ; i++)
		cid->pnm[i] = buf[4+i];
	cid->pnm[6] = 0;
	cid->prv = buf[10];
	cid->psn = PARSE_U32(buf,11);
	cid->mdt = buf[15];

	DEBUG(2,"mmc_unpack_cid: mid=%d oid=%d pnm=%s prv=%d.%d psn=%08x mdt=%d/%d\n",
	      cid->mid, cid->oid, cid->pnm, 
	      (cid->prv>>4), (cid->prv&0xf), 
	      cid->psn, (cid->mdt>>4), (cid->mdt&0xf)+1997);

	if (buf[0] != 0x3f)  return MMC_ERROR_HEADER_MISMATCH;
      	return 0;
}

int mmc_unpack_r3(struct mmc_request *request, struct mmc_response_r3 *r3)
{
	u8 *buf = request->response;

	if (request->result) return request->result;

	r3->ocr = PARSE_U32(buf,1);
	DEBUG(2,"mmc_unpack_r3: ocr=%08x\n", r3->ocr);

	if (buf[0] != 0x3f)  return MMC_ERROR_HEADER_MISMATCH;
	return 0;
}

#define KBPS 1
#define MBPS 1000

static u32 ts_exp[] = { 100*KBPS, 1*MBPS, 10*MBPS, 100*MBPS, 0, 0, 0, 0 };
static u32 ts_mul[] = { 0,    1000, 1200, 1300, 1500, 2000, 2500, 3000, 
			3500, 4000, 4500, 5000, 5500, 6000, 7000, 8000 };

u32 mmc_tran_speed(u8 ts)
{
	u32 rate = ts_exp[(ts & 0x7)] * ts_mul[(ts & 0x78) >> 3];

	if (rate <= 0) {
		DEBUG(0, "mmc_tran_speed: error - unrecognized speed 0x%02x\n", ts);
		return 1;
	}

	return rate;
}

void mmc_send_cmd(struct mmc_request *request, int cmd, u32 arg, 
		   u16 nob, u16 block_len, enum mmc_rsp_t rtype, u8 *buffer)
{
	request->cmd       = cmd;
	request->arg       = arg;
	request->rtype     = rtype;
	request->nob       = nob;
	request->block_len = block_len;
	request->buffer    = buffer;
	request->cnt       = nob * block_len;

	jz_mmc_exec_cmd(request);
}

#endif	/* CONFIG_MMC */
#endif  /* CONFIG_JZ4740 */
