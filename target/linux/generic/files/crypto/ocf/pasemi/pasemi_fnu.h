/*
 * Copyright (C) 2007 PA Semi, Inc
 *
 * Driver for the PA Semi PWRficient DMA Crypto Engine, soft state and
 * hardware register layouts.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef PASEMI_FNU_H
#define PASEMI_FNU_H

#include <linux/spinlock.h>

#define	PASEMI_SESSION(sid)	((sid) & 0xffffffff)
#define	PASEMI_SID(sesn)	((sesn) & 0xffffffff)
#define	DPRINTF(a...)	if (debug) { printk(DRV_NAME ": " a); }

/* Must be a power of two */
#define RX_RING_SIZE 512
#define TX_RING_SIZE 512
#define TX_DESC(ring, num)	((ring)->desc[2 * (num & (TX_RING_SIZE-1))])
#define TX_DESC_INFO(ring, num)	((ring)->desc_info[(num) & (TX_RING_SIZE-1)])
#define MAX_DESC_SIZE 8
#define PASEMI_INITIAL_SESSIONS 10
#define PASEMI_FNU_CHANNELS 8

/* DMA descriptor */
struct pasemi_desc {
	u64 quad[2*MAX_DESC_SIZE];
	int quad_cnt;
	int size;
	int postop;
};

/*
 * Holds per descriptor data
 */
struct pasemi_desc_info {
	int			desc_size;
	int			desc_postop;
#define PASEMI_CHECK_SIG 0x1

	struct cryptop          *cf_crp;
};

/*
 * Holds per channel data
 */
struct pasemi_fnu_txring {
	volatile u64		*desc;
	volatile struct
	pasemi_desc_info	*desc_info;
	dma_addr_t		dma;
	struct timer_list       crypto_timer;
	spinlock_t		fill_lock;
	spinlock_t		clean_lock;
	unsigned int		next_to_fill;
	unsigned int		next_to_clean;
	u16			total_pktcnt;
	int			irq;
	int			sesn;
	char			irq_name[10];
};

/*
 * Holds data specific to a single pasemi device.
 */
struct pasemi_softc {
	softc_device_decl	sc_cdev;
	struct pci_dev		*dma_pdev;	/* device backpointer */
	struct pci_dev		*iob_pdev;	/* device backpointer */
	void __iomem		*dma_regs;
	void __iomem		*iob_regs;
	int			base_irq;
	int			base_chan;
	int32_t			sc_cid;		/* crypto tag */
	int			sc_nsessions;
	struct pasemi_session	**sc_sessions;
	int			sc_num_channels;/* number of crypto channels */

	/* pointer to the array of txring datastructures, one txring per channel */
	struct pasemi_fnu_txring *tx;

	/*
	 * mutual exclusion for the channel scheduler
	 */
	spinlock_t		sc_chnlock;
	/* last channel used, for now use round-robin to allocate channels */
	int			sc_lastchn;
};

struct pasemi_session {
	u64 civ[2];
	u64 keysz;
	u64 key[4];
	u64 ccmd;
	u64 hkey[4];
	u64 hseq;
	u64 giv[2];
	u64 hiv[4];

	int used;
	dma_addr_t	dma_addr;
	int chan;
};

/* status register layout in IOB region, at 0xfd800000 */
struct pasdma_status {
	u64 rx_sta[64];
	u64 tx_sta[20];
};

#define ALG_IS_CIPHER(alg) ((alg == CRYPTO_DES_CBC)		|| \
				(alg == CRYPTO_3DES_CBC)	|| \
				(alg == CRYPTO_AES_CBC)		|| \
				(alg == CRYPTO_ARC4)		|| \
				(alg == CRYPTO_NULL_CBC))

#define ALG_IS_SIG(alg) ((alg == CRYPTO_MD5)			|| \
				(alg == CRYPTO_MD5_HMAC)	|| \
				(alg == CRYPTO_SHA1)		|| \
				(alg == CRYPTO_SHA1_HMAC)	|| \
				(alg == CRYPTO_NULL_HMAC))

enum {
	PAS_DMA_COM_TXCMD = 0x100,	/* Transmit Command Register  */
	PAS_DMA_COM_TXSTA = 0x104,	/* Transmit Status Register   */
	PAS_DMA_COM_RXCMD = 0x108,	/* Receive Command Register   */
	PAS_DMA_COM_RXSTA = 0x10c,	/* Receive Status Register    */
	PAS_DMA_COM_CFG   = 0x114,	/* DMA Configuration Register */
};

/* All these registers live in the PCI configuration space for the DMA PCI
 * device. Use the normal PCI config access functions for them.
 */

#define PAS_DMA_COM_CFG_FWF	0x18000000

#define PAS_DMA_COM_TXCMD_EN	0x00000001 /* enable */
#define PAS_DMA_COM_TXSTA_ACT	0x00000001 /* active */
#define PAS_DMA_COM_RXCMD_EN	0x00000001 /* enable */
#define PAS_DMA_COM_RXSTA_ACT	0x00000001 /* active */

#define _PAS_DMA_TXCHAN_STRIDE	0x20    /* Size per channel		*/
#define _PAS_DMA_TXCHAN_TCMDSTA	0x300	/* Command / Status		*/
#define _PAS_DMA_TXCHAN_CFG	0x304	/* Configuration		*/
#define _PAS_DMA_TXCHAN_DSCRBU	0x308	/* Descriptor BU Allocation	*/
#define _PAS_DMA_TXCHAN_INCR	0x310	/* Descriptor increment		*/
#define _PAS_DMA_TXCHAN_CNT	0x314	/* Descriptor count/offset	*/
#define _PAS_DMA_TXCHAN_BASEL	0x318	/* Descriptor ring base (low)	*/
#define _PAS_DMA_TXCHAN_BASEU	0x31c	/*			(high)	*/
#define PAS_DMA_TXCHAN_TCMDSTA(c) (0x300+(c)*_PAS_DMA_TXCHAN_STRIDE)
#define    PAS_DMA_TXCHAN_TCMDSTA_EN	0x00000001	/* Enabled */
#define    PAS_DMA_TXCHAN_TCMDSTA_ST	0x00000002	/* Stop interface */
#define    PAS_DMA_TXCHAN_TCMDSTA_ACT	0x00010000	/* Active */
#define PAS_DMA_TXCHAN_CFG(c)     (0x304+(c)*_PAS_DMA_TXCHAN_STRIDE)
#define    PAS_DMA_TXCHAN_CFG_TY_FUNC	0x00000002	/* Type = interface */
#define    PAS_DMA_TXCHAN_CFG_TY_IFACE	0x00000000	/* Type = interface */
#define    PAS_DMA_TXCHAN_CFG_TATTR_M	0x0000003c
#define    PAS_DMA_TXCHAN_CFG_TATTR_S	2
#define    PAS_DMA_TXCHAN_CFG_TATTR(x)	(((x) << PAS_DMA_TXCHAN_CFG_TATTR_S) & \
					 PAS_DMA_TXCHAN_CFG_TATTR_M)
#define    PAS_DMA_TXCHAN_CFG_WT_M	0x000001c0
#define    PAS_DMA_TXCHAN_CFG_WT_S	6
#define    PAS_DMA_TXCHAN_CFG_WT(x)	(((x) << PAS_DMA_TXCHAN_CFG_WT_S) & \
					 PAS_DMA_TXCHAN_CFG_WT_M)
#define    PAS_DMA_TXCHAN_CFG_LPSQ_FAST	0x00000400
#define    PAS_DMA_TXCHAN_CFG_LPDQ_FAST	0x00000800
#define    PAS_DMA_TXCHAN_CFG_CF	0x00001000	/* Clean first line */
#define    PAS_DMA_TXCHAN_CFG_CL	0x00002000	/* Clean last line */
#define    PAS_DMA_TXCHAN_CFG_UP	0x00004000	/* update tx descr when sent */
#define PAS_DMA_TXCHAN_INCR(c)    (0x310+(c)*_PAS_DMA_TXCHAN_STRIDE)
#define PAS_DMA_TXCHAN_BASEL(c)   (0x318+(c)*_PAS_DMA_TXCHAN_STRIDE)
#define    PAS_DMA_TXCHAN_BASEL_BRBL_M	0xffffffc0
#define    PAS_DMA_TXCHAN_BASEL_BRBL_S	0
#define    PAS_DMA_TXCHAN_BASEL_BRBL(x)	(((x) << PAS_DMA_TXCHAN_BASEL_BRBL_S) & \
					 PAS_DMA_TXCHAN_BASEL_BRBL_M)
#define PAS_DMA_TXCHAN_BASEU(c)   (0x31c+(c)*_PAS_DMA_TXCHAN_STRIDE)
#define    PAS_DMA_TXCHAN_BASEU_BRBH_M	0x00000fff
#define    PAS_DMA_TXCHAN_BASEU_BRBH_S	0
#define    PAS_DMA_TXCHAN_BASEU_BRBH(x)	(((x) << PAS_DMA_TXCHAN_BASEU_BRBH_S) & \
					 PAS_DMA_TXCHAN_BASEU_BRBH_M)
/* # of cache lines worth of buffer ring */
#define    PAS_DMA_TXCHAN_BASEU_SIZ_M	0x3fff0000
#define    PAS_DMA_TXCHAN_BASEU_SIZ_S	16		/* 0 = 16K */
#define    PAS_DMA_TXCHAN_BASEU_SIZ(x)	(((x) << PAS_DMA_TXCHAN_BASEU_SIZ_S) & \
					 PAS_DMA_TXCHAN_BASEU_SIZ_M)

#define    PAS_STATUS_PCNT_M		0x000000000000ffffull
#define    PAS_STATUS_PCNT_S		0
#define    PAS_STATUS_DCNT_M		0x00000000ffff0000ull
#define    PAS_STATUS_DCNT_S		16
#define    PAS_STATUS_BPCNT_M		0x0000ffff00000000ull
#define    PAS_STATUS_BPCNT_S		32
#define    PAS_STATUS_CAUSE_M		0xf000000000000000ull
#define    PAS_STATUS_TIMER		0x1000000000000000ull
#define    PAS_STATUS_ERROR		0x2000000000000000ull
#define    PAS_STATUS_SOFT		0x4000000000000000ull
#define    PAS_STATUS_INT		0x8000000000000000ull

#define PAS_IOB_DMA_RXCH_CFG(i)		(0x1100 + (i)*4)
#define    PAS_IOB_DMA_RXCH_CFG_CNTTH_M		0x00000fff
#define    PAS_IOB_DMA_RXCH_CFG_CNTTH_S		0
#define    PAS_IOB_DMA_RXCH_CFG_CNTTH(x)	(((x) << PAS_IOB_DMA_RXCH_CFG_CNTTH_S) & \
						 PAS_IOB_DMA_RXCH_CFG_CNTTH_M)
#define PAS_IOB_DMA_TXCH_CFG(i)		(0x1200 + (i)*4)
#define    PAS_IOB_DMA_TXCH_CFG_CNTTH_M		0x00000fff
#define    PAS_IOB_DMA_TXCH_CFG_CNTTH_S		0
#define    PAS_IOB_DMA_TXCH_CFG_CNTTH(x)	(((x) << PAS_IOB_DMA_TXCH_CFG_CNTTH_S) & \
						 PAS_IOB_DMA_TXCH_CFG_CNTTH_M)
#define PAS_IOB_DMA_RXCH_STAT(i)	(0x1300 + (i)*4)
#define    PAS_IOB_DMA_RXCH_STAT_INTGEN	0x00001000
#define    PAS_IOB_DMA_RXCH_STAT_CNTDEL_M	0x00000fff
#define    PAS_IOB_DMA_RXCH_STAT_CNTDEL_S	0
#define    PAS_IOB_DMA_RXCH_STAT_CNTDEL(x)	(((x) << PAS_IOB_DMA_RXCH_STAT_CNTDEL_S) &\
						 PAS_IOB_DMA_RXCH_STAT_CNTDEL_M)
#define PAS_IOB_DMA_TXCH_STAT(i)	(0x1400 + (i)*4)
#define    PAS_IOB_DMA_TXCH_STAT_INTGEN	0x00001000
#define    PAS_IOB_DMA_TXCH_STAT_CNTDEL_M	0x00000fff
#define    PAS_IOB_DMA_TXCH_STAT_CNTDEL_S	0
#define    PAS_IOB_DMA_TXCH_STAT_CNTDEL(x)	(((x) << PAS_IOB_DMA_TXCH_STAT_CNTDEL_S) &\
						 PAS_IOB_DMA_TXCH_STAT_CNTDEL_M)
#define PAS_IOB_DMA_RXCH_RESET(i)	(0x1500 + (i)*4)
#define    PAS_IOB_DMA_RXCH_RESET_PCNT_M	0xffff0000
#define    PAS_IOB_DMA_RXCH_RESET_PCNT_S	16
#define    PAS_IOB_DMA_RXCH_RESET_PCNT(x)	(((x) << PAS_IOB_DMA_RXCH_RESET_PCNT_S) & \
						 PAS_IOB_DMA_RXCH_RESET_PCNT_M)
#define    PAS_IOB_DMA_RXCH_RESET_PCNTRST	0x00000020
#define    PAS_IOB_DMA_RXCH_RESET_DCNTRST	0x00000010
#define    PAS_IOB_DMA_RXCH_RESET_TINTC		0x00000008
#define    PAS_IOB_DMA_RXCH_RESET_DINTC		0x00000004
#define    PAS_IOB_DMA_RXCH_RESET_SINTC		0x00000002
#define    PAS_IOB_DMA_RXCH_RESET_PINTC		0x00000001
#define PAS_IOB_DMA_TXCH_RESET(i)	(0x1600 + (i)*4)
#define    PAS_IOB_DMA_TXCH_RESET_PCNT_M	0xffff0000
#define    PAS_IOB_DMA_TXCH_RESET_PCNT_S	16
#define    PAS_IOB_DMA_TXCH_RESET_PCNT(x)	(((x) << PAS_IOB_DMA_TXCH_RESET_PCNT_S) & \
						 PAS_IOB_DMA_TXCH_RESET_PCNT_M)
#define    PAS_IOB_DMA_TXCH_RESET_PCNTRST	0x00000020
#define    PAS_IOB_DMA_TXCH_RESET_DCNTRST	0x00000010
#define    PAS_IOB_DMA_TXCH_RESET_TINTC		0x00000008
#define    PAS_IOB_DMA_TXCH_RESET_DINTC		0x00000004
#define    PAS_IOB_DMA_TXCH_RESET_SINTC		0x00000002
#define    PAS_IOB_DMA_TXCH_RESET_PINTC		0x00000001

#define PAS_IOB_DMA_COM_TIMEOUTCFG		0x1700
#define    PAS_IOB_DMA_COM_TIMEOUTCFG_TCNT_M	0x00ffffff
#define    PAS_IOB_DMA_COM_TIMEOUTCFG_TCNT_S	0
#define    PAS_IOB_DMA_COM_TIMEOUTCFG_TCNT(x)	(((x) << PAS_IOB_DMA_COM_TIMEOUTCFG_TCNT_S) & \
						 PAS_IOB_DMA_COM_TIMEOUTCFG_TCNT_M)

/* Transmit descriptor fields */
#define	XCT_MACTX_T		0x8000000000000000ull
#define	XCT_MACTX_ST		0x4000000000000000ull
#define XCT_MACTX_NORES		0x0000000000000000ull
#define XCT_MACTX_8BRES		0x1000000000000000ull
#define XCT_MACTX_24BRES	0x2000000000000000ull
#define XCT_MACTX_40BRES	0x3000000000000000ull
#define XCT_MACTX_I		0x0800000000000000ull
#define XCT_MACTX_O		0x0400000000000000ull
#define XCT_MACTX_E		0x0200000000000000ull
#define XCT_MACTX_VLAN_M	0x0180000000000000ull
#define XCT_MACTX_VLAN_NOP	0x0000000000000000ull
#define XCT_MACTX_VLAN_REMOVE	0x0080000000000000ull
#define XCT_MACTX_VLAN_INSERT   0x0100000000000000ull
#define XCT_MACTX_VLAN_REPLACE  0x0180000000000000ull
#define XCT_MACTX_CRC_M		0x0060000000000000ull
#define XCT_MACTX_CRC_NOP	0x0000000000000000ull
#define XCT_MACTX_CRC_INSERT	0x0020000000000000ull
#define XCT_MACTX_CRC_PAD	0x0040000000000000ull
#define XCT_MACTX_CRC_REPLACE	0x0060000000000000ull
#define XCT_MACTX_SS		0x0010000000000000ull
#define XCT_MACTX_LLEN_M	0x00007fff00000000ull
#define XCT_MACTX_LLEN_S	32ull
#define XCT_MACTX_LLEN(x)	((((long)(x)) << XCT_MACTX_LLEN_S) & \
				 XCT_MACTX_LLEN_M)
#define XCT_MACTX_IPH_M		0x00000000f8000000ull
#define XCT_MACTX_IPH_S		27ull
#define XCT_MACTX_IPH(x)	((((long)(x)) << XCT_MACTX_IPH_S) & \
				 XCT_MACTX_IPH_M)
#define XCT_MACTX_IPO_M		0x0000000007c00000ull
#define XCT_MACTX_IPO_S		22ull
#define XCT_MACTX_IPO(x)	((((long)(x)) << XCT_MACTX_IPO_S) & \
				 XCT_MACTX_IPO_M)
#define XCT_MACTX_CSUM_M	0x0000000000000060ull
#define XCT_MACTX_CSUM_NOP	0x0000000000000000ull
#define XCT_MACTX_CSUM_TCP	0x0000000000000040ull
#define XCT_MACTX_CSUM_UDP	0x0000000000000060ull
#define XCT_MACTX_V6		0x0000000000000010ull
#define XCT_MACTX_C		0x0000000000000004ull
#define XCT_MACTX_AL2		0x0000000000000002ull

#define XCT_PTR_T		0x8000000000000000ull
#define XCT_PTR_LEN_M		0x7ffff00000000000ull
#define XCT_PTR_LEN_S		44
#define XCT_PTR_LEN(x)		((((long)(x)) << XCT_PTR_LEN_S) & \
				 XCT_PTR_LEN_M)
#define XCT_PTR_ADDR_M		0x00000fffffffffffull
#define XCT_PTR_ADDR_S		0
#define XCT_PTR_ADDR(x)		((((long)(x)) << XCT_PTR_ADDR_S) & \
				 XCT_PTR_ADDR_M)

/* Function descriptor fields */
#define	XCT_FUN_T		0x8000000000000000ull
#define	XCT_FUN_ST		0x4000000000000000ull
#define XCT_FUN_NORES		0x0000000000000000ull
#define XCT_FUN_8BRES		0x1000000000000000ull
#define XCT_FUN_24BRES		0x2000000000000000ull
#define XCT_FUN_40BRES		0x3000000000000000ull
#define XCT_FUN_I		0x0800000000000000ull
#define XCT_FUN_O		0x0400000000000000ull
#define XCT_FUN_E		0x0200000000000000ull
#define XCT_FUN_FUN_S		54
#define XCT_FUN_FUN_M		0x01c0000000000000ull
#define XCT_FUN_FUN(num)	((((long)(num)) << XCT_FUN_FUN_S) & \
				XCT_FUN_FUN_M)
#define XCT_FUN_CRM_NOP		0x0000000000000000ull
#define XCT_FUN_CRM_SIG		0x0008000000000000ull
#define XCT_FUN_CRM_ENC		0x0010000000000000ull
#define XCT_FUN_CRM_DEC		0x0018000000000000ull
#define XCT_FUN_CRM_SIG_ENC	0x0020000000000000ull
#define XCT_FUN_CRM_ENC_SIG	0x0028000000000000ull
#define XCT_FUN_CRM_SIG_DEC	0x0030000000000000ull
#define XCT_FUN_CRM_DEC_SIG	0x0038000000000000ull
#define XCT_FUN_LLEN_M		0x0007ffff00000000ull
#define XCT_FUN_LLEN_S		32ULL
#define XCT_FUN_LLEN(x)		((((long)(x)) << XCT_FUN_LLEN_S) & \
				 XCT_FUN_LLEN_M)
#define XCT_FUN_SHL_M		0x00000000f8000000ull
#define XCT_FUN_SHL_S		27ull
#define XCT_FUN_SHL(x)		((((long)(x)) << XCT_FUN_SHL_S) & \
				 XCT_FUN_SHL_M)
#define XCT_FUN_CHL_M		0x0000000007c00000ull
#define XCT_FUN_CHL_S		22ull
#define XCT_FUN_CHL(x)		((((long)(x)) << XCT_FUN_CHL_S) & \
				 XCT_FUN_CHL_M)
#define XCT_FUN_HSZ_M		0x00000000003c0000ull
#define XCT_FUN_HSZ_S		18ull
#define XCT_FUN_HSZ(x)		((((long)(x)) << XCT_FUN_HSZ_S) & \
				 XCT_FUN_HSZ_M)
#define XCT_FUN_ALG_DES		0x0000000000000000ull
#define XCT_FUN_ALG_3DES	0x0000000000008000ull
#define XCT_FUN_ALG_AES		0x0000000000010000ull
#define XCT_FUN_ALG_ARC		0x0000000000018000ull
#define XCT_FUN_ALG_KASUMI	0x0000000000020000ull
#define XCT_FUN_BCM_ECB		0x0000000000000000ull
#define XCT_FUN_BCM_CBC		0x0000000000001000ull
#define XCT_FUN_BCM_CFB		0x0000000000002000ull
#define XCT_FUN_BCM_OFB		0x0000000000003000ull
#define XCT_FUN_BCM_CNT		0x0000000000003800ull
#define XCT_FUN_BCM_KAS_F8	0x0000000000002800ull
#define XCT_FUN_BCM_KAS_F9	0x0000000000001800ull
#define XCT_FUN_BCP_NO_PAD	0x0000000000000000ull
#define XCT_FUN_BCP_ZRO		0x0000000000000200ull
#define XCT_FUN_BCP_PL		0x0000000000000400ull
#define XCT_FUN_BCP_INCR	0x0000000000000600ull
#define XCT_FUN_SIG_MD5		(0ull << 4)
#define XCT_FUN_SIG_SHA1	(2ull << 4)
#define XCT_FUN_SIG_HMAC_MD5	(8ull << 4)
#define XCT_FUN_SIG_HMAC_SHA1	(10ull << 4)
#define XCT_FUN_A		0x0000000000000008ull
#define XCT_FUN_C		0x0000000000000004ull
#define XCT_FUN_AL2		0x0000000000000002ull
#define XCT_FUN_SE		0x0000000000000001ull

#define XCT_FUN_SRC_PTR(len, addr)	(XCT_PTR_LEN(len) | XCT_PTR_ADDR(addr))
#define XCT_FUN_DST_PTR(len, addr)	(XCT_FUN_SRC_PTR(len, addr) | \
					0x8000000000000000ull)

#define XCT_CTRL_HDR_FUN_NUM_M		0x01c0000000000000ull
#define XCT_CTRL_HDR_FUN_NUM_S		54
#define XCT_CTRL_HDR_LEN_M		0x0007ffff00000000ull
#define XCT_CTRL_HDR_LEN_S		32
#define XCT_CTRL_HDR_REG_M		0x00000000000000ffull
#define XCT_CTRL_HDR_REG_S		0

#define XCT_CTRL_HDR(funcN,len,reg)	(0x9400000000000000ull | \
			((((long)(funcN)) << XCT_CTRL_HDR_FUN_NUM_S) \
			& XCT_CTRL_HDR_FUN_NUM_M) | \
			((((long)(len)) << \
			XCT_CTRL_HDR_LEN_S) & XCT_CTRL_HDR_LEN_M) | \
			((((long)(reg)) << \
			XCT_CTRL_HDR_REG_S) & XCT_CTRL_HDR_REG_M))

/* Function config command options */
#define	DMA_CALGO_DES			0x00
#define	DMA_CALGO_3DES			0x01
#define	DMA_CALGO_AES			0x02
#define	DMA_CALGO_ARC			0x03

#define DMA_FN_CIV0			0x02
#define DMA_FN_CIV1			0x03
#define DMA_FN_HKEY0			0x0a

#define XCT_PTR_ADDR_LEN(ptr)		((ptr) & XCT_PTR_ADDR_M), \
			(((ptr) & XCT_PTR_LEN_M) >> XCT_PTR_LEN_S)

#endif /* PASEMI_FNU_H */
