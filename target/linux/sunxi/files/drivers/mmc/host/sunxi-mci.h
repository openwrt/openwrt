/*
 * Driver for sunxi SD/MMC host controllers
 * (C) Copyright 2007-2011 Reuuimlla Technology Co., Ltd.
 * (C) Copyright 2007-2011 Aaron Maoye <leafy.myeh@reuuimllatech.com>
 * (C) Copyright 2013-2013 O2S GmbH <www.o2s.ch>
 * (C) Copyright 2013-2013 David Lanzendörfer <david.lanzendoerfer@o2s.ch>
 * (C) Copyright 2013-2013 Hans de Goede <hdegoede@redhat.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef __SUNXI_MCI_H__
#define __SUNXI_MCI_H__

/* register offset define */
#define SDXC_REG_GCTRL	(0x00) /* SMC Global Control Register */
#define SDXC_REG_CLKCR	(0x04) /* SMC Clock Control Register */
#define SDXC_REG_TMOUT	(0x08) /* SMC Time Out Register */
#define SDXC_REG_WIDTH	(0x0C) /* SMC Bus Width Register */
#define SDXC_REG_BLKSZ	(0x10) /* SMC Block Size Register */
#define SDXC_REG_BCNTR	(0x14) /* SMC Byte Count Register */
#define SDXC_REG_CMDR	(0x18) /* SMC Command Register */
#define SDXC_REG_CARG	(0x1C) /* SMC Argument Register */
#define SDXC_REG_RESP0	(0x20) /* SMC Response Register 0 */
#define SDXC_REG_RESP1	(0x24) /* SMC Response Register 1 */
#define SDXC_REG_RESP2	(0x28) /* SMC Response Register 2 */
#define SDXC_REG_RESP3	(0x2C) /* SMC Response Register 3 */
#define SDXC_REG_IMASK	(0x30) /* SMC Interrupt Mask Register */
#define SDXC_REG_MISTA	(0x34) /* SMC Masked Interrupt Status Register */
#define SDXC_REG_RINTR	(0x38) /* SMC Raw Interrupt Status Register */
#define SDXC_REG_STAS	(0x3C) /* SMC Status Register */
#define SDXC_REG_FTRGL	(0x40) /* SMC FIFO Threshold Watermark Registe */
#define SDXC_REG_FUNS	(0x44) /* SMC Function Select Register */
#define SDXC_REG_CBCR	(0x48) /* SMC CIU Byte Count Register */
#define SDXC_REG_BBCR	(0x4C) /* SMC BIU Byte Count Register */
#define SDXC_REG_DBGC	(0x50) /* SMC Debug Enable Register */
#define SDXC_REG_HWRST	(0x78) /* SMC Card Hardware Reset for Register */
#define SDXC_REG_DMAC	(0x80) /* SMC IDMAC Control Register */
#define SDXC_REG_DLBA	(0x84) /* SMC IDMAC Descriptor List Base Addre */
#define SDXC_REG_IDST	(0x88) /* SMC IDMAC Status Register */
#define SDXC_REG_IDIE	(0x8C) /* SMC IDMAC Interrupt Enable Register */
#define SDXC_REG_CHDA	(0x90)
#define SDXC_REG_CBDA	(0x94)

#define mci_readl(host, reg) \
	__raw_readl((host)->reg_base + SDXC_##reg)
#define mci_writel(host, reg, value) \
	__raw_writel((value), (host)->reg_base + SDXC_##reg)

/* global control register bits */
#define SDXC_SoftReset		BIT(0)
#define SDXC_FIFOReset		BIT(1)
#define SDXC_DMAReset		BIT(2)
#define SDXC_HWReset		(SDXC_SoftReset|SDXC_FIFOReset|SDXC_DMAReset)
#define SDXC_INTEnb		BIT(4)
#define SDXC_DMAEnb		BIT(5)
#define SDXC_DebounceEnb	BIT(8)
#define SDXC_PosedgeLatchData	BIT(9)
#define SDXC_DDR_MODE		BIT(10)
#define SDXC_MemAccessDone	BIT(29)
#define SDXC_AccessDoneDirect	BIT(30)
#define SDXC_ACCESS_BY_AHB	BIT(31)
#define SDXC_ACCESS_BY_DMA	(0U << 31)
/* clock control bits */
#define SDXC_CardClkOn		BIT(16)
#define SDXC_LowPowerOn		BIT(17)
/* bus width */
#define SDXC_WIDTH1		(0)
#define SDXC_WIDTH4		(1)
#define SDXC_WIDTH8		(2)
/* smc command bits */
#define SDXC_RspExp		BIT(6)
#define SDXC_LongRsp		BIT(7)
#define SDXC_CheckRspCRC	BIT(8)
#define SDXC_DataExp		BIT(9)
#define SDXC_Write		BIT(10)
#define SDXC_Seqmod		BIT(11)
#define SDXC_SendAutoStop	BIT(12)
#define SDXC_WaitPreOver	BIT(13)
#define SDXC_StopAbortCMD	BIT(14)
#define SDXC_SendInitSeq	BIT(15)
#define SDXC_UPCLKOnly		BIT(21)
#define SDXC_RdCEATADev		BIT(22)
#define SDXC_CCSExp		BIT(23)
#define SDXC_EnbBoot		BIT(24)
#define SDXC_AltBootOpt		BIT(25)
#define SDXC_BootACKExp		BIT(26)
#define SDXC_BootAbort		BIT(27)
#define SDXC_VolSwitch	        BIT(28)
#define SDXC_UseHoldReg	        BIT(29)
#define SDXC_Start	        BIT(31)
/* interrupt bits */
#define SDXC_RespErr		BIT(1)
#define SDXC_CmdDone		BIT(2)
#define SDXC_DataOver		BIT(3)
#define SDXC_TxDataReq		BIT(4)
#define SDXC_RxDataReq		BIT(5)
#define SDXC_RespCRCErr		BIT(6)
#define SDXC_DataCRCErr		BIT(7)
#define SDXC_RespTimeout	BIT(8)
#define SDXC_DataTimeout	BIT(9)
#define SDXC_VolChgDone		BIT(10)
#define SDXC_FIFORunErr		BIT(11)
#define SDXC_HardWLocked	BIT(12)
#define SDXC_StartBitErr	BIT(13)
#define SDXC_AutoCMDDone	BIT(14)
#define SDXC_EndBitErr		BIT(15)
#define SDXC_SDIOInt		BIT(16)
#define SDXC_CardInsert		BIT(30)
#define SDXC_CardRemove		BIT(31)
#define SDXC_IntErrBit		(SDXC_RespErr | SDXC_RespCRCErr | \
				 SDXC_DataCRCErr | SDXC_RespTimeout | \
				 SDXC_DataTimeout | SDXC_FIFORunErr | \
				 SDXC_HardWLocked | SDXC_StartBitErr | \
				 SDXC_EndBitErr) /* 0xbbc2 */
#define SDXC_IntDoneBit		(SDXC_AutoCMDDone | SDXC_DataOver | \
				 SDXC_CmdDone | SDXC_VolChgDone)
/* status */
#define SDXC_RXWLFlag		BIT(0)
#define SDXC_TXWLFlag		BIT(1)
#define SDXC_FIFOEmpty		BIT(2)
#define SDXC_FIFOFull		BIT(3)
#define SDXC_CardPresent	BIT(8)
#define SDXC_CardDataBusy	BIT(9)
#define SDXC_DataFSMBusy	BIT(10)
#define SDXC_DMAReq		BIT(31)
#define SDXC_FIFO_SIZE		(16)
/* Function select */
#define SDXC_CEATAOn		(0xceaaU << 16)
#define SDXC_SendIrqRsp		BIT(0)
#define SDXC_SDIORdWait		BIT(1)
#define SDXC_AbtRdData		BIT(2)
#define SDXC_SendCCSD		BIT(8)
#define SDXC_SendAutoStopCCSD	BIT(9)
#define SDXC_CEATADevIntEnb	BIT(10)
/* IDMA controller bus mod bit field */
#define SDXC_IDMACSoftRST	BIT(0)
#define SDXC_IDMACFixBurst	BIT(1)
#define SDXC_IDMACIDMAOn	BIT(7)
#define SDXC_IDMACRefetchDES	BIT(31)
/* IDMA status bit field */
#define SDXC_IDMACTransmitInt	BIT(0)
#define SDXC_IDMACReceiveInt	BIT(1)
#define SDXC_IDMACFatalBusErr	BIT(2)
#define SDXC_IDMACDesInvalid	BIT(4)
#define SDXC_IDMACCardErrSum	BIT(5)
#define SDXC_IDMACNormalIntSum	BIT(8)
#define SDXC_IDMACAbnormalIntSum BIT(9)
#define SDXC_IDMACHostAbtInTx	BIT(10)
#define SDXC_IDMACHostAbtInRx	BIT(10)
#define SDXC_IDMACIdle		(0U << 13)
#define SDXC_IDMACSuspend	(1U << 13)
#define SDXC_IDMACDESCRd	(2U << 13)
#define SDXC_IDMACDESCCheck	(3U << 13)
#define SDXC_IDMACRdReqWait	(4U << 13)
#define SDXC_IDMACWrReqWait	(5U << 13)
#define SDXC_IDMACRd		(6U << 13)
#define SDXC_IDMACWr		(7U << 13)
#define SDXC_IDMACDESCClose	(8U << 13)

struct sunxi_idma_des {
	u32	config;
#define SDXC_IDMAC_DES0_DIC	BIT(1)  /* disable interrupt on completion */
#define SDXC_IDMAC_DES0_LD	BIT(2)  /* last descriptor */
#define SDXC_IDMAC_DES0_FD	BIT(3)  /* first descriptor */
#define SDXC_IDMAC_DES0_CH	BIT(4)  /* chain mode */
#define SDXC_IDMAC_DES0_ER	BIT(5)  /* end of ring */
#define SDXC_IDMAC_DES0_CES	BIT(30) /* card error summary */
#define SDXC_IDMAC_DES0_OWN	BIT(31) /* 1-idma owns it, 0-host owns it */

	/*
	 * If the idma-des-size-bits of property is ie 13, bufsize bits are:
	 *  Bits  0-12: buf1 size
	 *  Bits 13-25: buf2 size
	 *  Bits 26-31: not used
	 * Since we only ever set buf1 size, we can simply store it directly.
	 */
	u32	buf_size;
	u32	buf_addr_ptr1;
	u32	buf_addr_ptr2;
};

struct sunxi_mmc_host {
	struct mmc_host *mmc;
	struct regulator *vmmc;

	/* IO mapping base */
	void __iomem *reg_base;

	spinlock_t lock;
	struct tasklet_struct tasklet;

	/* clock management */
	struct clk *clk_ahb;
	struct clk *clk_mod;

	/* indicator pins */
	int wp_pin;
	int cd_pin;
	int cd_mode;
#define CARD_DETECT_BY_GPIO_POLL (1)	/* mmc detected by gpio check */
#define CARD_ALWAYS_PRESENT      (2)	/* mmc always present */

	/* ios information */
	u32		clk_mod_rate;
	u32		bus_width;
	u32		idma_des_size_bits;
	u32		ddr;
	u32		voltage_switching;

	/* irq */
	int		irq;
	u32		int_sum;
	u32		sdio_imask;

	/* flags */
	u32		power_on:1;
	u32		io_flag:1;
	u32		wait_dma:1;

	dma_addr_t	sg_dma;
	void		*sg_cpu;

	struct mmc_request *mrq;
	u32		ferror;
};

#define MMC_CLK_400K            0
#define MMC_CLK_25M             1
#define MMC_CLK_50M             2
#define MMC_CLK_50MDDR          3
#define MMC_CLK_50MDDR_8BIT     4
#define MMC_CLK_100M            5
#define MMC_CLK_200M            6
#define MMC_CLK_MOD_NUM         7

struct sunxi_mmc_clk_dly {
	u32 mode;
	u32 oclk_dly;
	u32 sclk_dly;
};

#endif
