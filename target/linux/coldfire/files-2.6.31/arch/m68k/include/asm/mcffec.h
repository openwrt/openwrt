/*
 * Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All rights reserved.
 * Author: Chenghu Wu <b16972@freescale.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
#ifndef __MCFFEC_H__
#define __MCFFEC_H
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <asm/pgtable.h>

/* The FEC stores dest/src/type, data, and checksum for receive packets.
 */
#define PKT_MAXBUF_SIZE         1518

/*
 * The 5270/5271/5280/5282/532x RX control register also contains maximum frame
 * size bits. Other FEC hardware does not, so we need to take that into
 * account when setting it.
 */
#if defined(CONFIG_M523x) || defined(CONFIG_M527x) || defined(CONFIG_M528x) || \
	defined(CONFIG_M520x) || defined(CONFIG_M532x) || \
	defined(CONFIG_M537x) || defined(CONFIG_M5301x) || \
	defined(CONFIG_M5445X)
#define OPT_FRAME_SIZE  (PKT_MAXBUF_SIZE << 16)
#else
#define OPT_FRAME_SIZE  0
#endif
/*
 * Some hardware gets it MAC address out of local flash memory.
 * if this is non-zero then assume it is the address to get MAC from.
 */
#if defined(CONFIG_NETtel)
#define FEC_FLASHMAC    0xf0006006
#elif defined(CONFIG_GILBARCONAP) || defined(CONFIG_SCALES)
#define FEC_FLASHMAC    0xf0006000
#elif defined(CONFIG_CANCam)
#define FEC_FLASHMAC    0xf0020000
#elif defined(CONFIG_M5272C3)
#define FEC_FLASHMAC    (0xffe04000 + 4)
#elif defined(CONFIG_MOD5272)
#define FEC_FLASHMAC    0xffc0406b
#else
#define FEC_FLASHMAC    0
#endif

#ifdef CONFIG_FEC_DMA_USE_SRAM
#define TX_RING_SIZE            8      /* Must be power of two */
#define TX_RING_MOD_MASK        7      /*   for this to work */
#else
#define TX_RING_SIZE            16      /* Must be power of two */
#define TX_RING_MOD_MASK        15      /*   for this to work */
#endif

typedef struct fec {
	unsigned long   fec_reserved0;
	unsigned long   fec_ievent;             /* Interrupt event reg */
	unsigned long   fec_imask;              /* Interrupt mask reg */
	unsigned long   fec_reserved1;
	unsigned long   fec_r_des_active;       /* Receive descriptor reg */
	unsigned long   fec_x_des_active;       /* Transmit descriptor reg */
	unsigned long   fec_reserved2[3];
	unsigned long   fec_ecntrl;             /* Ethernet control reg */
	unsigned long   fec_reserved3[6];
	unsigned long   fec_mii_data;           /* MII manage frame reg */
	unsigned long   fec_mii_speed;          /* MII speed control reg */
	unsigned long   fec_reserved4[7];
	unsigned long   fec_mib_ctrlstat;       /* MIB control/status reg */
	unsigned long   fec_reserved5[7];
	unsigned long   fec_r_cntrl;            /* Receive control reg */
	unsigned long   fec_reserved6[15];
	unsigned long   fec_x_cntrl;            /* Transmit Control reg */
	unsigned long   fec_reserved7[7];
	unsigned long   fec_addr_low;           /* Low 32bits MAC address */
	unsigned long   fec_addr_high;          /* High 16bits MAC address */
	unsigned long   fec_opd;                /* Opcode + Pause duration */
	unsigned long   fec_reserved8[10];
	unsigned long   fec_hash_table_high;    /* High 32bits hash table */
	unsigned long   fec_hash_table_low;     /* Low 32bits hash table */
	unsigned long   fec_grp_hash_table_high;/* High 32bits hash table */
	unsigned long   fec_grp_hash_table_low; /* Low 32bits hash table */
	unsigned long   fec_reserved9[7];
	unsigned long   fec_x_wmrk;             /* FIFO transmit water mark */
	unsigned long   fec_reserved10;
	unsigned long   fec_r_bound;            /* FIFO receive bound reg */
	unsigned long   fec_r_fstart;           /* FIFO receive start reg */
	unsigned long   fec_reserved11[11];
	unsigned long   fec_r_des_start;        /* Receive descriptor ring */
	unsigned long   fec_x_des_start;        /* Transmit descriptor ring */
	unsigned long   fec_r_buff_size;        /* Maximum receive buff size */
} fec_t;

/*
 *      Define the buffer descriptor structure.
 */
typedef struct bufdesc {
	unsigned short  cbd_sc;                 /* Control and status info */
	unsigned short  cbd_datlen;             /* Data length */
	unsigned long   cbd_bufaddr;            /* Buffer address */
} cbd_t;

/* Forward declarations of some structures to support different PHYs
 */
typedef struct {
	uint mii_data;
	void (*funct)(uint mii_reg, struct net_device *dev);
} phy_cmd_t;

typedef struct {
	uint id;
	char *name;

	const phy_cmd_t *config;
	const phy_cmd_t *startup;
	const phy_cmd_t *ack_int;
	const phy_cmd_t *shutdown;
} phy_info_t;

/* The FEC buffer descriptors track the ring buffers.  The rx_bd_base and
 * tx_bd_base always point to the base of the buffer descriptors.  The
 * cur_rx and cur_tx point to the currently available buffer.
 * The dirty_tx tracks the current buffer that is being sent by the
 * controller.  The cur_tx and dirty_tx are equal under both completely
 * empty and completely full conditions.  The empty/ready indicator in
 * the buffer descriptor determines the actual condition.
 */
struct fec_enet_private {
	/* Hardware registers of the FEC device */
	volatile fec_t	*hwp;

	struct net_device *netdev;
	struct platform_device *pdev;
	/* The saved address of a sent-in-place packet/buffer, for skfree(). */
	unsigned char *tx_bounce[TX_RING_SIZE];
	struct	sk_buff *tx_skbuff[TX_RING_SIZE];
	ushort	skb_cur;
	ushort	skb_dirty;

	/* CPM dual port RAM relative addresses.
	*/
	cbd_t	*rx_bd_base;		/* Address of Rx and Tx buffers. */
	cbd_t	*tx_bd_base;
	cbd_t	*cur_rx, *cur_tx;		/* The next free ring entry */
	cbd_t	*dirty_tx;	/* The ring entries to be free()ed. */
	uint	tx_full;
	/* hold while accessing the HW like ringbuffer for tx/rx but not MAC */
	spinlock_t hw_lock;

	/* hold while accessing the mii_list_t() elements */
	spinlock_t mii_lock;
	struct mii_bus *mdio_bus;
	struct phy_device *phydev;

	uint	phy_id;
	uint	phy_id_done;
	uint	phy_status;
	uint	phy_speed;
	phy_info_t const	*phy;
	struct work_struct phy_task;
	volatile fec_t	*phy_hwp;

	uint	sequence_done;
	uint	mii_phy_task_queued;

	uint	phy_addr;

	int	index;
	int	opened;
	int	link;
	int	old_link;
	int	full_duplex;
	int     duplex;
	int	speed;
	int     msg_enable;
};

struct fec_platform_private {
	struct platform_device  *pdev;

	unsigned long           quirks;
	int                     num_slots;      /* Slots on controller */
	struct fec_enet_private *fep_host[0];      /* Pointers to hosts */
};

#endif
