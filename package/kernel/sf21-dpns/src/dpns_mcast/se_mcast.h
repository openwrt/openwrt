#ifndef __SE_MCAST_PRIV_H__
#define __SE_MCAST_PRIV_H__

#include "dpns_common.h"
#include "se_mcast_cfg.h"

#define NPU_NAT_PKT_TYPE_IGNORE         0x00004
#define NAT_IGNORE_FRAG_PKT             BIT(0)
#define NAT_IGNORE_MCAST_SNAT           BIT(1)
#define NAT_IGNORE_MCAST_DNAT           BIT(2)
#define NAT_IGNORE_UCAST                BIT(3)
#define NAT_IGNORE_IPV6                 BIT(4)
#define NAT_IGNORE_IPV4                 BIT(5)
#define NAT_IGNORE_UDP                  BIT(6)
#define NAT_IGNORE_TCP                  BIT(7)

#define NPU_ARP_MPP_CFG                 0x10000
#define MC_DMAC_REPLACE_EN              BIT(17)

#define NPU_L3_MPP_CFG                  0x20000
#define L3_MPP_MC_USE_L2_EN             BIT(26)
#define L3_MPP_ASM_EN                   BIT(18)
#define L3_MPP_SSM_EN                   BIT(17)

#define SE_TCAM_TB_WRDATA_LO            0x190040 /* lower 64 bits */
#define SE_TCAM_TB_WRDATA_HI            0x190048 /* upper 8 bits */

#define SE_TCAM_TB_RDDATA_LO            0x190080 /* lower 64 bits */
#define SE_TCAM_TB_RDDATA_HI            0x190088 /* upper 8 bits */

#define SE_TB_WRDATA0		        0x180040
#define SE_TB_RDDATA0		        0x180080
#define SE_TB_DATA_SIZE		        0x40

#define SE_TCAM_STATUS                  0x190000
#define TCAM_STATUS_P4_BP_OVF           BIT(4)
#define TCAM_STATUS_P3_BP_OVF           BIT(3)
#define TCAM_STATUS_P2_BP_OVF           BIT(2)
#define TCAM_STATUS_P1_BP_OVF           BIT(1)
#define TCAM_STATUS_P0_BP_OVF           BIT(0)

#define SE_TCAM_CLR                     0x190004
#define TCAM_CLR_BLK4_TB                BIT(4)
#define TCAM_CLR_BLK3_TB                BIT(3)
#define TCAM_CLR_BLK2_TB                BIT(2)
#define TCAM_CLR_BLK1_TB                BIT(1)
#define TCAM_CLR_BLK0_TB                BIT(0)

#define SE_TCAM_BLK_CONFIG0             0x190008
#define TCAM_BLK_CONFIG0_BLK4_CFG       GENMASK(14, 12)
#define TCAM_BLK_CONFIG0_BLK3_CFG       GENMASK(11, 9)
#define TCAM_BLK_CONFIG0_BLK2_CFG       GENMASK(8, 6)
#define TCAM_BLK_CONFIG0_BLK1_CFG       GENMASK(5, 3)
#define TCAM_BLK_CONFIG0_BLK0_CFG       GENMASK(2, 0)

#define TCAM_BLK_CFG0_BLK_SEL_INVALID   GENMASK(2, 0)
#define TCAM_BLK_CFG0_BLK_SEL_CNT       (5)
#define TCAM_BLK_CFG0_BLK_SEL_WIDTH     (3)

#define TCAM_BLK_REQ_ID(blk_id, slice)  ((blk_id) * 9 + (slice))
#define TCAM_BLK_MODE_ID(blk_id)        ((blk_id) * 9 + 8)

#define L3_MCSG_KEY_MODE                1       // represents key size
#define L3_MCSG_TBL_ID                  2       // indicate tcam item type
#define L3_MCSG_SLICE_CNT               BIT(L3_MCSG_KEY_MODE)

#define L3_MCAG_KEY_MODE                1
#define L3_MCAG_TBL_ID                  2
#define L3_MCAG_SLICE_CNT               BIT(L3_MCAG_KEY_MODE)

#define TCAM_ITEM_CNT                   (32)
#define TCAM_ITEM_BITS                  (576)
#define TCAM_SLICE_BITS                 (72)
#define TCAM_SLICES_PER_ITEM            (TCAM_ITEM_BITS / TCAM_SLICE_BITS)
#define TCAM_MCSG_SLICES                (TCAM_SLICES_PER_ITEM / L3_MCSG_SLICE_CNT)


#define for_each_blk_item(i) for ((i) = 0; (i) < ARRAY_SIZE(((se_mcsg_blk_t *)0)->items); (i)++)

typedef struct se_mscg_tbl_item {
        DECLARE_BITMAP(slice_map, TCAM_MCSG_SLICES);
} se_mcsg_tbl_item_t;

typedef struct se_mcsg_blk {
        // actually 64, even item stores key, odd stores mask
        // one item holds 576 bits and contains 8 slices, 72 bits per slice
        se_mcsg_tbl_item_t items[TCAM_ITEM_CNT];
} se_mcsg_blk_t;

// (S, G), 144 bits
typedef struct se_l3_mcsg_rule {
        u64 intf_idx     : 6;   // smac to replace with specified interface
        u64 oport_bitmap : 27;
        u64 iport_id     : 5;
        u64 rsv0         : 30;
        u64 dip          : 32;
        u64 sip          : 32;
        u64 ovid         : 12;
} __packed se_l3_mcsg_rule_t;

// (*, G), 144 bits
typedef struct se_l3_mcag_rule {
        u64 intf_idx     : 6;
        u64 oport_bitmap : 36;
        u64 rsv0         : 58;
        u64 dip          : 32;
        u64 ovid         : 12;
} __packed se_l3_mcag_rule_t;

typedef struct l3_mcast_entry_key {
        u32 sip;
        u32 dip;
        u32 iif_idx;
} l3_mcast_entry_key_t;

typedef struct tcam_blk_idx {
        int item;  // index of tcam block item
        int slice; // index of item slice
} tcam_blk_idx_t;

typedef struct l3_mcast_entry {
        struct rhash_head node;

        l3_mcast_entry_key_t key;

        union {
                se_l3_mcag_rule_t mcag;
                se_l3_mcsg_rule_t mcsg;
        } rule;

        // rule location in tcam block
        tcam_blk_idx_t tcam_idx;

        s32 l2_idx;

        // for userspace query
        se_l3_mcast_cfg_t cfg;
} l3_mcast_entry_t;

int se_l3_mcast_add(se_l3_mcast_cfg_t *cfg);
int se_l3_mcast_del(se_l3_mcast_cfg_t *cfg);
int se_l3_mcast_list(se_l3_mcast_cfg_t **list, size_t *sz);
int se_l3_mcast_del_marked(char *mark);
int se_l3_mcsg_write(se_l3_mcsg_rule_t *rule, tcam_blk_idx_t *idx);
int se_l3_mcsg_clear(tcam_blk_idx_t *idx);
int is_valid_tcam_idx(tcam_blk_idx_t *idx);
#ifdef CONFIG_SIFLOWER_DPNS_MCAST_GENL
int se_mcast_genl_init(void);
int se_mcast_genl_exit(void);
#endif

struct mcast_ubus_work {
	struct work_struct work;
	struct mcast_genl_msg *msg;
        se_l3_mcast_cfg_t cfg;
	MCAST_t *priv;
};

#endif // __SE_MCAST_PRIV_H__
