/*
 * hardware interfaces for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef XRADIO_HWIO_H_INCLUDED
#define XRADIO_HWIO_H_INCLUDED

/* extern */ struct xradio_common;

/* DPLL initial values */
#define DPLL_INIT_VAL_XRADIO      (0x0EC4F121)

/* Hardware Type Definitions */
#define HIF_HW_TYPE_XRADIO        (1)


/* boot loader start address in SRAM */
#define DOWNLOAD_BOOT_LOADER_OFFSET   (0x00000000)
/* 32K, 0x4000 to 0xDFFF */
#define DOWNLOAD_FIFO_OFFSET          (0x00004000)
/* 32K */
#define DOWNLOAD_FIFO_SIZE            (0x00008000)
/* 128 bytes, 0xFF80 to 0xFFFF */
#define DOWNLOAD_CTRL_OFFSET          (0x0000FF80)
#define DOWNLOAD_CTRL_DATA_DWORDS     (32-6)

/* Download control area */
struct download_cntl_t {
	/* size of whole firmware file (including Cheksum), host init */
	u32 ImageSize;
	/* downloading flags */
	u32 Flags;
	/* No. of bytes put into the download, init & updated by host */
	u32 Put;
	/* last traced program counter, last ARM reg_pc */
	u32 TracePc;
	/* No. of bytes read from the download, host init, device updates */
	u32 Get;
	/* r0, boot losader status, host init to pending, device updates */
	u32 Status;
	/* Extra debug info, r1 to r14 if status=r0=DOWNLOAD_EXCEPTION */
	u32 DebugData[DOWNLOAD_CTRL_DATA_DWORDS];
};

#define	DOWNLOAD_IMAGE_SIZE_REG		\
	(DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, ImageSize))
#define	DOWNLOAD_FLAGS_REG		\
	(DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, Flags))
#define DOWNLOAD_PUT_REG		\
	(DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, Put))
#define DOWNLOAD_TRACE_PC_REG		\
	(DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, TracePc))
#define	DOWNLOAD_GET_REG		\
	(DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, Get))
#define	DOWNLOAD_STATUS_REG		\
	(DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, Status))
#define DOWNLOAD_DEBUG_DATA_REG		\
	(DOWNLOAD_CTRL_OFFSET + offsetof(struct download_cntl_t, DebugData))
	
#define DOWNLOAD_DEBUG_DATA_LEN   (108)
#define DOWNLOAD_BLOCK_SIZE       (1024)

/* For boot loader detection */
#define DOWNLOAD_ARE_YOU_HERE     (0x87654321)
#define DOWNLOAD_I_AM_HERE        (0x12345678)

/* Download error code */
#define DOWNLOAD_PENDING        (0xFFFFFFFF)
#define DOWNLOAD_SUCCESS        (0)
#define DOWNLOAD_EXCEPTION      (1)
#define DOWNLOAD_ERR_MEM_1      (2)
#define DOWNLOAD_ERR_MEM_2      (3)
#define DOWNLOAD_ERR_SOFTWARE   (4)
#define DOWNLOAD_ERR_FILE_SIZE  (5)
#define DOWNLOAD_ERR_CHECKSUM   (6)
#define DOWNLOAD_ERR_OVERFLOW   (7)
#define DOWNLOAD_ERR_IMAGE      (8)
#define DOWNLOAD_ERR_HOST       (9)
#define DOWNLOAD_ERR_ABORT      (10)

#define SYS_BASE_ADDR_SILICON      (0)
#define AHB_MEMORY_ADDRESS         (SYS_BASE_ADDR_SILICON + 0x08000000)
#define PAC_BASE_ADDRESS_SILICON   (SYS_BASE_ADDR_SILICON + 0x09000000)
#define PAC_SHARED_MEMORY_SILICON  (PAC_BASE_ADDRESS_SILICON)
#define APB_ADDR(addr)             (PAC_SHARED_MEMORY_SILICON + (addr))

/* ***************************************************************
*Device register definitions
*************************************************************** */
/* WBF - SPI Register Addresses */
#define HIF_ADDR_ID_BASE             (0x0000)
/* 16/32 bits */
#define HIF_CONFIG_REG_ID            (0x0000)
/* 16/32 bits */
#define HIF_CONTROL_REG_ID           (0x0001)
/* 16 bits, Q mode W/R */
#define HIF_IN_OUT_QUEUE_REG_ID      (0x0002)
/* 32 bits, AHB bus R/W */
#define HIF_AHB_DPORT_REG_ID         (0x0003)
/* 16/32 bits */
#define HIF_SRAM_BASE_ADDR_REG_ID    (0x0004)
/* 32 bits, APB bus R/W */
#define HIF_SRAM_DPORT_REG_ID        (0x0005)
/* 32 bits, t_settle/general */
#define HIF_TSET_GEN_R_W_REG_ID      (0x0006)
/* 16 bits, Q mode read, no length */
#define HIF_FRAME_OUT_REG_ID         (0x0007)
#define HIF_ADDR_ID_MAX              (HIF_FRAME_OUT_REG_ID)

/* WBF - Control register bit set */
/* next o/p length, bit 11 to 0 */
#define HIF_CTRL_NEXT_LEN_MASK     (0x0FFF)
#define HIF_CTRL_WUP_BIT           (BIT(12))
#define HIF_CTRL_RDY_BIT           (BIT(13))
#define HIF_CTRL_IRQ_ENABLE        (BIT(14))
#define HIF_CTRL_RDY_ENABLE        (BIT(15))
#define HIF_CTRL_IRQ_RDY_ENABLE    (BIT(14)|BIT(15))

/* SPI Config register bit set */
#define HIF_CONFIG_FRAME_BIT       (BIT(2))
#define HIF_CONFIG_WORD_MODE_BITS  (BIT(3)|BIT(4))
#define HIF_CONFIG_WORD_MODE_1     (BIT(3))
#define HIF_CONFIG_WORD_MODE_2     (BIT(4))
#define HIF_CONFIG_ERROR_0_BIT     (BIT(5))
#define HIF_CONFIG_ERROR_1_BIT     (BIT(6))
#define HIF_CONFIG_ERROR_2_BIT     (BIT(7))
/* TBD: Sure??? */
#define HIF_CONFIG_CSN_FRAME_BIT   (BIT(7))
#define HIF_CONFIG_ERROR_3_BIT     (BIT(8))
#define HIF_CONFIG_ERROR_4_BIT     (BIT(9))
/* QueueM */
#define HIF_CONFIG_ACCESS_MODE_BIT (BIT(10))
/* AHB bus */
#define HIF_CONFIG_AHB_PFETCH_BIT  (BIT(11))
#define HIF_CONFIG_CPU_CLK_DIS_BIT (BIT(12))
/* APB bus */
#define HIF_CONFIG_PFETCH_BIT      (BIT(13))
/* cpu reset */
#define HIF_CONFIG_CPU_RESET_BIT   (BIT(14))
#define HIF_CONFIG_CLEAR_INT_BIT   (BIT(15))

/* For XRADIO the IRQ Enable and Ready Bits are in CONFIG register */
#define HIF_CONF_IRQ_RDY_ENABLE	(BIT(16)|BIT(17))

int xradio_data_read(struct xradio_common *hw_priv, void *buf, size_t buf_len);
int xradio_data_write(struct xradio_common *hw_priv, const void *buf, size_t buf_len);
int xradio_reg_read(struct xradio_common *hw_priv, u16 addr, void *buf, size_t buf_len);
int xradio_reg_write(struct xradio_common *hw_priv, u16 addr, const void *buf, size_t buf_len);
int xradio_indirect_read(struct xradio_common *hw_priv, u32 addr, void *buf, 
                         size_t buf_len, u32 prefetch, u16 port_addr);
int xradio_apb_write(struct xradio_common *hw_priv, u32 addr, const void *buf, size_t buf_len);
int xradio_ahb_write(struct xradio_common *hw_priv, u32 addr, const void *buf, size_t buf_len);


static inline int xradio_reg_read_16(struct xradio_common *hw_priv,
                                     u16 addr, u16 *val)
{
	int ret    = 0;
	u32 bigVal = 0;
	ret = xradio_reg_read(hw_priv, addr, &bigVal, sizeof(bigVal));
	*val = (u16)bigVal;
	return ret;
}

static inline int xradio_reg_write_16(struct xradio_common *hw_priv,
                                      u16 addr, u16 val)
{
	u32 bigVal = (u32)val;
	return xradio_reg_write(hw_priv, addr, &bigVal, sizeof(bigVal));
}

static inline int xradio_reg_read_32(struct xradio_common *hw_priv,
                                      u16 addr, u32 *val)
{
	return xradio_reg_read(hw_priv, addr, val, sizeof(val));
}

static inline int xradio_reg_write_32(struct xradio_common *hw_priv,
                                      u16 addr, u32 val)
{
	return xradio_reg_write(hw_priv, addr, &val, sizeof(val));
}

static inline int xradio_apb_read(struct xradio_common *hw_priv, u32 addr,
                                  void *buf, size_t buf_len)
{
	return xradio_indirect_read(hw_priv, addr, buf, buf_len, HIF_CONFIG_PFETCH_BIT, 
	                            HIF_SRAM_DPORT_REG_ID);
}

static inline int xradio_ahb_read(struct xradio_common *hw_priv, u32 addr,
                                  void *buf, size_t buf_len)
{
	return xradio_indirect_read(hw_priv, addr, buf, buf_len, HIF_CONFIG_AHB_PFETCH_BIT, 
	                            HIF_AHB_DPORT_REG_ID);
}

static inline int xradio_apb_read_32(struct xradio_common *hw_priv,
                                      u32 addr, u32 *val)
{
	return xradio_apb_read(hw_priv, addr, val, sizeof(val));
}

static inline int xradio_apb_write_32(struct xradio_common *hw_priv,
                                      u32 addr, u32 val)
{
	return xradio_apb_write(hw_priv, addr, &val, sizeof(val));
}

static inline int xradio_ahb_read_32(struct xradio_common *hw_priv,
                                      u32 addr, u32 *val)
{
	return xradio_ahb_read(hw_priv, addr, val, sizeof(val));
}

static inline int xradio_ahb_write_32(struct xradio_common *hw_priv,
                                      u32 addr, u32 val)
{
	return xradio_ahb_write(hw_priv, addr, &val, sizeof(val));
}

#endif /* XRADIO_HWIO_H_INCLUDED */
