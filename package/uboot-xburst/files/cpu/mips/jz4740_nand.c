/*
 * Platform independend driver for JZ4740.
 *
 * Copyright (c) 2007 Ingenic Semiconductor Inc.
 * Author: <jlwei@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include <common.h>

#if defined(CONFIG_CMD_NAND) && defined(CONFIG_JZ4740)

#include <nand.h>
#include <asm/jz4740.h>
#include <asm/io.h>

#define PAR_SIZE 9
#define __nand_ecc_enable()    (REG_EMC_NFECR = EMC_NFECR_ECCE | EMC_NFECR_ERST )
#define __nand_ecc_disable()   (REG_EMC_NFECR &= ~EMC_NFECR_ECCE)

#define __nand_select_rs_ecc() (REG_EMC_NFECR |= EMC_NFECR_RS)

#define __nand_rs_ecc_encoding()	(REG_EMC_NFECR |= EMC_NFECR_RS_ENCODING)
#define __nand_rs_ecc_decoding()	(REG_EMC_NFECR |= EMC_NFECR_RS_DECODING)
#define __nand_ecc_encode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_ENCF))
#define __nand_ecc_decode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_DECF))

static void jz_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *this = mtd->priv;
	unsigned long nandaddr = (unsigned long)this->IO_ADDR_W;

	if (ctrl & NAND_CTRL_CHANGE) {
		/* Change this to use I/O accessors. */
		if (ctrl & NAND_NCE)
			REG_EMC_NFCSR |= EMC_NFCSR_NFCE1; 
		else
			REG_EMC_NFCSR &= ~EMC_NFCSR_NFCE1;
	}

	if (cmd == NAND_CMD_NONE)
		return;

	if (ctrl & NAND_CLE)
		nandaddr |= 0x00008000;
	else /* must be ALE */
		nandaddr |= 0x00010000;

	writeb(cmd, (uint8_t *)nandaddr);
}

static int jz_device_ready(struct mtd_info *mtd)
{
	int ready;
	udelay(20);	/* FIXME: add 20us delay */
	ready = (REG_GPIO_PXPIN(2) & 0x40000000) ? 1 : 0;
	return ready;
}

/*
 * EMC setup
 */
static void jz_device_setup(void)
{
	/* Set NFE bit */
	REG_EMC_NFCSR |= EMC_NFCSR_NFE1;
	REG_EMC_SMCR1 = 0x094c4400;
	/* REG_EMC_SMCR3 = 0x04444400; */
}

void board_nand_select_device(struct nand_chip *nand, int chip)
{
	/*
	 * Don't use "chip" to address the NAND device,
	 * generate the cs from the address where it is encoded.
	 */
}

static int jzsoc_nand_calculate_rs_ecc(struct mtd_info* mtd, const u_char* dat,
				u_char* ecc_code)
{
	volatile u8 *paraddr = (volatile u8 *)EMC_NFPAR0;
	short i;

	__nand_ecc_encode_sync()
	__nand_ecc_disable();

	for(i = 0; i < PAR_SIZE; i++)
		ecc_code[i] = *paraddr++;

	return 0;
}

static void jzsoc_nand_enable_rs_hwecc(struct mtd_info* mtd, int mode)
{
 	__nand_ecc_enable();
	__nand_select_rs_ecc();

	REG_EMC_NFINTS = 0x0;
	if (NAND_ECC_READ == mode){
		__nand_rs_ecc_decoding();
	}
	if (NAND_ECC_WRITE == mode){
		__nand_rs_ecc_encoding();
	}
}

/* Correct 1~9-bit errors in 512-bytes data */
static void jzsoc_rs_correct(unsigned char *dat, int idx, int mask)
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

static int jzsoc_nand_rs_correct_data(struct mtd_info *mtd, u_char *dat,
				 u_char *read_ecc, u_char *calc_ecc)
{
	volatile u8 *paraddr = (volatile u8 *)EMC_NFPAR0;
	short k;
	u32 stat;
	/* Set PAR values */

	for (k = 0; k < PAR_SIZE; k++) {
		*paraddr++ = read_ecc[k];
	}

	/* Set PRDY */
	REG_EMC_NFECR |= EMC_NFECR_PRDY;

	/* Wait for completion */
	__nand_ecc_decode_sync();
	__nand_ecc_disable();

	/* Check decoding */
	stat = REG_EMC_NFINTS;
	if (stat & EMC_NFINTS_ERR) {
		if (stat & EMC_NFINTS_UNCOR) {
			printk("Uncorrectable error occurred\n");
			return -1;
		}
		else {
			u32 errcnt = (stat & EMC_NFINTS_ERRCNT_MASK) >> EMC_NFINTS_ERRCNT_BIT;
			switch (errcnt) {
			case 4:
				jzsoc_rs_correct(dat, (REG_EMC_NFERR3 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT, (REG_EMC_NFERR3 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT);
			case 3:
				jzsoc_rs_correct(dat, (REG_EMC_NFERR2 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT, (REG_EMC_NFERR2 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT);
			case 2:
				jzsoc_rs_correct(dat, (REG_EMC_NFERR1 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT, (REG_EMC_NFERR1 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT);
			case 1:
				jzsoc_rs_correct(dat, (REG_EMC_NFERR0 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT, (REG_EMC_NFERR0 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT);
				return 0;
			default:
				break;
	   		}
		}
	}
	/* no error need to be correct */
	return 0;
}

/*
 * Main initialization routine
 */
int board_nand_init(struct nand_chip *nand)
{
	jz_device_setup();

	nand->cmd_ctrl =  jz_hwcontrol;
	nand->dev_ready = jz_device_ready;

	/* FIXME: should use NAND_ECC_SOFT */
	nand->ecc.hwctl = jzsoc_nand_enable_rs_hwecc;
	nand->ecc.correct = jzsoc_nand_rs_correct_data;
	nand->ecc.calculate = jzsoc_nand_calculate_rs_ecc;
	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.size = 512;
	nand->ecc.bytes = 9;

	/* 20 us command delay time */
	nand->chip_delay = 20;

	return 0;
}
#endif /* (CONFIG_SYS_CMD_NAND) */
