/*
 * Copyright (C) 2007 Ingenic Semiconductor Inc.
 * Author: Peter <jlwei@ingenic.cn>
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

#include <common.h>
#include <nand.h>

#include <asm/io.h>
#include <asm/jz4740.h>

#define KEY_U_OUT       (32 * 2 + 16)
#define KEY_U_IN        (32 * 3 + 19)

/*
 * NAND flash definitions
 */

#define NAND_DATAPORT	0xb8000000
#define NAND_ADDRPORT	0xb8010000
#define NAND_COMMPORT	0xb8008000

#define ECC_BLOCK	512
#define ECC_POS		6
#define PAR_SIZE	9

#define __nand_enable()		(REG_EMC_NFCSR |= EMC_NFCSR_NFE1 | EMC_NFCSR_NFCE1)
#define __nand_disable()	(REG_EMC_NFCSR &= ~(EMC_NFCSR_NFCE1))
#define __nand_ecc_rs_encoding() \
	(REG_EMC_NFECR = EMC_NFECR_ECCE | EMC_NFECR_ERST | EMC_NFECR_RS | EMC_NFECR_RS_ENCODING)
#define __nand_ecc_rs_decoding() \
	(REG_EMC_NFECR = EMC_NFECR_ECCE | EMC_NFECR_ERST | EMC_NFECR_RS | EMC_NFECR_RS_DECODING)
#define __nand_ecc_disable()	(REG_EMC_NFECR &= ~EMC_NFECR_ECCE)
#define __nand_ecc_encode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_ENCF))
#define __nand_ecc_decode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_DECF))

static inline void __nand_dev_ready(void)
{
	unsigned int timeout = 10000;
	while ((REG_GPIO_PXPIN(2) & 0x40000000) && timeout--);
	while (!(REG_GPIO_PXPIN(2) & 0x40000000));
}

#define __nand_cmd(n)		(REG8(NAND_COMMPORT) = (n))
#define __nand_addr(n)		(REG8(NAND_ADDRPORT) = (n))
#define __nand_data8()		REG8(NAND_DATAPORT)
#define __nand_data16()		REG16(NAND_DATAPORT)

#if (JZ4740_NANDBOOT_CFG == JZ4740_NANDBOOT_B8R3)
	#define NAND_BUS_WIDTH 8
	#define NAND_ROW_CYCLE 3
#elif (JZ4740_NANDBOOT_CFG == JZ4740_NANDBOOT_B8R2)
	#define NAND_BUS_WIDTH 8
	#define NAND_ROW_CYCLE 2
#elif (JZ4740_NANDBOOT_CFG == JZ4740_NANDBOOT_B16R3)
	#define NAND_BUS_WIDTH 16
	#define NAND_ROW_CYCLE 3
#elif (JZ4740_NANDBOOT_CFG == JZ4740_NANDBOOT_B16R2)
	#define NAND_BUS_WIDTH 16
	#define NAND_ROW_CYCLE 2
#endif

/*
 * NAND flash parameters
 */
static int page_size = 2048;
static int oob_size = 64;
static int ecc_count = 4;
static int page_per_block = 64;
static int bad_block_pos = 0;
static int block_size = 131072;

static unsigned char oob_buf[128] = {0};

/*
 * External routines
 */
extern void flush_cache_all(void);
extern int serial_init(void);
extern void serial_puts(const char *s);
extern void sdram_init(void);
extern void pll_init(void);
extern void usb_boot();

/*
 * NAND flash routines
 */
#if NAND_BUS_WIDTH == 16
static inline void nand_read_buf16(void *buf, int count)
{
	int i;
	u16 *p = (u16 *)buf;

	for (i = 0; i < count; i += 2)
		*p++ = __nand_data16();
}
#define nand_read_buf nand_read_buf16

#elif NAND_BUS_WIDTH == 8
static inline void nand_read_buf8(void *buf, int count)
{
	int i;
	u8 *p = (u8 *)buf;

	for (i = 0; i < count; i++)
		*p++ = __nand_data8();
}
#define nand_read_buf nand_read_buf8

#endif

/* Correct 1~9-bit errors in 512-bytes data */
static void rs_correct(unsigned char *dat, int idx, int mask)
{
	int i;

	idx--;

	i = idx + (idx >> 3);
	if (i >= 512)
		return;

	mask <<= (idx & 0x7);

	dat[i] ^= mask & 0xff;
	if (i < 511)
		dat[i+1] ^= (mask >> 8) & 0xff;
}

static int nand_read_oob(int page_addr, uchar *buf, int size)
{
	int col_addr;
	if (page_size != 512)
		col_addr = page_size;
	else {
		col_addr = 0;
		__nand_dev_ready();
	}

	if (page_size != 512)
		/* Send READ0 command */
		__nand_cmd(NAND_CMD_READ0);
	else
		/* Send READOOB command */
		__nand_cmd(NAND_CMD_READOOB);

	/* Send column address */
	__nand_addr(col_addr & 0xff);
	if (page_size != 512)
		__nand_addr((col_addr >> 8) & 0xff);

	/* Send page address */
	__nand_addr(page_addr & 0xff);
	__nand_addr((page_addr >> 8) & 0xff);
	#ifdef NAND_ROW_CYCLE == 3
		__nand_addr((page_addr >> 16) & 0xff);
	#endif

	/* Send READSTART command for 2048 or 4096 ps NAND */
	if (page_size != 512)
		__nand_cmd(NAND_CMD_READSTART);

	/* Wait for device ready */
	__nand_dev_ready();

	/* Read oob data */
	nand_read_buf(buf, size);
	if (page_size == 512)
		__nand_dev_ready();
	return 0;
}

static int nand_read_page(int page_addr, uchar *dst, uchar *oobbuf)
{
	uchar *databuf = dst, *tmpbuf;
	int i, j;

	/*
	 * Read oob data
	 */
	nand_read_oob(page_addr, oobbuf, oob_size);

	/*
	 * Read page data
	 */

	/* Send READ0 command */
	__nand_cmd(NAND_CMD_READ0);

	/* Send column address */
	__nand_addr(0);
	if (page_size != 512)
		__nand_addr(0);

	/* Send page address */
	__nand_addr(page_addr & 0xff);
	__nand_addr((page_addr >> 8) & 0xff);
	#if NAND_ROW_CYCLE == 3
		__nand_addr((page_addr >> 16) & 0xff);
	#endif

	/* Send READSTART command for 2048 or 4096 ps NAND */
	if (page_size != 512)
		__nand_cmd(NAND_CMD_READSTART);

	/* Wait for device ready */
	__nand_dev_ready();

	/* Read page data */
	tmpbuf = databuf;

	for (i = 0; i < ecc_count; i++) {
		volatile unsigned char *paraddr = (volatile unsigned char *)EMC_NFPAR0;
		unsigned int stat;

		/* Enable RS decoding */
		REG_EMC_NFINTS = 0x0;
		__nand_ecc_rs_decoding();

		/* Read data */
		nand_read_buf((void *)tmpbuf, ECC_BLOCK);

		/* Set PAR values */
		for (j = 0; j < PAR_SIZE; j++) {
#if defined(CONFIG_SYS_NAND_ECC_POS)
			*paraddr++ = oobbuf[CONFIG_SYS_NAND_ECC_POS + i*PAR_SIZE + j];
#else
			*paraddr++ = oobbuf[ECC_POS + i*PAR_SIZE + j];
#endif
		}

		/* Set PRDY */
		REG_EMC_NFECR |= EMC_NFECR_PRDY;

		/* Wait for completion */
		__nand_ecc_decode_sync();

		/* Disable decoding */
		__nand_ecc_disable();

		/* Check result of decoding */
		stat = REG_EMC_NFINTS;
		if (stat & EMC_NFINTS_ERR) {
			/* Error occurred */
			/* serial_puts("\n Error occurred\n"); */
			if (stat & EMC_NFINTS_UNCOR) {
				/* Uncorrectable error occurred */
				/* serial_puts("\nUncorrectable error occurred\n"); */
			}
			else {
				unsigned int errcnt, index, mask;

				errcnt = (stat & EMC_NFINTS_ERRCNT_MASK) >> EMC_NFINTS_ERRCNT_BIT;
				switch (errcnt) {
				case 4:
					index = (REG_EMC_NFERR3 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT;
					mask = (REG_EMC_NFERR3 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT;
					rs_correct(tmpbuf, index, mask);
					/* FALL-THROUGH */
				case 3:
					index = (REG_EMC_NFERR2 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT;
					mask = (REG_EMC_NFERR2 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT;
					rs_correct(tmpbuf, index, mask);
					/* FALL-THROUGH */
				case 2:
					index = (REG_EMC_NFERR1 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT;
					mask = (REG_EMC_NFERR1 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT;
					rs_correct(tmpbuf, index, mask);
					/* FALL-THROUGH */
				case 1:
					index = (REG_EMC_NFERR0 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT;
					mask = (REG_EMC_NFERR0 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT;
					rs_correct(tmpbuf, index, mask);
					break;
				default:
					break;
				}
			}
		}

		tmpbuf += ECC_BLOCK;
	}

	return 0;
}

#ifndef CONFIG_SYS_NAND_BADBLOCK_PAGE
#define CONFIG_SYS_NAND_BADBLOCK_PAGE 0 /* NAND bad block was marked at this page in a block, starting from 0 */
#endif

static void nand_load(int offs, int uboot_size, uchar *dst)
{
	int page;
	int pagecopy_count;

	__nand_enable();

	page = offs / page_size;
	pagecopy_count = 0;
	while (pagecopy_count < (uboot_size / page_size)) {
		if (page % page_per_block == 0) {
			nand_read_oob(page + CONFIG_SYS_NAND_BADBLOCK_PAGE, oob_buf, oob_size);
			if (oob_buf[bad_block_pos] != 0xff) {
				page += page_per_block;
				/* Skip bad block */
				continue;
			}
		}
		/* Load this page to dst, do the ECC */
		nand_read_page(page, dst, oob_buf);

		dst += page_size;
		page++;
		pagecopy_count++;
	}

	__nand_disable();
}

static void jz_nand_init(void) {

 	/* Optimize the timing of nand */
	REG_EMC_SMCR1 = 0x094c4400;
}

static void gpio_init(void)
{
	/*
	 * Initialize SDRAM pins
	 */
#if defined(CONFIG_JZ4720)
	__gpio_as_sdram_16bit_4720();
#elif defined(CONFIG_JZ4725)
	__gpio_as_sdram_16bit_4725();
#else
	__gpio_as_sdram_32bit();
#endif

	/*
	 * Initialize UART0 pins
	 */
	__gpio_as_uart0();
}

static int is_usb_boot()
{
 	int keyU = 0;

 	__gpio_as_input(KEY_U_IN);
 	__gpio_enable_pull(KEY_U_IN);

 	__gpio_as_output(KEY_U_OUT);
 	__gpio_clear_pin(KEY_U_OUT);

 	keyU = __gpio_get_pin(KEY_U_IN);

 	if (keyU)
 		serial_puts("[U] not pressed\n");
 	else
 		serial_puts("[U] pressed\n");

	return !keyU;
}

void nand_boot(void)
{
	void (*uboot)(void);

	/*
	 * Init hardware
	 */
	jz_nand_init();
	gpio_init();
	serial_init();

	serial_puts("\n\nNAND Secondary Program Loader\n\n");

	pll_init();
	sdram_init();

#if defined(CONFIG_NANONOTE)
	if(is_usb_boot()) {
		serial_puts("enter USB BOOT mode\n");
		usb_boot();
	}
#endif

	page_size = CONFIG_SYS_NAND_PAGE_SIZE;
	block_size = CONFIG_SYS_NAND_BLOCK_SIZE;
	page_per_block = CONFIG_SYS_NAND_BLOCK_SIZE / CONFIG_SYS_NAND_PAGE_SIZE;
	bad_block_pos = (page_size == 512) ? 5 : 0;
	oob_size = page_size / 32;
	ecc_count = page_size / ECC_BLOCK;

	/*
	 * Load U-Boot image from NAND into RAM
	 */
	nand_load(CONFIG_SYS_NAND_U_BOOT_OFFS, CONFIG_SYS_NAND_U_BOOT_SIZE,
		  (uchar *)CONFIG_SYS_NAND_U_BOOT_DST);

	uboot = (void (*)(void))CONFIG_SYS_NAND_U_BOOT_START;

	serial_puts("Starting U-Boot ...\n");

	/*
	 * Flush caches
	 */
	flush_cache_all();

	/*
	 * Jump to U-Boot image
	 */
	(*uboot)();
}
