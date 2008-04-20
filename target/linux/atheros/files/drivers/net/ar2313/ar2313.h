#ifndef _AR2313_H_
#define _AR2313_H_

#include <linux/autoconf.h>
#include <asm/bootinfo.h>
#include <ar531x_platform.h>

/*
 * probe link timer - 5 secs
 */
#define LINK_TIMER    (5*HZ)

#define IS_DMA_TX_INT(X)   (((X) & (DMA_STATUS_TI)) != 0)
#define IS_DMA_RX_INT(X)   (((X) & (DMA_STATUS_RI)) != 0)
#define IS_DRIVER_OWNED(X) (((X) & (DMA_TX_OWN))    == 0)

#define AR2313_TX_TIMEOUT (HZ/4)

/*
 * Rings
 */
#define DSC_RING_ENTRIES_SIZE	(AR2313_DESCR_ENTRIES * sizeof(struct desc))
#define DSC_NEXT(idx)	        ((idx + 1) & (AR2313_DESCR_ENTRIES - 1))

static inline int tx_space(u32 csm, u32 prd)
{
	return (csm - prd - 1) & (AR2313_DESCR_ENTRIES - 1);
}

#if MAX_SKB_FRAGS
#define TX_RESERVED	(MAX_SKB_FRAGS+1)	/* +1 for message header */
#define tx_ring_full(csm, prd)	(tx_space(csm, prd) <= TX_RESERVED)
#else
#define tx_ring_full		0
#endif

#define AR2313_MBGET		2
#define AR2313_MBSET    	3
#define AR2313_PCI_RECONFIG	4
#define AR2313_PCI_DUMP  	5
#define AR2313_TEST_PANIC	6
#define AR2313_TEST_NULLPTR	7
#define AR2313_READ_DATA	8
#define AR2313_WRITE_DATA	9
#define AR2313_GET_VERSION	10
#define AR2313_TEST_HANG	11
#define AR2313_SYNC		12


//
// New Combo structure for Both Eth0 AND eth1
//
typedef struct {
	volatile unsigned int mac_control;	/* 0x00 */
	volatile unsigned int mac_addr[2];	/* 0x04 - 0x08 */
	volatile unsigned int mcast_table[2];	/* 0x0c - 0x10 */
	volatile unsigned int mii_addr;	/* 0x14 */
	volatile unsigned int mii_data;	/* 0x18 */
	volatile unsigned int flow_control;	/* 0x1c */
	volatile unsigned int vlan_tag;	/* 0x20 */
	volatile unsigned int pad[7];	/* 0x24 - 0x3c */
	volatile unsigned int ucast_table[8];	/* 0x40-0x5c */

} ETHERNET_STRUCT;

/********************************************************************
 * Interrupt controller
 ********************************************************************/

typedef struct {
	volatile unsigned int wdog_control;	/* 0x08 */
	volatile unsigned int wdog_timer;	/* 0x0c */
	volatile unsigned int misc_status;	/* 0x10 */
	volatile unsigned int misc_mask;	/* 0x14 */
	volatile unsigned int global_status;	/* 0x18 */
	volatile unsigned int reserved;	/* 0x1c */
	volatile unsigned int reset_control;	/* 0x20 */
} INTERRUPT;

/********************************************************************
 * DMA controller
 ********************************************************************/
typedef struct {
	volatile unsigned int bus_mode;	/* 0x00 (CSR0) */
	volatile unsigned int xmt_poll;	/* 0x04 (CSR1) */
	volatile unsigned int rcv_poll;	/* 0x08 (CSR2) */
	volatile unsigned int rcv_base;	/* 0x0c (CSR3) */
	volatile unsigned int xmt_base;	/* 0x10 (CSR4) */
	volatile unsigned int status;	/* 0x14 (CSR5) */
	volatile unsigned int control;	/* 0x18 (CSR6) */
	volatile unsigned int intr_ena;	/* 0x1c (CSR7) */
	volatile unsigned int rcv_missed;	/* 0x20 (CSR8) */
	volatile unsigned int reserved[11];	/* 0x24-0x4c (CSR9-19) */
	volatile unsigned int cur_tx_buf_addr;	/* 0x50 (CSR20) */
	volatile unsigned int cur_rx_buf_addr;	/* 0x50 (CSR21) */
} DMA;

/*
 * Struct private for the Sibyte.
 *
 * Elements are grouped so variables used by the tx handling goes
 * together, and will go into the same cache lines etc. in order to
 * avoid cache line contention between the rx and tx handling on SMP.
 *
 * Frequently accessed variables are put at the beginning of the
 * struct to help the compiler generate better/shorter code.
 */
struct ar2313_private {
	struct net_device *dev;
	int version;
	u32 mb[2];

	volatile ETHERNET_STRUCT *phy_regs;
	volatile ETHERNET_STRUCT *eth_regs;
	volatile DMA *dma_regs;
	volatile u32 *int_regs;
	struct ar531x_eth *cfg;

	spinlock_t lock;			/* Serialise access to device */

	/*
	 * RX and TX descriptors, must be adjacent
	 */
	ar2313_descr_t *rx_ring;
	ar2313_descr_t *tx_ring;


	struct sk_buff **rx_skb;
	struct sk_buff **tx_skb;

	/*
	 * RX elements
	 */
	u32 rx_skbprd;
	u32 cur_rx;

	/*
	 * TX elements
	 */
	u32 tx_prd;
	u32 tx_csm;

	/*
	 * Misc elements
	 */
	int board_idx;
	char name[48];
	struct {
		u32 address;
		u32 length;
		char *mapping;
	} desc;


	struct timer_list link_timer;
	unsigned short phy;			/* merlot phy = 1, samsung phy = 0x1f */
	unsigned short mac;
	unsigned short link;		/* 0 - link down, 1 - link up */
	u16 phyData;

	struct tasklet_struct rx_tasklet;
	int unloading;

	struct phy_device *phy_dev;
	struct mii_bus mii_bus;
	int oldduplex;
};


/*
 * Prototypes
 */
static int ar2313_init(struct net_device *dev);
#ifdef TX_TIMEOUT
static void ar2313_tx_timeout(struct net_device *dev);
#endif
#if 0
static void ar2313_multicast_list(struct net_device *dev);
#endif
static int ar2313_restart(struct net_device *dev);
#if DEBUG
static void ar2313_dump_regs(struct net_device *dev);
#endif
static void ar2313_load_rx_ring(struct net_device *dev, int bufs);
static irqreturn_t ar2313_interrupt(int irq, void *dev_id);
static int ar2313_open(struct net_device *dev);
static int ar2313_start_xmit(struct sk_buff *skb, struct net_device *dev);
static int ar2313_close(struct net_device *dev);
static int ar2313_ioctl(struct net_device *dev, struct ifreq *ifr,
						int cmd);
static void ar2313_init_cleanup(struct net_device *dev);
static int ar2313_setup_timer(struct net_device *dev);
static void ar2313_link_timer_fn(unsigned long data);
static void ar2313_check_link(struct net_device *dev);
#endif							/* _AR2313_H_ */
