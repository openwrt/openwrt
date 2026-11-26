#ifndef _DPNS_COMMON_H_
#define _DPNS_COMMON_H_

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/types.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/rhashtable.h>
#include <linux/netfilter.h>
#include <linux/reset.h>
#include <linux/platform_device.h>
#include "dma.h"
#include "sfxgmac-ext.h"
#include "nat.h"
#include "hw.h"

static inline void dev_dp_port_store(const struct net_device *dev, void *ptr)
{
	struct gmac_common *priv = netdev_priv(dev);

	priv->dp_port = ptr;
}

static inline void *dev_dp_port_fetch(const struct net_device *dev)
{
	struct gmac_common *priv = netdev_priv(dev);

	return priv->dp_port;
}

#define HASH1_TABLE_BEGIN	2
#define EXTDEV_OFFSET		7
#define CRC_BIT_COUNT		8
#define DPNS_HOST_PORT		6
#define DPNS_MAX_PORT		27
#define DPNS_MAX_IRQ_CNT    	58

#define SE_TCAM_CONFIG1                 0x19000c
#define TCAM_CONFIG1_P4_FIFO_BP_THRESH  GENMASK(18, 16)
#define TCAM_CONFIG1_P3_FIFO_BP_THRESH  GENMASK(14, 12)
#define TCAM_CONFIG1_P2_FIFO_BP_THRESH  GENMASK(10, 8)
#define TCAM_CONFIG1_P1_FIFO_BP_THRESH  GENMASK(6, 4)
#define TCAM_CONFIG1_P0_FIFO_BP_THRESH  GENMASK(2, 0)
#define TCAM_LINE		16

#define CONFIG1_RGT_ADDR		0x18000C	// L2 Hash poly selection

#define CONFIG2_RGT_ADDR		0x180010	// global function en/disable
#define CONFIG2_MAC_SEG_NUM		GENMASK(3, 0)	//Maximum sub table
#define CONFIG2_MAC_AGE_EN		BIT(4)		// aging read-clear control
#define CONFIG2_MACSPL_MODE_EN		BIT(5)
#define CONFIG2_MAC_TAB_EN		BIT(6)
#define CONFIG2_INTF_VID_EN		BIT(7)
#define CONFIG2_EVLKP_CFG_ENTR_VLD	GENMASK(13, 8)
#define CONFIG2_EVLKP_CFG_DIS_TB	BIT(14)
#define CONFIG2_EVLAN_OTPID_EN		BIT(15)
#define CONFIG2_EVLAN_PTPID_EN		BIT(16)
#define CONFIG2_MAC_SPL_CNT_MODE		GENMASK(18, 17)
#define CONFIG2_MAC_SPL_ZERO_LMT_EN		BIT(19)
#define CONFIG2_L2_MFSPL_MODE		BIT(20)
#define CONFIG2_L2_MFSPL_CNT_MODE		GENMASK(22, 21)
#define CONFIG2_L2_MFSPL_ZERO_LIMIT		BIT(23)
#define CONFIG2_EVXLT_CFG_ENTR_VLD	GENMASK(29, 24)
#define CONFIG2_EVXLT_CFG_DIS_TB	BIT(30)
#define CONFIG2_EVACT_EN		BIT(31)

#define SE_IRAM_OPT_ADDR	0x18003c		// table operations
#define IRAM_OPT_BUSY		BIT(31)			// 31: 1
#define IRAM_OPT_WR		BIT(24)			// 24: 1
#define IRAM_OPT_ID		GENMASK(20, 16)		// 16: 5bit
#define IRAM_OPT_REQ_ADDR	GENMASK(15, 0)		// 16bit
#define IRAM_W_ADDR(idx)	0x180040 + ((idx)*4)	// 16 x 4
#define IRAM_R_ADDR(idx)	0x180080 + ((idx)*4)	// 16 x 4

#define SE_TCAM_OPT_ADDR	0x19003c
#define TCAM_OPT_BUSY		BIT(31)
#define TCAM_OPT_WR		BIT(24)
#define TCAM_OPT_ID		GENMASK(22, 16)
#define TCAM_OPT_REQ_ADDR	GENMASK(4, 0)
/* 3 x 4, size of the last register is one byte, thus only 9 bytes can be
 * accessed at once */
#define TCAM_SLICE_SIZE		9
#define TCAM_W_ADDR(idx)	(0x190040 + (idx)*4)
#define TCAM_R_ADDR(idx)	(0x190080 + (idx)*4)

#define SE_TCAM_STATUS	0x190000
#define TCAM_STATUS_P4_BP_OVF BIT(4)
#define TCAM_STATUS_P3_BP_OVF BIT(3)
#define TCAM_STATUS_P2_BP_OVF BIT(2)
#define TCAM_STATUS_P1_BP_OVF BIT(1)
#define TCAM_STATUS_P0_BP_OVF BIT(0)

#define SE_TCAM_CLR	0x190004
#define TCAM_CLR_BLK4_TB BIT(4)
#define TCAM_CLR_BLK3_TB BIT(3)
#define TCAM_CLR_BLK2_TB BIT(2)
#define TCAM_CLR_BLK1_TB BIT(1)
#define TCAM_CLR_BLK0_TB BIT(0)

#define SE_TCAM_BLK_CONFIG0	0x190008
#define TCAM_BLK_CONFIG0_BLK4_CFG GENMASK(14, 12)
#define TCAM_BLK_CONFIG0_BLK3_CFG GENMASK(11, 9)
#define TCAM_BLK_CONFIG0_BLK2_CFG GENMASK(8, 6)
#define TCAM_BLK_CONFIG0_BLK1_CFG GENMASK(5, 3)
#define TCAM_BLK_CONFIG0_BLK0_CFG GENMASK(2, 0)

#define CONFIG0_RGT_ADDR			0x180008
#define CONFIG0_PORTBV_EN			BIT(9)		// port base vlan enable
#define CONFIG0_IPORT_EN			BIT(8)

#define CONFIG_L2_MPP_CFG2_ADDR     		0x24004

#define SE_SPL_CONFIG0_RGT_REG_ADDR 		0x180028 //mac spl timer
#define SE_IRAM_OPT_ADDR			0x18003c		// table operations

#define SE_MAC_AGE_RAM				0x198000

#define ETHSYS_MRI_Q_EN				0xb8
#define ETHSYS_MRI_OVPORT_TOP_PRIO		GENMASK(5, 0)
#define XGMAC_MTL_RXQ_DMA_MAP0			0x00001030

#define ETHSYS_SHP0_CTRL            0x94
#define ETHSYS_SHP0_MAX_CRDT        GENMASK(29, 8)
#define ETHSYS_SHP0_CLK_DIV         GENMASK(4, 1)
#define ETHSYS_SHP0_EN              BIT(0)

#define ETHSYS_SHP0_WGHT            0x9c
#define ETHSYS_SHP0_MIN_CRDT        0xa0

#define SE_MAC_LKP_REQ				0x180030
#define SE_MAC_KEY_RAM_DATA0			0x198100
#define SE_MAC_RAM_DATA_SIZE			0x4
#define SE_MAC_KEY_RAM_DATA(n)			(SE_MAC_KEY_RAM_DATA0 + (n) * SE_MAC_RAM_DATA_SIZE)

#define SE_HW_KEY_DATA0_MAC_0_31		GENMASK(31, 0)
#define SE_HW_KEY_DATA1_MAC_32_47		GENMASK(15, 0)
#define SE_HW_KEY_DATA1_VID			GENMASK(27, 16)
#define SE_HW_KEY_DATA1_IPORT_0_3		GENMASK(31, 28)
#define SE_HW_KEY_DATA2_IPORT_4			BIT(0)
#define SE_HW_KEY_DATA2_V4_FLAG			BIT(1)

#define SE_MAC_RESULT_RAM_DATA0			0x198180
#define SE_MAC_RESULT_RAM_DATA(n)		(SE_MAC_RESULT_RAM_DATA0 + (n) * SE_MAC_RAM_DATA_SIZE)

#define SE_HW_RESULT0_DATA0_BITMAP_0_18		GENMASK(22, 4)
#define SE_HW_RESULT0_DATA0_SA_CML		GENMASK(3, 2)
#define SE_HW_RESULT0_DATA0_DA_CML		GENMASK(1, 0)

#define SE_HW_RESULT0_DATA1_HIT			BIT(22)
#define SE_HW_RESULT0_DATA1_VLAN_OFFLOAD	BIT(21)
#define SE_HW_RESULT0_DATA1_ISO_OFFLOAD		BIT(20)
#define SE_HW_RESULT0_DATA1_MAC_IDX		GENMASK(19, 9)
#define SE_HW_RESULT0_DATA1_ISO_FLAG		BIT(8)
#define SE_HW_RESULT0_DATA1_BITMAP_19_26	GENMASK(7, 0)

#define CLR_CTRL_RAM_ADDR	0x180004	// 16bit, clear ig,eg,vlan,pvlan,l2,tmu
#define PORT_CNT_NUM(x)     (0x280040 + (x) * 4)

#define SF_VLAN_VPORT_MAP_MAX		10

#define SF_IVLAN_LKP_TAB_MAX		64
#define SF_EVLAN_LKP_TAB_MAX		64

/* SF_VLAN_TAB_MAX is set to the maximum value
 * between SF_IVLAN_LKP_TAB_MAX and SF_EVLAN_LKP_TAB_MAX.
 * If there are any subsequent modifications to the entry length
 * or the addition of new entries
 * it is necessary to update the value of SF_VLAN_TAB_MAX accordingly
 * */
#define SF_VLAN_TAB_MAX			64

/* SF_TAB_TYPE_NO_FOUND is set to a value greater than SF_VLAN_TAB_MAX
 * If there are any subsequent modifications to the entry length
 * or the addition of new entries
 * it is necessary to update the value of SF_TAB_TYPE_NO_FOUND accordingly
 * */
#define SF_TAB_TYPE_NO_FOUND		65

#define TBID_0				0
#define TBID_1				1
#define TBID_2				2
#define TBID_3				3

#define KMOD_0				0
#define KMOD_1				1
#define KMOD_2				2
#define KMOD_3				3

#define TCAM_KMD			GENMASK(3, 2)
#define TCAM_TBID			GENMASK(1, 0)

#define TBID_KMD_VFP_V4			(FIELD_PREP(TCAM_KMD, KMOD_2) | FIELD_PREP(TCAM_TBID, TBID_0))
#define TBID_KMD_VFP_V6			(FIELD_PREP(TCAM_KMD, KMOD_3) | FIELD_PREP(TCAM_TBID, TBID_1))
#define TBID_KMD_V4_UC			(FIELD_PREP(TCAM_KMD, KMOD_0) | FIELD_PREP(TCAM_TBID, TBID_0))
#define TBID_KMD_V6_UC			(FIELD_PREP(TCAM_KMD, KMOD_2) | FIELD_PREP(TCAM_TBID, TBID_1))
#define TBID_KMD_V4_xG			(FIELD_PREP(TCAM_KMD, KMOD_1) | FIELD_PREP(TCAM_TBID, TBID_2))
#define TBID_KMD_V6_xG			(FIELD_PREP(TCAM_KMD, KMOD_2) | FIELD_PREP(TCAM_TBID, TBID_3))
#define TBID_KMD_V4_SG			(FIELD_PREP(TCAM_KMD, KMOD_1) | FIELD_PREP(TCAM_TBID, TBID_2))
#define TBID_KMD_V6_SG			(FIELD_PREP(TCAM_KMD, KMOD_3) | FIELD_PREP(TCAM_TBID, TBID_3))
#define TBID_KMD_V4_MOD0		(FIELD_PREP(TCAM_KMD, KMOD_0) | FIELD_PREP(TCAM_TBID, TBID_0))
#define TBID_KMD_V4_MOD1		(FIELD_PREP(TCAM_KMD, KMOD_0) | FIELD_PREP(TCAM_TBID, TBID_1))
#define TBID_KMD_V4_MOD2		(FIELD_PREP(TCAM_KMD, KMOD_0) | FIELD_PREP(TCAM_TBID, TBID_2))
#define TBID_KMD_V4_MOD3		(FIELD_PREP(TCAM_KMD, KMOD_1) | FIELD_PREP(TCAM_TBID, TBID_0))
#define TBID_KMD_V4_MOD4		(FIELD_PREP(TCAM_KMD, KMOD_2) | FIELD_PREP(TCAM_TBID, TBID_0))
#define TBID_KMD_V4_MOD5		(FIELD_PREP(TCAM_KMD, KMOD_2) | FIELD_PREP(TCAM_TBID, TBID_1))
#define TBID_KMD_V4_MOD6		(FIELD_PREP(TCAM_KMD, KMOD_3) | FIELD_PREP(TCAM_TBID, TBID_0))
#define TBID_KMD_V4_MOD7		(FIELD_PREP(TCAM_KMD, KMOD_3) | FIELD_PREP(TCAM_TBID, TBID_1))
#define TBID_KMD_V6_MOD0		(FIELD_PREP(TCAM_KMD, KMOD_0) | FIELD_PREP(TCAM_TBID, TBID_0))
#define TBID_KMD_V6_MOD1		(FIELD_PREP(TCAM_KMD, KMOD_2) | FIELD_PREP(TCAM_TBID, TBID_0))
#define TBID_KMD_V6_MOD2		(FIELD_PREP(TCAM_KMD, KMOD_2) | FIELD_PREP(TCAM_TBID, TBID_2))
#define TBID_KMD_V6_MOD3		(FIELD_PREP(TCAM_KMD, KMOD_2) | FIELD_PREP(TCAM_TBID, TBID_3))
#define TBID_KMD_V6_MOD7		(FIELD_PREP(TCAM_KMD, KMOD_3) | FIELD_PREP(TCAM_TBID, TBID_1))

#define MAC_SZ				2048

enum nat_offload_mode {
	OFFLOAD_OFF,
	VLAN_OFFLOAD,
	RELAY_OFFLOAD,
};

struct relay_info {
	struct net_device *dev;
	char ifname[IFNAMSIZ];
	u8   mac[ETH_ALEN];
};

enum en_iram_table_type {
    L2_SE_HASH0_TABLE          = 5,
    L2_SE_HASH1_TABLE          = 6,
    ARP_SE_MAC_TABLE           = 7,
    ARP_SE_MACSPL_TABLE        = 8,
    ARP_SE_INTF_DIRECT_TABLE   = 9,
    IVLAN_SPL_TABLE            = 0,
    IVLAN_IPORT_TABLE          = 1,
    IVLAN_XLT_LITE_HASH_TABLE  = 2 ,
    IVLAN_PBV_DIRECT_TABLE     = 3 ,
    IVLAN_LKP_LITE_HASH_TABLE  = 4 ,
    EVLAN_OTPID_DIRECT_TABLE   = 10,
    EVLAN_TPID_HASH_TABLE      = 11,
    EVLAN_XLT_LITE_HASH_TABLE   = 12,
    EVLAN_VID_LITE_HASH_TABLE   = 13,
    EVLAN_ACT_DIRECT_TABLE      = 14,
    MODIFY_HEADER_TABLE         = 16,
    L2_ISO_TABLE                =17,
    L2_MC_PORT_MAP_TABLE        =18,
    L2_MC_FLOOD_SPEED_LMT_TABLE =19,
    L2_UC_PORT_MAP_TABLE        =20,
};

enum tcam_blk_cfg {
	TCAM_VFP,
	TCAM_L3UCMCG,
	TCAM_L3MCSG,
	TCAM_IACL,
	TCAM_EACL,
	TCAM_SPL,
	TCAM_BLK_CFG_MAX,
};

enum se_opcode {
	SE_OPT_R,
	SE_OPT_W,
};

enum {
	CML_DROP = 0,
	CML_TO_CPU = 1,
	CML_FORWARD = 2,
	CML_FWD_AND_CPU = 3,
};

 enum log_level{
	EMERG_LV,
	ALERT_LV,
	CRIT_LV,
	ERR_LV,
	WARN_LV,
	NOTICE_LV,
	INFO_LV,
	DBG_LV,
};

enum module_num{
	DPNS_COMMON,
	DPNS_GENL,
	DPNS_VLAN,
	DPNS_L2,
	DPNS_NAT,
	DPNS_L3,
	DPNS_ACL,
	DPNS_TMU,
	DPNS_MCAST,
	DPNS_MAX,
};

//TODO: GENL/MCAST module
enum dpns_mem_type {
	DPNS_COMMON_M,
	DPNS_L2_M,
	DPNS_VLAN_M,
	DPNS_NAT_M,
	DPNS_L3_M,
	DPNS_MCAST_M,
	DPNS_ACL_M,
	DPNS_TMU_M,
	DPNS_CNT_M,
};

#ifdef CONFIG_DEBUG_FS
enum debugfs_node_type {
	COMMON_DEBUG,
	DPNS_INTF,
	DPNS_ISO,
	DPNS_LOG,
	DPNS_MIB,
	DPNS_DEV,
	DPNS_DEBUG,
	DPNS_MEM,
	DENTRY_MAX_NUM,
};
#endif

enum add_mode {
	FIRST_ILKP,
	FIRST_ELKP,
	SWAP_DYAM,
};

struct dpns_mem_info {
	struct list_head list;
	size_t size;
	char buf[0];
};

struct dpns_mem {
	struct list_head list;
	size_t total;
	spinlock_t lock;
};

typedef struct l2_hw_search_table {
	uint64_t mac			:48;
	uint64_t vid			:12;
	uint64_t iport			:4;
	uint64_t v4_flag		:1;
	uint64_t hit			:1;
	uint64_t vlan_offload		:1;
	uint64_t iso_offload		:1;
	uint64_t mac_index		:11;
	uint64_t iso_flags		:1;
	uint64_t da_cml			:2;
	uint64_t sa_cml			:2;
	uint64_t port_bitmap		:27;
} __packed l2_hw_search_table_t;

union mac_search_table {
	l2_hw_search_table_t table;
	uint32_t data[4];
};

struct dpns_port_vlan_info {
	struct list_head node;
	struct net_device *dev;
	u16 vlan_id;
	u8 port_id;
};

typedef struct sf_dpns_port dpns_port_t;
struct sf_dpns_port {
	struct net_device *dev;
	struct list_head vlan_list;
	spinlock_t lock;
	u8 port_id;
	u8 ref_count;
	int stp_state;
	u32 brport_flags;
	bool ctrls[DPA_CTRL_MAX];
};

struct dpns_common_priv {
	struct platform_device 	*pdev;
	void __iomem            *iobase;
	struct regmap           *ethsys;
	struct reset_control	*npu_rstc;
	struct reset_control	*npu2ddr_rstc;
	struct clk              *clk;
	int                     dpns_irq[DPNS_MAX_IRQ_CNT];
	void                    *dpns_priv;
	struct dpns_vlan_priv	*vlan_priv;
	struct dpns_router_priv	*router_priv;
	void                    *edma_priv;
	struct dpns_mac_priv	*mac_priv;
	struct dpns_nat_priv	*nat_priv;
	struct dpns_tmu_priv	*tmu_priv;
	struct dpns_mcast_priv	*mcast_priv;
#ifdef CONFIG_DEBUG_FS
	struct dentry           *debug[DENTRY_MAX_NUM];
#endif
	spinlock_t				hw_lock;
	u32						port_count;
	u32					member_ports;
	dpns_port_t				**ports;		/* mirror of every netdevice attached to dpns */
	struct notifier_block   netdevice_nb;
	struct relay_info 	relay_mac;

	int (*table_read)(struct dpns_common_priv *priv,
			u8 ram_id, u16 table_addr, u32* data, u32 size);
	int (*table_write)(struct dpns_common_priv *priv,
			u8 ram_id, u16 table_addr, u32* data, u32 size);
	int (*tcam_access)(struct dpns_common_priv *priv, int opcode, u8 req_id,
			u8 req_addr, void *data, u32 size);
	void (*tcam_update)(struct dpns_common_priv *priv, u8 block_id,
			u8 req_id, u8 req_addr, void *data, void *mask,
			u32 size, u8 tbid_and_kmd);
	void (*tcam_clean)(struct dpns_common_priv *priv, u8 block_id);
	int (*intf_add)(struct dpns_common_priv *priv, int vid, bool pppoe_en, bool tunnel_en,
			bool wan_flag, u8 *smac);
	void (*intf_del)(struct dpns_common_priv *priv, u32 index);
	bool (*port_dev_check)(struct dpns_common_priv *priv, struct net_device *dev);
	dpns_port_t *(*port_by_netdev)(struct dpns_common_priv *priv, const struct net_device* dev);
	int (*port_id_by_netdev)(struct dpns_common_priv *priv, const struct net_device *dev, u8 *port_id);
	void (*se_wait)(struct dpns_common_priv *priv, u32 reg, u32 mask);
};

/*
 * QinQ (two 802.1Q tags) on a port: the NPU handles the inner VLAN like a
 * plain VLAN; the port's egress VLAN translation pushes the outer tag in
 * front of it. One outer/inner pair per port.
 */
struct dpns_vlan_qinq {
	struct net_device *dev;	/* port device, carries the RX handler */
	u16 svid;
	u16 cvid;
	bool valid;
};

struct dpns_vlan_priv {
	struct dpns_common_priv	*cpriv;
	void __iomem            *iobase;

	struct notifier_block   netdevice_nb;
	struct notifier_block	switchdev_blocking;
	struct list_head		vlan_list;
	struct list_head		vport_list;
	spinlock_t			vlan_lock;
	spinlock_t			vport_lock;
	u32				member_ports;
	struct workqueue_struct 	*owq;

#ifdef CONFIG_DEBUG_FS
	struct dentry           *vlan_debug;
#endif

	void (*vlan_init)(struct dpns_vlan_priv *priv);
	DECLARE_BITMAP(phy_ports_bitmap, DPNS_MAX_PORT);
	DECLARE_BITMAP(ivlan_index_bitmap, SF_IVLAN_LKP_TAB_MAX);
	DECLARE_BITMAP(evlan_index_bitmap, SF_EVLAN_LKP_TAB_MAX);
	DECLARE_BITMAP(vport_index_bitmap, SF_VLAN_VPORT_MAP_MAX);
	struct dpns_vlan_qinq		qinq[DPNS_HOST_PORT];
};

struct vlan_vport_entry {
	struct list_head node;
	u16 vlan_id;
	u8 port;
	u8 vport;
	u8 vport_index;
};

struct dpns_router_priv {
	struct dpns_common_priv	*cpriv;
	void __iomem            *iobase;
	unsigned int            rt4_count;
	unsigned int            rt6_count;
	struct mutex            lock;
	struct list_head        rt4_list;
	struct list_head        rt6_list;
	struct notifier_block   fib_nb;
	struct notifier_block   netevent_nb;
	struct notifier_block   netdevice_nb;
	struct notifier_block   inetaddr_nb;
	struct workqueue_struct *owq;
	struct net_device		*rep_dev;
#ifdef CONFIG_DEBUG_FS
	struct dentry           *router_debug;
#endif
};

struct dpns_mac_priv {
	struct dpns_common_priv		*cpriv;
	spinlock_t			mac_lock;
	spinlock_t			ts_lock;
	spinlock_t			mac_tbl_lock;
	spinlock_t			bit_lock;
	spinlock_t			dev_num_lock;
	void __iomem			*iobase;
	ulong				age_update_time;
	ulong				ageing_time;
	ulong				mib_time;
	bool				dpnsmib_en;
	bool				l2_age_en;
	bool				l2_learning_en;
	bool				wan_bridge_to_br;
	struct timer_list		l2_cleanup_ts_timer;
	struct timer_list		l2_cleanup_age_timer;
	struct proc_dir_entry		*mac_mib;
	struct hlist_head		ts_list[256];
	struct list_head		mac_table_list;

	struct notifier_block		switchdev_notifier;
	struct notifier_block		switchdev_blocking_nb;
	struct notifier_block   	netdevice_nb;
	struct workqueue_struct 	*owq;
	struct workqueue_struct 	*ubus_wq;
	u8 				mibmode;
	DECLARE_BITMAP(iso_ports, DPNS_MAX_PORT);	/* bridge ports with BR_ISOLATED */
	atomic_t 			work_cnt;

	DECLARE_BITMAP(mac_tbl_bitmap, MAC_SZ);
	DECLARE_BITMAP(dev_port_bitmap, DPNS_MAX_PORT);
	int dev_num[DPNS_MAX_PORT];
	struct sock *nl_sock;

	int (*mac_table_update)(struct dpns_mac_priv *priv,
						 const u8 *dsmac,
						 u8 valid,
						 u16 vlan_id,
						 u64 port_map,
						 u8 age_en,
						 u8 l3_en,
						 u8 sa_cml,
						 u8 da_cml,
						 u8 vlan_en,
						 u16 sta_id,
						 u16 repeater_id);

	int (*mac_table_del)(struct dpns_mac_priv *priv, u32 item_idx);
	int (*set_term_mac)(struct dpns_mac_priv *priv, const u8 *mac, u16 vlan_id, bool is_add, bool is_dev_down);
	int (*mac_del_entry)(struct dpns_mac_priv *priv, const u8 *dmac, u16 vlan_id, bool is_switchdev_event, bool is_netdev_event);
	int (*mac_init)(struct dpns_mac_priv *priv);
	int (*hw_search)(struct dpns_mac_priv *priv, const u8 *dsmac, u16 vid, u32 *result);
	void (*iso_table_dump)(struct dpns_mac_priv  *priv, u8 iport_num);
	void (*iso_table_update)(struct dpns_mac_priv  *priv, u8 iport_num, u32 port_bitmap, u32 offload_bitmap);
	u8   (*get_mibmode)(struct dpns_mac_priv *priv, const u8 *mac, u16 vlan_id, int nat_id, u16 soft_key_crc);
	int (*sf_del_ts_info)(struct dpns_mac_priv *priv, const u8 *mac, u16 vid, int nat_id, u16 soft_key_crc);
};

struct hash_position {
	bool valid;
	bool hash1;
	u16 hash;
};

struct dpns_nat_subnet_info {
	struct {
		u32 ip;
		u8 masklen;
		bool valid;
	} v4;
	struct {
		u8 ip[16];
		u8 masklen;
		bool valid;
	} v6;
	char ifname[IFNAMSIZ];
};

struct dpns_natmib_info {
	int *nat_id;
	bool natmib_en;
	uint8_t mib_mode;
	uint16_t mib_index;
	uint32_t public_ip[4];
	uint32_t private_ip[4];
	uint32_t router_ip[4];
	uint16_t public_port;
	uint16_t private_port;
	uint16_t router_port;
	bool is_v6;
	bool is_udp;
	bool is_dnat;
};

struct dpns_nat_priv {
	void __iomem *iobase;
	struct dpns_common_priv	*cpriv;
	union nat_table_u *dnat_table;
	union nat_table_u *snat_table;
	struct mutex tbl_lock;
	struct rhashtable flow_table;
	struct kmem_cache *swnapt_cache;
	dma_addr_t dnat_phys;
	dma_addr_t snat_phys;
#ifdef CONFIG_DEBUG_FS
	struct dentry *dir;
	struct dentry *entry;
#endif
	struct {
		struct hash_position dnat, snat;
	} nat_inapt01_hash[2];
	struct nf_hook_ops *nfh_ops[28];
	/**
	 * NAPT refcnts should only be 0-2.
	 * Use two bitmaps for each NAPT entry:
	 * first one for whether this entry is in use, second one to
	 * record if there are odd references to the current entry.
	 * When deleting one, flip odd_ref bit and if it's zero,
	 * we know refcnt == 0 and can remove the in_use bit.
	 */
	DECLARE_BITMAP(nat0_bitmap, NAT_ILKP_SZ);
	DECLARE_BITMAP(nat1_bitmap, NAT_ILKP_SZ);
	DECLARE_BITMAP(nat0_odd_hash, NAT_ILKP_SZ);
	DECLARE_BITMAP(nat1_odd_hash, NAT_ILKP_SZ);
	/* INAPT ids are fixed. These are for ENAPT ID allocation. */
	DECLARE_BITMAP(natid_bitmap, NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID);
	DECLARE_BITMAP(natid_odd_entries, NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID);
	DECLARE_BITMAP(stats_cache, NPU_HNAT_SIZE);
	/* hardware flow counters, see dpns_nat_mib_get() */
	DECLARE_BITMAP(mib_bitmap, DPNS_NAT_MIB_NUM);
	spinlock_t mib_lock;
	struct delayed_work visit_dwork;
	u32 elkp_size;
	int refcnt;
	u8 elkp_v4_acs_times;
	u8 elkp_v6_acs_times;
	u8 nat_offload_mode;
	bool nat_offload_en;
	int napt_add_mode;
	int (*set_natmib_en)(struct dpns_nat_priv *priv, struct dpns_natmib_info *info);
};

int set_natmib_en(struct dpns_nat_priv *priv, struct dpns_natmib_info *info);

struct acl_priv {
	struct dpns_common_priv *cpriv;
        struct list_head iacl_list;
        struct list_head eacl_list;
        DECLARE_BITMAP(iacl_bitmap, TCAM_LINE);
        DECLARE_BITMAP(eacl_bitmap, TCAM_LINE);
        u32 iacl_last_index;
        u32 eacl_last_index;
        u32 iv4_mode;
        u32 iv6_mode;
        u32 ev4_mode;
        u32 ev6_mode;
        u32 iacl_v4_line;
        u32 eacl_v4_line;
        u32 iacl_v6_line;
        u32 eacl_v6_line;
        u32 iacl_v4_cnt;
        u32 iacl_v6_cnt;
        u32 eacl_v4_cnt;
        u32 eacl_v6_cnt;
        u16 iacl_size;
        u16 eacl_size;
        u16 v4_w_addr;
        u16 v6_w_addr;
};

struct dpns_tmu_priv {
	struct dpns_common_priv	*cpriv;
	void __iomem            *iobase;
};

struct dpns_mcast_priv {
	struct dpns_common_priv	*cpriv;
	void __iomem            *iobase;
	struct workqueue_struct *ubus_wq;
};

typedef struct dpns_vlan_priv VLAN_t;
typedef struct dpns_router_priv ROUTER_t;
typedef struct dpns_common_priv COMMON_t;
typedef struct dpns_mac_priv MAC_t;
typedef struct dpns_tmu_priv TMU_t;
typedef struct dpns_mcast_priv MCAST_t;
typedef struct acl_priv ACL_t;

#define sf_readb(p, reg)	\
	readb((p)->iobase + (reg))

#define sf_readw(p, reg)	\
	readw((p)->iobase + (reg))

#define sf_writew(p, reg, val)	\
	writew((val), (p)->iobase + (reg))

#define sf_readl(p, reg)	\
	readl((p)->iobase + (reg))

#define sf_writel(p, reg, val)	\
	writel((val), (p)->iobase + (reg))

#define sf_readq(p, reg)	\
	readq((p)->iobase + (reg))

#define sf_writeq(p, reg, val)	\
	writeq((val), (p)->iobase + (reg))

#define sf_update(p, reg, mask, val)		\
	do {	\
		u32 tmp = readl((p)->iobase + (reg));		\
		tmp &= ~(mask);						\
		tmp |= (val);						\
		writel(tmp, (p)->iobase + (reg));			\
	}while(0)

static inline const char *get_name(const char *file_name)
{
	char *split;

	for (split = strstr(file_name, "/"); split; split = strstr(file_name, "/"))
		file_name = split + 1;

	return file_name;
}

extern unsigned char g_dbg_log[DPNS_MAX];

#define DPNS_DBG(module_name, dbg_level, args, ...)		\
	do {		\
		if (dbg_level < g_dbg_log[module_name]) 	\
			printk("["KBUILD_MODNAME ":"#dbg_level"] %s:%d "args, get_name(__FILE__), __LINE__, ##__VA_ARGS__);		\
	} while (0)

#define COMMON_DBG(dbg_level, args, ...)    DPNS_DBG(DPNS_COMMON, dbg_level, args, ##__VA_ARGS__)
#define GENL_DBG(dbg_level, args, ...)      DPNS_DBG(DPNS_GENL, dbg_level, args, ##__VA_ARGS__)
#define VLAN_DBG(dbg_level, args, ...)      DPNS_DBG(DPNS_VLAN, dbg_level, args, ##__VA_ARGS__)
#define L2_DBG(dbg_level, args, ...)        DPNS_DBG(DPNS_L2, dbg_level, args, ##__VA_ARGS__)
#define NAT_DBG(dbg_level, args, ...)       DPNS_DBG(DPNS_NAT, dbg_level, args, ##__VA_ARGS__)
#define L3_DBG(dbg_level, args, ...)        DPNS_DBG(DPNS_L3, dbg_level, args, ##__VA_ARGS__)
#define ACL_DBG(dbg_level, args, ...)       DPNS_DBG(DPNS_ACL, dbg_level, args, ##__VA_ARGS__)
#define TMU_DBG(dbg_level, args, ...)       DPNS_DBG(DPNS_TMU, dbg_level, args, ##__VA_ARGS__)
#define MCAST_DBG(dbg_level, args, ...)     DPNS_DBG(DPNS_MCAST, dbg_level, args, ##__VA_ARGS__)

extern void * dpns_kmalloc(size_t size, gfp_t flag, u8 module);
extern void dpns_kfree(const void *data, u8 module);

static inline void * dpns_kzalloc(size_t size, gfp_t flag, u8 module)
{
	return dpns_kmalloc(size, flag | __GFP_ZERO, module);
}

static inline void * dpns_vmalloc(unsigned long size, gfp_t gfp_mask, u8 module)
{
	return dpns_kmalloc(size, (gfp_mask | __GFP_COMP) & ~__GFP_HIGHMEM, module);
}

static inline void * dpns_vzalloc(unsigned long size, u8 module)
{
	return dpns_vmalloc(size, GFP_KERNEL | __GFP_HIGHMEM | __GFP_ZERO, module);
}

static inline void dpns_vfree(const void *addr, u8 module)
{
	dpns_kfree(addr, module);
}

#define common_kzalloc(size, flag)	dpns_kzalloc(size, flag, DPNS_COMMON_M)
#define common_kmalloc(size, flag)	dpns_kmalloc(size, flag, DPNS_COMMON_M)
#define common_kfree(data)			dpns_kfree(data, DPNS_COMMON_M)

#define vlan_kzalloc(size, flag)	dpns_kzalloc(size, flag, DPNS_VLAN_M)
#define vlan_kmalloc(size, flag)	dpns_kmalloc(size, flag, DPNS_VLAN_M)
#define vlan_kfree(data)			dpns_kfree(data, DPNS_VLAN_M)

#define l2_kzalloc(size, flag)		dpns_kzalloc(size, flag, DPNS_L2_M)
#define l2_kmalloc(size, flag)		dpns_kmalloc(size, flag, DPNS_L2_M)
#define l2_kfree(data)				dpns_kfree(data, DPNS_L2_M)

#define nat_kzalloc(size, flag)		dpns_kzalloc(size, flag, DPNS_NAT_M)
#define nat_kmalloc(size, flag)		dpns_kmalloc(size, flag, DPNS_NAT_M)
#define nat_kfree(data)				dpns_kfree(data, DPNS_NAT_M)

#define l3_kzalloc(size, flag)		dpns_kzalloc(size, flag, DPNS_L3_M)
#define l3_kmalloc(size, flag)		dpns_kmalloc(size, flag, DPNS_L3_M)
#define l3_kfree(data)				dpns_kfree(data, DPNS_L3_M)

#define mcast_kzalloc(size, flag)	dpns_kzalloc(size, flag, DPNS_MCAST_M)
#define mcast_kmalloc(size, flag)	dpns_kmalloc(size, flag, DPNS_MCAST_M)
#define mcast_kfree(data)			dpns_kfree(data, DPNS_MCAST_M)

#define acl_kzalloc(size, flag)		dpns_kzalloc(size, flag, DPNS_ACL_M)
#define acl_kmalloc(size, flag)		dpns_kmalloc(size, flag, DPNS_ACL_M)
#define acl_kfree(data)				dpns_kfree(data, DPNS_ACL_M)

#define tmu_kzalloc(size, flag)		dpns_kzalloc(size, flag, DPNS_TMU_M)
#define tmu_kfree(data)				dpns_kfree(data, DPNS_TMU_M)
#define tmu_kmalloc(size, flag)		dpns_kmalloc(size, flag, DPNS_TMU_M)
#define tmu_vzalloc(size)			dpns_vzalloc(size, DPNS_TMU_M)
#define tmu_vfree(addr)				dpns_vfree(addr, DPNS_TMU_M)


/* dev_fill_forward_path() takes a context since Linux 6.18 */
static inline int dpns_fill_forward_path(struct net_device *dev, const u8 *daddr,
					 struct net_device_path_stack *stack)
{
	struct net_device_path_ctx ctx = { .dev = dev };

	memcpy(ctx.daddr, daddr, sizeof(ctx.daddr));
	return dev_fill_forward_path(&ctx, stack);
}

#endif //_DPNS_COMMON_H_
