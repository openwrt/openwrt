/******************************************************************************/
/*                                                                            */
/* Broadcom BCM5700 Linux Network Driver, Copyright (c) 2000 - 2005 Broadcom  */
/* Corporation.                                                               */
/* All rights reserved.                                                       */
/*                                                                            */
/* This program is free software; you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation, located in the file LICENSE.                 */
/*                                                                            */
/******************************************************************************/


char bcm5700_driver[] = "bcm57xx";
char bcm5700_version[] = "8.3.14";
char bcm5700_date[] = "(11/2/05)";

#define B57UM
#include "mm.h"
#include "linux/mii.h" //@.@jack add it 2006/06/28.
#include "typedefs.h"
#include "osl.h"
#include "bcmdefs.h"
#include "bcmdevs.h"
#include "sbconfig.h"
#include "sbutils.h"
#include "hndgige.h"
#include "bcmrobo.h"
#include "robo_register.c"

#include "bcmendian.h"
#include "bcmnvram.h"
#include "proto/ethernet.h"
#include "proto/vlan.h"
#include "proto/bcmtcp.h"
#include "proto/bcmip.h"
#define	PKTDATA(osh, skb)		(((struct sk_buff*)(skb))->data)

/* this is needed to get good and stable performances */
#define EXTRA_HDR BCMEXTRAHDROOM

#define SIOCGREG_STATUS  0x8996          /* Read Switch register (for debug)*/
#define SIOCSREG_STATUS  0x8997          /* Write Switch register(for debug)*/

/* This structure is used in SIOCXREG_STATUS ioctl calls*/
struct reg_ioctl_data {
        u16             page_num;
        u16             addr_num;
        u16             len;
	u16		val_in[4];
	u16		val_out[4];
};

/* A few user-configurable values. */

#define MAX_UNITS 16
/* Used to pass the full-duplex flag, etc. */
static int line_speed[MAX_UNITS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int auto_speed[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int full_duplex[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int rx_flow_control[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int tx_flow_control[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int auto_flow_control[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
static int mtu[MAX_UNITS] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500};	/* Jumbo MTU for interfaces. */
#endif
static int tx_checksum[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int rx_checksum[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int scatter_gather[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int activate_gpio = -1;

#define TX_DESC_CNT DEFAULT_TX_PACKET_DESC_COUNT
static unsigned int tx_pkt_desc_cnt[MAX_UNITS] =
	{TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,
	TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,
	TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,TX_DESC_CNT,
	TX_DESC_CNT};

#define RX_DESC_CNT DEFAULT_STD_RCV_DESC_COUNT
static unsigned int rx_std_desc_cnt[MAX_UNITS] =
	{RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,
	RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,
	RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,RX_DESC_CNT,
	RX_DESC_CNT };

#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
#define JBO_DESC_CNT DEFAULT_JUMBO_RCV_DESC_COUNT
static unsigned int rx_jumbo_desc_cnt[MAX_UNITS] =
	{JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,
	JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,
	JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,JBO_DESC_CNT,
	JBO_DESC_CNT };
#endif

#ifdef BCM_INT_COAL
#ifdef BCM_NAPI_RXPOLL
static unsigned int adaptive_coalesce[MAX_UNITS] =
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#else
static unsigned int adaptive_coalesce[MAX_UNITS] =
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
#endif

#define RX_COAL_TK DEFAULT_RX_COALESCING_TICKS
static unsigned int rx_coalesce_ticks[MAX_UNITS] =
	{RX_COAL_TK,RX_COAL_TK,RX_COAL_TK,RX_COAL_TK,RX_COAL_TK,
	RX_COAL_TK, RX_COAL_TK,RX_COAL_TK,RX_COAL_TK,RX_COAL_TK,
	RX_COAL_TK,RX_COAL_TK, RX_COAL_TK,RX_COAL_TK,RX_COAL_TK,
	RX_COAL_TK};

#define RX_COAL_FM DEFAULT_RX_MAX_COALESCED_FRAMES
static unsigned int rx_max_coalesce_frames[MAX_UNITS] =
	{RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,
	RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,
	RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,RX_COAL_FM,
	RX_COAL_FM};

#define TX_COAL_TK DEFAULT_TX_COALESCING_TICKS
static unsigned int tx_coalesce_ticks[MAX_UNITS] =
	{TX_COAL_TK,TX_COAL_TK,TX_COAL_TK,TX_COAL_TK,TX_COAL_TK,
	TX_COAL_TK, TX_COAL_TK,TX_COAL_TK,TX_COAL_TK,TX_COAL_TK,
	TX_COAL_TK,TX_COAL_TK, TX_COAL_TK,TX_COAL_TK,TX_COAL_TK,
	TX_COAL_TK};

#define TX_COAL_FM DEFAULT_TX_MAX_COALESCED_FRAMES
static unsigned int tx_max_coalesce_frames[MAX_UNITS] =
	{TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,
	TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,
	TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,TX_COAL_FM,
	TX_COAL_FM};

#define ST_COAL_TK DEFAULT_STATS_COALESCING_TICKS
static unsigned int stats_coalesce_ticks[MAX_UNITS] =
	{ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,
	ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,
	ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,ST_COAL_TK,
	ST_COAL_TK,};

#endif
#ifdef BCM_WOL
static int enable_wol[MAX_UNITS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif
#ifdef BCM_TSO
static int enable_tso[MAX_UNITS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
#endif
#ifdef BCM_NIC_SEND_BD
static int nic_tx_bd[MAX_UNITS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif
#ifdef BCM_ASF
static int vlan_tag_mode[MAX_UNITS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif
static int delay_link[MAX_UNITS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int disable_d3hot[MAX_UNITS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)
static int disable_msi[MAX_UNITS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int bcm_msi_chipset_bug = 0;
#endif

#define BCM_TIMER_GRANULARITY  (1000000 / HZ)

/* Hack to hook the data path to the BCM WL dirver */
#ifdef BCM_WL_EMULATOR
#include "bcmnvram.h"
#include "wl_bcm57emu.h" 
#ifdef SKB_MANAGER
int skb_old_alloc = 0;
#endif
#endif /* BCM_WL_EMULATOR */

/* Operational parameters that usually are not changed. */
/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT  (2*HZ)

#if (LINUX_VERSION_CODE < 0x02030d)
#define pci_resource_start(dev, bar)	(dev->base_address[bar] & PCI_BASE_ADDRESS_MEM_MASK)
#elif (LINUX_VERSION_CODE < 0x02032b)
#define pci_resource_start(dev, bar)	(dev->resource[bar] & PCI_BASE_ADDRESS_MEM_MASK)
#endif

#if (LINUX_VERSION_CODE < 0x02032b)
#define dev_kfree_skb_irq(skb)  dev_kfree_skb(skb)
#define netif_wake_queue(dev)	clear_bit(0, &dev->tbusy); mark_bh(NET_BH)
#define netif_stop_queue(dev)	set_bit(0, &dev->tbusy)

static inline void netif_start_queue(struct net_device *dev)
{
	dev->tbusy = 0;
	dev->interrupt = 0;
	dev->start = 1;
}

#define netif_queue_stopped(dev)	dev->tbusy
#define netif_running(dev)		dev->start

static inline void tasklet_schedule(struct tasklet_struct *tasklet)
{
	queue_task(tasklet, &tq_immediate);
	mark_bh(IMMEDIATE_BH);
}

static inline void tasklet_init(struct tasklet_struct *tasklet,
				void (*func)(unsigned long),
				unsigned long data)
{
		tasklet->next = NULL;
		tasklet->sync = 0;
		tasklet->routine = (void (*)(void *))func;
		tasklet->data = (void *)data;
}

#define tasklet_kill(tasklet)

#endif

#if (LINUX_VERSION_CODE < 0x020300)
struct pci_device_id {
	unsigned int vendor, device;		/* Vendor and device ID or PCI_ANY_ID */
	unsigned int subvendor, subdevice;	/* Subsystem ID's or PCI_ANY_ID */
	unsigned int class, class_mask;		/* (class,subclass,prog-if) triplet */
	unsigned long driver_data;		/* Data private to the driver */
};

#define PCI_ANY_ID		0

#define pci_set_drvdata(pdev, dev)
#define pci_get_drvdata(pdev) 0

#define pci_enable_device(pdev) 0

#define __devinit		__init
#define __devinitdata		__initdata
#define __devexit

#define SET_MODULE_OWNER(dev)
#define MODULE_DEVICE_TABLE(pci, pci_tbl)

#endif

#if (LINUX_VERSION_CODE < 0x020411)
#ifndef __devexit_p
#define __devexit_p(x)	x
#endif
#endif

#ifndef MODULE_LICENSE
#define MODULE_LICENSE(license)
#endif

#ifndef IRQ_RETVAL
typedef void irqreturn_t;
#define IRQ_RETVAL(x)
#endif

#if (LINUX_VERSION_CODE < 0x02032a)
static inline void *pci_alloc_consistent(struct pci_dev *pdev, size_t size,
					 dma_addr_t *dma_handle)
{
	void *virt_ptr;

	/* Maximum in slab.c */
	if (size > 131072)
		return 0;

	virt_ptr = kmalloc(size, GFP_KERNEL);
	*dma_handle = virt_to_bus(virt_ptr);
	return virt_ptr;
}
#define pci_free_consistent(dev, size, ptr, dma_ptr)	kfree(ptr)

#endif /*#if (LINUX_VERSION_CODE < 0x02032a) */


#if (LINUX_VERSION_CODE < 0x02040d)

#if (LINUX_VERSION_CODE >= 0x020409) && defined(RED_HAT_LINUX_KERNEL)

#define BCM_32BIT_DMA_MASK ((u64) 0x00000000ffffffffULL)
#define BCM_64BIT_DMA_MASK ((u64) 0xffffffffffffffffULL)

#else
/* pci_set_dma_mask is using dma_addr_t */

#define BCM_32BIT_DMA_MASK ((dma_addr_t) 0xffffffff)
#define BCM_64BIT_DMA_MASK ((dma_addr_t) 0xffffffff)

#endif

#else /* (LINUX_VERSION_CODE < 0x02040d) */

#define BCM_32BIT_DMA_MASK ((u64) 0x00000000ffffffffULL)
#define BCM_64BIT_DMA_MASK ((u64) 0xffffffffffffffffULL)
#endif

#if (LINUX_VERSION_CODE < 0x020329)
#define pci_set_dma_mask(pdev, mask) (0)
#else
#if (LINUX_VERSION_CODE < 0x020403)
int
pci_set_dma_mask(struct pci_dev *dev, dma_addr_t mask)
{
    if(! pci_dma_supported(dev, mask))
        return -EIO;

    dev->dma_mask = mask;

    return 0;
}
#endif
#endif

#if (LINUX_VERSION_CODE < 0x020547)
#define pci_set_consistent_dma_mask(pdev, mask) (0)
#endif

#if (LINUX_VERSION_CODE < 0x020402)
#define pci_request_regions(pdev, name) (0)
#define pci_release_regions(pdev)
#endif

#if !defined(spin_is_locked)
#define spin_is_locked(lock)    (test_bit(0,(lock)))
#endif

#define BCM5700_LOCK(pUmDevice, flags)					\
	if ((pUmDevice)->do_global_lock) {				\
		spin_lock_irqsave(&(pUmDevice)->global_lock, flags);	\
	}

#define BCM5700_UNLOCK(pUmDevice, flags)				\
	if ((pUmDevice)->do_global_lock) {				\
		spin_unlock_irqrestore(&(pUmDevice)->global_lock, flags);\
	}

inline void
bcm5700_intr_lock(PUM_DEVICE_BLOCK pUmDevice)
{
	if (pUmDevice->do_global_lock) {
		spin_lock(&pUmDevice->global_lock);
	}
}

inline void
bcm5700_intr_unlock(PUM_DEVICE_BLOCK pUmDevice)
{
	if (pUmDevice->do_global_lock) {
		spin_unlock(&pUmDevice->global_lock);
	}
}

void
bcm5700_intr_off(PUM_DEVICE_BLOCK pUmDevice)
{
	atomic_inc(&pUmDevice->intr_sem);
	LM_DisableInterrupt(&pUmDevice->lm_dev);
#if (LINUX_VERSION_CODE >= 0x2051c)
	synchronize_irq(pUmDevice->dev->irq);
#else
	synchronize_irq();
#endif
	LM_DisableInterrupt(&pUmDevice->lm_dev);
}

void
bcm5700_intr_on(PUM_DEVICE_BLOCK pUmDevice)
{
	if (atomic_dec_and_test(&pUmDevice->intr_sem)) {
		LM_EnableInterrupt(&pUmDevice->lm_dev);
	}
}


int MM_Packet_Desc_Size = sizeof(UM_PACKET);

#if defined(MODULE)
MODULE_AUTHOR("Michael Chan <mchan at broadcom dot com> and Gary Zambrano <zambrano at broadcom dot com>");
MODULE_DESCRIPTION("BCM5700 Driver");
MODULE_LICENSE("GPL");

#if (LINUX_VERSION_CODE < 0x020605)

MODULE_PARM(debug, "i");
MODULE_PARM(msglevel, "i");
MODULE_PARM(activate_gpio, "0-15i");
MODULE_PARM(line_speed, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(auto_speed, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(full_duplex, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(rx_flow_control, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(tx_flow_control, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(auto_flow_control, "1-" __MODULE_STRING(MAX_UNITS) "i");
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
MODULE_PARM(mtu, "1-" __MODULE_STRING(MAX_UNITS) "i");
#endif
MODULE_PARM(tx_checksum, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(rx_checksum, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(scatter_gather, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(tx_pkt_desc_cnt, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(rx_std_desc_cnt, "1-" __MODULE_STRING(MAX_UNITS) "i");
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
MODULE_PARM(rx_jumbo_desc_cnt, "1-" __MODULE_STRING(MAX_UNITS) "i");
#endif
#ifdef BCM_INT_COAL
MODULE_PARM(adaptive_coalesce, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(rx_coalesce_ticks, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(rx_max_coalesce_frames, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(tx_coalesce_ticks, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(tx_max_coalesce_frames, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(stats_coalesce_ticks, "1-" __MODULE_STRING(MAX_UNITS) "i");
#endif
#ifdef BCM_WOL
MODULE_PARM(enable_wol, "1-" __MODULE_STRING(MAX_UNITS) "i");
#endif
#ifdef BCM_TSO
MODULE_PARM(enable_tso, "1-" __MODULE_STRING(MAX_UNITS) "i");
#endif
#ifdef BCM_NIC_SEND_BD
MODULE_PARM(nic_tx_bd, "1-" __MODULE_STRING(MAX_UNITS) "i");
#endif
#ifdef BCM_ASF
MODULE_PARM(vlan_tag_mode, "1-" __MODULE_STRING(MAX_UNITS) "i");
#endif
MODULE_PARM(delay_link, "1-" __MODULE_STRING(MAX_UNITS) "i");
MODULE_PARM(disable_d3hot, "1-" __MODULE_STRING(MAX_UNITS) "i");

#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)
MODULE_PARM(disable_msi, "1-" __MODULE_STRING(MAX_UNITS) "i");
#endif

#else /* parms*/

#if (LINUX_VERSION_CODE >= 0x020605) && (LINUX_VERSION_CODE < 0x02060a)

static int var;

#define numvar var

#endif

#if (LINUX_VERSION_CODE >= 0x2060a)

#define numvar NULL

#endif

module_param_array(line_speed, int, numvar, 0);
module_param_array(auto_speed, int, numvar, 0);
module_param_array(full_duplex, int, numvar, 0);
module_param_array(rx_flow_control, int, numvar, 0);
module_param_array(tx_flow_control, int, numvar, 0);
module_param_array(auto_flow_control, int, numvar, 0);
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
module_param_array(mtu, int, numvar, 0);
#endif
module_param_array(tx_checksum, int, numvar, 0);
module_param_array(rx_checksum, int, numvar, 0);
module_param_array(scatter_gather, int, numvar, 0);
module_param_array(tx_pkt_desc_cnt, int, numvar, 0);
module_param_array(rx_std_desc_cnt, int, numvar, 0);
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
module_param_array(rx_jumbo_desc_cnt, int, numvar, 0);
#endif
#ifdef BCM_INT_COAL
module_param_array(adaptive_coalesce, int, numvar, 0);
module_param_array(rx_coalesce_ticks, int, numvar, 0);
module_param_array(rx_max_coalesce_frames, int, numvar, 0);
module_param_array(tx_coalesce_ticks, int, numvar, 0);
module_param_array(tx_max_coalesce_frames, int, numvar, 0);
module_param_array(stats_coalesce_ticks, int, numvar, 0);
#endif
#ifdef BCM_WOL
module_param_array(enable_wol, int, numvar, 0);
#endif
#ifdef BCM_TSO
module_param_array(enable_tso, int, numvar, 0);
#endif
#ifdef BCM_NIC_SEND_BD
module_param_array(nic_tx_bd, int, numvar, 0);
#endif
#ifdef BCM_ASF
module_param_array(vlan_tag_mode, int, numvar, 0);
#endif
module_param_array(delay_link, int, numvar, 0);
module_param_array(disable_d3hot, int, numvar, 0);

#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)
module_param_array(disable_msi, int, numvar, 0);
#endif


#endif /* params */


#endif

#define RUN_AT(x) (jiffies + (x))

char kernel_version[] = UTS_RELEASE;

#define PCI_SUPPORT_VER2

#if !defined(CAP_NET_ADMIN)
#define capable(CAP_XXX) (suser())
#endif

#define tigon3_debug debug
#if TIGON3_DEBUG
static int tigon3_debug = TIGON3_DEBUG;
#else
static int tigon3_debug = 0;
#endif
static int msglevel = 0xdeadbeef;
int b57_msg_level;

int bcm5700_open(struct net_device *dev);
STATIC void bcm5700_timer(unsigned long data);
STATIC void bcm5700_stats_timer(unsigned long data);
STATIC void bcm5700_reset(struct net_device *dev);
STATIC int bcm5700_start_xmit(struct sk_buff *skb, struct net_device *dev);
STATIC irqreturn_t bcm5700_interrupt(int irq, void *dev_instance, struct pt_regs *regs);
#ifdef BCM_TASKLET
STATIC void bcm5700_tasklet(unsigned long data);
#endif
STATIC int bcm5700_close(struct net_device *dev);
STATIC struct net_device_stats *bcm5700_get_stats(struct net_device *dev);
STATIC int bcm5700_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
STATIC void bcm5700_do_rx_mode(struct net_device *dev);
STATIC void bcm5700_set_rx_mode(struct net_device *dev);
STATIC int bcm5700_set_mac_addr(struct net_device *dev, void *p);
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
STATIC int bcm5700_change_mtu(struct net_device *dev, int new_mtu);
#endif
#ifdef BCM_NAPI_RXPOLL
STATIC int bcm5700_poll(struct net_device *dev, int *budget);
#endif
STATIC int replenish_rx_buffers(PUM_DEVICE_BLOCK pUmDevice, int max);
STATIC int bcm5700_freemem(struct net_device *dev);
#ifdef BCM_INT_COAL
#ifndef BCM_NAPI_RXPOLL
STATIC int bcm5700_adapt_coalesce(PUM_DEVICE_BLOCK pUmDevice);
#endif
#endif
STATIC void bcm5700_set_vlan_mode(UM_DEVICE_BLOCK *pUmDevice);
STATIC int bcm5700_init_counters(PUM_DEVICE_BLOCK pUmDevice);
#ifdef BCM_VLAN
STATIC void bcm5700_vlan_rx_register(struct net_device *dev, struct vlan_group *vlgrp);
STATIC void bcm5700_vlan_rx_kill_vid(struct net_device *dev, uint16_t vid);
#endif
void bcm5700_shutdown(UM_DEVICE_BLOCK *pUmDevice);
void bcm5700_free_remaining_rx_bufs(UM_DEVICE_BLOCK *pUmDevice);
void bcm5700_validate_param_range(UM_DEVICE_BLOCK *pUmDevice, int *param,
	char *param_name, int min, int max, int deflt);

static int bcm5700_notify_reboot(struct notifier_block *this, unsigned long event, void *unused);
static struct notifier_block bcm5700_reboot_notifier = {
	bcm5700_notify_reboot,
	NULL,
	0
};

#if defined(HAVE_POLL_CONTROLLER) || defined(CONFIG_NET_POLL_CONTROLLER)
STATIC void poll_bcm5700(struct net_device *dev);
#endif

/* A list of all installed bcm5700 devices. */
static struct net_device *root_tigon3_dev = NULL;

#if defined(CONFIG_SPARC64) || defined(CONFIG_X86_64) ||defined(CONFIG_PPC64)

#endif

typedef enum {
	BCM5700A6 = 0,
	BCM5700T6,
	BCM5700A9,
	BCM5700T9,
	BCM5700,
	BCM5701A5,
	BCM5701T1,
	BCM5701T8,
	BCM5701A7,
	BCM5701A10,
	BCM5701A12,
	BCM5701,
	BCM5702,
	BCM5703,
	BCM5703A31,
	BCM5703ARBUCKLE,
	TC996T,
	TC996ST,
	TC996SSX,
	TC996SX,
	TC996BT,
	TC997T,
	TC997SX,
	TC1000T,
	TC1000BT,
	TC940BR01,
	TC942BR01,
	TC998T,
	TC998SX,
	TC999T,
	NC6770,
	NC1020,
	NC150T,
	NC7760,
	NC7761,
	NC7770,
	NC7771,
	NC7780,
	NC7781,
	NC7772,
	NC7782,
	NC7783,
	NC320T,
	NC320I,
	NC325I,
	NC324I,
	NC326I,
	BCM5704CIOBE,
	BCM5704,
	BCM5704S,
	BCM5705,
	BCM5705M,
	BCM5705F,
	BCM5901,
	BCM5782,
	BCM5788,
	BCM5789,
	BCM5750,
	BCM5750M,
	BCM5720,
	BCM5751,
	BCM5751M,
	BCM5751F,
	BCM5721,
	BCM5753,
	BCM5753M,
	BCM5753F,
	BCM5781,
	BCM5752,
	BCM5752M,
	BCM5714,
	BCM5780,
	BCM5780S,
	BCM5715,
	BCM4785,
	BCM5903M,
	UNK5788
} board_t;


/* indexed by board_t, above */
static struct {
	char *name;
} board_info[] __devinitdata = {
	{ "Broadcom BCM5700 1000Base-T" },
	{ "Broadcom BCM5700 1000Base-SX" },
	{ "Broadcom BCM5700 1000Base-SX" },
	{ "Broadcom BCM5700 1000Base-T" },
	{ "Broadcom BCM5700" },
	{ "Broadcom BCM5701 1000Base-T" },
	{ "Broadcom BCM5701 1000Base-T" },
	{ "Broadcom BCM5701 1000Base-T" },
	{ "Broadcom BCM5701 1000Base-SX" },
	{ "Broadcom BCM5701 1000Base-T" },
	{ "Broadcom BCM5701 1000Base-T" },
	{ "Broadcom BCM5701" },
	{ "Broadcom BCM5702 1000Base-T" },
	{ "Broadcom BCM5703 1000Base-T" },
	{ "Broadcom BCM5703 1000Base-SX" },
	{ "Broadcom B5703 1000Base-SX" },
	{ "3Com 3C996 10/100/1000 Server NIC" },
	{ "3Com 3C996 10/100/1000 Server NIC" },
	{ "3Com 3C996 Gigabit Fiber-SX Server NIC" },
	{ "3Com 3C996 Gigabit Fiber-SX Server NIC" },
	{ "3Com 3C996B Gigabit Server NIC" },
	{ "3Com 3C997 Gigabit Server NIC" },
	{ "3Com 3C997 Gigabit Fiber-SX Server NIC" },
	{ "3Com 3C1000 Gigabit NIC" },
	{ "3Com 3C1000B-T 10/100/1000 PCI" },
	{ "3Com 3C940 Gigabit LOM (21X21)" },
	{ "3Com 3C942 Gigabit LOM (31X31)" },
	{ "3Com 3C998-T Dual Port 10/100/1000 PCI-X Server NIC" },
	{ "3Com 3C998-SX Dual Port 1000-SX PCI-X Server NIC" },
	{ "3Com 3C999-T Quad Port 10/100/1000 PCI-X Server NIC" },
	{ "HP NC6770 Gigabit Server Adapter" },
	{ "NC1020 HP ProLiant Gigabit Server Adapter 32 PCI" },
	{ "HP ProLiant NC 150T PCI 4-port Gigabit Combo Switch Adapter" },
	{ "HP NC7760 Gigabit Server Adapter" },
	{ "HP NC7761 Gigabit Server Adapter" },
	{ "HP NC7770 Gigabit Server Adapter" },
	{ "HP NC7771 Gigabit Server Adapter" },
	{ "HP NC7780 Gigabit Server Adapter" },
	{ "HP NC7781 Gigabit Server Adapter" },
	{ "HP NC7772 Gigabit Server Adapter" },
	{ "HP NC7782 Gigabit Server Adapter" },
	{ "HP NC7783 Gigabit Server Adapter" },
	{ "HP ProLiant NC 320T PCI Express Gigabit Server Adapter" },
	{ "HP ProLiant NC 320i PCI Express Gigabit Server Adapter" },
	{ "HP NC325i Integrated Dual Port PCI Express Gigabit Server Adapter" },
	{ "HP NC324i Integrated Dual Port PCI Express Gigabit Server Adapter" },
	{ "HP NC326i Integrated Dual Port PCI Express Gigabit Server Adapter" },
	{ "Broadcom BCM5704 CIOB-E 1000Base-T" },
	{ "Broadcom BCM5704 1000Base-T" },
	{ "Broadcom BCM5704 1000Base-SX" },
	{ "Broadcom BCM5705 1000Base-T" },
	{ "Broadcom BCM5705M 1000Base-T" },
	{ "Broadcom 570x 10/100 Integrated Controller" },
	{ "Broadcom BCM5901 100Base-TX" },
	{ "Broadcom NetXtreme Gigabit Ethernet for hp" },
	{ "Broadcom BCM5788 NetLink 1000Base-T" },
	{ "Broadcom BCM5789 NetLink 1000Base-T PCI Express" },
	{ "Broadcom BCM5750 1000Base-T PCI" },
	{ "Broadcom BCM5750M 1000Base-T PCI" },
	{ "Broadcom BCM5720 1000Base-T PCI" },
	{ "Broadcom BCM5751 1000Base-T PCI Express" },
	{ "Broadcom BCM5751M 1000Base-T PCI Express" },
	{ "Broadcom BCM5751F 100Base-TX PCI Express" },
	{ "Broadcom BCM5721 1000Base-T PCI Express" },
	{ "Broadcom BCM5753 1000Base-T PCI Express" },
	{ "Broadcom BCM5753M 1000Base-T PCI Express" },
	{ "Broadcom BCM5753F 100Base-TX PCI Express" },
	{ "Broadcom BCM5781 NetLink 1000Base-T PCI Express" },
	{ "Broadcom BCM5752 1000Base-T PCI Express" },
	{ "Broadcom BCM5752M 1000Base-T PCI Express" },
	{ "Broadcom BCM5714 1000Base-T " },
	{ "Broadcom BCM5780 1000Base-T" },
	{ "Broadcom BCM5780S 1000Base-SX" },
	{ "Broadcom BCM5715 1000Base-T " },
	{ "Broadcom BCM4785 10/100/1000 Integrated Controller" },
	{ "Broadcom BCM5903M Gigabit Ethernet " },
	{ "Unknown BCM5788 Gigabit Ethernet " },
	{ 0 }
	};

static struct pci_device_id bcm5700_pci_tbl[] __devinitdata = {
	{0x14e4, 0x1644, 0x14e4, 0x1644, 0, 0, BCM5700A6 },
	{0x14e4, 0x1644, 0x14e4, 0x2, 0, 0, BCM5700T6 },
	{0x14e4, 0x1644, 0x14e4, 0x3, 0, 0, BCM5700A9 },
	{0x14e4, 0x1644, 0x14e4, 0x4, 0, 0, BCM5700T9 },
	{0x14e4, 0x1644, 0x1028, 0xd1, 0, 0, BCM5700 },
	{0x14e4, 0x1644, 0x1028, 0x0106, 0, 0, BCM5700 },
	{0x14e4, 0x1644, 0x1028, 0x0109, 0, 0, BCM5700 },
	{0x14e4, 0x1644, 0x1028, 0x010a, 0, 0, BCM5700 },
	{0x14e4, 0x1644, 0x10b7, 0x1000, 0, 0, TC996T },
	{0x14e4, 0x1644, 0x10b7, 0x1001, 0, 0, TC996ST },
	{0x14e4, 0x1644, 0x10b7, 0x1002, 0, 0, TC996SSX },
	{0x14e4, 0x1644, 0x10b7, 0x1003, 0, 0, TC997T },
	{0x14e4, 0x1644, 0x10b7, 0x1005, 0, 0, TC997SX },
	{0x14e4, 0x1644, 0x10b7, 0x1008, 0, 0, TC942BR01 },
	{0x14e4, 0x1644, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5700 },
	{0x14e4, 0x1645, 0x14e4, 1, 0, 0, BCM5701A5 },
	{0x14e4, 0x1645, 0x14e4, 5, 0, 0, BCM5701T1 },
	{0x14e4, 0x1645, 0x14e4, 6, 0, 0, BCM5701T8 },
	{0x14e4, 0x1645, 0x14e4, 7, 0, 0, BCM5701A7 },
	{0x14e4, 0x1645, 0x14e4, 8, 0, 0, BCM5701A10 },
	{0x14e4, 0x1645, 0x14e4, 0x8008, 0, 0, BCM5701A12 },
	{0x14e4, 0x1645, 0x0e11, 0xc1, 0, 0, NC6770 },
	{0x14e4, 0x1645, 0x0e11, 0x7c, 0, 0, NC7770 },
	{0x14e4, 0x1645, 0x0e11, 0x85, 0, 0, NC7780 },
	{0x14e4, 0x1645, 0x1028, 0x0121, 0, 0, BCM5701 },
	{0x14e4, 0x1645, 0x10b7, 0x1004, 0, 0, TC996SX },
	{0x14e4, 0x1645, 0x10b7, 0x1006, 0, 0, TC996BT },
	{0x14e4, 0x1645, 0x10b7, 0x1007, 0, 0, TC1000T },
	{0x14e4, 0x1645, 0x10b7, 0x1008, 0, 0, TC940BR01 },
	{0x14e4, 0x1645, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5701 },
	{0x14e4, 0x1646, 0x14e4, 0x8009, 0, 0, BCM5702 },
	{0x14e4, 0x1646, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5702 },
	{0x14e4, 0x16a6, 0x14e4, 0x8009, 0, 0, BCM5702 },
	{0x14e4, 0x16a6, 0x14e4, 0x000c, 0, 0, BCM5702 },
	{0x14e4, 0x16a6, 0x0e11, 0xbb, 0, 0, NC7760 },
	{0x14e4, 0x16a6, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5702 },
	{0x14e4, 0x16c6, 0x10b7, 0x1100, 0, 0, TC1000BT },
	{0x14e4, 0x16c6, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5702 },
	{0x14e4, 0x1647, 0x14e4, 0x0009, 0, 0, BCM5703 },
	{0x14e4, 0x1647, 0x14e4, 0x000a, 0, 0, BCM5703A31 },
	{0x14e4, 0x1647, 0x14e4, 0x000b, 0, 0, BCM5703 },
	{0x14e4, 0x1647, 0x14e4, 0x800a, 0, 0, BCM5703 },
	{0x14e4, 0x1647, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5703 },
	{0x14e4, 0x16a7, 0x14e4, 0x0009, 0, 0, BCM5703 },
	{0x14e4, 0x16a7, 0x14e4, 0x000a, 0, 0, BCM5703A31 },
	{0x14e4, 0x16a7, 0x14e4, 0x000b, 0, 0, BCM5703 },
	{0x14e4, 0x16a7, 0x14e4, 0x800a, 0, 0, BCM5703 },
	{0x14e4, 0x16a7, 0x0e11, 0xca, 0, 0, NC7771 },
	{0x14e4, 0x16a7, 0x0e11, 0xcb, 0, 0, NC7781 },
	{0x14e4, 0x16a7, 0x1014, 0x0281, 0, 0, BCM5703ARBUCKLE },
	{0x14e4, 0x16a7, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5703 },
	{0x14e4, 0x16c7, 0x14e4, 0x000a, 0, 0, BCM5703A31 },
	{0x14e4, 0x16c7, 0x0e11, 0xca, 0, 0, NC7771 },
	{0x14e4, 0x16c7, 0x0e11, 0xcb, 0, 0, NC7781 },
	{0x14e4, 0x16c7, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5703 },
	{0x14e4, 0x1648, 0x0e11, 0xcf, 0, 0, NC7772 },
	{0x14e4, 0x1648, 0x0e11, 0xd0, 0, 0, NC7782 },
	{0x14e4, 0x1648, 0x0e11, 0xd1, 0, 0, NC7783 },
	{0x14e4, 0x1648, 0x10b7, 0x2000, 0, 0, TC998T },
	{0x14e4, 0x1648, 0x10b7, 0x3000, 0, 0, TC999T },
	{0x14e4, 0x1648, 0x1166, 0x1648, 0, 0, BCM5704CIOBE },
	{0x14e4, 0x1648, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5704 },
	{0x14e4, 0x1649, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5704S },
	{0x14e4, 0x16a8, 0x14e4, 0x16a8, 0, 0, BCM5704S },
	{0x14e4, 0x16a8, 0x10b7, 0x2001, 0, 0, TC998SX },
	{0x14e4, 0x16a8, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5704S },
	{0x14e4, 0x1653, 0x0e11, 0x00e3, 0, 0, NC7761 },
	{0x14e4, 0x1653, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5705 },
	{0x14e4, 0x1654, 0x0e11, 0x00e3, 0, 0, NC7761 },
	{0x14e4, 0x1654, 0x103c, 0x3100, 0, 0, NC1020 },
	{0x14e4, 0x1654, 0x103c, 0x3226, 0, 0, NC150T },
	{0x14e4, 0x1654, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5705 },
	{0x14e4, 0x165d, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5705M },
	{0x14e4, 0x165e, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5705M },
	{0x14e4, 0x166e, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5705F },
	{0x14e4, 0x1696, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5782 },
	{0x14e4, 0x169c, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5788 },
	{0x14e4, 0x169d, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5789 },
	{0x14e4, 0x170d, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5901 },
	{0x14e4, 0x170e, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5901 },
	{0x14e4, 0x1676, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5750 },
	{0x14e4, 0x167c, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5750M },
	{0x14e4, 0x1677, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5751 },
	{0x14e4, 0x167d, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5751M },
	{0x14e4, 0x167e, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5751F },
	{0x14e4, 0x1658, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5720 },
	{0x14e4, 0x1659, 0x103c, 0x7031, 0, 0, NC320T },
	{0x14e4, 0x1659, 0x103c, 0x7032, 0, 0, NC320T },
	{0x14e4, 0x166a, 0x103c, 0x7035, 0, 0, NC325I },
	{0x14e4, 0x166b, 0x103c, 0x7036, 0, 0, NC325I },
	{0x14e4, 0x1668, 0x103c, 0x7039, 0, 0, NC324I },
	{0x14e4, 0x1669, 0x103c, 0x703a, 0, 0, NC324I },
	{0x14e4, 0x1678, 0x103c, 0x703e, 0, 0, NC326I },
	{0x14e4, 0x1679, 0x103c, 0x703c, 0, 0, NC326I },
	{0x14e4, 0x1659, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5721 },
	{0x14e4, 0x16f7, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5753 },
	{0x14e4, 0x16fd, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5753M },
	{0x14e4, 0x16fe, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5753F },
	{0x14e4, 0x16dd, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5781 },
	{0x14e4, 0x1600, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5752 },
	{0x14e4, 0x1601, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5752M },
	{0x14e4, 0x1668, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5714 },
	{0x14e4, 0x166a, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5780 },
	{0x14e4, 0x166b, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5780S },
	{0x14e4, 0x1678, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5715 },
	{0x14e4, 0x471f, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM4785 },
	{0x14e4, 0x16ff, PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5903M },
	{0x173b, 0x03ed, PCI_ANY_ID, PCI_ANY_ID, 0, 0, UNK5788 },
	{0,}
	};

MODULE_DEVICE_TABLE(pci, bcm5700_pci_tbl);

#if (LINUX_VERSION_CODE >= 0x2060a)
	static struct pci_device_id pci_AMD762id[]={
		{ PCI_DEVICE(PCI_VENDOR_ID_AMD,
			PCI_DEVICE_ID_AMD_FE_GATE_700C) },
		{ }
	};
#endif

static int sbgige = -1;

/*******************************************************************************
 *******************************************************************************
*/

int get_csum_flag(LM_UINT32 ChipRevId)
{
        return NETIF_F_IP_CSUM;
}

/*******************************************************************************
 *******************************************************************************

   This function returns true if the device passed to it is attached to an
   ICH-ICH4. If the chip is not attached to an ICH, or is attached to an ICH5
   or newer, it returns false.

   This function determines which bridge it is attached to by scaning the pci
   bus looking for bridge chips (hdr_type=1). When a bridge chip is detected,
   the bridge's subordinate's secondary bus number is compared with this
   devices bus number. If they match, then the device is attached to this
   bridge. The bridge's device id is compared to a list of known device ids for
   ICH-ICH4. Since many older ICH's (ICH2-ICH7) share the same device id, the
   chip revision must also be checked to determine if the chip is older than an
   ICH5.

   To scan the bus, one of two functions is used depending on the kernel
   version. For 2.4 kernels, the pci_find_device function is used. This
   function has been depricated in the 2.6 kernel and replaced with the
   fucntion pci_get_device. The macro walk_pci_bus determines which function to
   use when the driver is built.
*/

#if (LINUX_VERSION_CODE >= 0x2060a)
#define walk_pci_bus(d)		while ((d = pci_get_device( \
					PCI_ANY_ID, PCI_ANY_ID, d)) != NULL)

#define unwalk_pci_bus(d)	pci_dev_put(d)

#else
#define walk_pci_bus(d)		while ((d = pci_find_device( \
					PCI_ANY_ID, PCI_ANY_ID, d)) != NULL)
#define unwalk_pci_bus(d)

#endif

#define ICH5_CHIP_VERSION	0xc0

static struct pci_device_id pci_ICHtable[] = {
	{0x8086, 0x2418}, /* PCI_DEVICE_ID_INTEL_82801AA_8  */
	{0x8086, 0x2428}, /* PCI_DEVICE_ID_INTEL_82801AB_8  */
	{0x8086, 0x244e}, /* PCI_DEVICE_ID_INTEL_82801BA_6  */
	{0x8086, 0x2448}, /* PCI_DEVICE_ID_INTEL_82801BA_11 */
	{0, 0}
};

int attached_to_ICH4_or_older( struct pci_dev *pdev)
{
	struct pci_dev *tmp_pdev = NULL;
	struct pci_device_id *ich_table;
	u8 chip_rev;

	walk_pci_bus (tmp_pdev) {
		if ((tmp_pdev->hdr_type == 1) &&
		   (tmp_pdev->subordinate != NULL) &&
		   (tmp_pdev->subordinate->secondary == pdev->bus->number)) {

			ich_table = pci_ICHtable;

			while (ich_table->vendor) {
				if ((ich_table->vendor == tmp_pdev->vendor) &&
				    (ich_table->device == tmp_pdev->device)) {

					pci_read_config_byte( tmp_pdev,
						PCI_REVISION_ID, &chip_rev);

					if (chip_rev < ICH5_CHIP_VERSION) {
						unwalk_pci_bus( tmp_pdev);
						return 1;
					}
				}
				ich_table++;
			}
		}
	}
	return 0;
}

static void robo_set_power_mode(void *h)
{
	//int status = 0;
	int i;
	//uint8 mode8;
	//uint16 mode16;
	uint32 flags = 0, temp32 = 0,val32 = 0, savephyaddr = 0;
	PUM_DEVICE_BLOCK pudev = (PUM_DEVICE_BLOCK)h;
	PLM_DEVICE_BLOCK pdev = &pudev->lm_dev;

	/*Brcm,Alex,2006.7.20. Adding Phy power mode setting*/
	BCM5700_PHY_LOCK(pudev, flags);
	savephyaddr = pdev->PhyAddr;

	for(i = 0; i < 8; i++)
	{
		pdev->PhyAddr = i;
		temp32 = 0x2007;
		LM_WritePhy(pdev, 0x18, temp32);
		LM_ReadPhy(pdev, 0x18, &val32);
//		printk(KERN_DEBUG "Alex: port = %x, read value =%x\n",i, val32);
		temp32 = 0xc042;
		LM_WritePhy(pdev, 0x18, temp32);
		/*Read back*/
		temp32 = 0x2007;
		val32 = 0;
		LM_WritePhy(pdev, 0x18, temp32);
		LM_ReadPhy(pdev, 0x18, &val32);
//		printk(KERN_ERR "Alex: read back value =%x\n",val32);
	}

	pdev->PhyAddr = savephyaddr;
	BCM5700_PHY_UNLOCK(pudev, flags);

	/*end of Brcm,Alex,2006.7.20. Adding Phy power mode setting*/

}

static int
__devinit bcm5700_init_board(struct pci_dev *pdev, struct net_device **dev_out, int board_idx)
{
	struct net_device *dev;
	PUM_DEVICE_BLOCK pUmDevice;
	PLM_DEVICE_BLOCK pDevice;
	bool rgmii = FALSE;
	sb_t *sbh = NULL;
	int rc;

	*dev_out = NULL;

	/* dev zeroed in init_etherdev */
#if (LINUX_VERSION_CODE >= 0x20600)
	dev = alloc_etherdev(sizeof(*pUmDevice));
#else
	dev = init_etherdev(NULL, sizeof(*pUmDevice));
#endif
	if (dev == NULL) {
		printk(KERN_ERR "%s: unable to alloc new ethernet\n", bcm5700_driver);
		return -ENOMEM;
	}
	SET_MODULE_OWNER(dev);
#if (LINUX_VERSION_CODE >= 0x20600)
	SET_NETDEV_DEV(dev, &pdev->dev);
#endif
	pUmDevice = (PUM_DEVICE_BLOCK) dev->priv;

	/* enable device (incl. PCI PM wakeup), and bus-mastering */
	rc = pci_enable_device(pdev);
	if (rc)
		goto err_out;

	/* init core specific stuff */
	if (pdev->device == T3_PCI_DEVICE_ID(T3_PCI_ID_BCM471F)) {
		sbh = sb_kattach(SB_OSH);
		sb_gige_init(sbh, ++sbgige, &rgmii);
	}

	rc = pci_request_regions(pdev, bcm5700_driver);
	if (rc) {
		if (!sbh)
			goto err_out;
		printk(KERN_INFO "bcm5700_init_board: pci_request_regions returned error %d\n"
				 "This may be because the region is already requested by"
				 " the SMBus driver. Ignore the PCI error messages.\n", rc);
	}

	pci_set_master(pdev);

	if (pci_set_dma_mask(pdev, BCM_64BIT_DMA_MASK) == 0) {
		pUmDevice->using_dac = 1;
		if (pci_set_consistent_dma_mask(pdev, BCM_64BIT_DMA_MASK) != 0) {
			printk(KERN_ERR "pci_set_consistent_dma_mask failed\n");
			pci_release_regions(pdev);
			goto err_out;
		}
	} else if (pci_set_dma_mask(pdev, BCM_32BIT_DMA_MASK) == 0) {
		pUmDevice->using_dac = 0;
	} else {
		printk(KERN_ERR "System does not support DMA\n");
		pci_release_regions(pdev);
		goto err_out;
	}

	pUmDevice->dev = dev;
	pUmDevice->pdev = pdev;
	pUmDevice->mem_list_num = 0;
	pUmDevice->next_module = root_tigon3_dev;
	pUmDevice->index = board_idx;
	pUmDevice->sbh = (void *)sbh;
	root_tigon3_dev = dev;

	spin_lock_init(&pUmDevice->global_lock);

	spin_lock_init(&pUmDevice->undi_lock);

	spin_lock_init(&pUmDevice->phy_lock);

	pDevice = &pUmDevice->lm_dev;
	pDevice->Flags = 0;
	pDevice->FunctNum = PCI_FUNC(pUmDevice->pdev->devfn);
	pUmDevice->boardflags = getintvar(NULL, "boardflags");
	if (sbh) {
		if (pUmDevice->boardflags & BFL_ENETROBO)
			pDevice->Flags |= ROBO_SWITCH_FLAG;
		pDevice->Flags |= rgmii ? RGMII_MODE_FLAG : 0;
		if (sb_chip(sbh) == BCM4785_CHIP_ID && sb_chiprev(sbh) < 2)
			pDevice->Flags |= ONE_DMA_AT_ONCE_FLAG;
		pDevice->Flags |= SB_CORE_FLAG;
		if (sb_chip(sbh) == BCM4785_CHIP_ID)
			pDevice->Flags |= FLUSH_POSTED_WRITE_FLAG;
	}

#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
	if (board_idx < MAX_UNITS) {
		bcm5700_validate_param_range(pUmDevice, &mtu[board_idx], "mtu", 1500, 9000, 1500);
		dev->mtu = mtu[board_idx];
	}
#endif

	if (attached_to_ICH4_or_older(pdev)) {
		pDevice->Flags |= UNDI_FIX_FLAG;
	}

#if (LINUX_VERSION_CODE >= 0x2060a)
	if (pci_dev_present(pci_AMD762id)) {
		pDevice->Flags |= FLUSH_POSTED_WRITE_FLAG;
		pDevice->Flags &= ~NIC_SEND_BD_FLAG;
	}
#else
	if (pci_find_device(0x1022, 0x700c, NULL)) {
		/* AMD762 writes I/O out of order */
		/* Setting bit 1 in 762's register 0x4C still doesn't work */
		/* in all cases */
		pDevice->Flags |= FLUSH_POSTED_WRITE_FLAG;
		pDevice->Flags &= ~NIC_SEND_BD_FLAG;
	}
#endif
	if (LM_GetAdapterInfo(pDevice) != LM_STATUS_SUCCESS) {
		rc = -ENODEV;
		goto err_out_unmap;
	}

	if (pDevice->Flags & ROBO_SWITCH_FLAG) {
		robo_info_t	*robo;

		if ((robo = bcm_robo_attach(sbh, pDevice, dev->name, NULL,
		                            robo_miird, robo_miiwr)) == NULL) {
			B57_ERR(("robo_setup: failed to attach robo switch \n"));
			goto robo_fail;
		}

		if (bcm_robo_enable_device(robo)) {
			B57_ERR(("robo_setup: failed to enable robo switch \n"));
robo_fail:
			bcm_robo_detach(robo);
			rc = -ENODEV;
			goto err_out_unmap;
		}

		/* 5397 power mode setting */
		robo_set_power_mode(robo->h);

		pUmDevice->robo = (void *)robo;
	}

	if ((pDevice->Flags & JUMBO_CAPABLE_FLAG) == 0) {
		if (dev->mtu > 1500) {
			dev->mtu = 1500;
			printk(KERN_WARNING
			       "%s-%d: Jumbo mtu sizes not supported, using mtu=1500\n",
			       bcm5700_driver, pUmDevice->index);
		}
	}

	pUmDevice->do_global_lock = 0;
	if (T3_ASIC_REV(pUmDevice->lm_dev.ChipRevId) == T3_ASIC_REV_5700) {
		/* The 5700 chip works best without interleaved register */
		/* accesses on certain machines. */
		pUmDevice->do_global_lock = 1;
	}

	if ((T3_ASIC_REV(pUmDevice->lm_dev.ChipRevId) == T3_ASIC_REV_5701) &&
		((pDevice->PciState & T3_PCI_STATE_NOT_PCI_X_BUS) == 0)) {

		pUmDevice->rx_buf_align = 0;
	} else {
		pUmDevice->rx_buf_align = 2;
	}
	dev->mem_start = pci_resource_start(pdev, 0);
	dev->mem_end = dev->mem_start + sizeof(T3_STD_MEM_MAP);
	dev->irq = pdev->irq;

	*dev_out = dev;
	return 0;

err_out_unmap:
	pci_release_regions(pdev);
	bcm5700_freemem(dev);

err_out:
#if (LINUX_VERSION_CODE < 0x020600)
	unregister_netdev(dev);
	kfree(dev);
#else
	free_netdev(dev);
#endif
	return rc;
}

static int __devinit
bcm5700_print_ver(void)
{
	printk(KERN_INFO "Broadcom Gigabit Ethernet Driver %s ",
		bcm5700_driver);
	printk("ver. %s %s\n", bcm5700_version, bcm5700_date);
	return 0;
}

static int __devinit
bcm5700_init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct net_device *dev = NULL;
	PUM_DEVICE_BLOCK pUmDevice;
	PLM_DEVICE_BLOCK pDevice;
	int i;
	static int board_idx = -1;
	static int printed_version = 0;
	struct pci_dev *pci_dev;

	board_idx++;

	if (!printed_version) {
		bcm5700_print_ver();
		printed_version = 1;
	}

	i = bcm5700_init_board(pdev, &dev, board_idx);
	if (i < 0) {
		return i;
	}

	if (dev == NULL)
		return -ENOMEM;

#ifdef BCM_IOCTL32
	if (atomic_read(&bcm5700_load_count) == 0) {
		register_ioctl32_conversion(SIOCNICE, bcm5700_ioctl32);
	}
	atomic_inc(&bcm5700_load_count);
#endif
	dev->open = bcm5700_open;
	dev->hard_start_xmit = bcm5700_start_xmit;
	dev->stop = bcm5700_close;
	dev->get_stats = bcm5700_get_stats;
	dev->set_multicast_list = bcm5700_set_rx_mode;
	dev->do_ioctl = bcm5700_ioctl;
	dev->set_mac_address = &bcm5700_set_mac_addr;
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
	dev->change_mtu = &bcm5700_change_mtu;
#endif
#if (LINUX_VERSION_CODE >= 0x20400)
	dev->tx_timeout = bcm5700_reset;
	dev->watchdog_timeo = TX_TIMEOUT;
#endif
#ifdef BCM_VLAN
	dev->vlan_rx_register = &bcm5700_vlan_rx_register;
	dev->vlan_rx_kill_vid = &bcm5700_vlan_rx_kill_vid;
#endif
#ifdef BCM_NAPI_RXPOLL
	dev->poll = bcm5700_poll;
	dev->weight = 64;
#endif

	pUmDevice = (PUM_DEVICE_BLOCK) dev->priv;
	pDevice = (PLM_DEVICE_BLOCK) pUmDevice;

	dev->base_addr = pci_resource_start(pdev, 0);
	dev->irq = pdev->irq;
#if defined(HAVE_POLL_CONTROLLER) || defined(CONFIG_NET_POLL_CONTROLLER)
	dev->poll_controller = poll_bcm5700;
#endif

#if (LINUX_VERSION_CODE >= 0x20600)
	if ((i = register_netdev(dev))) {
		printk(KERN_ERR "%s: Cannot register net device\n",
			bcm5700_driver);
		if (pUmDevice->lm_dev.pMappedMemBase)
			iounmap(pUmDevice->lm_dev.pMappedMemBase);
		pci_release_regions(pdev);
		bcm5700_freemem(dev);
		free_netdev(dev);
		return i;
	}
#endif


	pci_set_drvdata(pdev, dev);

	memcpy(dev->dev_addr, pDevice->NodeAddress, 6);
	pUmDevice->name = board_info[ent->driver_data].name,
	printk(KERN_INFO "%s: %s found at mem %lx, IRQ %d, ",
		dev->name, pUmDevice->name, dev->base_addr,
		dev->irq);
	printk("node addr ");
	for (i = 0; i < 6; i++) {
		printk("%2.2x", dev->dev_addr[i]);
	}
	printk("\n");

	printk(KERN_INFO "%s: ", dev->name);
	if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5400_PHY_ID)
		printk("Broadcom BCM5400 Copper ");
	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5401_PHY_ID)
		printk("Broadcom BCM5401 Copper ");
	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5411_PHY_ID)
		printk("Broadcom BCM5411 Copper ");
	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5461_PHY_ID)
		printk("Broadcom BCM5461 Copper ");
	else if (((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5701_PHY_ID) &&
		!(pDevice->TbiFlags & ENABLE_TBI_FLAG)) {
		printk("Broadcom BCM5701 Integrated Copper ");
	}
	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5703_PHY_ID) {
		printk("Broadcom BCM5703 Integrated ");
		if (pDevice->TbiFlags & ENABLE_TBI_FLAG)
			printk("SerDes ");
		else
			printk("Copper ");
	}
	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5704_PHY_ID) {
		printk("Broadcom BCM5704 Integrated ");
		if (pDevice->TbiFlags & ENABLE_TBI_FLAG)
			printk("SerDes ");
		else
			printk("Copper ");
	}
        else if (pDevice->PhyFlags & PHY_IS_FIBER){
            if(( pDevice->PhyId & PHY_ID_MASK ) == PHY_BCM5780_PHY_ID)
                printk("Broadcom BCM5780S Integrated Serdes ");

        }        
	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5705_PHY_ID)
		printk("Broadcom BCM5705 Integrated Copper ");
	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5750_PHY_ID)
		printk("Broadcom BCM5750 Integrated Copper ");

        else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5714_PHY_ID)
                printk("Broadcom BCM5714 Integrated Copper ");
        else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5780_PHY_ID)
                printk("Broadcom BCM5780 Integrated Copper ");

	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM5752_PHY_ID)
		printk("Broadcom BCM5752 Integrated Copper ");
	else if ((pDevice->PhyId & PHY_ID_MASK) == PHY_BCM8002_PHY_ID)
		printk("Broadcom BCM8002 SerDes ");
	else if (pDevice->TbiFlags & ENABLE_TBI_FLAG) {
		if (T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5703) {
			printk("Broadcom BCM5703 Integrated SerDes ");
		}
		else if (T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5704) {
			printk("Broadcom BCM5704 Integrated SerDes ");
		}
		else {
			printk("Agilent HDMP-1636 SerDes ");
		}
	}
	else {
		printk("Unknown ");
	}
	printk("transceiver found\n");

#if (LINUX_VERSION_CODE >= 0x20400)
	if (scatter_gather[board_idx]) {
		dev->features |= NETIF_F_SG;
		if (pUmDevice->using_dac && !(pDevice->Flags & BCM5788_FLAG))
			dev->features |= NETIF_F_HIGHDMA;
	}
	if ((pDevice->TaskOffloadCap & LM_TASK_OFFLOAD_TX_TCP_CHECKSUM) &&
		tx_checksum[board_idx]) {

		dev->features |= get_csum_flag( pDevice->ChipRevId);
	}
#ifdef BCM_VLAN
	dev->features |= NETIF_F_HW_VLAN_TX | NETIF_F_HW_VLAN_RX;
#endif
#ifdef BCM_TSO
	/* On 5714/15/80 chips, Jumbo Frames and TSO cannot both be enabled at
	   the same time. Since only one of these features can be enable at a
           time, we'll enable only Jumbo Frames and disable TSO when the user
	   tries to enable both.
	*/
	dev->features &= ~NETIF_F_TSO;

	if ((pDevice->TaskToOffload & LM_TASK_OFFLOAD_TCP_SEGMENTATION) &&
	    (enable_tso[board_idx])) {
		if (T3_ASIC_5714_FAMILY(pDevice->ChipRevId) &&
		   (dev->mtu > 1500)) {
			printk(KERN_ALERT "%s: Jumbo Frames and TSO cannot simultaneously be enabled. Jumbo Frames enabled. TSO disabled.\n", dev->name);
		} else {
			dev->features |= NETIF_F_TSO;
		}
	}
#endif
	printk(KERN_INFO "%s: Scatter-gather %s, 64-bit DMA %s, Tx Checksum %s, ",
			dev->name,
			(char *) ((dev->features & NETIF_F_SG) ? "ON" : "OFF"),
			(char *) ((dev->features & NETIF_F_HIGHDMA) ? "ON" : "OFF"),
			(char *) ((dev->features & get_csum_flag( pDevice->ChipRevId)) ? "ON" : "OFF"));
#endif
	if ((pDevice->ChipRevId != T3_CHIP_ID_5700_B0) &&
		rx_checksum[board_idx])
		printk("Rx Checksum ON");
	else
		printk("Rx Checksum OFF");
#ifdef BCM_VLAN
	printk(", 802.1Q VLAN ON");
#endif
#ifdef BCM_TSO
	if (dev->features & NETIF_F_TSO) {
		printk(", TSO ON");
	}
	else
#endif
#ifdef BCM_NAPI_RXPOLL
	printk(", NAPI ON");
#endif
	printk("\n");

	register_reboot_notifier(&bcm5700_reboot_notifier);
#ifdef BCM_TASKLET
	tasklet_init(&pUmDevice->tasklet, bcm5700_tasklet,
		(unsigned long) pUmDevice);
#endif
	if (T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5704) {
		if ((REG_RD(pDevice, PciCfg.DualMacCtrl) &
			T3_DUAL_MAC_CH_CTRL_MASK) == 3) {

printk(KERN_WARNING "%s: Device is configured for Hardware Based Teaming which is not supported with this operating system. Please consult the user diagnostic guide to disable Turbo Teaming.\n", dev->name);
		}
	}

#if (LINUX_VERSION_CODE > 0x20605)

	if ((pci_dev = pci_get_device(0x1022, 0x700c, NULL)))
#else
	if ((pci_dev = pci_find_device(0x1022, 0x700c, NULL)))
#endif
	{
		u32 val;

		/* Found AMD 762 North bridge */
		pci_read_config_dword(pci_dev, 0x4c, &val);
		if ((val & 0x02) == 0) {
			pci_write_config_dword(pci_dev, 0x4c, val | 0x02);
			printk(KERN_INFO "%s: Setting AMD762 Northbridge to enable PCI ordering compliance\n", bcm5700_driver);
		}
	}

#if (LINUX_VERSION_CODE > 0x20605)

	pci_dev_put(pci_dev);

#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)

	if ((pci_dev = pci_get_device(0x1066, 0x0017, NULL))) {
		bcm_msi_chipset_bug = 1;
	}
	pci_dev_put(pci_dev);
#endif
#endif

	return 0;
}


static void __devexit
bcm5700_remove_one (struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata (pdev);
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;

#ifdef BCM_IOCTL32
	atomic_dec(&bcm5700_load_count);
	if (atomic_read(&bcm5700_load_count) == 0)
		unregister_ioctl32_conversion(SIOCNICE);
#endif
	unregister_netdev(dev);

	if (pUmDevice->lm_dev.pMappedMemBase)
		iounmap(pUmDevice->lm_dev.pMappedMemBase);

	pci_release_regions(pdev);

#if (LINUX_VERSION_CODE < 0x020600)
	kfree(dev);
#else
	free_netdev(dev);
#endif

	pci_set_drvdata(pdev, NULL);

}

int b57_test_intr(UM_DEVICE_BLOCK *pUmDevice);
 
#ifdef BCM_WL_EMULATOR
/* new transmit callback  */ 
static int bcm5700emu_start_xmit(struct sk_buff *skb, struct net_device *dev);
/* keep track of the 2 gige devices */ 
static PLM_DEVICE_BLOCK pDev1;
static PLM_DEVICE_BLOCK pDev2;

static void 
bcm5700emu_open(struct net_device *dev)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;       
	static int instance = 0;
	static char *wlemu_if = NULL;
	char *wlemu_mode = NULL;
	//int wlemu_idx = 0;
	static int rx_enable = 0;
	static int tx_enable = 0;
	
	/* which interface is the emulator ? */
	if(instance == 0) {
		wlemu_if = nvram_get("wlemu_if");
		/* do we emulate rx, tx or both  */
		wlemu_mode = nvram_get("wlemu_mode");
		if(wlemu_mode) {
			if (!strcmp(wlemu_mode,"rx"))
			{
				rx_enable = 1;
			} 
			else if (!strcmp(wlemu_mode,"tx"))
			{
				
				tx_enable = 1;
				
			}
			else if (!strcmp(wlemu_mode,"rx_tx"))
			{
				
				rx_enable = 1;
				tx_enable = 1;
			}
		}
	}
	
	instance++;

	/* The context is used for accessing the OSL for emulating devices */
	pDevice->wlc = NULL;
	
	/* determines if this device is an emulator */
	pDevice->wl_emulate_rx = 0;
	pDevice->wl_emulate_tx = 0;

	if(wlemu_if && !strcmp(dev->name,wlemu_if))
	{
		/* create an emulator context. */
		pDevice->wlc = (void *)wlcemu_wlccreate((void *)dev);
		B57_INFO(("Using %s for wl emulation \n", dev->name));
		if(rx_enable)
		{
			B57_INFO(("Enabling wl RX emulation \n"));
			pDevice->wl_emulate_rx = 1;
		}
		/* re-direct transmit callback to emulator */
		if(tx_enable)
		{
			pDevice->wl_emulate_tx = 1;
			dev->hard_start_xmit = bcm5700emu_start_xmit;
			B57_INFO(("Enabling wl TX emulation \n"));
		}  
	}
	/* for debug access to configured devices only */
	if(instance == 1)
		pDev1 = pDevice;
	else if (instance == 2)
		pDev2 = pDevice;	
}	

/* Public API to get current emulation info */
int bcm5700emu_get_info(char *buf)
{
	int len = 0;
	PLM_DEVICE_BLOCK p;
	
	/* look for an emulating device */
	if(pDev1->wlc) {
		p = pDev1;
		len += sprintf(buf+len,"emulation device : eth0\n");
	}
	else if (pDev2->wlc) {
		p = pDev2;
		len += sprintf(buf+len,"emulation device : eth1\n");
	}
	else {
		len += sprintf(buf+len,"emulation not activated\n");
		return len;
	}
	if(p->wl_emulate_rx)
		len += sprintf(buf+len,"RX emulation enabled\n");
	else 
		len += sprintf(buf+len,"RX emulation disabled\n");
	if(p->wl_emulate_tx)
		len += sprintf(buf+len,"TX emulation enabled\n");
	else 
		len += sprintf(buf+len,"TX emulation disabled\n");
	return len;
	
} 


/* Public API to access the bcm5700_start_xmit callback */

int 
bcm5700emu_forward_xmit(struct sk_buff *skb, struct net_device *dev)
{
  return bcm5700_start_xmit(skb, dev);
}


/* hook to kernel txmit callback */
STATIC int
bcm5700emu_start_xmit(struct sk_buff *skb, struct net_device *dev)
{

  PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
  PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
  return wlcemu_start_xmit(skb,pDevice->wlc);
}	
	 
#endif /* BCM_WL_EMULATOR */
 
int
bcm5700_open(struct net_device *dev)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	int rc;

	if (pUmDevice->suspended){
            return -EAGAIN;
        }

#ifdef BCM_WL_EMULATOR
	bcm5700emu_open(dev);
#endif

	/* delay for 6 seconds */
	pUmDevice->delayed_link_ind = (6 * HZ) / pUmDevice->timer_interval;

#ifdef BCM_INT_COAL
#ifndef BCM_NAPI_RXPOLL
	pUmDevice->adaptive_expiry = HZ / pUmDevice->timer_interval;
#endif
#endif

#ifdef INCLUDE_TBI_SUPPORT
	if ((pDevice->TbiFlags & ENABLE_TBI_FLAG) &&
		(pDevice->TbiFlags & TBI_POLLING_FLAGS)) {
		pUmDevice->poll_tbi_interval = HZ / pUmDevice->timer_interval;
		if (T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5703) {
			pUmDevice->poll_tbi_interval /= 4;
		}
		pUmDevice->poll_tbi_expiry = pUmDevice->poll_tbi_interval;
	}
#endif
	/* set this timer for 2 seconds */
	pUmDevice->asf_heartbeat = (2 * HZ) / pUmDevice->timer_interval;

#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)


	if ( (  (T3_ASIC_IS_575X_PLUS(pDevice->ChipRevId) ) &&
		(T3_ASIC_REV(pDevice->ChipRevId) != T3_ASIC_REV_5714_A0 ) &&
		(T3_CHIP_REV(pDevice->ChipRevId) != T3_CHIP_REV_5750_AX ) &&
		(T3_CHIP_REV(pDevice->ChipRevId) != T3_CHIP_REV_5750_BX ) ) &&
		!bcm_msi_chipset_bug	){

		if (disable_msi[pUmDevice->index]==1){
			/* do nothing-it's not turned on */
		}else{
			pDevice->Flags |= USING_MSI_FLAG;

                        REG_WR(pDevice, Msi.Mode,  2 );

			rc = pci_enable_msi(pUmDevice->pdev);

			if(rc!=0){
				pDevice->Flags &= ~ USING_MSI_FLAG;
                        	REG_WR(pDevice, Msi.Mode,  1 );
			}
		}
	}


#endif

	if ((rc= request_irq(pUmDevice->pdev->irq, &bcm5700_interrupt, SA_SHIRQ, dev->name, dev)))
	{

#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)

		if(pDevice->Flags & USING_MSI_FLAG)  {

			pci_disable_msi(pUmDevice->pdev);
			pDevice->Flags &= ~USING_MSI_FLAG;
                       	REG_WR(pDevice, Msi.Mode,  1 );

		}
#endif
		return rc;
	}

	pUmDevice->opened = 1;
	if (LM_InitializeAdapter(pDevice) != LM_STATUS_SUCCESS) {
		pUmDevice->opened = 0;
		free_irq(dev->irq, dev);
		bcm5700_freemem(dev);
		return -EAGAIN;
	}

	bcm5700_set_vlan_mode(pUmDevice);
	bcm5700_init_counters(pUmDevice);

	if (pDevice->Flags & UNDI_FIX_FLAG) {
		printk(KERN_INFO "%s: Using indirect register access\n", dev->name);
	}

	if (memcmp(dev->dev_addr, pDevice->NodeAddress, 6))
	{
		/* Do not use invalid eth addrs: any multicast & all zeros */
		if( is_valid_ether_addr(dev->dev_addr) ){
			LM_SetMacAddress(pDevice, dev->dev_addr);
		}
		else
		{
			printk(KERN_INFO "%s: Invalid administered node address\n",dev->name);
			memcpy(dev->dev_addr, pDevice->NodeAddress, 6);
		}
	}

	if (tigon3_debug > 1)
		printk(KERN_DEBUG "%s: tigon3_open() irq %d.\n", dev->name, dev->irq);

	QQ_InitQueue(&pUmDevice->rx_out_of_buf_q.Container,
        MAX_RX_PACKET_DESC_COUNT);


#if (LINUX_VERSION_CODE < 0x020300)
	MOD_INC_USE_COUNT;
#endif

	atomic_set(&pUmDevice->intr_sem, 0);

	LM_EnableInterrupt(pDevice);

#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)

	if (pDevice->Flags & USING_MSI_FLAG){

		/* int test to check support on older machines */
		if (b57_test_intr(pUmDevice) != 1) {

			LM_DisableInterrupt(pDevice);
			free_irq(pUmDevice->pdev->irq, dev);
			pci_disable_msi(pUmDevice->pdev);
                        REG_WR(pDevice, Msi.Mode,  1 );
			pDevice->Flags &= ~USING_MSI_FLAG;

			rc = LM_ResetAdapter(pDevice);
printk(KERN_ALERT " The MSI support in this system is not functional.\n");

			if (rc == LM_STATUS_SUCCESS)
				rc = 0;
			else
				rc = -ENODEV;

			if(rc == 0){
				rc = request_irq(pUmDevice->pdev->irq, &bcm5700_interrupt,
					    SA_SHIRQ, dev->name, dev);
			}

			if(rc){
				LM_Halt(pDevice);
				bcm5700_freemem(dev);
				pUmDevice->opened = 0;
				return rc;
			}


			pDevice->InitDone = TRUE;
			atomic_set(&pUmDevice->intr_sem, 0);
			LM_EnableInterrupt(pDevice);
		}
	}
#endif

	init_timer(&pUmDevice->timer);
	pUmDevice->timer.expires = RUN_AT(pUmDevice->timer_interval);
	pUmDevice->timer.data = (unsigned long)dev;
	pUmDevice->timer.function = &bcm5700_timer;
	add_timer(&pUmDevice->timer);

	if (T3_ASIC_IS_5705_BEYOND(pDevice->ChipRevId)) {
		init_timer(&pUmDevice->statstimer);
		pUmDevice->statstimer.expires = RUN_AT(pUmDevice->statstimer_interval);
		pUmDevice->statstimer.data = (unsigned long)dev;
		pUmDevice->statstimer.function = &bcm5700_stats_timer;
		add_timer(&pUmDevice->statstimer);
	}

	if(pDevice->Flags & USING_MSI_FLAG)
		printk(KERN_INFO "%s: Using Message Signaled Interrupt (MSI)  \n", dev->name);
	else
		printk(KERN_INFO "%s: Using PCI INTX interrupt \n", dev->name);

	netif_start_queue(dev);

	return 0;
}


STATIC void
bcm5700_stats_timer(unsigned long data)
{
	struct net_device *dev = (struct net_device *)data;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	unsigned long flags = 0;

	if (!pUmDevice->opened)
		return;

	if (!atomic_read(&pUmDevice->intr_sem) &&
	    !pUmDevice->suspended              &&
	   (pDevice->LinkStatus == LM_STATUS_LINK_ACTIVE)) {
		BCM5700_LOCK(pUmDevice, flags);
		LM_GetStats(pDevice);
		BCM5700_UNLOCK(pUmDevice, flags);
	}

	pUmDevice->statstimer.expires = RUN_AT(pUmDevice->statstimer_interval);

	add_timer(&pUmDevice->statstimer);
}


STATIC void
bcm5700_timer(unsigned long data)
{
	struct net_device *dev = (struct net_device *)data;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	unsigned long flags = 0;
	LM_UINT32 value32;

	if (!pUmDevice->opened)
		return;

	/* BCM4785: Flush posted writes from GbE to host memory. */
	if (pDevice->Flags & FLUSH_POSTED_WRITE_FLAG)
		REG_RD(pDevice, HostCoalesce.Mode);

	if (atomic_read(&pUmDevice->intr_sem) || pUmDevice->suspended) {
		pUmDevice->timer.expires = RUN_AT(pUmDevice->timer_interval);
		add_timer(&pUmDevice->timer);
		return;
	}

#ifdef INCLUDE_TBI_SUPPORT
	if ((pDevice->TbiFlags & TBI_POLLING_FLAGS) &&
		(--pUmDevice->poll_tbi_expiry <= 0)) {

		BCM5700_PHY_LOCK(pUmDevice, flags);
		value32 = REG_RD(pDevice, MacCtrl.Status);
		if (((pDevice->LinkStatus == LM_STATUS_LINK_ACTIVE) &&
			((value32 & (MAC_STATUS_LINK_STATE_CHANGED |
				MAC_STATUS_CFG_CHANGED)) ||
			!(value32 & MAC_STATUS_PCS_SYNCED)))
			||
			((pDevice->LinkStatus != LM_STATUS_LINK_ACTIVE) &&
			(value32 & (MAC_STATUS_PCS_SYNCED |
				MAC_STATUS_SIGNAL_DETECTED))))
		{
			LM_SetupPhy(pDevice);
		}
		BCM5700_PHY_UNLOCK(pUmDevice, flags);
		pUmDevice->poll_tbi_expiry = pUmDevice->poll_tbi_interval;

        }
#endif

	if (pUmDevice->delayed_link_ind > 0) {
		if (pUmDevice->delayed_link_ind == 1)
			MM_IndicateStatus(pDevice, pDevice->LinkStatus);
		else
			pUmDevice->delayed_link_ind--;
	}

	if (pUmDevice->crc_counter_expiry > 0)
		pUmDevice->crc_counter_expiry--;

	if (!pUmDevice->interrupt) {
		if (!(pDevice->Flags & USE_TAGGED_STATUS_FLAG)) {
			BCM5700_LOCK(pUmDevice, flags);
			if (pDevice->pStatusBlkVirt->Status & STATUS_BLOCK_UPDATED) {
				/* This will generate an interrupt */
				REG_WR(pDevice, Grc.LocalCtrl,
					pDevice->GrcLocalCtrl |
					GRC_MISC_LOCAL_CTRL_SET_INT);
			}
			else {
				REG_WR(pDevice, HostCoalesce.Mode,
					pDevice->CoalesceMode |
					HOST_COALESCE_ENABLE |
					HOST_COALESCE_NOW);
			}
			if (!(REG_RD(pDevice, DmaWrite.Mode) &
				DMA_WRITE_MODE_ENABLE)) {
				BCM5700_UNLOCK(pUmDevice, flags);
				bcm5700_reset(dev);
			}
			else {
				BCM5700_UNLOCK(pUmDevice, flags);
			}
			if (pUmDevice->tx_queued) {
				pUmDevice->tx_queued = 0;
				netif_wake_queue(dev);
			}
		}
#if (LINUX_VERSION_CODE < 0x02032b)
		if ((QQ_GetEntryCnt(&pDevice->TxPacketFreeQ.Container) !=
			pDevice->TxPacketDescCnt) &&
			((jiffies - dev->trans_start) > TX_TIMEOUT)) {

			printk(KERN_WARNING "%s: Tx hung\n", dev->name);
			bcm5700_reset(dev);
		}
#endif
	}
#ifdef BCM_INT_COAL
#ifndef BCM_NAPI_RXPOLL
	if (pUmDevice->adaptive_coalesce) {
		pUmDevice->adaptive_expiry--;
		if (pUmDevice->adaptive_expiry == 0) {
			pUmDevice->adaptive_expiry = HZ /
				pUmDevice->timer_interval;
			bcm5700_adapt_coalesce(pUmDevice);
		}
	}
#endif
#endif
	if (QQ_GetEntryCnt(&pUmDevice->rx_out_of_buf_q.Container) >
		(unsigned int) pUmDevice->rx_buf_repl_panic_thresh) {
		/* Generate interrupt and let isr allocate buffers */
		REG_WR(pDevice, HostCoalesce.Mode, pDevice->CoalesceMode |
			HOST_COALESCE_ENABLE | HOST_COALESCE_NOW);
	}

#ifdef BCM_ASF
	if (pDevice->AsfFlags & ASF_ENABLED) {
		pUmDevice->asf_heartbeat--;
		if (pUmDevice->asf_heartbeat == 0) {
			if( (pDevice->Flags & UNDI_FIX_FLAG) || 
			    (pDevice->Flags & ENABLE_PCIX_FIX_FLAG)) {
				MEM_WR_OFFSET(pDevice, T3_CMD_MAILBOX,
					T3_CMD_NICDRV_ALIVE2);
				MEM_WR_OFFSET(pDevice, T3_CMD_LENGTH_MAILBOX,
					4);
				MEM_WR_OFFSET(pDevice, T3_CMD_DATA_MAILBOX, 5);
			} else {
				LM_RegWr(pDevice, 
					 (T3_NIC_MBUF_POOL_ADDR + 
					  T3_CMD_MAILBOX), 
					 T3_CMD_NICDRV_ALIVE2, 1);
				LM_RegWr(pDevice, 
					 (T3_NIC_MBUF_POOL_ADDR + 
					  T3_CMD_LENGTH_MAILBOX),4,1);
				LM_RegWr(pDevice, 
					 (T3_NIC_MBUF_POOL_ADDR + 
					  T3_CMD_DATA_MAILBOX),5,1);
 			}

			value32 = REG_RD(pDevice, Grc.RxCpuEvent);
			REG_WR(pDevice, Grc.RxCpuEvent, value32 | BIT_14);
			pUmDevice->asf_heartbeat = (2 * HZ) /
				pUmDevice->timer_interval;
		}
	}
#endif

	if (pDevice->PhyFlags & PHY_IS_FIBER){
		BCM5700_PHY_LOCK(pUmDevice, flags);
		LM_5714_FamFiberCheckLink(pDevice);
		BCM5700_PHY_UNLOCK(pUmDevice, flags);
	}

	pUmDevice->timer.expires = RUN_AT(pUmDevice->timer_interval);
	add_timer(&pUmDevice->timer);
}

STATIC int
bcm5700_init_counters(PUM_DEVICE_BLOCK pUmDevice)
{
#ifdef BCM_INT_COAL
#ifndef BCM_NAPI_RXPOLL
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;

	pUmDevice->rx_curr_coalesce_frames = pDevice->RxMaxCoalescedFrames;
	pUmDevice->rx_curr_coalesce_ticks = pDevice->RxCoalescingTicks;
	pUmDevice->tx_curr_coalesce_frames = pDevice->TxMaxCoalescedFrames;
	pUmDevice->rx_last_cnt = 0;
	pUmDevice->tx_last_cnt = 0;
#endif
#endif
	pUmDevice->phy_crc_count = 0;
#if TIGON3_DEBUG
	pUmDevice->tx_zc_count = 0;
	pUmDevice->tx_chksum_count = 0;
	pUmDevice->tx_himem_count = 0;
	pUmDevice->rx_good_chksum_count = 0;
	pUmDevice->rx_bad_chksum_count = 0;
#endif
#ifdef BCM_TSO
	pUmDevice->tso_pkt_count = 0;
#endif
	return 0;
}

#ifdef BCM_INT_COAL
#ifndef BCM_NAPI_RXPOLL
STATIC int
bcm5700_do_adapt_coalesce(PUM_DEVICE_BLOCK pUmDevice,
	int rx_frames, int rx_ticks, int tx_frames, int rx_frames_intr)
{
	unsigned long flags = 0;
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;

	if (pUmDevice->do_global_lock) {
		if (spin_is_locked(&pUmDevice->global_lock))
			return 0;
		spin_lock_irqsave(&pUmDevice->global_lock, flags);
	}
	pUmDevice->rx_curr_coalesce_frames = rx_frames;
	pUmDevice->rx_curr_coalesce_ticks = rx_ticks;
	pUmDevice->tx_curr_coalesce_frames = tx_frames;
	pUmDevice->rx_curr_coalesce_frames_intr = rx_frames_intr;
	REG_WR(pDevice, HostCoalesce.RxMaxCoalescedFrames, rx_frames);

	REG_WR(pDevice, HostCoalesce.RxCoalescingTicks, rx_ticks);

	REG_WR(pDevice, HostCoalesce.TxMaxCoalescedFrames, tx_frames);

	REG_WR(pDevice, HostCoalesce.RxMaxCoalescedFramesDuringInt,
		rx_frames_intr);

	BCM5700_UNLOCK(pUmDevice, flags);
	return 0;
}

STATIC int
bcm5700_adapt_coalesce(PUM_DEVICE_BLOCK pUmDevice)
{
	PLM_DEVICE_BLOCK pDevice = &pUmDevice->lm_dev;
	uint rx_curr_cnt, tx_curr_cnt, rx_delta, tx_delta, total_delta;

	rx_curr_cnt = pDevice->pStatsBlkVirt->ifHCInUcastPkts.Low;
	tx_curr_cnt = pDevice->pStatsBlkVirt->ifHCOutUcastPkts.Low;
	if ((rx_curr_cnt <= pUmDevice->rx_last_cnt) ||
		(tx_curr_cnt < pUmDevice->tx_last_cnt)) {

		/* skip if there is counter rollover */
		pUmDevice->rx_last_cnt = rx_curr_cnt;
		pUmDevice->tx_last_cnt = tx_curr_cnt;
		return 0;
	}

	rx_delta = rx_curr_cnt - pUmDevice->rx_last_cnt;
	tx_delta = tx_curr_cnt - pUmDevice->tx_last_cnt;
	total_delta = (((rx_delta + rx_delta) + tx_delta) / 3) << 1;

	pUmDevice->rx_last_cnt = rx_curr_cnt;
	pUmDevice->tx_last_cnt = tx_curr_cnt;

	if (total_delta < ADAPTIVE_LO_PKT_THRESH) {
		if (pUmDevice->rx_curr_coalesce_frames !=
			ADAPTIVE_LO_RX_MAX_COALESCED_FRAMES) {

			bcm5700_do_adapt_coalesce(pUmDevice,
				ADAPTIVE_LO_RX_MAX_COALESCED_FRAMES,
				ADAPTIVE_LO_RX_COALESCING_TICKS,
				ADAPTIVE_LO_TX_MAX_COALESCED_FRAMES,
				ADAPTIVE_LO_RX_MAX_COALESCED_FRAMES_DURING_INT);
		}
	}
	else if (total_delta < ADAPTIVE_HI_PKT_THRESH) {
		if (pUmDevice->rx_curr_coalesce_frames !=
			DEFAULT_RX_MAX_COALESCED_FRAMES) {

			bcm5700_do_adapt_coalesce(pUmDevice,
				DEFAULT_RX_MAX_COALESCED_FRAMES,
				DEFAULT_RX_COALESCING_TICKS,
				DEFAULT_TX_MAX_COALESCED_FRAMES,
				DEFAULT_RX_MAX_COALESCED_FRAMES_DURING_INT);
		}
	}
	else {
		if (pUmDevice->rx_curr_coalesce_frames !=
			ADAPTIVE_HI_RX_MAX_COALESCED_FRAMES) {

			bcm5700_do_adapt_coalesce(pUmDevice,
				ADAPTIVE_HI_RX_MAX_COALESCED_FRAMES,
				ADAPTIVE_HI_RX_COALESCING_TICKS,
				ADAPTIVE_HI_TX_MAX_COALESCED_FRAMES,
				ADAPTIVE_HI_RX_MAX_COALESCED_FRAMES_DURING_INT);
		}
	}
	return 0;
}
#endif
#endif

STATIC void
bcm5700_reset(struct net_device *dev)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	unsigned long flags;

#ifdef BCM_TSO

	if( (dev->features & NETIF_F_TSO) &&
		(pUmDevice->tx_full) )	   {

		dev->features &= ~NETIF_F_TSO;
	}
#endif

	netif_stop_queue(dev);
	bcm5700_intr_off(pUmDevice);
	BCM5700_PHY_LOCK(pUmDevice, flags);
	LM_ResetAdapter(pDevice);
	pDevice->InitDone = TRUE;
	bcm5700_do_rx_mode(dev);
	bcm5700_set_vlan_mode(pUmDevice);
	bcm5700_init_counters(pUmDevice);
	if (memcmp(dev->dev_addr, pDevice->NodeAddress, 6)) {
		LM_SetMacAddress(pDevice, dev->dev_addr);
	}
	BCM5700_PHY_UNLOCK(pUmDevice, flags);
	atomic_set(&pUmDevice->intr_sem, 1);
	bcm5700_intr_on(pUmDevice);
	netif_wake_queue(dev);
}

STATIC void
bcm5700_set_vlan_mode(UM_DEVICE_BLOCK *pUmDevice)
{
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;
	LM_UINT32 ReceiveMask = pDevice->ReceiveMask;
	int vlan_tag_mode = pUmDevice->vlan_tag_mode;

	if (vlan_tag_mode == VLAN_TAG_MODE_AUTO_STRIP) {
	        if (pDevice->AsfFlags & ASF_ENABLED) {
			vlan_tag_mode = VLAN_TAG_MODE_FORCED_STRIP;
		}
		else {
			vlan_tag_mode = VLAN_TAG_MODE_NORMAL_STRIP;
		}
	}
	if (vlan_tag_mode == VLAN_TAG_MODE_NORMAL_STRIP) {
		ReceiveMask |= LM_KEEP_VLAN_TAG;
#ifdef BCM_VLAN
		if (pUmDevice->vlgrp)
			ReceiveMask &= ~LM_KEEP_VLAN_TAG;
#endif
	}
	else if (vlan_tag_mode == VLAN_TAG_MODE_FORCED_STRIP) {
		ReceiveMask &= ~LM_KEEP_VLAN_TAG;
	}
	if (ReceiveMask != pDevice->ReceiveMask)
	{
		LM_SetReceiveMask(pDevice, ReceiveMask);
	}
}

static void
bcm5700_poll_wait(UM_DEVICE_BLOCK *pUmDevice)
{
#ifdef BCM_NAPI_RXPOLL
	while (pUmDevice->lm_dev.RxPoll) {
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(1);
	}
#endif
}


#ifdef BCM_VLAN
STATIC void
bcm5700_vlan_rx_register(struct net_device *dev, struct vlan_group *vlgrp)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) dev->priv;

	bcm5700_intr_off(pUmDevice);
	bcm5700_poll_wait(pUmDevice);
	pUmDevice->vlgrp = vlgrp;
	bcm5700_set_vlan_mode(pUmDevice);
	bcm5700_intr_on(pUmDevice);
}

STATIC void
bcm5700_vlan_rx_kill_vid(struct net_device *dev, uint16_t vid)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) dev->priv;

	bcm5700_intr_off(pUmDevice);
	bcm5700_poll_wait(pUmDevice);
	if (pUmDevice->vlgrp) {
		pUmDevice->vlgrp->vlan_devices[vid] = NULL;
	}
	bcm5700_intr_on(pUmDevice);
}
#endif

STATIC int
bcm5700_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	PLM_PACKET pPacket;
	PUM_PACKET pUmPacket;
	unsigned long flags = 0;
	int frag_no;
#ifdef BCM_TSO
	LM_UINT32 mss = 0 ;
	uint16_t ip_tcp_len, tcp_opt_len, tcp_seg_flags;
#endif

	if ((pDevice->LinkStatus == LM_STATUS_LINK_DOWN) ||
		!pDevice->InitDone || pUmDevice->suspended)
	{
		dev_kfree_skb(skb);
		return 0;
	}

#if (LINUX_VERSION_CODE < 0x02032b)
	if (test_and_set_bit(0, &dev->tbusy)) {
	    return 1;
	}
#endif

	if (pUmDevice->do_global_lock && pUmDevice->interrupt) {
		netif_stop_queue(dev);
		pUmDevice->tx_queued = 1;
		if (!pUmDevice->interrupt) {
			netif_wake_queue(dev);
			pUmDevice->tx_queued = 0;
		}
	    return 1;
	}

	pPacket = (PLM_PACKET)
		QQ_PopHead(&pDevice->TxPacketFreeQ.Container);
	if (pPacket == 0) {
		netif_stop_queue(dev);
		pUmDevice->tx_full = 1;
		if (QQ_GetEntryCnt(&pDevice->TxPacketFreeQ.Container)) {
			netif_wake_queue(dev);
			pUmDevice->tx_full = 0;
		}
	    return 1;
	}
	pUmPacket = (PUM_PACKET) pPacket;
	pUmPacket->skbuff = skb;
	pUmDevice->stats.tx_bytes += skb->len; 

	if (skb->ip_summed == CHECKSUM_HW) {
		pPacket->Flags = SND_BD_FLAG_TCP_UDP_CKSUM;
#if TIGON3_DEBUG
		pUmDevice->tx_chksum_count++;
#endif
	}
	else {
		pPacket->Flags = 0;
	}
#if MAX_SKB_FRAGS
	frag_no = skb_shinfo(skb)->nr_frags;
#else
	frag_no = 0;
#endif
	if (atomic_read(&pDevice->SendBdLeft) < (frag_no + 1)) {
		netif_stop_queue(dev);
		pUmDevice->tx_full = 1;
		QQ_PushHead(&pDevice->TxPacketFreeQ.Container, pPacket);
		if (atomic_read(&pDevice->SendBdLeft) >= (frag_no + 1)) {
			netif_wake_queue(dev);
			pUmDevice->tx_full = 0;
		}
		return 1;
	}

	pPacket->u.Tx.FragCount = frag_no + 1;
#if TIGON3_DEBUG
	if (pPacket->u.Tx.FragCount > 1)
		pUmDevice->tx_zc_count++;
#endif

#ifdef BCM_VLAN
	if (pUmDevice->vlgrp && vlan_tx_tag_present(skb)) {
		pPacket->VlanTag = vlan_tx_tag_get(skb);
		pPacket->Flags |= SND_BD_FLAG_VLAN_TAG;
	}
#endif

#ifdef BCM_TSO
	if ((mss = (LM_UINT32) skb_shinfo(skb)->tso_size) &&
		(skb->len > pDevice->TxMtu)) {

#if (LINUX_VERSION_CODE >= 0x02060c)

		if (skb_header_cloned(skb) &&
			pskb_expand_head(skb, 0, 0, GFP_ATOMIC)) {

			dev_kfree_skb(skb);
			return 0;
		}
#endif
		pUmDevice->tso_pkt_count++;

		pPacket->Flags |= SND_BD_FLAG_CPU_PRE_DMA |
			SND_BD_FLAG_CPU_POST_DMA;

		tcp_opt_len = 0;
		if (skb->h.th->doff > 5) {
			tcp_opt_len = (skb->h.th->doff - 5) << 2;
		}
		ip_tcp_len = (skb->nh.iph->ihl << 2) + sizeof(struct tcphdr);
		skb->nh.iph->check = 0;

		if ( T3_ASIC_IS_575X_PLUS(pDevice->ChipRevId) ){
			skb->h.th->check = 0;
			pPacket->Flags &= ~SND_BD_FLAG_TCP_UDP_CKSUM;
		}
		else {
			skb->h.th->check = ~csum_tcpudp_magic(
				skb->nh.iph->saddr, skb->nh.iph->daddr,
				0, IPPROTO_TCP, 0);
		}

		skb->nh.iph->tot_len = htons(mss + ip_tcp_len + tcp_opt_len);
		tcp_seg_flags = 0;

		if (tcp_opt_len || (skb->nh.iph->ihl > 5)) {
			if ( T3_ASIC_IS_5705_BEYOND(pDevice->ChipRevId) ){
				tcp_seg_flags =
					((skb->nh.iph->ihl - 5) +
					(tcp_opt_len >> 2)) << 11;
			}
			else {
				pPacket->Flags |=
					((skb->nh.iph->ihl - 5) +
					(tcp_opt_len >> 2)) << 12;
			}
		}
		pPacket->u.Tx.MaxSegmentSize = mss | tcp_seg_flags;
	}
	else
	{
		pPacket->u.Tx.MaxSegmentSize = 0;
	}
#endif
	BCM5700_LOCK(pUmDevice, flags);
	LM_SendPacket(pDevice, pPacket);
	BCM5700_UNLOCK(pUmDevice, flags);

#if (LINUX_VERSION_CODE < 0x02032b)
	netif_wake_queue(dev);
#endif
	dev->trans_start = jiffies;


	return 0;
}

#ifdef BCM_NAPI_RXPOLL
STATIC int
bcm5700_poll(struct net_device *dev, int *budget)
{
	int orig_budget = *budget;
	int work_done;
	UM_DEVICE_BLOCK *pUmDevice = (UM_DEVICE_BLOCK *) dev->priv;
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;
	unsigned long flags = 0;
	LM_UINT32 tag;

	if (orig_budget > dev->quota)
		orig_budget = dev->quota;

	BCM5700_LOCK(pUmDevice, flags);
	/* BCM4785: Flush posted writes from GbE to host memory. */
	if (pDevice->Flags & FLUSH_POSTED_WRITE_FLAG)
		REG_RD(pDevice, HostCoalesce.Mode);
	work_done = LM_ServiceRxPoll(pDevice, orig_budget);
	*budget -= work_done;
	dev->quota -= work_done;

	if (QQ_GetEntryCnt(&pUmDevice->rx_out_of_buf_q.Container)) {
		replenish_rx_buffers(pUmDevice, 0);
	}
	BCM5700_UNLOCK(pUmDevice, flags);
	if (work_done) {
		MM_IndicateRxPackets(pDevice);
		BCM5700_LOCK(pUmDevice, flags);
		LM_QueueRxPackets(pDevice);
		BCM5700_UNLOCK(pUmDevice, flags);
	}
	if ((work_done < orig_budget) || atomic_read(&pUmDevice->intr_sem) ||
		pUmDevice->suspended) {

		netif_rx_complete(dev);
		BCM5700_LOCK(pUmDevice, flags);
		REG_WR(pDevice, Grc.Mode, pDevice->GrcMode);
		pDevice->RxPoll = FALSE;
		if (pDevice->RxPoll) {
			BCM5700_UNLOCK(pUmDevice, flags);
			return 0;
		}
		/* Take care of possible missed rx interrupts */
		REG_RD_BACK(pDevice, Grc.Mode);	/* flush the register write */
		tag = pDevice->pStatusBlkVirt->StatusTag;
		if ((pDevice->pStatusBlkVirt->Status & STATUS_BLOCK_UPDATED) ||
			(pDevice->pStatusBlkVirt->Idx[0].RcvProdIdx !=
			pDevice->RcvRetConIdx)) {

			REG_WR(pDevice, HostCoalesce.Mode,
				pDevice->CoalesceMode | HOST_COALESCE_ENABLE |
				HOST_COALESCE_NOW);
		}
		/* If a new status block is pending in the WDMA state machine */
		/* before the register write to enable the rx interrupt,      */
		/* the new status block may DMA with no interrupt. In this    */
		/* scenario, the tag read above will be older than the tag in */
		/* the pending status block and writing the older tag will    */
		/* cause interrupt to be generated.                           */
		else if (pDevice->Flags & USE_TAGGED_STATUS_FLAG) {
			MB_REG_WR(pDevice, Mailbox.Interrupt[0].Low,
				tag << 24);
			/* Make sure we service tx in case some tx interrupts */
			/* are cleared */
			if (atomic_read(&pDevice->SendBdLeft) <
				(T3_SEND_RCB_ENTRY_COUNT / 2)) {
				REG_WR(pDevice, HostCoalesce.Mode,
					pDevice->CoalesceMode |
					HOST_COALESCE_ENABLE |
					HOST_COALESCE_NOW);
			}
		}
		BCM5700_UNLOCK(pUmDevice, flags);
		return 0;
	}
	return 1;
}
#endif /* BCM_NAPI_RXPOLL */

STATIC irqreturn_t
bcm5700_interrupt(int irq, void *dev_instance, struct pt_regs *regs)
{
	struct net_device *dev = (struct net_device *)dev_instance;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	LM_UINT32 oldtag, newtag;
	int i, max_intr_loop;
#ifdef BCM_TASKLET
	int repl_buf_count;
#endif
	unsigned int handled = 1;

	if (!pDevice->InitDone) {
		handled = 0;
		return IRQ_RETVAL(handled);
	}

	bcm5700_intr_lock(pUmDevice);
	if (atomic_read(&pUmDevice->intr_sem)) {
		MB_REG_WR(pDevice, Mailbox.Interrupt[0].Low, 1);
		bcm5700_intr_unlock(pUmDevice);
		handled = 0;
		return IRQ_RETVAL(handled);
	}

	if (test_and_set_bit(0, (void*)&pUmDevice->interrupt)) {
		printk(KERN_ERR "%s: Duplicate entry of the interrupt handler\n",
			dev->name);
		bcm5700_intr_unlock(pUmDevice);
		handled = 0;
		return IRQ_RETVAL(handled);
	}

	/* BCM4785: Flush posted writes from GbE to host memory. */
	if (pDevice->Flags & FLUSH_POSTED_WRITE_FLAG)
		REG_RD(pDevice, HostCoalesce.Mode);

	if ((pDevice->Flags & USING_MSI_FLAG) ||
		(pDevice->pStatusBlkVirt->Status & STATUS_BLOCK_UPDATED) ||
		!(REG_RD(pDevice,PciCfg.PciState) & T3_PCI_STATE_INTERRUPT_NOT_ACTIVE) )
	{

		if (pUmDevice->intr_test) {
			if (!(REG_RD(pDevice, PciCfg.PciState) &
					T3_PCI_STATE_INTERRUPT_NOT_ACTIVE) ||
						pDevice->Flags & USING_MSI_FLAG ) {
				pUmDevice->intr_test_result = 1;
			}
			pUmDevice->intr_test = 0;
		}

#ifdef BCM_NAPI_RXPOLL
		max_intr_loop = 1;
#else
		max_intr_loop = 50;
#endif
		if (pDevice->Flags & USE_TAGGED_STATUS_FLAG) {
			MB_REG_WR(pDevice, Mailbox.Interrupt[0].Low, 1);
			oldtag = pDevice->pStatusBlkVirt->StatusTag;

			for (i = 0; ; i++) {
				pDevice->pStatusBlkVirt->Status &= ~STATUS_BLOCK_UPDATED;

				LM_ServiceInterrupts(pDevice);
				/* BCM4785: Flush GbE posted writes to host memory. */
				if (pDevice->Flags & FLUSH_POSTED_WRITE_FLAG)
					MB_REG_RD(pDevice, Mailbox.Interrupt[0].Low);
				newtag = pDevice->pStatusBlkVirt->StatusTag;
				if ((newtag == oldtag) || (i > max_intr_loop)) {
					MB_REG_WR(pDevice, Mailbox.Interrupt[0].Low, oldtag << 24);
					pDevice->LastTag = oldtag;
					if (pDevice->Flags & UNDI_FIX_FLAG) {
						REG_WR(pDevice, Grc.LocalCtrl,
						pDevice->GrcLocalCtrl | 0x2);
					}
					break;
				}
				oldtag = newtag;
			}
		}
		else
		{
			i = 0;
			do {
				uint dummy;

				MB_REG_WR(pDevice, Mailbox.Interrupt[0].Low, 1);
				pDevice->pStatusBlkVirt->Status &= ~STATUS_BLOCK_UPDATED;
				LM_ServiceInterrupts(pDevice);
				MB_REG_WR(pDevice, Mailbox.Interrupt[0].Low, 0);
				dummy = MB_REG_RD(pDevice, Mailbox.Interrupt[0].Low);
				i++;
			}
			while ((pDevice->pStatusBlkVirt->Status & STATUS_BLOCK_UPDATED) &&
				(i < max_intr_loop));

			if (pDevice->Flags & UNDI_FIX_FLAG) {
				REG_WR(pDevice, Grc.LocalCtrl,
				pDevice->GrcLocalCtrl | 0x2);
			}
		}
	}
	else
	{
		/* not my interrupt */
		handled = 0;
	}

#ifdef BCM_TASKLET
	repl_buf_count = QQ_GetEntryCnt(&pUmDevice->rx_out_of_buf_q.Container);
	if (((repl_buf_count > pUmDevice->rx_buf_repl_panic_thresh) ||
		pDevice->QueueAgain) &&
		(!test_and_set_bit(0, &pUmDevice->tasklet_busy))) {

		replenish_rx_buffers(pUmDevice, pUmDevice->rx_buf_repl_isr_limit);
		clear_bit(0, (void*)&pUmDevice->tasklet_busy);
	}
	else if ((repl_buf_count > pUmDevice->rx_buf_repl_thresh) &&
		!pUmDevice->tasklet_pending) {

		pUmDevice->tasklet_pending = 1;
		tasklet_schedule(&pUmDevice->tasklet);
	}
#else
#ifdef BCM_NAPI_RXPOLL
	if (!pDevice->RxPoll &&
		QQ_GetEntryCnt(&pUmDevice->rx_out_of_buf_q.Container)) {
		pDevice->RxPoll = 1;
		MM_ScheduleRxPoll(pDevice);
	}
#else
	if (QQ_GetEntryCnt(&pUmDevice->rx_out_of_buf_q.Container)) {
		replenish_rx_buffers(pUmDevice, 0);
	}

	if (QQ_GetEntryCnt(&pDevice->RxPacketFreeQ.Container) ||
		pDevice->QueueAgain) {

		LM_QueueRxPackets(pDevice);
	}
#endif
#endif

	clear_bit(0, (void*)&pUmDevice->interrupt);
	bcm5700_intr_unlock(pUmDevice);
	if (pUmDevice->tx_queued) {
		pUmDevice->tx_queued = 0;
		netif_wake_queue(dev);
	}
	return IRQ_RETVAL(handled);
}


#ifdef BCM_TASKLET
STATIC void
bcm5700_tasklet(unsigned long data)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)data;
	unsigned long flags = 0;

	/* RH 7.2 Beta 3 tasklets are reentrant */
	if (test_and_set_bit(0, &pUmDevice->tasklet_busy)) {
		pUmDevice->tasklet_pending = 0;
		return;
	}

	pUmDevice->tasklet_pending = 0;
	if (pUmDevice->opened && !pUmDevice->suspended) {
		BCM5700_LOCK(pUmDevice, flags);
		replenish_rx_buffers(pUmDevice, 0);
		BCM5700_UNLOCK(pUmDevice, flags);
	}

	clear_bit(0, &pUmDevice->tasklet_busy);
}
#endif

STATIC int
bcm5700_close(struct net_device *dev)
{

	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;

#if (LINUX_VERSION_CODE < 0x02032b)
	dev->start = 0;
#endif
	netif_stop_queue(dev);
	pUmDevice->opened = 0;

#ifdef BCM_ASF
	if( !(pDevice->AsfFlags & ASF_ENABLED) )
#endif
#ifdef BCM_WOL
		if( enable_wol[pUmDevice->index] == 0 )
#endif
			B57_INFO(("%s: %s NIC Link is DOWN\n", bcm5700_driver, dev->name));

	if (tigon3_debug > 1)
		printk(KERN_DEBUG "%s: Shutting down Tigon3\n",
			   dev->name);

	LM_MulticastClear(pDevice);
	bcm5700_shutdown(pUmDevice);

	if (T3_ASIC_IS_5705_BEYOND(pDevice->ChipRevId)) {
		del_timer_sync(&pUmDevice->statstimer);
	}

	del_timer_sync(&pUmDevice->timer);

	free_irq(pUmDevice->pdev->irq, dev);

#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)

	if(pDevice->Flags & USING_MSI_FLAG) {
		pci_disable_msi(pUmDevice->pdev);
                REG_WR(pDevice, Msi.Mode,  1 );
		pDevice->Flags &= ~USING_MSI_FLAG;
	}

#endif


#if (LINUX_VERSION_CODE < 0x020300)
	MOD_DEC_USE_COUNT;
#endif
	{
	/* BCM4785: Don't go to low-power state because it will power down the smbus block. */
	if (!(pDevice->Flags & SB_CORE_FLAG))
		LM_SetPowerState(pDevice, LM_POWER_STATE_D3);
	}

	bcm5700_freemem(dev);

	QQ_InitQueue(&pDevice->RxPacketFreeQ.Container,
        		MAX_RX_PACKET_DESC_COUNT);

	return 0;
}

STATIC int
bcm5700_freemem(struct net_device *dev)
{
	int i;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	LM_DEVICE_BLOCK	 *pDevice = &pUmDevice->lm_dev;

	for (i = 0; i < pUmDevice->mem_list_num; i++) {
		if (pUmDevice->mem_size_list[i] == 0) {
			kfree(pUmDevice->mem_list[i]);
		}
		else {
			pci_free_consistent(pUmDevice->pdev,
				(size_t) pUmDevice->mem_size_list[i],
				pUmDevice->mem_list[i],
				pUmDevice->dma_list[i]);
		}
	}

	pDevice->pStatusBlkVirt = 0;
	pDevice->pStatsBlkVirt  = 0;
	pUmDevice->mem_list_num = 0;

	return 0;
}

uint64_t
bcm5700_crc_count(PUM_DEVICE_BLOCK pUmDevice)
{
	PLM_DEVICE_BLOCK pDevice = &pUmDevice->lm_dev;
	LM_UINT32 Value32;
	PT3_STATS_BLOCK pStats = (PT3_STATS_BLOCK) pDevice->pStatsBlkVirt;
	unsigned long flags;

	if ((T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5700 ||
		T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5701) &&
		!(pDevice->TbiFlags & ENABLE_TBI_FLAG)) {

		if (!pUmDevice->opened || !pDevice->InitDone)
		{

			return 0;
		}

		/* regulate MDIO access during run time */
		if (pUmDevice->crc_counter_expiry > 0)
			return pUmDevice->phy_crc_count;

		pUmDevice->crc_counter_expiry = (5 * HZ) /
			pUmDevice->timer_interval;

		BCM5700_PHY_LOCK(pUmDevice, flags);
		LM_ReadPhy(pDevice, 0x1e, &Value32);
		if ((Value32 & 0x8000) == 0)
			LM_WritePhy(pDevice, 0x1e, Value32 | 0x8000);
		LM_ReadPhy(pDevice, 0x14, &Value32);
		BCM5700_PHY_UNLOCK(pUmDevice, flags);
		/* Sometimes data on the MDIO bus can be corrupted */
		if (Value32 != 0xffff)
			pUmDevice->phy_crc_count += Value32;
		return pUmDevice->phy_crc_count;
	}
	else if (pStats == 0) {
		return 0;
	}
	else {
		return (MM_GETSTATS64(pStats->dot3StatsFCSErrors));
	}
}

uint64_t
bcm5700_rx_err_count(UM_DEVICE_BLOCK *pUmDevice)
{
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;
	T3_STATS_BLOCK *pStats = (T3_STATS_BLOCK *) pDevice->pStatsBlkVirt;

	if (pStats == 0)
		return 0;
	return (bcm5700_crc_count(pUmDevice) +
		MM_GETSTATS64(pStats->dot3StatsAlignmentErrors) +
		MM_GETSTATS64(pStats->etherStatsUndersizePkts) +
		MM_GETSTATS64(pStats->etherStatsFragments) +
		MM_GETSTATS64(pStats->dot3StatsFramesTooLong) +
		MM_GETSTATS64(pStats->etherStatsJabbers));
}

STATIC struct net_device_stats *
bcm5700_get_stats(struct net_device *dev)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	PT3_STATS_BLOCK pStats = (PT3_STATS_BLOCK) pDevice->pStatsBlkVirt;
	struct net_device_stats *p_netstats = &pUmDevice->stats;

	if (pStats == 0)
		return p_netstats;

	/* Get stats from LM */
	p_netstats->rx_packets =
		MM_GETSTATS(pStats->ifHCInUcastPkts) +
		MM_GETSTATS(pStats->ifHCInMulticastPkts) +
		MM_GETSTATS(pStats->ifHCInBroadcastPkts);
	p_netstats->tx_packets =
		MM_GETSTATS(pStats->ifHCOutUcastPkts) +
		MM_GETSTATS(pStats->ifHCOutMulticastPkts) +
		MM_GETSTATS(pStats->ifHCOutBroadcastPkts);
	/* There counters seem to be innacurate. Use byte number accumulation 
	   instead.
	   p_netstats->rx_bytes = MM_GETSTATS(pStats->ifHCInOctets);
	   p_netstats->tx_bytes = MM_GETSTATS(pStats->ifHCOutOctets);
	*/
	p_netstats->tx_errors =
		MM_GETSTATS(pStats->dot3StatsInternalMacTransmitErrors) +
		MM_GETSTATS(pStats->dot3StatsCarrierSenseErrors) +
		MM_GETSTATS(pStats->ifOutDiscards) +
		MM_GETSTATS(pStats->ifOutErrors);
	p_netstats->multicast = MM_GETSTATS(pStats->ifHCInMulticastPkts);
	p_netstats->collisions = MM_GETSTATS(pStats->etherStatsCollisions);
	p_netstats->rx_length_errors =
		MM_GETSTATS(pStats->dot3StatsFramesTooLong) +
		MM_GETSTATS(pStats->etherStatsUndersizePkts);
	p_netstats->rx_over_errors = MM_GETSTATS(pStats->nicNoMoreRxBDs);
	p_netstats->rx_frame_errors =
		MM_GETSTATS(pStats->dot3StatsAlignmentErrors);
	p_netstats->rx_crc_errors = (unsigned long)
		bcm5700_crc_count(pUmDevice);
	p_netstats->rx_errors = (unsigned long)
		bcm5700_rx_err_count(pUmDevice);

	p_netstats->tx_aborted_errors = MM_GETSTATS(pStats->ifOutDiscards);
	p_netstats->tx_carrier_errors =
		MM_GETSTATS(pStats->dot3StatsCarrierSenseErrors);

	return p_netstats;
}

void
b57_suspend_chip(UM_DEVICE_BLOCK *pUmDevice)
{
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;

	if (pUmDevice->opened) {
		bcm5700_intr_off(pUmDevice);
		netif_carrier_off(pUmDevice->dev);
		netif_stop_queue(pUmDevice->dev);
#ifdef BCM_TASKLET
		tasklet_kill(&pUmDevice->tasklet);
#endif
		bcm5700_poll_wait(pUmDevice);
	}
	pUmDevice->suspended = 1;
	LM_ShutdownChip(pDevice, LM_SUSPEND_RESET);
}

void
b57_resume_chip(UM_DEVICE_BLOCK *pUmDevice)
{
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;

	if (pUmDevice->suspended) {
		pUmDevice->suspended = 0;
		if (pUmDevice->opened) {
			bcm5700_reset(pUmDevice->dev);
		}
		else {
			LM_ShutdownChip(pDevice, LM_SHUTDOWN_RESET);
		}
	}
}

/* Returns 0 on failure, 1 on success */
int
b57_test_intr(UM_DEVICE_BLOCK *pUmDevice)
{
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;
	int j;

	if (!pUmDevice->opened)
		return 0;
	pUmDevice->intr_test_result = 0;
	pUmDevice->intr_test = 1;

	REG_WR(pDevice, HostCoalesce.Mode,
		pDevice->CoalesceMode | HOST_COALESCE_ENABLE |
		HOST_COALESCE_NOW);

	for (j = 0; j < 10; j++) {
		if (pUmDevice->intr_test_result){
			break;
		}

		REG_WR(pDevice, HostCoalesce.Mode,
		pDevice->CoalesceMode | HOST_COALESCE_ENABLE |
		HOST_COALESCE_NOW);

		MM_Sleep(pDevice, 1);
	}

	return pUmDevice->intr_test_result;

}

#ifdef SIOCETHTOOL

#ifdef ETHTOOL_GSTRINGS

#define ETH_NUM_STATS 30
#define RX_CRC_IDX 5
#define RX_MAC_ERR_IDX 14

struct {
	char string[ETH_GSTRING_LEN];
} bcm5700_stats_str_arr[ETH_NUM_STATS] = {
	{ "rx_unicast_packets" },
	{ "rx_multicast_packets" },
	{ "rx_broadcast_packets" },
	{ "rx_bytes" },
	{ "rx_fragments" },
	{ "rx_crc_errors" },	/* this needs to be calculated */
	{ "rx_align_errors" },
	{ "rx_xon_frames" },
	{ "rx_xoff_frames" },
	{ "rx_long_frames" },
	{ "rx_short_frames" },
	{ "rx_jabber" },
	{ "rx_discards" },
	{ "rx_errors" },
	{ "rx_mac_errors" },	/* this needs to be calculated */
	{ "tx_unicast_packets" },
	{ "tx_multicast_packets" },
	{ "tx_broadcast_packets" },
	{ "tx_bytes" },
	{ "tx_deferred" },
	{ "tx_single_collisions" },
	{ "tx_multi_collisions" },
	{ "tx_total_collisions" },
	{ "tx_excess_collisions" },
	{ "tx_late_collisions" },
	{ "tx_xon_frames" },
	{ "tx_xoff_frames" },
	{ "tx_internal_mac_errors" },
	{ "tx_carrier_errors" },
	{ "tx_errors" },
};

#define STATS_OFFSET(offset_name) ((OFFSETOF(T3_STATS_BLOCK, offset_name)) / sizeof(uint64_t))

#ifdef __BIG_ENDIAN
#define SWAP_DWORD_64(x) (x)
#else
#define SWAP_DWORD_64(x) ((x << 32) | (x >> 32))
#endif

unsigned long bcm5700_stats_offset_arr[ETH_NUM_STATS] = {
	STATS_OFFSET(ifHCInUcastPkts),
	STATS_OFFSET(ifHCInMulticastPkts),
	STATS_OFFSET(ifHCInBroadcastPkts),
	STATS_OFFSET(ifHCInOctets),
	STATS_OFFSET(etherStatsFragments),
	0,
	STATS_OFFSET(dot3StatsAlignmentErrors),
	STATS_OFFSET(xonPauseFramesReceived),
	STATS_OFFSET(xoffPauseFramesReceived),
	STATS_OFFSET(dot3StatsFramesTooLong),
	STATS_OFFSET(etherStatsUndersizePkts),
	STATS_OFFSET(etherStatsJabbers),
	STATS_OFFSET(ifInDiscards),
	STATS_OFFSET(ifInErrors),
	0,
	STATS_OFFSET(ifHCOutUcastPkts),
	STATS_OFFSET(ifHCOutMulticastPkts),
	STATS_OFFSET(ifHCOutBroadcastPkts),
	STATS_OFFSET(ifHCOutOctets),
	STATS_OFFSET(dot3StatsDeferredTransmissions),
	STATS_OFFSET(dot3StatsSingleCollisionFrames),
	STATS_OFFSET(dot3StatsMultipleCollisionFrames),
	STATS_OFFSET(etherStatsCollisions),
	STATS_OFFSET(dot3StatsExcessiveCollisions),
	STATS_OFFSET(dot3StatsLateCollisions),
	STATS_OFFSET(outXonSent),
	STATS_OFFSET(outXoffSent),
	STATS_OFFSET(dot3StatsInternalMacTransmitErrors),
	STATS_OFFSET(dot3StatsCarrierSenseErrors),
	STATS_OFFSET(ifOutErrors),
};

#endif /* ETHTOOL_GSTRINGS */


#ifdef ETHTOOL_GREGS
#if (LINUX_VERSION_CODE >= 0x02040f)
static void
bcm5700_get_reg_blk(UM_DEVICE_BLOCK *pUmDevice, u32 **buf, u32 start, u32 end,
		int reserved)
{
	u32 offset;
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;

	if (reserved) {
		memset(*buf, 0, end - start);
		*buf = *buf + (end - start)/4;
		return;
	}
	for (offset = start; offset < end; offset+=4, *buf = *buf + 1) {
		if (T3_ASIC_IS_5705_BEYOND(pDevice->ChipRevId)){
			if (((offset >= 0x3400) && (offset < 0x3c00)) ||
				((offset >= 0x5400) && (offset < 0x5800)) ||
				((offset >= 0x6400) && (offset < 0x6800))) {
				**buf = 0;
				continue;
			}
		}
		**buf = REG_RD_OFFSET(pDevice, offset);
	}
}
#endif
#endif

static int netdev_ethtool_ioctl(struct net_device *dev, void *useraddr)
{
	struct ethtool_cmd ethcmd;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;

	if (mm_copy_from_user(&ethcmd, useraddr, sizeof(ethcmd)))
		return -EFAULT;

        switch (ethcmd.cmd) {
#ifdef ETHTOOL_GDRVINFO
        case ETHTOOL_GDRVINFO: {
		struct ethtool_drvinfo info = {ETHTOOL_GDRVINFO};

		strcpy(info.driver,  bcm5700_driver);
#ifdef INCLUDE_5701_AX_FIX
		if(pDevice->ChipRevId == T3_CHIP_ID_5701_A0) {
			extern int t3FwReleaseMajor;
			extern int t3FwReleaseMinor;
			extern int t3FwReleaseFix;

			sprintf(info.fw_version, "%i.%i.%i",
				t3FwReleaseMajor, t3FwReleaseMinor,
				t3FwReleaseFix);
		}
#endif
		strcpy(info.fw_version, pDevice->BootCodeVer);
		strcpy(info.version, bcm5700_version);
#if (LINUX_VERSION_CODE <= 0x020422)
		strcpy(info.bus_info, pUmDevice->pdev->slot_name);
#else
		strcpy(info.bus_info, pci_name(pUmDevice->pdev));
#endif



#ifdef ETHTOOL_GEEPROM
		BCM_EEDUMP_LEN(&info, pDevice->NvramSize);
#endif
#ifdef ETHTOOL_GREGS
		/* dump everything, including holes in the register space */
		info.regdump_len = 0x6c00;
#endif
#ifdef ETHTOOL_GSTATS
		info.n_stats = ETH_NUM_STATS;
#endif
		if (mm_copy_to_user(useraddr, &info, sizeof(info)))
			return -EFAULT;
		return 0;
	}
#endif
        case ETHTOOL_GSET: {
		if ((pDevice->TbiFlags & ENABLE_TBI_FLAG)||
			(pDevice->PhyFlags & PHY_IS_FIBER)) {
			ethcmd.supported =
				(SUPPORTED_1000baseT_Full |
				SUPPORTED_Autoneg);
			ethcmd.supported |= SUPPORTED_FIBRE;
			ethcmd.port = PORT_FIBRE;
		} else {
			ethcmd.supported =
				(SUPPORTED_10baseT_Half |
				SUPPORTED_10baseT_Full |
				SUPPORTED_100baseT_Half |
				SUPPORTED_100baseT_Full |
				SUPPORTED_1000baseT_Half |
				SUPPORTED_1000baseT_Full |
				SUPPORTED_Autoneg);
			ethcmd.supported |= SUPPORTED_TP;
			ethcmd.port = PORT_TP;
		}

		ethcmd.transceiver = XCVR_INTERNAL;
		ethcmd.phy_address = 0;

		if (pDevice->LineSpeed == LM_LINE_SPEED_1000MBPS)
			ethcmd.speed = SPEED_1000;
		else if (pDevice->LineSpeed == LM_LINE_SPEED_100MBPS)
			ethcmd.speed = SPEED_100;
		else if (pDevice->LineSpeed == LM_LINE_SPEED_10MBPS)
			ethcmd.speed = SPEED_10;
		else
			ethcmd.speed = 0;

		if (pDevice->DuplexMode == LM_DUPLEX_MODE_FULL)
			ethcmd.duplex = DUPLEX_FULL;
		else
			ethcmd.duplex = DUPLEX_HALF;

		if (pDevice->DisableAutoNeg == FALSE) {
			ethcmd.autoneg = AUTONEG_ENABLE;
			ethcmd.advertising = ADVERTISED_Autoneg;
			if ((pDevice->TbiFlags & ENABLE_TBI_FLAG) ||
				(pDevice->PhyFlags & PHY_IS_FIBER)) {
				ethcmd.advertising |=
					ADVERTISED_1000baseT_Full |
					ADVERTISED_FIBRE;
			}
			else {
				ethcmd.advertising |=
					ADVERTISED_TP;
				if (pDevice->advertising &
					PHY_AN_AD_10BASET_HALF) {

					ethcmd.advertising |=
						ADVERTISED_10baseT_Half;
				}
				if (pDevice->advertising &
					PHY_AN_AD_10BASET_FULL) {

					ethcmd.advertising |=
						ADVERTISED_10baseT_Full;
				}
				if (pDevice->advertising &
					PHY_AN_AD_100BASETX_HALF) {

					ethcmd.advertising |=
						ADVERTISED_100baseT_Half;
				}
				if (pDevice->advertising &
					PHY_AN_AD_100BASETX_FULL) {

					ethcmd.advertising |=
						ADVERTISED_100baseT_Full;
				}
				if (pDevice->advertising1000 &
					BCM540X_AN_AD_1000BASET_HALF) {

					ethcmd.advertising |=
						ADVERTISED_1000baseT_Half;
				}
				if (pDevice->advertising1000 &
					BCM540X_AN_AD_1000BASET_FULL) {

					ethcmd.advertising |=
						ADVERTISED_1000baseT_Full;
				}
			}
		}
		else {
			ethcmd.autoneg = AUTONEG_DISABLE;
			ethcmd.advertising = 0;
		}

		ethcmd.maxtxpkt = pDevice->TxMaxCoalescedFrames;
		ethcmd.maxrxpkt = pDevice->RxMaxCoalescedFrames;

		if(mm_copy_to_user(useraddr, &ethcmd, sizeof(ethcmd)))
			return -EFAULT;
		return 0;
	}
	case ETHTOOL_SSET: {
		unsigned long flags;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (ethcmd.autoneg == AUTONEG_ENABLE) {
			pDevice->RequestedLineSpeed = LM_LINE_SPEED_AUTO;
			pDevice->RequestedDuplexMode = LM_DUPLEX_MODE_UNKNOWN;
			pDevice->DisableAutoNeg = FALSE;
		}
		else {
			if (ethcmd.speed == SPEED_1000 &&
				pDevice->PhyFlags & PHY_NO_GIGABIT)
					return -EINVAL;

			if (ethcmd.speed == SPEED_1000 &&
			    (pDevice->TbiFlags & ENABLE_TBI_FLAG ||
			     pDevice->PhyFlags & PHY_IS_FIBER ) ) {

				pDevice->RequestedLineSpeed =
					LM_LINE_SPEED_1000MBPS;

				pDevice->RequestedDuplexMode =
					LM_DUPLEX_MODE_FULL;
			}
			else if (ethcmd.speed == SPEED_100 &&
			        !(pDevice->TbiFlags & ENABLE_TBI_FLAG) &&
			     	!(pDevice->PhyFlags & PHY_IS_FIBER)) {

				pDevice->RequestedLineSpeed =
					LM_LINE_SPEED_100MBPS;
			}
			else if (ethcmd.speed == SPEED_10  &&
			        !(pDevice->TbiFlags & ENABLE_TBI_FLAG) &&
			     	!(pDevice->PhyFlags & PHY_IS_FIBER)) {

                                pDevice->RequestedLineSpeed =
					LM_LINE_SPEED_10MBPS;
			}
			else {
				return -EINVAL;
			}

			pDevice->DisableAutoNeg = TRUE;
			if (ethcmd.duplex == DUPLEX_FULL) {
				pDevice->RequestedDuplexMode =
					LM_DUPLEX_MODE_FULL;
			}
			else {
				if (!(pDevice->TbiFlags & ENABLE_TBI_FLAG) &&
			     	    !(pDevice->PhyFlags & PHY_IS_FIBER)  ) {

					pDevice->RequestedDuplexMode =
							LM_DUPLEX_MODE_HALF;
				}
			}
		}
		if (netif_running(dev)) {
			BCM5700_PHY_LOCK(pUmDevice, flags);
			LM_SetupPhy(pDevice);
			BCM5700_PHY_UNLOCK(pUmDevice, flags);
		}
		return 0;
	}
#ifdef ETHTOOL_GWOL
#ifdef BCM_WOL
	case ETHTOOL_GWOL: {
		struct ethtool_wolinfo wol = {ETHTOOL_GWOL};

		if (((pDevice->TbiFlags & ENABLE_TBI_FLAG) &&
			!(pDevice->Flags & FIBER_WOL_CAPABLE_FLAG)) ||
			(pDevice->Flags & DISABLE_D3HOT_FLAG)) {
			wol.supported = 0;
			wol.wolopts = 0;
		}
		else {
			wol.supported = WAKE_MAGIC;
			if (pDevice->WakeUpMode == LM_WAKE_UP_MODE_MAGIC_PACKET)
			{
				wol.wolopts = WAKE_MAGIC;
			}
			else {
				wol.wolopts = 0;
			}
		}
		if (mm_copy_to_user(useraddr, &wol, sizeof(wol)))
			return -EFAULT;
		return 0;
	}
	case ETHTOOL_SWOL: {
		struct ethtool_wolinfo wol;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (mm_copy_from_user(&wol, useraddr, sizeof(wol)))
			return -EFAULT;
		if ((((pDevice->TbiFlags & ENABLE_TBI_FLAG) &&
			!(pDevice->Flags & FIBER_WOL_CAPABLE_FLAG)) ||
			(pDevice->Flags & DISABLE_D3HOT_FLAG)) &&
			wol.wolopts) {
			return -EINVAL;
		}

		if ((wol.wolopts & ~WAKE_MAGIC) != 0) {
			return -EINVAL;
		}
		if (wol.wolopts & WAKE_MAGIC) {
			pDevice->WakeUpModeCap = LM_WAKE_UP_MODE_MAGIC_PACKET;
			pDevice->WakeUpMode = LM_WAKE_UP_MODE_MAGIC_PACKET;
		}
		else {
			pDevice->WakeUpModeCap = LM_WAKE_UP_MODE_NONE;
			pDevice->WakeUpMode = LM_WAKE_UP_MODE_NONE;
		}
		return 0;
        }
#endif
#endif
#ifdef ETHTOOL_GLINK
	case ETHTOOL_GLINK: {
		struct ethtool_value edata = {ETHTOOL_GLINK};

		/* ifup only waits for 5 seconds for link up */
		/* NIC may take more than 5 seconds to establish link */
		if ((pUmDevice->delayed_link_ind > 0) &&
			delay_link[pUmDevice->index])
			return -EOPNOTSUPP;

		if (pDevice->LinkStatus == LM_STATUS_LINK_ACTIVE) {
			edata.data =  1;
		}
		else {
			edata.data =  0;
		}
		if (mm_copy_to_user(useraddr, &edata, sizeof(edata)))
			return -EFAULT;
		return 0;
	}
#endif
#ifdef ETHTOOL_NWAY_RST
	case ETHTOOL_NWAY_RST: {
		LM_UINT32 phyctrl;
		unsigned long flags;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (pDevice->DisableAutoNeg) {
			return -EINVAL;
		}
		if (!netif_running(dev))
			return -EAGAIN;
		BCM5700_PHY_LOCK(pUmDevice, flags);
		if (pDevice->TbiFlags & ENABLE_TBI_FLAG) { 
			pDevice->RequestedLineSpeed = LM_LINE_SPEED_1000MBPS;
			pDevice->DisableAutoNeg = TRUE;
			LM_SetupPhy(pDevice);

			pDevice->RequestedLineSpeed = LM_LINE_SPEED_AUTO;
			pDevice->DisableAutoNeg = FALSE;
			LM_SetupPhy(pDevice);
		}
		else {
			if ((T3_ASIC_REV(pDevice->ChipRevId) ==
					T3_ASIC_REV_5703) ||
				(T3_ASIC_REV(pDevice->ChipRevId) ==
					T3_ASIC_REV_5704) ||
				(T3_ASIC_REV(pDevice->ChipRevId) ==
					T3_ASIC_REV_5705))
			{
				LM_ResetPhy(pDevice);
				LM_SetupPhy(pDevice);
			}
			pDevice->PhyFlags &= ~PHY_FIBER_FALLBACK;
			LM_ReadPhy(pDevice, PHY_CTRL_REG, &phyctrl);
			LM_WritePhy(pDevice, PHY_CTRL_REG, phyctrl |
				PHY_CTRL_AUTO_NEG_ENABLE |
				PHY_CTRL_RESTART_AUTO_NEG);
		}
		BCM5700_PHY_UNLOCK(pUmDevice, flags);
		return 0;
	}
#endif
#ifdef ETHTOOL_GEEPROM
	case ETHTOOL_GEEPROM: {
		struct ethtool_eeprom eeprom;
		LM_UINT32 *buf = 0;
		LM_UINT32 buf1[64/4];
		int i, j, offset, len;

		if (mm_copy_from_user(&eeprom, useraddr, sizeof(eeprom)))
			return -EFAULT;

		if (eeprom.offset >= pDevice->NvramSize)
			return -EFAULT;

		/* maximum data limited */
		/* to read more, call again with a different offset */
		if (eeprom.len > 0x800) {
			eeprom.len = 0x800;
			if (mm_copy_to_user(useraddr, &eeprom, sizeof(eeprom)))
				return -EFAULT;
		}

		if (eeprom.len > 64) {
			buf = kmalloc(eeprom.len, GFP_KERNEL);
			if (!buf)
				return -ENOMEM;
		}
		else {
			buf = buf1;
		}
		useraddr += offsetof(struct ethtool_eeprom, data);

		offset = eeprom.offset;
		len = eeprom.len;
		if (offset & 3) {
			offset &= 0xfffffffc;
			len += (offset & 3);
		}
		len = (len + 3) & 0xfffffffc;
		for (i = 0, j = 0; j < len; i++, j += 4) {
			if (LM_NvramRead(pDevice, offset + j, buf + i) !=
				LM_STATUS_SUCCESS) {
				break;
			}
		}
		if (j >= len) {
			buf += (eeprom.offset & 3);
			i = mm_copy_to_user(useraddr, buf, eeprom.len);
		}
		if (eeprom.len > 64) {
			kfree(buf);
		}
		if ((j < len) || i)
			return -EFAULT;
		return 0;
	}
	case ETHTOOL_SEEPROM: {
		struct ethtool_eeprom eeprom;
		LM_UINT32 buf[64/4];
		int i, offset, len;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (mm_copy_from_user(&eeprom, useraddr, sizeof(eeprom)))
			return -EFAULT;

		if ((eeprom.offset & 3) || (eeprom.len & 3) ||
			(eeprom.offset >= pDevice->NvramSize)) {
			return -EFAULT;
		}

		if ((eeprom.offset + eeprom.len) >= pDevice->NvramSize) {
			eeprom.len = pDevice->NvramSize - eeprom.offset;
		}

		useraddr += offsetof(struct ethtool_eeprom, data);

		len = eeprom.len;
		offset = eeprom.offset;
		for (; len > 0; ) {
			if (len < 64)
				i = len;
			else
				i = 64;
			if (mm_copy_from_user(&buf, useraddr, i))
				return -EFAULT;

			bcm5700_intr_off(pUmDevice);
			/* Prevent race condition on Grc.Mode register */
			bcm5700_poll_wait(pUmDevice);

			if (LM_NvramWriteBlock(pDevice, offset, buf, i/4) !=
				LM_STATUS_SUCCESS) {
				bcm5700_intr_on(pUmDevice);
				return -EFAULT;
			}
			bcm5700_intr_on(pUmDevice);
			len -= i;
			offset += i;
			useraddr += i;
		}
		return 0;
	}
#endif
#ifdef ETHTOOL_GREGS
#if (LINUX_VERSION_CODE >= 0x02040f)
	case ETHTOOL_GREGS: {
		struct ethtool_regs eregs;
		LM_UINT32 *buf, *buf1;
		unsigned int i;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (pDevice->Flags & UNDI_FIX_FLAG)
			return -EOPNOTSUPP;
		if (mm_copy_from_user(&eregs, useraddr, sizeof(eregs)))
			return -EFAULT;
		if (eregs.len > 0x6c00)
			eregs.len = 0x6c00;
		eregs.version = 0x0;
		if (mm_copy_to_user(useraddr, &eregs, sizeof(eregs)))
			return -EFAULT;
		buf = buf1 = kmalloc(eregs.len, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;
		bcm5700_get_reg_blk(pUmDevice, &buf, 0,      0xb0,   0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0xb0,   0x200,  1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x200,  0x8f0,  0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x8f0,  0xc00,  1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0xc00,  0xce0,  0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0xce0,  0x1000, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x1000, 0x1004, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x1004, 0x1400, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x1400, 0x1480, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x1480, 0x1800, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x1800, 0x1848, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x1848, 0x1c00, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x1c00, 0x1c04, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x1c04, 0x2000, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x2000, 0x225c, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x225c, 0x2400, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x2400, 0x24c4, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x24c4, 0x2800, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x2800, 0x2804, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x2804, 0x2c00, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x2c00, 0x2c20, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x2c20, 0x3000, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x3000, 0x3014, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x3014, 0x3400, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x3400, 0x3408, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x3408, 0x3800, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x3800, 0x3808, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x3808, 0x3c00, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x3c00, 0x3d00, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x3d00, 0x4000, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x4000, 0x4010, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x4010, 0x4400, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x4400, 0x4458, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x4458, 0x4800, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x4800, 0x4808, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x4808, 0x4c00, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x4c00, 0x4c08, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x4c08, 0x5000, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x5000, 0x5050, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x5050, 0x5400, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x5400, 0x5450, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x5450, 0x5800, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x5800, 0x5a10, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x5a10, 0x6000, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x6000, 0x600c, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x600c, 0x6400, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x6400, 0x6404, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x6404, 0x6800, 1);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x6800, 0x6848, 0);
		bcm5700_get_reg_blk(pUmDevice, &buf, 0x6848, 0x6c00, 1);

		i = mm_copy_to_user(useraddr + sizeof(eregs), buf1, eregs.len);
		kfree(buf1);
		if (i)
			return -EFAULT;
		return 0;
	}
#endif
#endif
#ifdef ETHTOOL_GPAUSEPARAM
	case ETHTOOL_GPAUSEPARAM: {
		struct ethtool_pauseparam epause = { ETHTOOL_GPAUSEPARAM };

		if (!pDevice->DisableAutoNeg) {
			epause.autoneg = (pDevice->FlowControlCap &
				LM_FLOW_CONTROL_AUTO_PAUSE) != 0;
		}
		else {
			epause.autoneg = 0;
		}
		epause.rx_pause =
			(pDevice->FlowControl &
			LM_FLOW_CONTROL_RECEIVE_PAUSE) != 0;
		epause.tx_pause =
			(pDevice->FlowControl &
			LM_FLOW_CONTROL_TRANSMIT_PAUSE) != 0;
		if (mm_copy_to_user(useraddr, &epause, sizeof(epause)))
			return -EFAULT;

		return 0;
	}
	case ETHTOOL_SPAUSEPARAM: {
		struct ethtool_pauseparam epause;
		unsigned long flags;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (mm_copy_from_user(&epause, useraddr, sizeof(epause)))
			return -EFAULT;
		pDevice->FlowControlCap = 0;
		if (epause.autoneg && !pDevice->DisableAutoNeg) {
			pDevice->FlowControlCap |= LM_FLOW_CONTROL_AUTO_PAUSE;
		}
		if (epause.rx_pause)  {
			pDevice->FlowControlCap |=
				LM_FLOW_CONTROL_RECEIVE_PAUSE;
		}
		if (epause.tx_pause)  {
			pDevice->FlowControlCap |=
				LM_FLOW_CONTROL_TRANSMIT_PAUSE;
		}
		if (netif_running(dev)) {
			BCM5700_PHY_LOCK(pUmDevice, flags);
			LM_SetupPhy(pDevice);
			BCM5700_PHY_UNLOCK(pUmDevice, flags);
		}

		return 0;
	}
#endif
#ifdef ETHTOOL_GRXCSUM
	case ETHTOOL_GRXCSUM: {
		struct ethtool_value edata = { ETHTOOL_GRXCSUM };

		edata.data =
			(pDevice->TaskToOffload &
			LM_TASK_OFFLOAD_RX_TCP_CHECKSUM) != 0;
		if (mm_copy_to_user(useraddr, &edata, sizeof(edata)))
			return -EFAULT;

		return 0;
	}
	case ETHTOOL_SRXCSUM: {
		struct ethtool_value edata;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (mm_copy_from_user(&edata, useraddr, sizeof(edata)))
			return -EFAULT;
		if (edata.data) {
			if (!(pDevice->TaskOffloadCap &
				LM_TASK_OFFLOAD_TX_TCP_CHECKSUM)) {

				return -EINVAL;
			}
			pDevice->TaskToOffload |=
				LM_TASK_OFFLOAD_RX_TCP_CHECKSUM |
				LM_TASK_OFFLOAD_RX_UDP_CHECKSUM;
		}
		else {
			pDevice->TaskToOffload &=
				~(LM_TASK_OFFLOAD_RX_TCP_CHECKSUM |
				LM_TASK_OFFLOAD_RX_UDP_CHECKSUM);
		}
		return 0;
	}
	case ETHTOOL_GTXCSUM: {
		struct ethtool_value edata = { ETHTOOL_GTXCSUM };

		edata.data =
			(dev->features & get_csum_flag( pDevice->ChipRevId)) != 0;
		if (mm_copy_to_user(useraddr, &edata, sizeof(edata)))
			return -EFAULT;

		return 0;
	}
	case ETHTOOL_STXCSUM: {
		struct ethtool_value edata;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (mm_copy_from_user(&edata, useraddr, sizeof(edata)))
			return -EFAULT;
		if (edata.data) {
			if (!(pDevice->TaskOffloadCap &
				LM_TASK_OFFLOAD_TX_TCP_CHECKSUM)) {

				return -EINVAL;
			}
			dev->features |= get_csum_flag( pDevice->ChipRevId);
			pDevice->TaskToOffload |=
				LM_TASK_OFFLOAD_TX_TCP_CHECKSUM |
				LM_TASK_OFFLOAD_TX_UDP_CHECKSUM;
		}
		else {
			dev->features &= ~get_csum_flag( pDevice->ChipRevId);
			pDevice->TaskToOffload &=
				~(LM_TASK_OFFLOAD_TX_TCP_CHECKSUM |
				LM_TASK_OFFLOAD_TX_UDP_CHECKSUM);
		}
		return 0;
	}
	case ETHTOOL_GSG: {
		struct ethtool_value edata = { ETHTOOL_GSG };

		edata.data =
			(dev->features & NETIF_F_SG) != 0;
		if (mm_copy_to_user(useraddr, &edata, sizeof(edata)))
			return -EFAULT;
		return 0;
	}
	case ETHTOOL_SSG: {
		struct ethtool_value edata;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (mm_copy_from_user(&edata, useraddr, sizeof(edata)))
			return -EFAULT;
		if (edata.data) {
			dev->features |= NETIF_F_SG;
		}
		else {
			dev->features &= ~NETIF_F_SG;
		}
		return 0;
	}
#endif
#ifdef ETHTOOL_GRINGPARAM
	case ETHTOOL_GRINGPARAM: {
		struct ethtool_ringparam ering = { ETHTOOL_GRINGPARAM };

		ering.rx_max_pending = T3_STD_RCV_RCB_ENTRY_COUNT - 1;
		ering.rx_pending = pDevice->RxStdDescCnt;
		ering.rx_mini_max_pending = 0;
		ering.rx_mini_pending = 0;
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
		ering.rx_jumbo_max_pending = T3_JUMBO_RCV_RCB_ENTRY_COUNT - 1;
		ering.rx_jumbo_pending = pDevice->RxJumboDescCnt;
#else
		ering.rx_jumbo_max_pending = 0;
		ering.rx_jumbo_pending = 0;
#endif
		ering.tx_max_pending = MAX_TX_PACKET_DESC_COUNT - 1;
		ering.tx_pending = pDevice->TxPacketDescCnt;
		if (mm_copy_to_user(useraddr, &ering, sizeof(ering)))
			return -EFAULT;
		return 0;
	}
#endif
#ifdef ETHTOOL_PHYS_ID
	case ETHTOOL_PHYS_ID: {
		struct ethtool_value edata;

		if(!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (mm_copy_from_user(&edata, useraddr, sizeof(edata)))
			return -EFAULT;
		if (LM_BlinkLED(pDevice, edata.data) == LM_STATUS_SUCCESS)
			return 0;
		return -EINTR;
	}
#endif
#ifdef ETHTOOL_GSTRINGS
	case ETHTOOL_GSTRINGS: {
		struct ethtool_gstrings egstr = { ETHTOOL_GSTRINGS };

		if (mm_copy_from_user(&egstr, useraddr, sizeof(egstr)))
			return -EFAULT;
		switch(egstr.string_set) {
#ifdef ETHTOOL_GSTATS
		case ETH_SS_STATS:
			egstr.len = ETH_NUM_STATS;
			if (mm_copy_to_user(useraddr, &egstr, sizeof(egstr)))
				return -EFAULT;
			if (mm_copy_to_user(useraddr + sizeof(egstr),
				bcm5700_stats_str_arr,
				sizeof(bcm5700_stats_str_arr)))
				return -EFAULT;
			return 0;
#endif
		default:
			return -EOPNOTSUPP;
		}
		}
#endif
#ifdef ETHTOOL_GSTATS
	case ETHTOOL_GSTATS: {
		struct ethtool_stats estats = { ETHTOOL_GSTATS };
		uint64_t stats[ETH_NUM_STATS];
		int i;
		uint64_t *pStats =
			(uint64_t *) pDevice->pStatsBlkVirt;

		estats.n_stats = ETH_NUM_STATS;
		if (pStats == 0) {
			memset(stats, 0, sizeof(stats));
		}
		else {

			for (i = 0; i < ETH_NUM_STATS; i++) {
				if (bcm5700_stats_offset_arr[i] != 0) {
					stats[i] = SWAP_DWORD_64(*(pStats +
						bcm5700_stats_offset_arr[i]));
				}
				else if (i == RX_CRC_IDX) {
					stats[i] =
						bcm5700_crc_count(pUmDevice);
				}
				else if (i == RX_MAC_ERR_IDX) {
					stats[i] =
						bcm5700_rx_err_count(pUmDevice);
				}
			}
		}
		if (mm_copy_to_user(useraddr, &estats, sizeof(estats))) {
			return -EFAULT;
		}
		if (mm_copy_to_user(useraddr + sizeof(estats), &stats,
			sizeof(stats))) {
			return -EFAULT;
		}
		return 0;
	}
#endif
#ifdef ETHTOOL_GTSO
	case ETHTOOL_GTSO: {
		struct ethtool_value edata = { ETHTOOL_GTSO };

#ifdef BCM_TSO
		edata.data =
			(dev->features & NETIF_F_TSO) != 0;
#else
		edata.data = 0;
#endif
		if (mm_copy_to_user(useraddr, &edata, sizeof(edata)))
			return -EFAULT;
		return 0;
	}
#endif
#ifdef ETHTOOL_STSO
	case ETHTOOL_STSO: {
#ifdef BCM_TSO
		struct ethtool_value edata;

		if (!capable(CAP_NET_ADMIN))
			return -EPERM;

		if (mm_copy_from_user(&edata, useraddr, sizeof(edata)))
			return -EFAULT;

		if (!(pDevice->TaskToOffload &
			LM_TASK_OFFLOAD_TCP_SEGMENTATION)) {
			return -EINVAL;
		}

		dev->features &= ~NETIF_F_TSO;

		if (edata.data) {
			if (T3_ASIC_5714_FAMILY(pDevice->ChipRevId) &&
			   (dev->mtu > 1500)) {
				printk(KERN_ALERT "%s: Jumbo Frames and TSO cannot simultaneously be enabled. Jumbo Frames enabled. TSO disabled.\n", dev->name);
			return -EINVAL;
			} else {
				dev->features |= NETIF_F_TSO;
			}
	        }
		return 0;
#else
		return -EINVAL;
#endif
	}
#endif
	}

	return -EOPNOTSUPP;
}
#endif /* #ifdef SIOCETHTOOL */

#if (LINUX_VERSION_CODE >= 0x20400) && (LINUX_VERSION_CODE < 0x20600)
#include <linux/iobuf.h>
#endif

/* Provide ioctl() calls to examine the MII xcvr state. */
STATIC int bcm5700_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	u16 *data = (u16 *)&rq->ifr_data;
	u32 value = 0;
	u16 page_num =0, addr_num =0, len =0;
	unsigned long flags;

	switch(cmd) {
	case SIOCGREG_STATUS: //Get register
	{
		struct reg_ioctl_data *rdata =(struct reg_ioctl_data *)rq->ifr_data;
                robo_info_t *robo = (robo_info_t *)pUmDevice->robo;
		page_num = rdata->page_num;
		addr_num = rdata->addr_num;
		len = rdata->len;
                printk("b57um SIOCGREG_STATUS cmd page[0x%x]addr[0x%x]len[%d].\n",page_num,addr_num,len);
		if (len == 6)
		{
			ReadDataFromRegister(robo,page_num,addr_num,len,(void *)&rdata->val_out);
			printk("val[0x%04x-0x%04x-0x%04x].\n",rdata->val_out[0],rdata->val_out[1],rdata->val_out[2]);
		}
		else if (len == 8)
		{
			ReadDataFromRegister(robo,page_num,addr_num,len,(void *)&rdata->val_out);
                        printk("val[0x%04x%04x-0x%04x%04x].\n",rdata->val_out[0],rdata->val_out[1],
				rdata->val_out[2],rdata->val_out[3]);
		}
		else if (len == 4) 
		{
			ReadDataFromRegister(robo,page_num,addr_num,len,(void *)&rdata->val_out);
                        printk("val[0x%04x%04x].\n",rdata->val_out[0],rdata->val_out[1]);
		}
		else 
		{
                        ReadDataFromRegister(robo,page_num,addr_num,len,(void *)&rdata->val_out);
                        printk("val[0x%04x].\n",rdata->val_out[0]);

		}
                if (mm_copy_to_user(rq->ifr_data, rdata, sizeof(struct reg_ioctl_data)))
		{
			printk("Fail mm_copy_to_user.\n");
                        return -EFAULT;
		}
                return 0;
	}
	break;
	case SIOCSREG_STATUS://Set register
	{
                struct reg_ioctl_data * wdata =(struct reg_ioctl_data *)rq->ifr_data;
		len = wdata->len;
		page_num = wdata->page_num;
		addr_num = wdata->addr_num;
                robo_info_t *robo = (robo_info_t *)pUmDevice->robo;
                if (len == 6)
                {
                        WriteDataToRegister(robo,page_num,addr_num,len,(void *)&wdata->val_in);
                        //printk("val[0x%04x-0x%04x-0x%04x].\n",val48[0],val48[1],val48[2]);
                }
                else if (len == 8)
                {
                        WriteDataToRegister(robo,page_num,addr_num,len,(void *)&wdata->val_in);
                        //printk("val[0x%04x-0x%04x-0x%04x-0x%04x].\n",val64[0],val64[1],val64[2],val64[3]);
                }
                else if (len == 4)
                {
                        WriteDataToRegister(robo,page_num,addr_num,len,(void *)&wdata->val_in);
                        //printk("val[0x%08x].\n",value);
                }
		else
		{
                        WriteDataToRegister(robo,page_num,addr_num,len,(void *)&wdata->val_in);
                        //printk("len[%d] val[0x%04x].\n",len,val16);
		}

                return 0;
	}
	break;
#ifdef SIOCGMIIPHY
	case SIOCGMIIPHY:		/* Get the address of the PHY in use. */

		data[0] = pDevice->PhyAddr;
		return 0;
#endif

#ifdef SIOCGMIIREG
	case SIOCGMIIREG:		/* Read the specified MII register. */
	{
		uint32 savephyaddr = 0;

		if (pDevice->TbiFlags & ENABLE_TBI_FLAG)
			return -EOPNOTSUPP;

		/* ifup only waits for 5 seconds for link up */
		/* NIC may take more than 5 seconds to establish link */
		if ((pUmDevice->delayed_link_ind > 0) &&
			delay_link[pUmDevice->index]) {
			return -EAGAIN;
		}

		BCM5700_PHY_LOCK(pUmDevice, flags);
		if (data[0] != 0xffff) {
			savephyaddr = pDevice->PhyAddr;
			pDevice->PhyAddr = data[0];
		}
		LM_ReadPhy(pDevice, data[1] & 0x1f, (LM_UINT32 *)&value);
		if (data[0] != 0xffff)
			pDevice->PhyAddr = savephyaddr;
		BCM5700_PHY_UNLOCK(pUmDevice, flags);
		data[3] = value & 0xffff;
		return 0;
	}
#endif

#ifdef SIOCSMIIREG
	case SIOCSMIIREG:		/* Write the specified MII register */
	{
		uint32 savephyaddr = 0;

		if (!capable(CAP_NET_ADMIN))
			return -EPERM;

		if (pDevice->TbiFlags & ENABLE_TBI_FLAG)
			return -EOPNOTSUPP;

		BCM5700_PHY_LOCK(pUmDevice, flags);
		if (data[0] != 0xffff) {
			savephyaddr = pDevice->PhyAddr;
			pDevice->PhyAddr = data[0];
		}
		LM_WritePhy(pDevice, data[1] & 0x1f, data[2]);
		if (data[0] != 0xffff)
			pDevice->PhyAddr = savephyaddr;
		BCM5700_PHY_UNLOCK(pUmDevice, flags);
		data[3] = 0;
		return 0;
	}
#endif

#ifdef SIOCETHTOOL
	case SIOCETHTOOL:
		return netdev_ethtool_ioctl(dev, (void *) rq->ifr_data);
#endif
	default:
		return -EOPNOTSUPP;
	}
	return -EOPNOTSUPP;
}

STATIC void bcm5700_do_rx_mode(struct net_device *dev)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	int i;
	struct dev_mc_list *mclist;

	LM_MulticastClear(pDevice);
	for (i = 0, mclist = dev->mc_list; mclist && i < dev->mc_count;
			 i++, mclist = mclist->next) {
		LM_MulticastAdd(pDevice, (PLM_UINT8) &mclist->dmi_addr);
	}
	if (dev->flags & IFF_ALLMULTI) {
		if (!(pDevice->ReceiveMask & LM_ACCEPT_ALL_MULTICAST)) {
			LM_SetReceiveMask(pDevice,
				pDevice->ReceiveMask | LM_ACCEPT_ALL_MULTICAST);
		}
	}
	else if (pDevice->ReceiveMask & LM_ACCEPT_ALL_MULTICAST) {
		LM_SetReceiveMask(pDevice,
			pDevice->ReceiveMask & ~LM_ACCEPT_ALL_MULTICAST);
	}
	if (dev->flags & IFF_PROMISC) {
		if (!(pDevice->ReceiveMask & LM_PROMISCUOUS_MODE)) {
			LM_SetReceiveMask(pDevice,
				pDevice->ReceiveMask | LM_PROMISCUOUS_MODE);
		}
	}
	else if (pDevice->ReceiveMask & LM_PROMISCUOUS_MODE) {
		LM_SetReceiveMask(pDevice,
			pDevice->ReceiveMask & ~LM_PROMISCUOUS_MODE);
	}

}

STATIC void bcm5700_set_rx_mode(struct net_device *dev)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	int i;
	struct dev_mc_list *mclist;
	unsigned long flags;

	BCM5700_PHY_LOCK(pUmDevice, flags);

	LM_MulticastClear(pDevice);
	for (i = 0, mclist = dev->mc_list; mclist && i < dev->mc_count;
			 i++, mclist = mclist->next) {
		LM_MulticastAdd(pDevice, (PLM_UINT8) &mclist->dmi_addr);
	}
	if (dev->flags & IFF_ALLMULTI) {
		if (!(pDevice->ReceiveMask & LM_ACCEPT_ALL_MULTICAST)) {
			LM_SetReceiveMask(pDevice,
				pDevice->ReceiveMask | LM_ACCEPT_ALL_MULTICAST);
		}
	}
	else if (pDevice->ReceiveMask & LM_ACCEPT_ALL_MULTICAST) {
		LM_SetReceiveMask(pDevice,
			pDevice->ReceiveMask & ~LM_ACCEPT_ALL_MULTICAST);
	}
	if (dev->flags & IFF_PROMISC) {
		if (!(pDevice->ReceiveMask & LM_PROMISCUOUS_MODE)) {
			LM_SetReceiveMask(pDevice,
				pDevice->ReceiveMask | LM_PROMISCUOUS_MODE);
		}
	}
	else if (pDevice->ReceiveMask & LM_PROMISCUOUS_MODE) {
		LM_SetReceiveMask(pDevice,
			pDevice->ReceiveMask & ~LM_PROMISCUOUS_MODE);
	}

	BCM5700_PHY_UNLOCK(pUmDevice, flags);
}

/*
 * Set the hardware MAC address.
 */
STATIC int bcm5700_set_mac_addr(struct net_device *dev, void *p)
{
	struct sockaddr *addr=p;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) dev->priv;
	UM_DEVICE_BLOCK *pUmDevice = (UM_DEVICE_BLOCK *) pDevice;

	if(is_valid_ether_addr(addr->sa_data)){

	    memcpy(dev->dev_addr, addr->sa_data,dev->addr_len);
	    if (pUmDevice->opened)
	        LM_SetMacAddress(pDevice, dev->dev_addr);
			bcm_robo_set_macaddr(pUmDevice->robo, dev->dev_addr);
            return 0;
        }
	return -EINVAL;
}

#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
STATIC int bcm5700_change_mtu(struct net_device *dev, int new_mtu)
{
	int pkt_size = new_mtu + ETHERNET_PACKET_HEADER_SIZE;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK)dev->priv;
	PLM_DEVICE_BLOCK pDevice = &pUmDevice->lm_dev;
	unsigned long flags;
	int reinit = 0;

	if ((pkt_size < MIN_ETHERNET_PACKET_SIZE_NO_CRC) ||
		(pkt_size > MAX_ETHERNET_JUMBO_PACKET_SIZE_NO_CRC)) {

		return -EINVAL;
	}
	if ( !(pDevice->Flags & JUMBO_CAPABLE_FLAG)    &&
		(pkt_size > MAX_ETHERNET_PACKET_SIZE_NO_CRC) ) {

		return -EINVAL;
	}
	if (pUmDevice->suspended)
		return -EAGAIN;

	if (pUmDevice->opened && (new_mtu != dev->mtu) &&
		(pDevice->Flags & JUMBO_CAPABLE_FLAG)) {
		reinit = 1;
	}

	BCM5700_PHY_LOCK(pUmDevice, flags);
	if (reinit) {
		netif_stop_queue(dev);
		bcm5700_shutdown(pUmDevice);
		bcm5700_freemem(dev);
	}

	dev->mtu = new_mtu;
	if (pkt_size < MAX_ETHERNET_PACKET_SIZE_NO_CRC) {
		pDevice->RxMtu = pDevice->TxMtu =
			MAX_ETHERNET_PACKET_SIZE_NO_CRC;
	}
	else {
		pDevice->RxMtu = pDevice->TxMtu = pkt_size;
	}

	if (dev->mtu <= 1514)  {
		pDevice->RxJumboDescCnt = 0;
	}
	else if (pDevice->Flags & JUMBO_CAPABLE_FLAG){
		pDevice->RxJumboDescCnt =
			rx_jumbo_desc_cnt[pUmDevice->index];
	}
	pDevice->RxPacketDescCnt = pDevice->RxJumboDescCnt +
		pDevice->RxStdDescCnt;

	pDevice->RxJumboBufferSize = (pDevice->RxMtu + 8 /* CRC + VLAN */ +
		COMMON_CACHE_LINE_SIZE-1) & ~COMMON_CACHE_LINE_MASK;

#ifdef BCM_TSO
	if (T3_ASIC_5714_FAMILY(pDevice->ChipRevId) &&
	   (dev->mtu > 1514) ) {
		if (dev->features & NETIF_F_TSO) {
			dev->features &= ~NETIF_F_TSO;
			printk(KERN_ALERT "%s: TSO previously enabled. Jumbo Frames and TSO cannot simultaneously be enabled. Jumbo Frames enabled. TSO disabled.\n", dev->name);
		}
	}
#endif

	if (reinit) {
		LM_InitializeAdapter(pDevice);
		bcm5700_do_rx_mode(dev);
		bcm5700_set_vlan_mode(pUmDevice);
		bcm5700_init_counters(pUmDevice);
		if (memcmp(dev->dev_addr, pDevice->NodeAddress, 6)) {
			LM_SetMacAddress(pDevice, dev->dev_addr);
		}
		netif_start_queue(dev);
		bcm5700_intr_on(pUmDevice);
	}
	BCM5700_PHY_UNLOCK(pUmDevice, flags);

	return 0;
}
#endif


#if (LINUX_VERSION_CODE < 0x020300)
int
bcm5700_probe(struct net_device *dev)
{
	int cards_found = 0;
	struct pci_dev *pdev = NULL;
	struct pci_device_id *pci_tbl;
	u16 ssvid, ssid;

	if ( ! pci_present())
		return -ENODEV;

	pci_tbl = bcm5700_pci_tbl;
	while ((pdev = pci_find_class(PCI_CLASS_NETWORK_ETHERNET << 8, pdev))) {
		int idx;

		pci_read_config_word(pdev, PCI_SUBSYSTEM_VENDOR_ID, &ssvid);
		pci_read_config_word(pdev, PCI_SUBSYSTEM_ID, &ssid);
		for (idx = 0; pci_tbl[idx].vendor; idx++) {
			if ((pci_tbl[idx].vendor == PCI_ANY_ID ||
				pci_tbl[idx].vendor == pdev->vendor) &&
				(pci_tbl[idx].device == PCI_ANY_ID ||
				pci_tbl[idx].device == pdev->device) &&
				(pci_tbl[idx].subvendor == PCI_ANY_ID ||
				pci_tbl[idx].subvendor == ssvid) &&
				(pci_tbl[idx].subdevice == PCI_ANY_ID ||
				pci_tbl[idx].subdevice == ssid))
			{

				break;
			}
		}
		if (pci_tbl[idx].vendor == 0)
			continue;


		if (bcm5700_init_one(pdev, &pci_tbl[idx]) == 0)
			cards_found++;
	}

	return cards_found ? 0 : -ENODEV;
}

#ifdef MODULE
int init_module(void)
{
	return bcm5700_probe(NULL);
}

void cleanup_module(void)
{
	struct net_device *next_dev;
	PUM_DEVICE_BLOCK pUmDevice;

	/* No need to check MOD_IN_USE, as sys_delete_module() checks. */
	while (root_tigon3_dev) {
		pUmDevice = (PUM_DEVICE_BLOCK)root_tigon3_dev->priv;
		next_dev = pUmDevice->next_module;
		unregister_netdev(root_tigon3_dev);
		if (pUmDevice->lm_dev.pMappedMemBase)
			iounmap(pUmDevice->lm_dev.pMappedMemBase);
#if (LINUX_VERSION_CODE < 0x020600)
		kfree(root_tigon3_dev);
#else
		free_netdev(root_tigon3_dev);
#endif
		root_tigon3_dev = next_dev;
	}
#ifdef BCM_IOCTL32
	unregister_ioctl32_conversion(SIOCNICE);
#endif
}

#endif  /* MODULE */
#else	/* LINUX_VERSION_CODE < 0x020300 */

#if (LINUX_VERSION_CODE >= 0x020406)
static int bcm5700_suspend (struct pci_dev *pdev, u32 state)
#else
static void bcm5700_suspend (struct pci_dev *pdev)
#endif
{
	struct net_device *dev = (struct net_device *) pci_get_drvdata(pdev);
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) dev->priv;
	PLM_DEVICE_BLOCK pDevice = &pUmDevice->lm_dev;

	if (!netif_running(dev))
#if (LINUX_VERSION_CODE >= 0x020406)
		return 0;
#else
		return;
#endif

	netif_device_detach (dev);
	bcm5700_shutdown(pUmDevice);

	LM_SetPowerState(pDevice, LM_POWER_STATE_D3);

/*	pci_power_off(pdev, -1);*/
#if (LINUX_VERSION_CODE >= 0x020406)
	return 0;
#endif
}


#if (LINUX_VERSION_CODE >= 0x020406)
static int bcm5700_resume(struct pci_dev *pdev)
#else
static void bcm5700_resume(struct pci_dev *pdev)
#endif
{
	struct net_device *dev = (struct net_device *) pci_get_drvdata(pdev);
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) dev->priv;
	PLM_DEVICE_BLOCK pDevice = &pUmDevice->lm_dev;

	if (!netif_running(dev))
#if (LINUX_VERSION_CODE >= 0x020406)
		return 0;
#else
		return;
#endif
/*	pci_power_on(pdev);*/
	netif_device_attach(dev);
	LM_SetPowerState(pDevice, LM_POWER_STATE_D0);
	MM_InitializeUmPackets(pDevice);
	bcm5700_reset(dev);
#if (LINUX_VERSION_CODE >= 0x020406)
	return 0;
#endif
}


static struct pci_driver bcm5700_pci_driver = {
	name:		bcm5700_driver,
	id_table:	bcm5700_pci_tbl,
	probe:		bcm5700_init_one,
	remove:		__devexit_p(bcm5700_remove_one),
	suspend:	bcm5700_suspend,
	resume:		bcm5700_resume,
};

static int
bcm5700_notify_reboot(struct notifier_block *this, unsigned long event, void *unused)
{
	switch (event) {
	case SYS_HALT:
	case SYS_POWER_OFF:
	case SYS_RESTART:
		break;
	default:
		return NOTIFY_DONE;
	}

	B57_INFO(("bcm5700 reboot notification\n"));
	pci_unregister_driver(&bcm5700_pci_driver);
	return NOTIFY_DONE;
}

static int __init bcm5700_init_module (void)
{
	int pin = 1 << 2;

	if (nvram_match("disabled_5397", "1") || (activate_gpio != -1)) {
		if ( activate_gpio != -1 ) pin = activate_gpio;
		printk("5397 switch GPIO-Reset (pin %d)\n", pin);
		sb_t *gpio_sbh;
		if (!(gpio_sbh = sb_kattach(SB_OSH))) return -ENODEV;
		sb_gpiosetcore(gpio_sbh);
//		sb_gpioreserve(gpio_sbh, 0x4, GPIO_HI_PRIORITY);
		sb_gpioouten(gpio_sbh, 0x4, 0x4, GPIO_HI_PRIORITY);
		sb_gpioout(gpio_sbh, 0x4, 0x4, GPIO_HI_PRIORITY);
		sb_detach(gpio_sbh);
	}

	if (msglevel != 0xdeadbeef) {
		b57_msg_level = msglevel;
		printf("%s: msglevel set to 0x%x\n", __FUNCTION__, b57_msg_level);
	} else
		b57_msg_level = B57_ERR_VAL;

	return pci_module_init(&bcm5700_pci_driver);
}

static void __exit bcm5700_cleanup_module (void)
{
	unregister_reboot_notifier(&bcm5700_reboot_notifier);
	pci_unregister_driver(&bcm5700_pci_driver);
}

module_init(bcm5700_init_module);
module_exit(bcm5700_cleanup_module);
#endif

/*
 * Middle Module
 *
 */


#ifdef BCM_NAPI_RXPOLL
LM_STATUS
MM_ScheduleRxPoll(LM_DEVICE_BLOCK *pDevice)
{
	struct net_device *dev = ((UM_DEVICE_BLOCK *) pDevice)->dev;

	if (netif_rx_schedule_prep(dev)) {
		__netif_rx_schedule(dev);
		return LM_STATUS_SUCCESS;
	}
	return LM_STATUS_FAILURE;
}
#endif

LM_STATUS
MM_ReadConfig16(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Offset,
	LM_UINT16 *pValue16)
{
	UM_DEVICE_BLOCK *pUmDevice;

	pUmDevice = (UM_DEVICE_BLOCK *) pDevice;
	pci_read_config_word(pUmDevice->pdev, Offset, (u16 *) pValue16);
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_ReadConfig32(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Offset,
	LM_UINT32 *pValue32)
{
	UM_DEVICE_BLOCK *pUmDevice;

	pUmDevice = (UM_DEVICE_BLOCK *) pDevice;
	pci_read_config_dword(pUmDevice->pdev, Offset, (u32 *) pValue32);
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_WriteConfig16(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Offset,
	LM_UINT16 Value16)
{
	UM_DEVICE_BLOCK *pUmDevice;

	pUmDevice = (UM_DEVICE_BLOCK *) pDevice;
	pci_write_config_word(pUmDevice->pdev, Offset, Value16);
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_WriteConfig32(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Offset,
	LM_UINT32 Value32)
{
	UM_DEVICE_BLOCK *pUmDevice;

	pUmDevice = (UM_DEVICE_BLOCK *) pDevice;
	pci_write_config_dword(pUmDevice->pdev, Offset, Value32);
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_AllocateSharedMemory(PLM_DEVICE_BLOCK pDevice, LM_UINT32 BlockSize,
	PLM_VOID *pMemoryBlockVirt, PLM_PHYSICAL_ADDRESS pMemoryBlockPhy,
	LM_BOOL Cached)
{
	PLM_VOID pvirt;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;
	dma_addr_t mapping;

	pvirt = pci_alloc_consistent(pUmDevice->pdev, BlockSize,
					       &mapping);
	if (!pvirt) {
		return LM_STATUS_FAILURE;
	}
	pUmDevice->mem_list[pUmDevice->mem_list_num] = pvirt;
	pUmDevice->dma_list[pUmDevice->mem_list_num] = mapping;
	pUmDevice->mem_size_list[pUmDevice->mem_list_num++] = BlockSize;
	memset(pvirt, 0, BlockSize);
	*pMemoryBlockVirt = (PLM_VOID) pvirt;
	MM_SetAddr(pMemoryBlockPhy, mapping);
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_AllocateMemory(PLM_DEVICE_BLOCK pDevice, LM_UINT32 BlockSize,
	PLM_VOID *pMemoryBlockVirt)
{
	PLM_VOID pvirt;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;


	/* Maximum in slab.c */
	if (BlockSize > 131072) {
		goto MM_Alloc_error;
	}

	pvirt = kmalloc(BlockSize, GFP_ATOMIC);
	if (!pvirt) {
		goto MM_Alloc_error;
	}
	pUmDevice->mem_list[pUmDevice->mem_list_num] = pvirt;
	pUmDevice->dma_list[pUmDevice->mem_list_num] = 0;
	pUmDevice->mem_size_list[pUmDevice->mem_list_num++] = 0;
	/* mem_size_list[i] == 0 indicates that the memory should be freed */
	/* using kfree */
	memset(pvirt, 0, BlockSize);
	*pMemoryBlockVirt = pvirt;
	return LM_STATUS_SUCCESS;

MM_Alloc_error:
	printk(KERN_WARNING "%s: Memory allocation failed - buffer parameters may be set too high\n", pUmDevice->dev->name);
	return LM_STATUS_FAILURE;
}

LM_STATUS
MM_MapMemBase(PLM_DEVICE_BLOCK pDevice)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;

	pDevice->pMappedMemBase = ioremap_nocache(
		pci_resource_start(pUmDevice->pdev, 0), sizeof(T3_STD_MEM_MAP));
	if (pDevice->pMappedMemBase == 0)
		return LM_STATUS_FAILURE;

	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_InitializeUmPackets(PLM_DEVICE_BLOCK pDevice)
{
	unsigned int i;
	struct sk_buff *skb;
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;
	PUM_PACKET pUmPacket;
	PLM_PACKET pPacket;

	for (i = 0; i < pDevice->RxPacketDescCnt; i++) {
		pPacket = QQ_PopHead(&pDevice->RxPacketFreeQ.Container);
		pUmPacket = (PUM_PACKET) pPacket;
		if (pPacket == 0) {
			printk(KERN_DEBUG "Bad RxPacketFreeQ\n");
		}
		if (pUmPacket->skbuff == 0) {
#ifdef BCM_WL_EMULATOR
			skb = (struct sk_buff *)wlcemu_pktget(pDevice->wlc,pPacket->u.Rx.RxBufferSize + 2);
#else
			skb = dev_alloc_skb(pPacket->u.Rx.RxBufferSize + 2 + EXTRA_HDR);
#endif
			if (skb == 0) {
				pUmPacket->skbuff = 0;
				QQ_PushTail(
					&pUmDevice->rx_out_of_buf_q.Container,
					pPacket);
				continue;
			}
			pUmPacket->skbuff = skb;
			skb->dev = pUmDevice->dev;
#ifndef BCM_WL_EMULATOR
			skb_reserve(skb, EXTRA_HDR - pUmDevice->rx_buf_align);
#endif
		}
		QQ_PushTail(&pDevice->RxPacketFreeQ.Container, pPacket);
	}
	if (T3_ASIC_REV(pUmDevice->lm_dev.ChipRevId) == T3_ASIC_REV_5700) {
		/* reallocate buffers in the ISR */
		pUmDevice->rx_buf_repl_thresh = 0;
		pUmDevice->rx_buf_repl_panic_thresh = 0;
		pUmDevice->rx_buf_repl_isr_limit = 0;
	}
	else {
		pUmDevice->rx_buf_repl_thresh = pDevice->RxPacketDescCnt / 8;
		pUmDevice->rx_buf_repl_panic_thresh =
			pDevice->RxPacketDescCnt  * 7 / 8;

		/* This limits the time spent in the ISR when the receiver */
		/* is in a steady state of being overrun. */
		pUmDevice->rx_buf_repl_isr_limit = pDevice->RxPacketDescCnt / 8;

#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
		if (pDevice->RxJumboDescCnt != 0) {
			if (pUmDevice->rx_buf_repl_thresh >=
				pDevice->RxJumboDescCnt) {

				pUmDevice->rx_buf_repl_thresh =
				pUmDevice->rx_buf_repl_panic_thresh =
					pDevice->RxJumboDescCnt - 1;
			}
			if (pUmDevice->rx_buf_repl_thresh >=
				pDevice->RxStdDescCnt) {

				pUmDevice->rx_buf_repl_thresh =
				pUmDevice->rx_buf_repl_panic_thresh =
					pDevice->RxStdDescCnt - 1;
			}
		}
#endif
	}
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_GetConfig(PLM_DEVICE_BLOCK pDevice)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;
	int index = pUmDevice->index;
	struct net_device *dev = pUmDevice->dev;

	if (index >= MAX_UNITS)
		return LM_STATUS_SUCCESS;

#if LINUX_KERNEL_VERSION < 0x0020609

	bcm5700_validate_param_range(pUmDevice, &auto_speed[index], "auto_speed",
		0, 1, 1);
	if (auto_speed[index] == 0)
		pDevice->DisableAutoNeg = TRUE;
	else
		pDevice->DisableAutoNeg = FALSE;

	if (line_speed[index] == 0) {
		pDevice->RequestedLineSpeed = LM_LINE_SPEED_AUTO;
		pDevice->DisableAutoNeg = FALSE;
	}
	else {
		bcm5700_validate_param_range(pUmDevice, &full_duplex[index],
			"full_duplex", 0, 1, 1);
		if (full_duplex[index]) {
			pDevice->RequestedDuplexMode = LM_DUPLEX_MODE_FULL;
		}
		else {
			pDevice->RequestedDuplexMode = LM_DUPLEX_MODE_HALF;
		}

		if (line_speed[index] == 1000) {
			pDevice->RequestedLineSpeed = LM_LINE_SPEED_1000MBPS;
			if (pDevice->PhyFlags & PHY_NO_GIGABIT) {
				pDevice->RequestedLineSpeed =
					LM_LINE_SPEED_100MBPS;
				printk(KERN_WARNING "%s-%d: Invalid line_speed parameter (1000), using 100\n", bcm5700_driver, index);
			}
			else {
				if ((pDevice->TbiFlags & ENABLE_TBI_FLAG) &&
					!full_duplex[index]) {
					printk(KERN_WARNING "%s-%d: Invalid full_duplex parameter (0) for fiber, using 1\n", bcm5700_driver, index);
					pDevice->RequestedDuplexMode =
						LM_DUPLEX_MODE_FULL;
				}

				if (!(pDevice->TbiFlags & ENABLE_TBI_FLAG) &&
					!auto_speed[index] && !(pDevice->PhyFlags & PHY_IS_FIBER) ) {
					printk(KERN_WARNING "%s-%d: Invalid auto_speed parameter (0) for copper, using 1\n", bcm5700_driver, index);
					pDevice->DisableAutoNeg = FALSE;
				}
			}
		}
		else if ((pDevice->TbiFlags & ENABLE_TBI_FLAG) ||
                         (pDevice->PhyFlags & PHY_IS_FIBER)){
			 pDevice->RequestedLineSpeed = LM_LINE_SPEED_AUTO;
			 pDevice->RequestedDuplexMode = LM_DUPLEX_MODE_FULL;
			 pDevice->DisableAutoNeg = FALSE;
			 printk(KERN_WARNING "%s-%d: Invalid line_speed parameter (%d), using auto\n", bcm5700_driver, index, line_speed[index]);
		}
		else if (line_speed[index] == 100) {

                        pDevice->RequestedLineSpeed = LM_LINE_SPEED_100MBPS;
		}
		else if (line_speed[index] == 10) {

			pDevice->RequestedLineSpeed = LM_LINE_SPEED_10MBPS;
		}
		else {
			pDevice->RequestedLineSpeed = LM_LINE_SPEED_AUTO;
			pDevice->DisableAutoNeg = FALSE;
			printk(KERN_WARNING "%s-%d: Invalid line_speed parameter (%d), using 0\n", bcm5700_driver, index, line_speed[index]);
		}

	}

#endif /* LINUX_KERNEL_VERSION */

	/* This is an unmanageable switch nic and will have link problems if
	   not set to auto
	*/
	if(pDevice->SubsystemVendorId==0x103c && pDevice->SubsystemId==0x3226)
	{
	    if(pDevice->RequestedLineSpeed != LM_LINE_SPEED_AUTO)
	    {
		printk(KERN_WARNING "%s-%d: Invalid line_speed parameter (%d), using 0\n",
			bcm5700_driver, index, line_speed[index]);
	    }
	    pDevice->RequestedLineSpeed = LM_LINE_SPEED_AUTO;
	    pDevice->DisableAutoNeg = FALSE;
	}

#if LINUX_KERNEL_VERSION < 0x0020609

	pDevice->FlowControlCap = 0;
	bcm5700_validate_param_range(pUmDevice, &rx_flow_control[index],
		"rx_flow_control", 0, 1, 0);
	if (rx_flow_control[index] != 0) {
		pDevice->FlowControlCap |= LM_FLOW_CONTROL_RECEIVE_PAUSE;
	}
	bcm5700_validate_param_range(pUmDevice, &tx_flow_control[index],
		"tx_flow_control", 0, 1, 0);
	if (tx_flow_control[index] != 0) {
		pDevice->FlowControlCap |= LM_FLOW_CONTROL_TRANSMIT_PAUSE;
	}
	bcm5700_validate_param_range(pUmDevice, &auto_flow_control[index],
		"auto_flow_control", 0, 1, 0);
	if (auto_flow_control[index] != 0) {
		if (pDevice->DisableAutoNeg == FALSE) {

			pDevice->FlowControlCap |= LM_FLOW_CONTROL_AUTO_PAUSE;
			if ((tx_flow_control[index] == 0) &&
				(rx_flow_control[index] == 0)) {

				pDevice->FlowControlCap |=
					LM_FLOW_CONTROL_TRANSMIT_PAUSE |
					LM_FLOW_CONTROL_RECEIVE_PAUSE;
			}
		}
	}

	if (dev->mtu > 1500) {
#ifdef BCM_TSO
		if (T3_ASIC_5714_FAMILY(pDevice->ChipRevId) &&
		   (dev->features & NETIF_F_TSO)) {
				dev->features &= ~NETIF_F_TSO;
				printk(KERN_ALERT "%s: TSO previously enabled. Jumbo Frames and TSO cannot simultaneously be enabled. Jumbo Frames enabled. TSO disabled.\n", dev->name);
		}
#endif
		pDevice->RxMtu = dev->mtu + 14;
	}

	if ((T3_ASIC_REV(pDevice->ChipRevId) != T3_ASIC_REV_5700) &&
		!(pDevice->Flags & BCM5788_FLAG)) {
		pDevice->Flags |= USE_TAGGED_STATUS_FLAG;
		pUmDevice->timer_interval = HZ;
		if ((T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5703) &&
			(pDevice->TbiFlags & ENABLE_TBI_FLAG)) {
			pUmDevice->timer_interval = HZ/4;
		}
	}
	else {
		pUmDevice->timer_interval = HZ/10;
	}

	bcm5700_validate_param_range(pUmDevice, &tx_pkt_desc_cnt[index],
		"tx_pkt_desc_cnt", 1, MAX_TX_PACKET_DESC_COUNT-1, TX_DESC_CNT);
	pDevice->TxPacketDescCnt = tx_pkt_desc_cnt[index];
	bcm5700_validate_param_range(pUmDevice, &rx_std_desc_cnt[index],
		"rx_std_desc_cnt", 1, T3_STD_RCV_RCB_ENTRY_COUNT-1,
		RX_DESC_CNT);
	pDevice->RxStdDescCnt = rx_std_desc_cnt[index];

#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
	bcm5700_validate_param_range(pUmDevice, &rx_jumbo_desc_cnt[index],
		"rx_jumbo_desc_cnt", 1, T3_JUMBO_RCV_RCB_ENTRY_COUNT-1,
		JBO_DESC_CNT);

	if (mtu[index] <= 1514)
		pDevice->RxJumboDescCnt = 0;
	else if(!T3_ASIC_IS_5705_BEYOND(pDevice->ChipRevId)){
		pDevice->RxJumboDescCnt = rx_jumbo_desc_cnt[index];
        }
#endif

#ifdef BCM_INT_COAL
	bcm5700_validate_param_range(pUmDevice, &adaptive_coalesce[index],
		"adaptive_coalesce", 0, 1, 1);
#ifdef BCM_NAPI_RXPOLL
	if (adaptive_coalesce[index]) {
		printk(KERN_WARNING "%s-%d: adaptive_coalesce not used in NAPI mode\n", bcm5700_driver, index);
		adaptive_coalesce[index] = 0;

	}
#endif
	pUmDevice->adaptive_coalesce = adaptive_coalesce[index];
	if (!pUmDevice->adaptive_coalesce) {
		bcm5700_validate_param_range(pUmDevice,
			&rx_coalesce_ticks[index], "rx_coalesce_ticks", 0,
			MAX_RX_COALESCING_TICKS, RX_COAL_TK);
		if ((rx_coalesce_ticks[index] == 0) &&
			(rx_max_coalesce_frames[index] == 0)) {

			printk(KERN_WARNING "%s-%d: Conflicting rx_coalesce_ticks (0) and rx_max_coalesce_frames (0) parameters, using %d and %d respectively\n",
				bcm5700_driver, index, RX_COAL_TK, RX_COAL_FM);

			rx_coalesce_ticks[index] = RX_COAL_TK;
			rx_max_coalesce_frames[index] = RX_COAL_FM;
		}
		pDevice->RxCoalescingTicks = pUmDevice->rx_curr_coalesce_ticks =
			rx_coalesce_ticks[index];
#ifdef BCM_NAPI_RXPOLL
		pDevice->RxCoalescingTicksDuringInt = rx_coalesce_ticks[index];
#endif

		bcm5700_validate_param_range(pUmDevice,
			&rx_max_coalesce_frames[index],
			"rx_max_coalesce_frames", 0,
			MAX_RX_MAX_COALESCED_FRAMES, RX_COAL_FM);

		pDevice->RxMaxCoalescedFrames =
			pUmDevice->rx_curr_coalesce_frames =
			rx_max_coalesce_frames[index];
#ifdef BCM_NAPI_RXPOLL
		pDevice->RxMaxCoalescedFramesDuringInt =
			rx_max_coalesce_frames[index];
#endif

		bcm5700_validate_param_range(pUmDevice,
			&tx_coalesce_ticks[index], "tx_coalesce_ticks", 0,
			MAX_TX_COALESCING_TICKS, TX_COAL_TK);
		if ((tx_coalesce_ticks[index] == 0) &&
			(tx_max_coalesce_frames[index] == 0)) {

			printk(KERN_WARNING "%s-%d: Conflicting tx_coalesce_ticks (0) and tx_max_coalesce_frames (0) parameters, using %d and %d respectively\n",
				bcm5700_driver, index, TX_COAL_TK, TX_COAL_FM);

			tx_coalesce_ticks[index] = TX_COAL_TK;
			tx_max_coalesce_frames[index] = TX_COAL_FM;
		}
		pDevice->TxCoalescingTicks = tx_coalesce_ticks[index];
		bcm5700_validate_param_range(pUmDevice,
			&tx_max_coalesce_frames[index],
			"tx_max_coalesce_frames", 0,
			MAX_TX_MAX_COALESCED_FRAMES, TX_COAL_FM);
		pDevice->TxMaxCoalescedFrames = tx_max_coalesce_frames[index];
		pUmDevice->tx_curr_coalesce_frames =
			pDevice->TxMaxCoalescedFrames;

		bcm5700_validate_param_range(pUmDevice,
			&stats_coalesce_ticks[index], "stats_coalesce_ticks",
			0, MAX_STATS_COALESCING_TICKS, ST_COAL_TK);
		if (adaptive_coalesce[index]) {
			printk(KERN_WARNING "%s-%d: Invalid stats_coalesce_ticks parameter set with with adaptive_coalesce parameter. Using adaptive_coalesce.\n", bcm5700_driver, index);
		}else{
			if ((stats_coalesce_ticks[index] > 0) &&
				(stats_coalesce_ticks[index] < 100)) {
				printk(KERN_WARNING "%s-%d: Invalid stats_coalesce_ticks parameter (%u), using 100\n", bcm5700_driver, index, (unsigned int) stats_coalesce_ticks[index]);
				stats_coalesce_ticks[index] = 100;
				pDevice->StatsCoalescingTicks = stats_coalesce_ticks[index];
				pDevice->StatsCoalescingTicks = stats_coalesce_ticks[index];
			}
		}
	}
	else {
		pUmDevice->rx_curr_coalesce_frames = RX_COAL_FM;
		pUmDevice->rx_curr_coalesce_ticks = RX_COAL_TK;
		pUmDevice->tx_curr_coalesce_frames = TX_COAL_FM;
	}
#endif

	if (T3_ASIC_IS_5705_BEYOND(pDevice->ChipRevId)) {
		unsigned int tmpvar;

		tmpvar = pDevice->StatsCoalescingTicks / BCM_TIMER_GRANULARITY;

		/*
		 * If the result is zero, the request is too demanding.
		 */
		if (tmpvar == 0) {
			tmpvar = 1;
		}

		pDevice->StatsCoalescingTicks = tmpvar * BCM_TIMER_GRANULARITY;

		pUmDevice->statstimer_interval = tmpvar;
	}

#ifdef BCM_WOL
	bcm5700_validate_param_range(pUmDevice, &enable_wol[index],
		"enable_wol", 0, 1, 0);
	if (enable_wol[index]) {
		pDevice->WakeUpModeCap = LM_WAKE_UP_MODE_MAGIC_PACKET;
		pDevice->WakeUpMode = LM_WAKE_UP_MODE_MAGIC_PACKET;
	}
#endif
#ifdef INCLUDE_TBI_SUPPORT
	if (pDevice->TbiFlags & ENABLE_TBI_FLAG) {
		if ((T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5704) ||
			(T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5703)) {
			/* just poll since we have hardware autoneg. in 5704 */
			pDevice->TbiFlags |= TBI_PURE_POLLING_FLAG;
		}
		else {
			pDevice->TbiFlags |= TBI_POLLING_INTR_FLAG;
		}
	}
#endif
	bcm5700_validate_param_range(pUmDevice, &scatter_gather[index],
		"scatter_gather", 0, 1, 1);
	bcm5700_validate_param_range(pUmDevice, &tx_checksum[index],
		"tx_checksum", 0, 1, 1);
	bcm5700_validate_param_range(pUmDevice, &rx_checksum[index],
		"rx_checksum", 0, 1, 1);
	if (!(pDevice->TaskOffloadCap & LM_TASK_OFFLOAD_TX_TCP_CHECKSUM)) {
		if (tx_checksum[index] || rx_checksum[index]) {

			pDevice->TaskToOffload = LM_TASK_OFFLOAD_NONE;
			printk(KERN_WARNING "%s-%d: Checksum offload not available on this NIC\n", bcm5700_driver, index);
		}
	}
	else {
		if (rx_checksum[index]) {
			pDevice->TaskToOffload |=
				LM_TASK_OFFLOAD_RX_TCP_CHECKSUM |
				LM_TASK_OFFLOAD_RX_UDP_CHECKSUM;
		}
		if (tx_checksum[index]) {
			pDevice->TaskToOffload |=
				LM_TASK_OFFLOAD_TX_TCP_CHECKSUM |
				LM_TASK_OFFLOAD_TX_UDP_CHECKSUM;
			pDevice->Flags |= NO_TX_PSEUDO_HDR_CSUM_FLAG;
		}
	}
#ifdef BCM_TSO
	bcm5700_validate_param_range(pUmDevice, &enable_tso[index],
		"enable_tso", 0, 1, 1);

	/* Always enable TSO firmware if supported */
	/* This way we can turn it on or off on the fly */
	if (pDevice->TaskOffloadCap & LM_TASK_OFFLOAD_TCP_SEGMENTATION)
	{
		pDevice->TaskToOffload |=
			LM_TASK_OFFLOAD_TCP_SEGMENTATION;
	}
	if (enable_tso[index] &&
		!(pDevice->TaskToOffload & LM_TASK_OFFLOAD_TCP_SEGMENTATION))
	{
		printk(KERN_WARNING "%s-%d: TSO not available on this NIC\n", bcm5700_driver, index);
	}
#endif
#ifdef BCM_ASF
	bcm5700_validate_param_range(pUmDevice, &vlan_tag_mode[index],
		"vlan_strip_mode", 0, 2, 0);
	pUmDevice->vlan_tag_mode = vlan_tag_mode[index];
#else
	pUmDevice->vlan_tag_mode = VLAN_TAG_MODE_NORMAL_STRIP;
#endif

#endif /* LINUX_KERNEL_VERSION */

#ifdef BCM_NIC_SEND_BD
	bcm5700_validate_param_range(pUmDevice, &nic_tx_bd[index], "nic_tx_bd",
		0, 1, 0);
	if (nic_tx_bd[index])
		pDevice->Flags |= NIC_SEND_BD_FLAG;
	if ((pDevice->Flags & ENABLE_PCIX_FIX_FLAG) ||
		(T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5705)) {
		if (pDevice->Flags & NIC_SEND_BD_FLAG) {
			pDevice->Flags &= ~NIC_SEND_BD_FLAG;
			printk(KERN_WARNING "%s-%d: Nic Send BDs not available on this NIC or not possible on this system\n", bcm5700_driver, index);
		}
	}
#endif
#if defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR)
	bcm5700_validate_param_range(pUmDevice, &disable_msi[pUmDevice->index],
		"disable_msi", 0, 1, 0);
#endif

	bcm5700_validate_param_range(pUmDevice, &delay_link[index],
		"delay_link", 0, 1, 0);

	bcm5700_validate_param_range(pUmDevice, &disable_d3hot[index],
		"disable_d3hot", 0, 1, 0);
	if (disable_d3hot[index]) {

#ifdef BCM_WOL
		if (enable_wol[index]) {
			pDevice->WakeUpModeCap = LM_WAKE_UP_MODE_NONE;
			pDevice->WakeUpMode = LM_WAKE_UP_MODE_NONE;
			printk(KERN_WARNING "%s-%d: Wake-On-Lan disabled because D3Hot is disabled\n", bcm5700_driver, index);
		}
#endif
		pDevice->Flags |= DISABLE_D3HOT_FLAG;
	}

    return LM_STATUS_SUCCESS;
}

/* From include/proto/ethernet.h */
#define ETHER_TYPE_8021Q	0x8100		/* 802.1Q */

/* From include/proto/vlan.h */
#define VLAN_PRI_MASK		7	/* 3 bits of priority */
#define VLAN_PRI_SHIFT		13

/* Replace the priority in a vlan tag */
#define	UPD_VLANTAG_PRIO(tag, prio) do { \
	tag &= ~(VLAN_PRI_MASK << VLAN_PRI_SHIFT); \
	tag |= prio << VLAN_PRI_SHIFT; \
} while (0)

/* Takes an Ethernet frame and sets out-of-bound PKTPRIO.
 * Also updates the inplace vlan tag if requested.
 * For debugging, it returns an indication of what it did.
 */
#define	PKTPRIO_VDSCP	0x100		/* DSCP prio found after VLAN tag */
#define	PKTPRIO_VLAN	0x200		/* VLAN prio found */
#define	PKTPRIO_UPD	0x400		/* DSCP used to update VLAN prio */
#define	PKTPRIO_DSCP	0x800		/* DSCP prio found */
#define	PKTSETPRIO(skb, x)		(((struct sk_buff*)(skb))->priority = (x))
static uint
pktsetprio(void *pkt, bool update_vtag)
{
	struct ether_header *eh;
	struct ethervlan_header *evh;
	uint8 *pktdata;
	int priority = 0;
	int rc = 0;

	pktdata = (uint8 *) PKTDATA(NULL, pkt);
	ASSERT(ISALIGNED((uintptr)pktdata, sizeof(uint16)));

	eh = (struct ether_header *) pktdata;

	if (ntoh16(eh->ether_type) == ETHER_TYPE_8021Q) {
		uint16 vlan_tag;
		int vlan_prio, dscp_prio = 0;

		evh = (struct ethervlan_header *)eh;

		vlan_tag = ntoh16(evh->vlan_tag);
		vlan_prio = (int) (vlan_tag >> VLAN_PRI_SHIFT) & VLAN_PRI_MASK;

		if (ntoh16(evh->ether_type) == ETHER_TYPE_IP) {
			uint8 *ip_body = pktdata + sizeof(struct ethervlan_header);
			uint8 tos_tc = IP_TOS(ip_body);
			dscp_prio = (int)(tos_tc >> IPV4_TOS_PREC_SHIFT);
			if ((IP_VER(ip_body) == IP_VER_4) && (IPV4_PROT(ip_body) == IP_PROT_TCP)) {
				int ip_len;
				int src_port;
				bool src_port_exc;
				uint8 *tcp_hdr;

				ip_len = IPV4_PAYLOAD_LEN(ip_body);
				tcp_hdr = IPV4_NO_OPTIONS_PAYLOAD(ip_body);
				src_port = TCP_SRC_PORT(tcp_hdr);
				src_port_exc = (src_port == 10110) || (src_port == 10120) ||
					(src_port == 10130) || (src_port == 10140);

				if ((ip_len == 40) && src_port_exc && TCP_IS_ACK(tcp_hdr)) {
					dscp_prio = 7;
				}
			}
		}

		/* DSCP priority gets precedence over 802.1P (vlan tag) */
		if (dscp_prio != 0) {
			priority = dscp_prio;
			rc |= PKTPRIO_VDSCP;
		} else {
			priority = vlan_prio;
			rc |= PKTPRIO_VLAN;
		}
		/* 
		 * If the DSCP priority is not the same as the VLAN priority,
		 * then overwrite the priority field in the vlan tag, with the
		 * DSCP priority value. This is required for Linux APs because
		 * the VLAN driver on Linux, overwrites the skb->priority field
		 * with the priority value in the vlan tag
		 */
		if (update_vtag && (priority != vlan_prio)) {
			vlan_tag &= ~(VLAN_PRI_MASK << VLAN_PRI_SHIFT);
			vlan_tag |= (uint16)priority << VLAN_PRI_SHIFT;
			evh->vlan_tag = hton16(vlan_tag);
			rc |= PKTPRIO_UPD;
		}
	} else if (ntoh16(eh->ether_type) == ETHER_TYPE_IP) {
		uint8 *ip_body = pktdata + sizeof(struct ether_header);
		uint8 tos_tc = IP_TOS(ip_body);
		priority = (int)(tos_tc >> IPV4_TOS_PREC_SHIFT);
		rc |= PKTPRIO_DSCP;
		if ((IP_VER(ip_body) == IP_VER_4) && (IPV4_PROT(ip_body) == IP_PROT_TCP)) {
			int ip_len;
			int src_port;
			bool src_port_exc;
			uint8 *tcp_hdr;

			ip_len = IPV4_PAYLOAD_LEN(ip_body);
			tcp_hdr = IPV4_NO_OPTIONS_PAYLOAD(ip_body);
			src_port = TCP_SRC_PORT(tcp_hdr);
			src_port_exc = (src_port == 10110) || (src_port == 10120) ||
				(src_port == 10130) || (src_port == 10140);

			if ((ip_len == 40) && src_port_exc && TCP_IS_ACK(tcp_hdr)) {
				priority = 7;
			}
		}
	}

	ASSERT(priority >= 0 && priority <= MAXPRIO);
	PKTSETPRIO(pkt, priority);
	return (rc | priority);
}

LM_STATUS
MM_IndicateRxPackets(PLM_DEVICE_BLOCK pDevice)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;
	PLM_PACKET pPacket;
	PUM_PACKET pUmPacket;
	struct sk_buff *skb;
	int size;
	int vlan_tag_size = 0;
	uint16 dscp_prio;

	if (pDevice->ReceiveMask & LM_KEEP_VLAN_TAG)
		vlan_tag_size = 4;

	while (1) {
		pPacket = (PLM_PACKET)
			QQ_PopHead(&pDevice->RxPacketReceivedQ.Container);
		if (pPacket == 0)
			break;
		pUmPacket = (PUM_PACKET) pPacket;
#if !defined(NO_PCI_UNMAP)
		pci_unmap_single(pUmDevice->pdev,
				pci_unmap_addr(pUmPacket, map[0]),
				pPacket->u.Rx.RxBufferSize,
				PCI_DMA_FROMDEVICE);
#endif
		if ((pPacket->PacketStatus != LM_STATUS_SUCCESS) ||
			((size = pPacket->PacketSize) >
			(pDevice->RxMtu + vlan_tag_size))) {

			/* reuse skb */
#ifdef BCM_TASKLET
			QQ_PushTail(&pUmDevice->rx_out_of_buf_q.Container, pPacket);
#else
			QQ_PushTail(&pDevice->RxPacketFreeQ.Container, pPacket);
#endif
			pUmDevice->rx_misc_errors++;
			continue;
		}
		skb = pUmPacket->skbuff;
		skb_put(skb, size);
		skb->pkt_type = 0;
		/* Extract priority from payload and put it in skb->priority */
		dscp_prio = 0;
		if (pUmDevice->qos) {
			uint rc;

			rc = pktsetprio(skb, TRUE);
			if (rc & (PKTPRIO_VDSCP | PKTPRIO_DSCP))
				dscp_prio = rc & VLAN_PRI_MASK;
			if (rc != 0)
				B57_INFO(("pktsetprio returned 0x%x, skb->priority: %d\n",
				          rc, skb->priority));
		}
		skb->protocol = eth_type_trans(skb, skb->dev);
		if (size > pDevice->RxMtu) {
			/* Make sure we have a valid VLAN tag */
			if (htons(skb->protocol) != ETHER_TYPE_8021Q) {
				dev_kfree_skb_irq(skb);
				pUmDevice->rx_misc_errors++;
				goto drop_rx;
			}
		}

		pUmDevice->stats.rx_bytes += skb->len;

		if ((pPacket->Flags & RCV_BD_FLAG_TCP_UDP_CHKSUM_FIELD) &&
			(pDevice->TaskToOffload &
				LM_TASK_OFFLOAD_RX_TCP_CHECKSUM)) {
			if (pPacket->u.Rx.TcpUdpChecksum == 0xffff) {

				skb->ip_summed = CHECKSUM_UNNECESSARY;
#if TIGON3_DEBUG
				pUmDevice->rx_good_chksum_count++;
#endif
			}
			else {
				skb->ip_summed = CHECKSUM_NONE;
				pUmDevice->rx_bad_chksum_count++;
			}
		}
		else {
			skb->ip_summed = CHECKSUM_NONE;
		}
		{
#ifdef BCM_VLAN
			if (pUmDevice->vlgrp &&
				(pPacket->Flags & RCV_BD_FLAG_VLAN_TAG)) {
				/* Override vlan priority with dscp priority */
				if (dscp_prio)
					UPD_VLANTAG_PRIO(pPacket->VlanTag, dscp_prio);
#ifdef BCM_NAPI_RXPOLL
				vlan_hwaccel_receive_skb(skb, pUmDevice->vlgrp,
					pPacket->VlanTag);
#else
				vlan_hwaccel_rx(skb, pUmDevice->vlgrp,
					pPacket->VlanTag);
#endif
			} else
#endif
			{
#ifdef BCM_WL_EMULATOR
				if(pDevice->wl_emulate_rx) {
					/* bcmstats("emu recv %d %d"); */
					wlcemu_receive_skb(pDevice->wlc, skb);
					/* bcmstats("emu recv end %d %d"); */
				}
				else 
#endif /* BCM_WL_EMULATOR  */
				{
#ifdef BCM_NAPI_RXPOLL
				netif_receive_skb(skb);
#else
				netif_rx(skb);
#endif
				}
			}
		}
		pUmDevice->dev->last_rx = jiffies;

drop_rx:
#ifdef BCM_TASKLET
		pUmPacket->skbuff = 0;
		QQ_PushTail(&pUmDevice->rx_out_of_buf_q.Container, pPacket);
#else
#ifdef BCM_WL_EMULATOR
		skb = (struct sk_buff *)wlcemu_pktget(pDevice->wlc,pPacket->u.Rx.RxBufferSize + 2);
#else
		skb = dev_alloc_skb(pPacket->u.Rx.RxBufferSize + 2 + EXTRA_HDR);
#endif /* BCM_WL_EMULATOR  */
		if (skb == 0) {
			pUmPacket->skbuff = 0;
			QQ_PushTail(&pUmDevice->rx_out_of_buf_q.Container, pPacket);
		}
		else {
			pUmPacket->skbuff = skb;
			skb->dev = pUmDevice->dev;
#ifndef BCM_WL_EMULATOR
			skb_reserve(skb, EXTRA_HDR - pUmDevice->rx_buf_align);
#endif
			QQ_PushTail(&pDevice->RxPacketFreeQ.Container, pPacket);
		}
#endif
	}
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_CoalesceTxBuffer(PLM_DEVICE_BLOCK pDevice, PLM_PACKET pPacket)
{
	PUM_PACKET pUmPacket = (PUM_PACKET) pPacket;
	struct sk_buff *skb = pUmPacket->skbuff;
	struct sk_buff *nskb;
#if !defined(NO_PCI_UNMAP)
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;

	pci_unmap_single(pUmDevice->pdev,
			pci_unmap_addr(pUmPacket, map[0]),
			pci_unmap_len(pUmPacket, map_len[0]),
			PCI_DMA_TODEVICE);
#if MAX_SKB_FRAGS
	{
		int i;

		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			pci_unmap_page(pUmDevice->pdev,
				pci_unmap_addr(pUmPacket, map[i + 1]),
				pci_unmap_len(pUmPacket, map_len[i + 1]),
				PCI_DMA_TODEVICE);
		}
	}
#endif
#endif
	if ((nskb = skb_copy(skb, GFP_ATOMIC))) {
		pUmPacket->lm_packet.u.Tx.FragCount = 1;
		dev_kfree_skb(skb);
		pUmPacket->skbuff = nskb;
		return LM_STATUS_SUCCESS;
	}
	dev_kfree_skb(skb);
	pUmPacket->skbuff = 0;
	return LM_STATUS_FAILURE;
}

/* Returns 1 if not all buffers are allocated */
STATIC int
replenish_rx_buffers(PUM_DEVICE_BLOCK pUmDevice, int max)
{
	PLM_PACKET pPacket;
	PUM_PACKET pUmPacket;
	PLM_DEVICE_BLOCK pDevice = (PLM_DEVICE_BLOCK) pUmDevice;
	struct sk_buff *skb;
	int queue_rx = 0;
	int alloc_cnt = 0;
	int ret = 0;

	while ((pUmPacket = (PUM_PACKET)
		QQ_PopHead(&pUmDevice->rx_out_of_buf_q.Container)) != 0) {
		pPacket = (PLM_PACKET) pUmPacket;
		if (pUmPacket->skbuff) {
			/* reuse an old skb */
			QQ_PushTail(&pDevice->RxPacketFreeQ.Container, pPacket);
			queue_rx = 1;
			continue;
		}
#ifdef BCM_WL_EMULATOR
		if ((skb = (struct sk_buff *)wlcemu_pktget(pDevice->wlc,pPacket->u.Rx.RxBufferSize + 2)) == 0)
#else 
	       if ((skb = dev_alloc_skb(pPacket->u.Rx.RxBufferSize + 2 + EXTRA_HDR)) == 0)
#endif /* BCM_WL_EMULATOR  */
	       {
		       QQ_PushHead(&pUmDevice->rx_out_of_buf_q.Container,
		                   pPacket);
		       ret = 1;
		       break;
	       }
		pUmPacket->skbuff = skb;
		skb->dev = pUmDevice->dev;
#ifndef BCM_WL_EMULATOR
			skb_reserve(skb, EXTRA_HDR - pUmDevice->rx_buf_align);
#endif
		QQ_PushTail(&pDevice->RxPacketFreeQ.Container, pPacket);
		queue_rx = 1;
		if (max > 0) {
			alloc_cnt++;
			if (alloc_cnt >= max)
				break;
		}
	}
	if (queue_rx || pDevice->QueueAgain) {
		LM_QueueRxPackets(pDevice);
	}
	return ret;
}

LM_STATUS
MM_IndicateTxPackets(PLM_DEVICE_BLOCK pDevice)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;
	PLM_PACKET pPacket;
	PUM_PACKET pUmPacket;
	struct sk_buff *skb;
#if !defined(NO_PCI_UNMAP) && MAX_SKB_FRAGS
	int i;
#endif

	while (1) {
		pPacket = (PLM_PACKET)
			QQ_PopHead(&pDevice->TxPacketXmittedQ.Container);
		if (pPacket == 0)
			break;
		pUmPacket = (PUM_PACKET) pPacket;
		skb = pUmPacket->skbuff;
#if !defined(NO_PCI_UNMAP)
		pci_unmap_single(pUmDevice->pdev,
				pci_unmap_addr(pUmPacket, map[0]),
				pci_unmap_len(pUmPacket, map_len[0]),
				PCI_DMA_TODEVICE);
#if MAX_SKB_FRAGS
		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			pci_unmap_page(pUmDevice->pdev,
				pci_unmap_addr(pUmPacket, map[i + 1]),
				pci_unmap_len(pUmPacket, map_len[i + 1]),
				PCI_DMA_TODEVICE);
		}
#endif
#endif
		dev_kfree_skb_irq(skb);
		pUmPacket->skbuff = 0;
		QQ_PushTail(&pDevice->TxPacketFreeQ.Container, pPacket);
	}
	if (pUmDevice->tx_full) {
		if (QQ_GetEntryCnt(&pDevice->TxPacketFreeQ.Container) >=
			(pDevice->TxPacketDescCnt >> 1)) {

			pUmDevice->tx_full = 0;
			netif_wake_queue(pUmDevice->dev);
		}
	}
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_IndicateStatus(PLM_DEVICE_BLOCK pDevice, LM_STATUS Status)
{
	PUM_DEVICE_BLOCK pUmDevice = (PUM_DEVICE_BLOCK) pDevice;
	struct net_device *dev = pUmDevice->dev;
	LM_FLOW_CONTROL flow_control;
	int speed = 0;

	if (!pUmDevice->opened)
		return LM_STATUS_SUCCESS;

	if (!pUmDevice->suspended) {
		if (Status == LM_STATUS_LINK_DOWN) {
			netif_carrier_off(dev);
		}
		else if (Status == LM_STATUS_LINK_ACTIVE) {
			netif_carrier_on(dev);
		}
	}

	if (pUmDevice->delayed_link_ind > 0) {
		pUmDevice->delayed_link_ind = 0;
		if (Status == LM_STATUS_LINK_DOWN) {
			B57_INFO(("%s: %s NIC Link is DOWN\n", bcm5700_driver, dev->name));
		}
		else if (Status == LM_STATUS_LINK_ACTIVE) {
			B57_INFO(("%s: %s NIC Link is UP, ", bcm5700_driver, dev->name));
		}
	}
	else {
		if (Status == LM_STATUS_LINK_DOWN) {
			B57_INFO(("%s: %s NIC Link is Down\n", bcm5700_driver, dev->name));
		}
		else if (Status == LM_STATUS_LINK_ACTIVE) {
			B57_INFO(("%s: %s NIC Link is Up, ", bcm5700_driver, dev->name));
		}
	}

	if (Status == LM_STATUS_LINK_ACTIVE) {
		if (pDevice->LineSpeed == LM_LINE_SPEED_1000MBPS)
			speed = 1000;
		else if (pDevice->LineSpeed == LM_LINE_SPEED_100MBPS)
			speed = 100;
		else if (pDevice->LineSpeed == LM_LINE_SPEED_10MBPS)
			speed = 10;

		B57_INFO(("%d Mbps ", speed));

		if (pDevice->DuplexMode == LM_DUPLEX_MODE_FULL)
			B57_INFO(("full duplex"));
		else
			B57_INFO(("half duplex"));

		flow_control = pDevice->FlowControl &
			(LM_FLOW_CONTROL_RECEIVE_PAUSE |
			LM_FLOW_CONTROL_TRANSMIT_PAUSE);
		if (flow_control) {
			if (flow_control & LM_FLOW_CONTROL_RECEIVE_PAUSE) {
				B57_INFO((", receive "));
				if (flow_control & LM_FLOW_CONTROL_TRANSMIT_PAUSE)
					B57_INFO(("& transmit "));
			}
			else {
				B57_INFO((", transmit "));
			}
			B57_INFO(("flow control ON"));
		}
		B57_INFO(("\n"));
	}
	return LM_STATUS_SUCCESS;
}

void
MM_UnmapRxDma(LM_DEVICE_BLOCK *pDevice, LM_PACKET *pPacket)
{
#if !defined(NO_PCI_UNMAP)
	UM_DEVICE_BLOCK *pUmDevice = (UM_DEVICE_BLOCK *) pDevice;
	UM_PACKET *pUmPacket = (UM_PACKET *) pPacket;

	if (!pUmPacket->skbuff)
		return;

	pci_unmap_single(pUmDevice->pdev,
			pci_unmap_addr(pUmPacket, map[0]),
			pPacket->u.Rx.RxBufferSize,
			PCI_DMA_FROMDEVICE);
#endif
}

LM_STATUS
MM_FreeRxBuffer(PLM_DEVICE_BLOCK pDevice, PLM_PACKET pPacket)
{
	PUM_PACKET pUmPacket;
	struct sk_buff *skb;

	if (pPacket == 0)
		return LM_STATUS_SUCCESS;
	pUmPacket = (PUM_PACKET) pPacket;
	if ((skb = pUmPacket->skbuff)) {
		/* DMA address already unmapped */
		dev_kfree_skb(skb);
	}
	pUmPacket->skbuff = 0;
	return LM_STATUS_SUCCESS;
}

LM_STATUS
MM_Sleep(LM_DEVICE_BLOCK *pDevice, LM_UINT32 msec)
{
	current->state = TASK_INTERRUPTIBLE;
	if (schedule_timeout(HZ * msec / 1000) != 0) {
		return LM_STATUS_FAILURE;
	}
	if (signal_pending(current))
		return LM_STATUS_FAILURE;

	return LM_STATUS_SUCCESS;
}

void
bcm5700_shutdown(UM_DEVICE_BLOCK *pUmDevice)
{
	LM_DEVICE_BLOCK *pDevice = (LM_DEVICE_BLOCK *) pUmDevice;

	bcm5700_intr_off(pUmDevice);
	netif_carrier_off(pUmDevice->dev);
#ifdef BCM_TASKLET
	tasklet_kill(&pUmDevice->tasklet);
#endif
	bcm5700_poll_wait(pUmDevice);

	LM_Halt(pDevice);

	pDevice->InitDone = 0;
	bcm5700_free_remaining_rx_bufs(pUmDevice);
}

void
bcm5700_free_remaining_rx_bufs(UM_DEVICE_BLOCK *pUmDevice)
{
	LM_DEVICE_BLOCK *pDevice = &pUmDevice->lm_dev;
	UM_PACKET *pUmPacket;
	int cnt, i;

	cnt = QQ_GetEntryCnt(&pUmDevice->rx_out_of_buf_q.Container);
	for (i = 0; i < cnt; i++) {
		if ((pUmPacket =
			QQ_PopHead(&pUmDevice->rx_out_of_buf_q.Container))
			!= 0) {

			MM_UnmapRxDma(pDevice, (LM_PACKET *) pUmPacket);
			MM_FreeRxBuffer(pDevice, &pUmPacket->lm_packet);
			QQ_PushTail(&pDevice->RxPacketFreeQ.Container,
				pUmPacket);
		}
	}
}

void
bcm5700_validate_param_range(UM_DEVICE_BLOCK *pUmDevice, int *param,
	char *param_name, int min, int max, int deflt)
{
	if (((unsigned int) *param < (unsigned int) min) ||
		((unsigned int) *param > (unsigned int) max)) {

		printk(KERN_WARNING "%s-%d: Invalid %s parameter (%u), using %u\n", bcm5700_driver, pUmDevice->index, param_name, (unsigned int) *param, (unsigned int) deflt);
		*param = deflt;
	}
}

struct net_device *
bcm5700_find_peer(struct net_device *dev)
{
	struct net_device *tmp_dev;
	UM_DEVICE_BLOCK *pUmDevice, *pUmTmp;
	LM_DEVICE_BLOCK *pDevice;

	tmp_dev = 0;
	pUmDevice = (UM_DEVICE_BLOCK *) dev->priv;
	pDevice = &pUmDevice->lm_dev;
	if (T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5704) {
		tmp_dev = root_tigon3_dev;
		while (tmp_dev) {
			pUmTmp = (PUM_DEVICE_BLOCK) tmp_dev->priv;
			if ((tmp_dev != dev) &&
				(pUmDevice->pdev->bus->number ==
				pUmTmp->pdev->bus->number) &&
				PCI_SLOT(pUmDevice->pdev->devfn) ==
				PCI_SLOT(pUmTmp->pdev->devfn)) {

				break;
			}
			tmp_dev = pUmTmp->next_module;
		}
	}
	return tmp_dev;
}

LM_DEVICE_BLOCK *
MM_FindPeerDev(LM_DEVICE_BLOCK *pDevice)
{
	UM_DEVICE_BLOCK *pUmDevice = (UM_DEVICE_BLOCK *) pDevice;
	struct net_device *dev = pUmDevice->dev;
	struct net_device *peer_dev;

	peer_dev = bcm5700_find_peer(dev);
	if (!peer_dev)
		return 0;
	return ((LM_DEVICE_BLOCK *) peer_dev->priv);
}

int MM_FindCapability(LM_DEVICE_BLOCK *pDevice, int capability)
{
	UM_DEVICE_BLOCK *pUmDevice = (UM_DEVICE_BLOCK *) pDevice;
	return (pci_find_capability(pUmDevice->pdev, capability));
}

#if defined(HAVE_POLL_CONTROLLER)||defined(CONFIG_NET_POLL_CONTROLLER)
STATIC void
poll_bcm5700(struct net_device *dev)
{
	UM_DEVICE_BLOCK *pUmDevice = dev->priv;

#if defined(RED_HAT_LINUX_KERNEL) && (LINUX_VERSION_CODE < 0x020605)
	if (netdump_mode) {
		bcm5700_interrupt(pUmDevice->pdev->irq, dev, NULL);
#ifdef BCM_NAPI_RXPOLL
		if (dev->poll_list.prev) {
			int budget = 64;

			bcm5700_poll(dev, &budget);
		}
#endif
	}
	else
#endif
	{
		disable_irq(pUmDevice->pdev->irq);
		bcm5700_interrupt(pUmDevice->pdev->irq, dev, NULL);
		enable_irq(pUmDevice->pdev->irq);
	}
}
#endif
