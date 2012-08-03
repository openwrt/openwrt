/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __STATUS_REG_H
#define __STATUS_REG_H

#define status_r32(reg) ltq_r32(&status->reg)
#define status_w32(val, reg) ltq_w32(val, &status->reg)
#define status_w32_mask(clear, set, reg) ltq_w32_mask(clear, set, &status->reg)

/** STATUS register structure */
struct svip_reg_status {
	unsigned long fuse_deu; /*  0x0000 */
	unsigned long fuse_cpu; /*  0x0004 */
	unsigned long fuse_pll; /*  0x0008 */
	unsigned long chipid; /*  0x000C */
	unsigned long config; /*  0x0010 */
	unsigned long chip_loc; /*  0x0014 */
	unsigned long fuse_spare; /*  0x0018 */
};

/*******************************************************************************
 * Fuse for DEU Settings
 ******************************************************************************/

/* Fuse for Enabling the TRNG (6) */
#define STATUS_FUSE_DEU_TRNG   (0x1 << 6)
#define STATUS_FUSE_DEU_TRNG_GET(val)   ((((val) & STATUS_FUSE_DEU_TRNG) >> 6) & 0x1)
/* Fuse for Enabling the DES Submodule (5) */
#define STATUS_FUSE_DEU_DES   (0x1 << 5)
#define STATUS_FUSE_DEU_DES_GET(val)   ((((val) & STATUS_FUSE_DEU_DES) >> 5) & 0x1)
/* Fuse for Enabling the 3DES Submodule (4) */
#define STATUS_FUSE_DEU_3DES   (0x1 << 4)
#define STATUS_FUSE_DEU_3DES_GET(val)   ((((val) & STATUS_FUSE_DEU_3DES) >> 4) & 0x1)
/* Fuse for Enabling the AES Submodule (3) */
#define STATUS_FUSE_DEU_AES   (0x1 << 3)
#define STATUS_FUSE_DEU_AES_GET(val)   ((((val) & STATUS_FUSE_DEU_AES) >> 3) & 0x1)
/* Fuse for Enabling the HASH Submodule (2) */
#define STATUS_FUSE_DEU_HASH   (0x1 << 2)
#define STATUS_FUSE_DEU_HASH_GET(val)   ((((val) & STATUS_FUSE_DEU_HASH) >> 2) & 0x1)
/* Fuse for Enabling the ARC4 Submodule (1) */
#define STATUS_FUSE_DEU_ARC4   (0x1 << 1)
#define STATUS_FUSE_DEU_ARC4_GET(val)   ((((val) & STATUS_FUSE_DEU_ARC4) >> 1) & 0x1)
/* Fuse for Enabling the DEU Module (0) */
#define STATUS_FUSE_DEU_DEU   (0x1)
#define STATUS_FUSE_DEU_DEU_GET(val)   ((((val) & STATUS_FUSE_DEU_DEU) >> 0) & 0x1)

/*******************************************************************************
 * Fuse for CPU Settings
 ******************************************************************************/

/* Fuse for Enabling CPU5 (5) */
#define STATUS_FUSE_CPU_CPU5   (0x1 << 5)
#define STATUS_FUSE_CPU_CPU5_GET(val)   ((((val) & STATUS_FUSE_CPU_CPU5) >> 5) & 0x1)
/* Fuse for Enabling the CPU4 (4) */
#define STATUS_FUSE_CPU_CPU4   (0x1 << 4)
#define STATUS_FUSE_CPU_CPU4_GET(val)   ((((val) & STATUS_FUSE_CPU_CPU4) >> 4) & 0x1)
/* Fuse for Enabling the CPU3 (3) */
#define STATUS_FUSE_CPU_CPU3   (0x1 << 3)
#define STATUS_FUSE_CPU_CPU3_GET(val)   ((((val) & STATUS_FUSE_CPU_CPU3) >> 3) & 0x1)
/* Fuse for Enabling the CPU2 (2) */
#define STATUS_FUSE_CPU_CPU2   (0x1 << 2)
#define STATUS_FUSE_CPU_CPU2_GET(val)   ((((val) & STATUS_FUSE_CPU_CPU2) >> 2) & 0x1)
/* Fuse for Enabling the CPU1 (1) */
#define STATUS_FUSE_CPU_CPU1   (0x1 << 1)
#define STATUS_FUSE_CPU_CPU1_GET(val)   ((((val) & STATUS_FUSE_CPU_CPU1) >> 1) & 0x1)
/* Fuse for Enabling the CPU0 (0) */
#define STATUS_FUSE_CPU_CPU0   (0x1)
#define STATUS_FUSE_CPU_CPU0_GET(val)   ((((val) & STATUS_FUSE_CPU_CPU0) >> 0) & 0x1)

/*******************************************************************************
 * Fuse for PLL Settings
 ******************************************************************************/

/* Fuse for Enabling PLL (7:0) */
#define STATUS_FUSE_PLL_PLL   (0xff)
#define STATUS_FUSE_PLL_PLL_GET(val)   ((((val) & STATUS_FUSE_PLL_PLL) >> 0) & 0xff)

/*******************************************************************************
 * Chip Identification Register
 ******************************************************************************/

/* Chip Version Number (31:28) */
#define STATUS_CHIPID_VERSION   (0xf << 28)
#define STATUS_CHIPID_VERSION_GET(val)   ((((val) & STATUS_CHIPID_VERSION) >> 28) & 0xf)
/* Part Number (27:12) */
#define STATUS_CHIPID_PART_NUMBER   (0xffff << 12)
#define STATUS_CHIPID_PART_NUMBER_GET(val)   ((((val) & STATUS_CHIPID_PART_NUMBER) >> 12) & 0xffff)
/* Manufacturer ID (11:1) */
#define STATUS_CHIPID_MANID   (0x7ff << 1)
#define STATUS_CHIPID_MANID_GET(val)   ((((val) & STATUS_CHIPID_MANID) >> 1) & 0x7ff)

/*******************************************************************************
 * Chip Configuration Register
 ******************************************************************************/

/* Number of Analog Channels (8:5) */
#define STATUS_CONFIG_ANA_CHAN   (0xf << 5)
#define STATUS_CONFIG_ANA_CHAN_GET(val)   ((((val) & STATUS_CONFIG_ANA_CHAN) >> 5) & 0xf)
/* Clock Mode (4) */
#define STATUS_CONFIG_CLK_MODE   (0x1 << 1)
#define STATUS_CONFIG_CLK_MODE_GET(val)   ((((val) & STATUS_CONFIG_CLK_MODE) >> 4) & 0x1)
/* Subversion Number (3:0) */
#define STATUS_CONFIG_SUB_VERS   (0xF)
#define STATUS_CONFIG_SUB_VERS_GET(val)   ((((val) & STATUS_SUBVER_SUB_VERS) >> 0) & 0xF)

/*******************************************************************************
 * Chip Location Register
 ******************************************************************************/

/* Chip Lot ID (31:16) */
#define STATUS_CHIP_LOC_CHIP_LOT   (0xffff << 16)
#define STATUS_CHIP_LOC_CHIP_LOT_GET(val)   ((((val) & STATUS_CHIP_LOC_CHIP_LOT) >> 16) & 0xffff)
/* Chip X Coordinate (15:8) */
#define STATUS_CHIP_LOC_CHIP_X   (0xff << 8)
#define STATUS_CHIP_LOC_CHIP_X_GET(val)   ((((val) & STATUS_CHIP_LOC_CHIP_X) >> 8) & 0xff)
/* Chip Y Coordinate (7:0) */
#define STATUS_CHIP_LOC_CHIP_Y   (0xff)
#define STATUS_CHIP_LOC_CHIP_Y_GET(val)   ((((val) & STATUS_CHIP_LOC_CHIP_Y) >> 0) & 0xff)

#endif
