/*
 **************************************************************************
 * Copyright (c) 2013-2021, The Linux Foundation. All rights reserved.
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

/*
 * na_core.h
 *	NSS driver core header file.
 */

#ifndef __NSS_CORE_H
#define __NSS_CORE_H

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>
#include <linux/workqueue.h>
#include <asm/cacheflush.h>

#include <nss_api_if.h>
#include "nss_phys_if.h"
#include "nss_hlos_if.h"
#include "nss_oam.h"
#include "nss_data_plane.h"
#include "nss_gmac_stats.h"
#include "nss_meminfo.h"
#include "nss_stats.h"

/*
 * NSS debug macros
 */
#define nss_info_always(s, ...) pr_alert(s, ##__VA_ARGS__)

#if (NSS_DEBUG_LEVEL < 1)
#define nss_assert(fmt, args...)
#else
#define nss_assert(c) if (!(c)) { BUG_ON(!(c)); }
#endif

#if defined(CONFIG_DYNAMIC_DEBUG)
/*
 * Compile messages for dynamic enable/disable
 */
#define nss_warning(s, ...) pr_debug("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define nss_info(s, ...) pr_debug("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define nss_trace(s, ...) pr_debug("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else

/*
 * Statically compile messages at different levels
 */
#if (NSS_DEBUG_LEVEL < 2)
#define nss_warning(s, ...)
#else
#define nss_warning(s, ...) pr_warn("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

#if (NSS_DEBUG_LEVEL < 3)
#define nss_info(s, ...)
#else
#define nss_info(s, ...) pr_notice("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

#if (NSS_DEBUG_LEVEL < 4)
#define nss_trace(s, ...)
#else
#define nss_trace(s, ...) pr_info("%s[%d]:" s, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif
#endif

#if (NSS_PKT_STATS_ENABLED == 1)
#define NSS_PKT_STATS_INC(x) nss_pkt_stats_inc((x))
#define NSS_PKT_STATS_DEC(x) nss_pkt_stats_dec((x))
#define NSS_PKT_STATS_ADD(x, i) nss_pkt_stats_add((x), (i))
#define NSS_PKT_STATS_SUB(x, i) nss_pkt_stats_sub((x), (i))
#define NSS_PKT_STATS_READ(x) nss_pkt_stats_read(x)
#else
#define NSS_PKT_STATS_INC(x)
#define NSS_PKT_STATS_DEC(x)
#define NSS_PKT_STATS_ADD(x, i)
#define NSS_PKT_STATS_SUB(x, i)
#define NSS_PKT_STATS_READ(x)
#endif

/*
 * Cache operation
 */
#define NSS_CORE_DSB() dsb(sy)
#define NSS_CORE_DMA_CACHE_MAINT(start, size, dir) nss_core_dma_cache_maint(start, size, dir)

/*
 * nss_core_dma_cache_maint()
 *	Perform the appropriate cache op based on direction
 */
static inline void nss_core_dma_cache_maint(void *start, uint32_t size, int direction)
{
	switch (direction) {
	case DMA_FROM_DEVICE:/* invalidate only */
		dmac_inv_range(start, start + size);
		break;
	case DMA_TO_DEVICE:/* writeback only */
		dmac_clean_range(start, start + size);
		break;
	case DMA_BIDIRECTIONAL:/* writeback and invalidate */
		dmac_flush_range(start, start + size);
		break;
	default:
		BUG();
	}
}

#define NSS_DEVICE_IF_START NSS_PHYSICAL_IF_START

#define NSS_IS_IF_TYPE(type, if_num) ((if_num >= NSS_##type##_IF_START) && (if_num < (NSS_##type##_IF_START + NSS_MAX_##type##_INTERFACES)))

/*
 * Default payload size for NSS buffers
 */
#define NSS_NBUF_PAYLOAD_SIZE NSS_EMPTY_BUFFER_SIZE
#define NSS_NBUF_PAD_EXTRA 256
#define NSS_NBUF_ETH_EXTRA 192

/*
 * N2H/H2N Queue IDs
 */
#define NSS_IF_N2H_EMPTY_BUFFER_RETURN_QUEUE 0
#define NSS_IF_N2H_DATA_QUEUE_0 1
#define NSS_IF_N2H_DATA_QUEUE_1 2
#define NSS_IF_N2H_DATA_QUEUE_2 3
#define NSS_IF_N2H_DATA_QUEUE_3 4

#define NSS_IF_H2N_EMPTY_BUFFER_QUEUE 0
#define NSS_IF_H2N_CMD_QUEUE 1
#define NSS_IF_H2N_EMPTY_PAGED_BUFFER_QUEUE 2
#define NSS_IF_H2N_DATA_QUEUE 3

/*
 * NSS Interrupt Causes
 */
#define NSS_INTR_CAUSE_INVALID 0
#define NSS_INTR_CAUSE_QUEUE 1
#define NSS_INTR_CAUSE_NON_QUEUE 2
#define NSS_INTR_CAUSE_EMERGENCY 3
#define NSS_INTR_CAUSE_SDMA 4

/*
 * NSS Core Status
 */
#define NSS_CORE_STATUS_SUCCESS 0
#define NSS_CORE_STATUS_FAILURE 1
#define NSS_CORE_STATUS_FAILURE_QUEUE 2

/*
 * NSS context magic
 */
#define NSS_CTX_MAGIC 0xDEDEDEDE

/*
 * Number of n2h descriptor rings
 */
#define NSS_N2H_DESC_RING_NUM 15
#define NSS_H2N_DESC_RING_NUM 16

/*
 * NSS maximum data queue per core
 */
#define NSS_MAX_DATA_QUEUE 4

/*
 * NSS maximum IRQ per interrupt instance/core
 */
#if defined(NSS_HAL_IPQ807x_SUPPORT) || defined(NSS_HAL_IPQ60XX_SUPPORT)
#define NSS_MAX_IRQ_PER_INSTANCE 6
#define NSS_MAX_IRQ_PER_CORE 10	/* must match with NSS_HAL_N2H_INTR_PURPOSE_MAX */
#elif defined(NSS_HAL_IPQ50XX_SUPPORT)
#define NSS_MAX_IRQ_PER_CORE 8
#else
#define NSS_MAX_IRQ_PER_INSTANCE 1
#define NSS_MAX_IRQ_PER_CORE 2
#endif

/*
 * NSS maximum clients
 */
#define NSS_MAX_CLIENTS 12

/*
 * Maximum number of service code NSS supports
 */
#define NSS_MAX_SERVICE_CODE 256

/*
 * Interrupt cause processing weights
 */
#define NSS_EMPTY_BUFFER_SOS_PROCESSING_WEIGHT 64
#define NSS_DATA_COMMAND_BUFFER_PROCESSING_WEIGHT 64
#define NSS_EMPTY_BUFFER_RETURN_PROCESSING_WEIGHT 64
#define NSS_TX_UNBLOCKED_PROCESSING_WEIGHT 1

/*
 * Cache line size of the NSS.
 */
#define NSS_CACHE_LINE_SIZE 32

/*
 * Statistics struct
 *
 * INFO: These numbers are based on previous generation chip
 *	These may change in future
 */

/*
 * NSS Frequency Defines and Values
 *
 * INFO: The LOW and MAX value together describe the "performance" band that we should operate the frequency at.
 *
 */
#define NSS_FREQ_SCALE_NA	0xFAADFAAD	/* Frequency scale not supported */
#define NSS_FREQ_NA		0x0		/* Instructions Per ms Min */

#define NSS_FREQ_110		110000000	/* Frequency in hz */
#define NSS_FREQ_110_MIN	0x03000		/* Instructions Per ms Min */
#define NSS_FREQ_110_MAX	0x07000		/* Instructions Per ms Max */

#define NSS_FREQ_187		187200000	/* Frequency in hz */
#if defined(NSS_HAL_IPQ60XX_SUPPORT)
#define NSS_FREQ_187_MIN	0x03000		/* Instructions Per ms Min */
#define NSS_FREQ_187_MAX	0x10000		/* Instructions Per ms Max */
#else
#define NSS_FREQ_187_MIN	0x03000		/* Instructions Per ms Min */
#define NSS_FREQ_187_MAX	0x07000		/* Instructions Per ms Max */
#endif

#define NSS_FREQ_275		275000000	/* Frequency in hz */
#define NSS_FREQ_275_MIN	0x03000		/* Instructions Per ms Min */
#define NSS_FREQ_275_MAX	0x07000		/* Instructions Per ms Max */

#define NSS_FREQ_550		550000000	/* Frequency in hz */
#define NSS_FREQ_550_MIN	0x07000		/* Instructions Per ms Min */
#define NSS_FREQ_550_MAX	0x08000		/* Instructions Per ms Max */

#define NSS_FREQ_600		600000000	/* Frequency in hz */
#define NSS_FREQ_600_MIN	0x07000		/* Instructions Per ms Min */
#define NSS_FREQ_600_MAX	0x08000		/* Instructions Per ms Max */

#define NSS_FREQ_733		733000000	/* Frequency in hz */
#define NSS_FREQ_733_MIN	0x07000		/* Instructions Per ms Min */
#define NSS_FREQ_733_MAX	0x25000		/* Instructions Per ms Max */

#define NSS_FREQ_748		748800000	/* Frequency in hz */
#if defined(NSS_HAL_IPQ60XX_SUPPORT)
#define NSS_FREQ_748_MIN	0x10000		/* Instructions Per ms Min */
#define NSS_FREQ_748_MAX	0x18000		/* Instructions Per ms Max */
#else
#define NSS_FREQ_748_MIN	0x07000		/* Instructions Per ms Min */
#define NSS_FREQ_748_MAX	0x14000		/* Instructions Per ms Max */
#endif

#define NSS_FREQ_800		800000000	/* Frequency in hz */
#define NSS_FREQ_800_MIN	0x07000		/* Instructions Per ms Min */
#define NSS_FREQ_800_MAX	0x25000		/* Instructions Per ms Max */

#define NSS_FREQ_850		850000000	/* Frequency in hz */
#define NSS_FREQ_850_MIN	0x07000		/* Instructions Per ms Min */
#define NSS_FREQ_850_MAX	0x0c000		/* Instructions Per ms Max */

#define NSS_FREQ_1000		1000000000	/* Frequency in hz */
#define NSS_FREQ_1000_MIN	0x0c000		/* Instructions Per ms Min */
#define NSS_FREQ_1000_MAX	0x25000		/* Instructions Per ms Max */

#define NSS_FREQ_1497		1497600000	/* Frequency in hz */
#if defined(NSS_HAL_IPQ60XX_SUPPORT)
#define NSS_FREQ_1497_MIN	0x18000		/* Instructions Per ms Min */
#define NSS_FREQ_1497_MAX	0x25000		/* Instructions Per ms Max */
#else
#define NSS_FREQ_1497_MIN	0x14000		/* Instructions Per ms Min */
#define NSS_FREQ_1497_MAX	0x25000		/* Instructions Per ms Max */
#endif

#define NSS_FREQ_1689		1689600000	/* Frequency in hz */
#define NSS_FREQ_1689_MIN	0x14000		/* Instructions Per ms Min */
#define NSS_FREQ_1689_MAX	0x25000		/* Instructions Per ms Max */

#if (NSS_DT_SUPPORT == 1)
#define NSSTCM_FREQ		400000000	/* NSS TCM Frequency in Hz */

/*
 * NSS Clock names
 */
#define NSS_CORE_CLK		"nss-core-clk"
#define NSS_TCM_SRC_CLK		"nss-tcm-src"
#define NSS_TCM_CLK		"nss-tcm-clk"
#define NSS_FABRIC0_CLK		"nss-fab0-clk"
#define NSS_FABRIC1_CLK		"nss-fab1-clk"

/*
 * NSS Fabric speeds
 */
#define NSS_FABRIC0_TURBO	533000000
#define NSS_FABRIC1_TURBO	266500000
#define NSS_FABRIC0_NOMINAL	400000000
#define NSS_FABRIC1_NOMINAL	200000000
#define NSS_FABRIC0_IDLE	133333000
#define NSS_FABRIC1_IDLE	133333000
#endif

/* Default NSS packet queue limit. */
#define NSS_DEFAULT_QUEUE_LIMIT 256

/*
 * Gives us important data from NSS platform data
 */
extern struct nss_top_instance nss_top_main;

/*
 * NSS core state
 */
enum nss_core_state {
	NSS_CORE_STATE_UNINITIALIZED = 0,
	NSS_CORE_STATE_INITIALIZED,
	/*
	 * in following cases, only interrupts work
	 */
	NSS_CORE_STATE_FW_DEAD = 2,
	NSS_CORE_STATE_FW_DUMP = 4,
	NSS_CORE_STATE_PANIC = 8,
};

/*
 * Forward declarations
 */
struct nss_top_instance;
struct nss_ctx_instance;
struct int_ctx_instance;
struct net_dev_priv_instance;

/*
 * Network device private data instance
 */
struct netdev_priv_instance {
	struct int_ctx_instance *int_ctx;	/* Back pointer to interrupt context */
};

/*
 * Interrupt context instance (one per queue per NSS core)
 */
struct int_ctx_instance {
	struct nss_ctx_instance *nss_ctx;
					/* Back pointer to NSS context of core that
					   owns this interrupt */
	uint32_t irq;			/* HLOS IRQ numbers bind to this instance */
	uint32_t shift_factor;	/* Shift factor for this IRQ queue */
	uint32_t cause;			/* Interrupt cause carried forward to BH */
	struct napi_struct napi;/* NAPI handler */
};

/*
 * N2H descriptor ring information
 */
struct hlos_n2h_desc_ring {
	struct n2h_desc_if_instance desc_ring;
					/* Descriptor ring */
	uint32_t hlos_index;		/* Current HLOS index for this ring */
	struct sk_buff *head;		/* First segment of an skb fraglist */
	struct sk_buff *tail;		/* Last segment received of an skb fraglist */
	struct sk_buff *jumbo_start;	/* First segment of an skb with frags[] */
};

/*
 * H2N descriptor ring information
 */
struct hlos_h2n_desc_rings {
	struct h2n_desc_if_instance desc_ring;	/* Descriptor ring */
	uint32_t hlos_index;
	spinlock_t lock;			/* Lock to save from simultaneous access */
	uint32_t flags;				/* Flags */
	uint64_t tx_q_full_cnt;			/* Descriptor queue full count */
};

#define NSS_H2N_DESC_RING_FLAGS_TX_STOPPED 0x1	/* Tx has been stopped for this queue */

/*
 * struct nss_shaper_bounce_registrant
 *	Registrant detail for shaper bounce operations
 */
struct nss_shaper_bounce_registrant {
	nss_shaper_bounced_callback_t bounced_callback;		/* Invoked for each shaper bounced packet returned from the NSS */
	void *app_data;						/* Argument given to the callback */
	struct module *owner;					/* Owning module of the callback + arg */
	bool registered;
	volatile bool callback_active;				/* true when the bounce callback is being called */
};

/*
 * CB function declarations
 */
typedef void (*nss_core_rx_callback_t)(struct nss_ctx_instance *, struct nss_cmn_msg *, void *);

/*
 * NSS Rx per interface callback structure
 */
struct nss_rx_cb_list {
	nss_if_rx_msg_callback_t msg_cb;
	nss_core_rx_callback_t cb;
	void *app_data;
};

/*
 * NSS core <-> subsystem data plane registration related paramaters.
 *	This struct is filled with if_register/data_plane register APIs and
 *	retrieved when handling a data packet/skb destined to that subsystem.
 */
struct nss_subsystem_dataplane_register {
	nss_phys_if_rx_callback_t cb;	/* callback to be invoked */
	nss_phys_if_xmit_callback_t xmit_cb;
					/* Callback to be invoked for sending the packets to the transmit path */
	nss_phys_if_rx_ext_data_callback_t ext_cb;
					/* Extended data plane callback to be invoked.
					   This is needed if driver needs extended handling
					   of data packet before giving to stack */
	void *app_data;			/* additional info passed during callback(for future use) */
	struct net_device *ndev;	/* Netdevice associated with the interface */
	uint32_t features;		/* skb types supported by this subsystem */
	uint32_t type;			/* Indicates the type of this data plane */
};

/*
 * Holds statistics for every worker thread on a core
 */
struct nss_worker_thread_stats {
	struct nss_project_irq_stats *irq_stats;
};

/*
 * NSS context instance (one per NSS core)
 */
struct nss_ctx_instance {
	struct nss_top_instance *nss_top;
					/* Back pointer to NSS Top */
	struct device *dev;		/* Pointer to the original device from probe */
	struct net_device napi_ndev;    /* Dummy_netdev for NAPI */
	uint32_t id;			/* Core ID for this instance */
	void __iomem *nmap;		/* Pointer to NSS CSM registers */
	void __iomem *vmap;		/* Virt mem pointer to virtual register map */
	void __iomem *qgic_map;		/* Virt mem pointer to QGIC register */
	uint32_t nphys;			/* Phys mem pointer to CSM register map */
	uint32_t vphys;			/* Phys mem pointer to virtual register map */
	uint32_t qgic_phys;		/* Phys mem pointer to QGIC register map */
	uint32_t load;			/* Load address for this core */
	struct nss_meminfo_ctx meminfo_ctx;	/* Meminfo context */
	enum nss_core_state state;	/* State of NSS core */
	uint32_t c2c_start;		/* C2C start address */
	uint32_t num_irq;                /* IRQ numbers per queue */
	struct int_ctx_instance int_ctx[NSS_MAX_IRQ_PER_CORE];
					/* Interrupt context instances for each queue */
	struct hlos_h2n_desc_rings h2n_desc_rings[NSS_H2N_DESC_RING_NUM];
					/* Host to NSS descriptor rings */
	struct hlos_n2h_desc_ring n2h_desc_ring[NSS_N2H_DESC_RING_NUM];
					/* NSS to Host descriptor rings */
	uint16_t rps_en;		/* N2H Enable Multiple queues for Data Packets */
	uint16_t n2h_mitigate_en;	/* N2H mitigation */
	uint32_t max_buf_size;		/* Maximum buffer size */
	uint32_t buf_sz_allocated;	/* size of bufs allocated from host */
	nss_cmn_queue_decongestion_callback_t queue_decongestion_callback[NSS_MAX_CLIENTS];
					/* Queue decongestion callbacks */
	void *queue_decongestion_ctx[NSS_MAX_CLIENTS];
					/* Queue decongestion callback contexts */
	nss_cmn_service_code_callback_t service_code_callback[NSS_MAX_SERVICE_CODE];
					/* Service code callbacks */
	void *service_code_ctx[NSS_MAX_SERVICE_CODE];
					/* Service code callback contexts */
	spinlock_t decongest_cb_lock;	/* Lock to protect queue decongestion cb table */
	uint16_t phys_if_mtu[NSS_MAX_PHYSICAL_INTERFACES];
					/* Current MTU value of physical interface */
	uint32_t worker_thread_count;	/* Number of NSS core worker threads for statistics */
	uint32_t irq_count;		/* Number of NSS core IRQs for statistics */
	struct nss_worker_thread_stats *wt_stats;
					/* Worker thread statistics */
	struct nss_unaligned_stats unaligned_stats;
					/* Unaligned emulation performance statistics */
	struct nss_rx_cb_list nss_rx_interface_handlers[NSS_MAX_NET_INTERFACES];
					/* NSS interface callback handlers */
	struct nss_subsystem_dataplane_register subsys_dp_register[NSS_MAX_NET_INTERFACES];
					/* Subsystem registration data */
	uint32_t magic;
					/* Magic protection */
};

/*
 * Main NSS context structure (singleton)
 */
struct nss_top_instance {
	uint8_t num_nss;			/* Number of NSS cores supported */
	uint8_t num_phys_ports;			/* Number of physical ports supported */
	uint32_t clk_src;			/* Clock source: default/alternate */
	spinlock_t lock;			/* Big lock for NSS driver */
	spinlock_t stats_lock;			/* Statistics lock */
	struct mutex wq_lock;			/* Mutex for NSS Work queue function */
	struct dentry *top_dentry;		/* Top dentry for nss */
	struct dentry *stats_dentry;		/* Top dentry for nss stats */
	struct dentry *strings_dentry;		/* Top dentry for nss stats strings */
	struct dentry *project_dentry;		/* per-project stats dentry */
	struct nss_ctx_instance nss[NSS_MAX_CORES];
						/* NSS contexts */
	/*
	 * Network processing handler core ids (CORE0/CORE1) for various interfaces
	 */
	uint8_t phys_if_handler_id[NSS_MAX_PHYSICAL_INTERFACES];
	uint8_t virt_if_handler_id;
	uint8_t gre_redir_handler_id;
	uint8_t gre_redir_lag_us_handler_id;
	uint8_t gre_redir_lag_ds_handler_id;
	uint8_t gre_tunnel_handler_id;
	uint8_t shaping_handler_id;
	uint8_t ipv4_handler_id;
	uint8_t ipv4_reasm_handler_id;
	uint8_t ipv6_handler_id;
	uint8_t ipv6_reasm_handler_id;
	uint8_t crypto_handler_id;
	uint8_t ipsec_handler_id;
	uint8_t wlan_handler_id;
	uint8_t tun6rd_handler_id;
	uint8_t wifi_handler_id;
	uint8_t ppe_handler_id;
	uint8_t pptp_handler_id;
	uint8_t pppoe_handler_id;
	uint8_t l2tpv2_handler_id;
	uint8_t dtls_handler_id;
	uint8_t gre_handler_id;
	uint8_t map_t_handler_id;
	uint8_t tunipip6_handler_id;
	uint8_t frequency_handler_id;
	uint8_t sjack_handler_id;
	uint8_t capwap_handler_id;
	uint8_t tstamp_handler_id;
	uint8_t portid_handler_id;
	uint8_t oam_handler_id;
	uint8_t edma_handler_id;
	uint8_t bridge_handler_id;
	uint8_t trustsec_tx_handler_id;
	uint8_t vlan_handler_id;
	uint8_t qvpn_handler_id;
	uint8_t pvxlan_handler_id;
	uint8_t igs_handler_id;
	uint8_t gre_redir_mark_handler_id;
	uint8_t clmap_handler_id;
	uint8_t vxlan_handler_id;
	uint8_t rmnet_rx_handler_id;
	uint8_t match_handler_id;
	uint8_t tls_handler_id;
	uint8_t mirror_handler_id;
	uint8_t wmdb_handler_id;
	uint8_t dma_handler_id;
	uint8_t udp_st_handler_id;

	/*
	 * Data/Message callbacks for various interfaces
	 */
	nss_phys_if_msg_callback_t phys_if_msg_callback[NSS_MAX_PHYSICAL_INTERFACES];
					/* Physical interface event callback functions */
	nss_virt_if_msg_callback_t virt_if_msg_callback[NSS_MAX_VIRTUAL_INTERFACES];
					/* Virtual interface messsage callback functions */
	nss_ipv4_msg_callback_t ipv4_callback;
					/* IPv4 sync/establish callback function */
	nss_ipv6_msg_callback_t ipv6_callback;
					/* IPv6 sync/establish callback function */
	nss_ipsec_msg_callback_t ipsec_encap_callback;
	nss_ipsec_msg_callback_t ipsec_decap_callback;
					/* IPsec event callback function */
	nss_crypto_msg_callback_t crypto_msg_callback;
	nss_crypto_cmn_msg_callback_t crypto_cmn_msg_callback;
	nss_crypto_buf_callback_t crypto_buf_callback;
	nss_crypto_pm_event_callback_t crypto_pm_callback;
					/* crypto interface callback functions */
	nss_profiler_callback_t profiler_callback[NSS_MAX_CORES];
					/* Profiler interface callback function */
	nss_tun6rd_msg_callback_t tun6rd_msg_callback;
					/* 6rd tunnel interface event callback function */
	nss_wifi_msg_callback_t wifi_msg_callback;
					/* wifi interface event callback function */
	nss_l2tpv2_msg_callback_t l2tpv2_msg_callback;
					/* l2tP tunnel interface event callback function */
	nss_dtls_msg_callback_t dtls_msg_callback; /* dtls interface event callback */

	nss_gre_tunnel_msg_callback_t gre_tunnel_msg_callback; /* gre tunnel interface event callback */

	nss_map_t_msg_callback_t map_t_msg_callback;
					/* map-t interface event callback function */
	nss_gre_msg_callback_t gre_msg_callback;
					/* gre interface event callback function */
	nss_gre_data_callback_t gre_inner_data_callback;
					/* gre inner data callback function */
	nss_gre_data_callback_t gre_outer_data_callback;
					/* gre outer data callback function */
	nss_tunipip6_msg_callback_t tunipip6_msg_callback;
					/* ipip6 tunnel interface event callback function */
	nss_pptp_msg_callback_t pptp_msg_callback;
					/* PPTP tunnel interface event callback function */
	nss_pppoe_msg_callback_t pppoe_msg_callback;
					/* PPPoE interface event callback function */
	struct nss_shaper_bounce_registrant bounce_interface_registrants[NSS_MAX_NET_INTERFACES];
					/* Registrants for interface shaper bounce operations */
	struct nss_shaper_bounce_registrant bounce_bridge_registrants[NSS_MAX_NET_INTERFACES];
					/* Registrants for bridge shaper bounce operations */
	nss_lag_event_callback_t lag_event_callback;
					/* Registrants for lag operations */
	nss_oam_msg_callback_t oam_callback;
					/* OAM call back */
	nss_edma_msg_callback_t edma_callback;
					/* EDMA callback */
	nss_bridge_msg_callback_t bridge_callback;
					/* Bridge callback */
	nss_vlan_msg_callback_t vlan_callback;
					/* Vlan callback */
	nss_wifili_msg_callback_t wifili_msg_callback;
					/* wifili interface event callback function */
	nss_ipsec_cmn_msg_callback_t ipsec_cmn_msg_callback;
					/*  IPSEC common interface event callback function */
	nss_qvpn_msg_callback_t qvpn_msg_callback;
					/* QVPN interface event callback function */
	nss_rmnet_rx_msg_callback_t rmnet_rx_msg_callback[NSS_MAX_VIRTUAL_INTERFACES];
					/* Virtual interface messsage callback functions */
	nss_wifi_mac_db_msg_callback_t wifi_mac_db_msg_callback;
					/* wifi mac database event callback function */

	uint32_t dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_MAX];

	/*
	 * Interface contexts (non network device)
	 */
	void *ipv4_ctx;			/* IPv4 connection manager context */
	void *ipv6_ctx;			/* IPv6 connection manager context */
	void *crypto_ctx;		/* Crypto interface context */
	void *crypto_pm_ctx;		/* Crypto PM context */
	void *profiler_ctx[NSS_MAX_CORES];
					/* Profiler interface context */
	void *ipsec_encap_ctx;		/* IPsec encap context */
	void *ipsec_decap_ctx;		/* IPsec decap context */
	void *oam_ctx;			/* oam context */
	void *edma_ctx;			/* edma context */
	void *bridge_ctx;		/* Bridge context */
	void *vlan_ctx;			/* Vlan context */

	/*
	 * Statistics for various interfaces
	 */
	atomic64_t stats_drv[NSS_DRV_STATS_MAX];
					/* Hlos driver statistics */
	uint64_t stats_gmac[NSS_MAX_PHYSICAL_INTERFACES][NSS_GMAC_STATS_MAX];
					/* GMAC statistics */
	uint64_t stats_node[NSS_MAX_NET_INTERFACES][NSS_STATS_NODE_MAX];
					/* IPv4 statistics per interface */
	bool nss_hal_common_init_done;

	uint16_t prev_mtu_sz;		/* mtu sz needed as of now */
	uint16_t crypto_enabled;	/* check if crypto is enabled on the platform */

	/*
	 * TODO: Review and update following fields
	 */
	uint64_t last_rx_jiffies;	/* Time of the last RX message from the NA in jiffies */
	struct nss_hal_ops *hal_ops;	/* nss_hal ops for this target platform */
	struct nss_data_plane_ops *data_plane_ops;
					/* nss_data_plane ops for this target platform */
};

#if (NSS_PKT_STATS_ENABLED == 1)
/*
 * nss_pkt_stats_inc()
 */
static inline void nss_pkt_stats_inc(atomic64_t *stat)
{
	atomic64_inc(stat);
}

/*
 * nss_pkt_stats_dec()
 */
static inline void nss_pkt_stats_dec(atomic64_t *stat)
{
	atomic64_dec(stat);
}

/*
 * nss_pkt_stats_add()
 */
static inline void nss_pkt_stats_add(atomic64_t *stat, uint32_t pkt)
{
	atomic64_add(pkt, stat);
}

/*
 * nss_pkt_stats_sub()
 */
static inline void nss_pkt_stats_sub(atomic64_t *stat, uint32_t pkt)
{
	atomic64_sub(pkt, stat);
}

/*
 * nss_pkt_stats_read()
 */
static inline uint64_t nss_pkt_stats_read(atomic64_t *stat)
{
	return atomic64_read(stat);
}

#endif

/*
 * NSS Statistics and Data for User Space
 */
struct nss_cmd_buffer {
	uint32_t current_freq;	/* Current Running Freq of NSS */
	int32_t auto_scale;	/* Enable or Disable auto_scale */
	int32_t max_freq;	/* Maximum supported frequency index value */
	uint32_t register_addr;	/* register addr buffer */
	uint32_t register_data;	/* register data buffer */
	uint32_t average_inst;	/* average of inst for nss core */
	uint32_t coredump;	/* cmd coredump buffer */
};
extern struct nss_cmd_buffer nss_cmd_buf;

/*
 * The scales for NSS
 */
typedef enum nss_freq_scales {
	NSS_FREQ_LOW_SCALE = 0,
	NSS_FREQ_MID_SCALE = 1,
	NSS_FREQ_HIGH_SCALE = 2,
	NSS_FREQ_MAX_SCALE = 3,
} nss_freq_scales_t;

/*
 * NSS Core Statistics and Frequencies
 */
#define NSS_SAMPLE_BUFFER_SIZE 4			/* Ring Buffer should be a Size of two */
#define NSS_SAMPLE_BUFFER_MASK (NSS_SAMPLE_BUFFER_SIZE - 1)
#define NSS_FREQUENCY_SCALE_RATE_LIMIT_UP 2		/* Adjust the Rate of Frequency Switching Up */
#define NSS_FREQUENCY_SCALE_RATE_LIMIT_DOWN 60000	/* Adjust the Rate of Frequency Switching Down */
#define NSS_MESSAGE_RATE_LIMIT 15000			/* Adjust the Rate of Displaying Statistic Messages */

/*
 * NSS Frequency Scale Info
 *
 * INFO: Contains the Scale information per Frequency
 *	Per Scale information needed to Program PLL and make switching decisions
 */
struct nss_scale_info {
	uint32_t frequency;	/* Frequency in Mhz */
	uint32_t minimum;	/* Minimum INST_CNT per Sec */
	uint32_t maximum;	/* Maximum INST_CNT per Sec */
};

/*
 * NSS Runtime Sample Structure
 *
 * INFO: Contains the runtime statistic of the NSS core
 *	Also contains the per frequency scale array
 */
struct nss_runtime_sampling {
	struct nss_scale_info freq_scale[NSS_FREQ_MAX_SCALE];	/* NSS Max Scale Per Freq */
	nss_freq_scales_t freq_scale_index;			/* Current Freq Index */
	uint32_t freq_scale_ready;				/* Allow Freq Scaling */
	uint32_t freq_scale_rate_limit_up;			/* Scaling Change Rate Limit */
	uint32_t freq_scale_rate_limit_down;			/* Scaling Change Rate Limit */
	uint32_t buffer[NSS_SAMPLE_BUFFER_SIZE];		/* Sample Ring Buffer */
	uint32_t buffer_index;					/* Running Buffer Index */
	uint32_t sum;						/* Total INST_CNT SUM */
	uint32_t sample_count;					/* Number of Samples stored in Ring Buffer */
	uint32_t average;					/* Average of INST_CNT */
	uint32_t message_rate_limit;				/* Debug Message Rate Limit */
	uint32_t initialized;					/* Flag to check for adequate initial samples */
};

/*
 * cpu_utilization
 */
struct nss_freq_cpu_usage {
	uint32_t used;					/* CPU utilization at a certain frequency percentage */
	uint32_t max_ins;				/* Maximum instructions that can be executed in 1ms at the current frequency
								This value is calculated by diving frequency by 1000.	*/
	uint32_t total;					/* Total usage added over a time of NSS_FREQ_USG_AVG_FREQUENCY milliseconds*/
	uint32_t max;					/* Maximum CPU usage since the boot (%) */
	uint32_t min;					/* Minimum CPU usage since the boot (%) */
	uint32_t avg_up;				/* Actual upper bound of the CPU USAGE (%)*/
	uint16_t avg_ctr;				/* Averaging counter */
};

#if (NSS_DT_SUPPORT == 1)
/*
 * nss_feature_enabled
 */
enum nss_feature_enabled {
	NSS_FEATURE_NOT_ENABLED = 0,		/* Feature is not enabled on this core */
	NSS_FEATURE_ENABLED,			/* Feature is enabled on this core */
};

/*
 * nss_platform_data
 *      Platform data per core
 */
struct nss_platform_data {
	uint32_t id;				/* NSS core ID */
	uint32_t num_queue;			/* No. of queues supported per core */
	uint32_t num_irq;			/* No. of irq binded per queue */
	uint32_t irq[NSS_MAX_IRQ_PER_CORE];	/* IRQ numbers per queue */
	void __iomem *nmap;			/* Virtual addr of NSS CSM space */
	void __iomem *vmap;			/* Virtual addr of NSS virtual register map */
	void __iomem *qgic_map;			/* Virtual addr of QGIC interrupt register */
	uint32_t nphys;				/* Physical addr of NSS CSM space */
	uint32_t vphys;				/* Physical addr of NSS virtual register map */
	uint32_t qgic_phys;			/* Physical addr of QGIC virtual register map */
	uint32_t load_addr;			/* Load address of NSS firmware */

	enum nss_feature_enabled capwap_enabled;
				/* Does this core handle capwap? */
	enum nss_feature_enabled crypto_enabled;
				/* Does this core handle crypto? */
	enum nss_feature_enabled dtls_enabled;
				/* Does this core handle DTLS sessions ? */
	enum nss_feature_enabled gre_redir_enabled;
				/* Does this core handle gre_redir Tunnel ? */
	enum nss_feature_enabled gre_tunnel_enabled;
				/* Does this core handle gre_tunnel Tunnel ? */
	enum nss_feature_enabled ipsec_enabled;
				/* Does this core handle IPsec? */
	enum nss_feature_enabled ipv4_enabled;
				/* Does this core handle IPv4? */
	enum nss_feature_enabled ipv4_reasm_enabled;
				/* Does this core handle IPv4 reassembly? */
	enum nss_feature_enabled ipv6_enabled;
				/* Does this core handle IPv6? */
	enum nss_feature_enabled ipv6_reasm_enabled;
				/* Does this core handle IPv6 reassembly? */
	enum nss_feature_enabled l2tpv2_enabled;
				/* Does this core handle l2tpv2 Tunnel ? */
	enum nss_feature_enabled map_t_enabled;
				/* Does this core handle map-t */
	enum nss_feature_enabled gre_enabled;
				/* Does this core handle GRE */
	enum nss_feature_enabled oam_enabled;
				/* Does this core handle oam? */
	enum nss_feature_enabled ppe_enabled;
				/* Does this core handle ppe ? */
	enum nss_feature_enabled pppoe_enabled;
				/* Does this core handle pppoe? */
	enum nss_feature_enabled pptp_enabled;
				/* Does this core handle pptp Tunnel ? */
	enum nss_feature_enabled portid_enabled;
				/* Does this core handle portid? */
	enum nss_feature_enabled shaping_enabled;
				/* Does this core handle shaping ? */
	enum nss_feature_enabled tstamp_enabled;
				/* Does this core handle timestamping? */
	enum nss_feature_enabled turbo_frequency;
				/* Does this core support turbo frequencies */
	enum nss_feature_enabled tun6rd_enabled;
				/* Does this core handle 6rd Tunnel ? */
	enum nss_feature_enabled tunipip6_enabled;
				/* Does this core handle ipip6 Tunnel ? */
	enum nss_feature_enabled wlanredirect_enabled;
				/* Does this core handle WLAN redirect? */
	enum nss_feature_enabled wifioffload_enabled;
				/* Does this core handle WIFI OFFLOAD? */
	enum nss_feature_enabled bridge_enabled;
				/* Does this core handle bridge configuration */
	enum nss_feature_enabled vlan_enabled;
				/* Does this core handle vlan configuration */
	enum nss_feature_enabled qvpn_enabled;
				/* Does this core handle QVPN Tunnel ? */
	enum nss_feature_enabled pvxlan_enabled;
				/* Does this core handle pvxlan? */
	enum nss_feature_enabled igs_enabled;
				/* Does this core handle igs? */
	enum nss_feature_enabled gre_redir_mark_enabled;
				/* Does this core handle GRE redir mark? */
	enum nss_feature_enabled clmap_enabled;
				/* Does this core handle clmap? */
	enum nss_feature_enabled vxlan_enabled;
				/* Does this core handle vxlan tunnel? */
	enum nss_feature_enabled rmnet_rx_enabled;
				/* Does this core handle rmnet rx? */
	enum nss_feature_enabled match_enabled;
				/* Does this core handle match node? */
	enum nss_feature_enabled tls_enabled;
				/* Does this core handle TLS Tunnel ? */
	enum nss_feature_enabled mirror_enabled;
				/* Does this core handle mirror? */
	enum nss_feature_enabled udp_st_enabled;
				/* Does this core handle udp st? */
};
#endif

/*
 * nss_core_log_msg_failures()
 *	Driver function for logging failed messages.
 */
static inline void nss_core_log_msg_failures(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm)
{
	if ((ncm->response == NSS_CMN_RESPONSE_ACK) || (ncm->response == NSS_CMN_RESPONSE_NOTIFY)) {
		return;
	}

	/*
	 * TODO: Is it worth doing value to name on these values?
	 */
	nss_warning("%px: msg failure - interface: %d, type: %d, response: %d, error: %d",
		nss_ctx, ncm->interface, ncm->type, ncm->response, ncm->error);
}

/*
 * NSS workqueue to change frequencies
 */
typedef struct {
	struct work_struct my_work;	/* Work Structure */
	uint32_t frequency;		/* Frequency To Change */
	uint32_t stats_enable;		/* Auto scale on/off */
} nss_work_t;

/*
 * APIs provided by nss_core.c
 */
extern int nss_core_handle_napi(struct napi_struct *napi, int budget);
extern int nss_core_handle_napi_queue(struct napi_struct *napi, int budget);
extern int nss_core_handle_napi_non_queue(struct napi_struct *napi, int budget);
extern int nss_core_handle_napi_emergency(struct napi_struct *napi, int budget);
extern int nss_core_handle_napi_sdma(struct napi_struct *napi, int budget);
extern int32_t nss_core_send_buffer(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					struct sk_buff *nbuf, uint16_t qid,
					uint8_t buffer_type, uint16_t flags);
extern int32_t nss_core_send_cmd(struct nss_ctx_instance *nss_ctx, void *msg, int size, int buf_size);
extern int32_t nss_core_send_packet(struct nss_ctx_instance *nss_ctx, struct sk_buff *nbuf, uint32_t if_num, uint32_t flag);
extern uint32_t nss_core_ddr_info(struct nss_mmu_ddr_info *coreinfo);
extern uint32_t nss_core_register_msg_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface, nss_if_rx_msg_callback_t msg_cb);
extern uint32_t nss_core_unregister_msg_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface);
extern uint32_t nss_core_register_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface, nss_core_rx_callback_t cb, void *app_data);
extern uint32_t nss_core_unregister_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface);
extern void nss_core_init_handlers(struct nss_ctx_instance *nss_ctx);
void nss_core_update_max_ipv4_conn(int conn);
void nss_core_update_max_ipv6_conn(int conn);
extern void nss_core_register_subsys_dp(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					nss_phys_if_rx_callback_t cb,
					nss_phys_if_rx_ext_data_callback_t ext_cb,
					void *app_data, struct net_device *ndev,
					uint32_t features);
extern void nss_core_unregister_subsys_dp(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
void nss_core_set_subsys_dp_type(struct nss_ctx_instance *nss_ctx, struct net_device *ndev, uint32_t if_num, uint32_t type);

static inline nss_if_rx_msg_callback_t nss_core_get_msg_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface)
{
	return nss_ctx->nss_rx_interface_handlers[interface].msg_cb;
}

static inline uint32_t nss_core_get_max_buf_size(struct nss_ctx_instance *nss_ctx)
{
	return nss_ctx->max_buf_size;
}

/*
 * APIs provided by nss_tx_rx.c
 */
extern void nss_rx_handle_status_pkt(struct nss_ctx_instance *nss_ctx, struct sk_buff *nbuf);

/*
 * APIs provided by nss_stats.c
 */
extern void nss_stats_init(void);
extern void nss_stats_clean(void);

/*
 * APIs provided by nss_log.c
 */
extern void nss_log_init(void);
extern bool nss_debug_log_buffer_alloc(uint8_t nss_id, uint32_t nentry);
extern int nss_logbuffer_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos);

/*
 * APIs to set jumbo_mru & paged_mode
 */
extern void nss_core_set_jumbo_mru(int jumbo_mru);
extern int nss_core_get_jumbo_mru(void);
extern void nss_core_set_paged_mode(int mode);
extern int nss_core_get_paged_mode(void);
#if (NSS_SKB_REUSE_SUPPORT == 1)
extern void nss_core_set_max_reuse(int max);
extern int nss_core_get_max_reuse(void);
extern uint32_t nss_core_get_min_reuse(struct nss_ctx_instance *nss_ctx);
#endif

/*
 * APIs for coredump
 */
extern void nss_coredump_notify_register(void);
extern void nss_fw_coredump_notify(struct nss_ctx_instance *nss_own, int intr);
extern int nss_coredump_init_delay_work(void);

/*
 * APIs provided by nss_freq.c
 */
extern bool nss_freq_sched_change(nss_freq_scales_t index, bool auto_scale);

/*
 * nss_freq_init_cpu_usage
 *	Initializes the cpu usage computation.
 */
extern void nss_freq_init_cpu_usage(void);

/*
 * APIs for PPE
 */
extern void nss_ppe_init(void);
extern void nss_ppe_free(void);

/*
 * APIs for N2H
 */
extern nss_tx_status_t nss_n2h_cfg_empty_pool_size(struct nss_ctx_instance *nss_ctx, uint32_t pool_sz);
extern nss_tx_status_t nss_n2h_paged_buf_pool_init(struct nss_ctx_instance *nss_ctx);

#endif /* __NSS_CORE_H */
