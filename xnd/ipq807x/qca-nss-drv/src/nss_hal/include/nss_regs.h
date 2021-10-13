/*
 **************************************************************************
 * Copyright (c) 2013, 2015-2017, 2019-2020, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/**
 * nss_regs.h
 *	NSS register definitions.
 */

#ifndef __NSS_REGS_H
#define __NSS_REGS_H

#include <linux/types.h>
#include <linux/io.h>

/*
 * CSM register offsets
 */
#define NSS_REGS_CORE_ID_OFFSET			0x0000
#define NSS_REGS_RESET_CTRL_OFFSET		0x0004
#define NSS_REGS_CORE_BAR_OFFSET		0x0008
#define NSS_REGS_CORE_AMC_OFFSET		0x000c
#define NSS_REGS_CORE_BOOT_ADDR_OFFSET		0x0010
#define NSS_REGS_C2C_INTR_STATUS_OFFSET		0x0014
#define NSS_REGS_C2C_INTR_SET_OFFSET		0x0018
#define NSS_REGS_C2C_INTR_CLR_OFFSET		0x001c
#define NSS_REGS_N2H_INTR_STATUS_OFFSET		0x0020
#define NSS_REGS_N2H_INTR_SET_OFFSET		0x0024
#define NSS_REGS_N2H_INTR_CLR_OFFSET		0x0028
#define NSS_REGS_N2H_INTR_MASK_OFFSET		0x002c
#define NSS_REGS_N2H_INTR_MASK_SET_OFFSET	0x0030
#define NSS_REGS_N2H_INTR_MASK_CLR_OFFSET	0x0034
#define NSS_REGS_CORE_INT_STAT0_TYPE_OFFSET	0x0038
#define NSS_REGS_CORE_INT_STAT1_TYPE_OFFSET	0x003c
#define NSS_REGS_CORE_INT_STAT2_TYPE_OFFSET	0x0040
#define NSS_REGS_CORE_INT_STAT3_TYPE_OFFSET	0x0044
#define NSS_REGS_CORE_IFETCH_RANGE_OFFSET	0x0048

/*
 * FPB register offsets
 */
#define NSS_REGS_FPB_CSR_CFG_OFFSET		0x0004

/*
 * Defines for N2H interrupts
 */
#define NSS_N2H_INTR_EMPTY_BUFFER_QUEUE		(1 << 0)
#define NSS_N2H_INTR_DATA_QUEUE_0		(1 << 1)
#define NSS_N2H_INTR_DATA_QUEUE_1		(1 << 2)
#define NSS_N2H_INTR_DATA_QUEUE_2		(1 << 3)
#define NSS_N2H_INTR_DATA_QUEUE_3		(1 << 4)
#define NSS_N2H_INTR_EMPTY_BUFFERS_SOS		(1 << 10)
#define NSS_N2H_INTR_TX_UNBLOCKED		(1 << 11)
#define NSS_N2H_INTR_PAGED_EMPTY_BUFFERS_SOS	(1 << 12)
#define NSS_N2H_INTR_PROFILE_DMA		(1 << 13)
#define NSS_N2H_INTR_COREDUMP_COMPLETE		(1 << 14)

/*
 * Types of H2N interrupts
 */
enum nss_h2n_intr_type {
	NSS_H2N_INTR_EMPTY_BUFFER_QUEUE = 0,
	NSS_H2N_INTR_DATA_COMMAND_QUEUE = 1,
	NSS_H2N_INTR_TX_UNBLOCKED = 2,
	NSS_H2N_INTR_TRIGGER_COREDUMP = 3,
	NSS_H2N_INTR_EMPTY_PAGED_BUFFER_QUEUE = 4,
	NSS_H2N_INTR_TYPE_MAX = 5,
};

/*
 * clock source for NSS cores
 */
enum nss_regs_clk_src_select {
	NSS_REGS_CLK_SRC_DEFAULT,
	NSS_REGS_CLK_SRC_ALTERNATE
};

/*
 * nss_read_32()
 *	Read NSS register
 */
static inline uint32_t nss_read_32(void __iomem *addr, uint32_t offs)
{
	return readl(addr + offs);
}

/*
 * nss_write_32()
 *	Write NSS register
 */
static inline void nss_write_32(void __iomem *addr, uint32_t offs, uint32_t val)
{
	writel(val, addr + offs);
}

#endif /* __NSS_REGS_H */
