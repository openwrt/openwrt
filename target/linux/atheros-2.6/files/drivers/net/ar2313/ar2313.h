#ifndef _AR2313_H_
#define _AR2313_H_

#include <linux/autoconf.h>
#include <asm/bootinfo.h>
#include <ar531x_platform.h>
#include "platform.h"

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

static inline int tx_space (u32 csm, u32 prd)
{
	return (csm - prd - 1) & (AR2313_DESCR_ENTRIES - 1);
}

#if MAX_SKB_FRAGS
#define TX_RESERVED	(MAX_SKB_FRAGS+1) /* +1 for message header */
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


struct ar2313_cmd {
	u32	cmd;
	u32     address;      /* virtual address of image */
	u32     length;       /* size of image to download */
	u32     mailbox;      /* mailbox to get/set */
	u32     data[2];      /* contents of mailbox to read/write */
};


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
struct ar2313_private
{
	struct net_device *dev;
	int			version;
	u32                     mb[2];
	
	volatile ETHERNET_STRUCT	*phy_regs;
	volatile ETHERNET_STRUCT	*eth_regs;
	volatile DMA			*dma_regs;
	volatile u32		        *int_regs;
	struct ar531x_eth *cfg;

	spinlock_t lock; 	/* Serialise access to device */

	/*
	 * RX and TX descriptors, must be adjacent
	 */
	ar2313_descr_t	        *rx_ring;
	ar2313_descr_t	        *tx_ring;


	struct sk_buff		**rx_skb;
	struct sk_buff		**tx_skb;

	/*
	 * RX elements
	 */
	u32			rx_skbprd;
	u32			cur_rx;

	/*
	 * TX elements
	 */
	u32			tx_prd;
	u32		        tx_csm;

	/*
	 * Misc elements
	 */
	int			board_idx;
	char			name[48];
	struct net_device_stats stats;
	struct {
	    u32 address;
	    u32 length;
	    char *mapping;
	} desc;


	struct timer_list link_timer;
	unsigned short phy;  /* merlot phy = 1,  samsung phy = 0x1f */
	unsigned short mac;
	unsigned short link; /* 0 - link down,  1 - link up */
	u16 phyData;

	struct tasklet_struct rx_tasklet;
	int unloading;
};


/*
 * Prototypes
 */
static int  ar2313_init(struct net_device *dev);
#ifdef TX_TIMEOUT
static void ar2313_tx_timeout(struct net_device *dev);
#endif
#if 0
static void ar2313_multicast_list(struct net_device *dev);
#endif
static int ar2313_restart(struct net_device *dev);
#if DEBUG
static void  ar2313_dump_regs(struct net_device *dev);
#endif
static void ar2313_load_rx_ring(struct net_device *dev, int bufs);
static irqreturn_t ar2313_interrupt(int irq, void *dev_id);
static int  ar2313_open(struct net_device *dev);
static int  ar2313_start_xmit(struct sk_buff *skb, struct net_device *dev);
static int  ar2313_close(struct net_device *dev);
static int  ar2313_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
static void ar2313_init_cleanup(struct net_device *dev);
static int  ar2313_setup_timer(struct net_device *dev);
static void ar2313_link_timer_fn(unsigned long data);
static void ar2313_check_link(struct net_device *dev);
static struct net_device_stats *ar2313_get_stats(struct net_device *dev);
#endif /* _AR2313_H_ */
