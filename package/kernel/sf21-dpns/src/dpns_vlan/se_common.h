#ifndef _SE_COMMON_H_
#define _SE_COMMON_H_

#include <linux/io.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include "init.h"

#define IVLAN_VID_MPP_CFG				0x8000
#define IVLAN_VID_MPP_CFG_PBV_EN			BIT(18)
#define IVLAN_LKP_MPP_CFG1				0xc004
#define IVLAN_LKP_CFG_OVID0_EN				BIT(11)
#define IVLAN_L2_MISS_FORWARD_EN			BIT(6)
#define IVLAN_LKP_CFG_DA_MISS_UP			BIT(5)
#define IVLAN_LKP_CFG_PFM_MODE				GENMASK(2, 1)
#define CONFIG0_RGT_ADDR				0x180008
#define CONFIG0_IPSPL_ZERO_LIMIT			BIT(19)
#define CONFIG0_IPSPL_CNT_MODE				GENMASK(18, 17) // 0:all bps, 1:all pps, 2/3:depends on iport entry
#define CONFIG0_IVLKP_CFG_DIS_TB			BIT(16)
#define CONFIG0_IVLKP_CFG_ENTR_VLD			GENMASK(15, 10)
#define CONFIG0_PBV_EN					BIT(9)
#define CONFIG0_IPORT_EN				BIT(8)
#define CONFIG0_IVXLT_CFG_DIS_TB			BIT(7)
#define CONFIG0_IVXLT_CFG_ENTR_VLD			GENMASK(6, 1)
#define CONFIG0_IPSPL_MODE				BIT(0) // 0:stric mode, 1:relaxed mode
#define L2_VID_ZERO_MODE_EN				BIT(27) //vlan 0 for any
#define EVLAN_ACT_CFG3					0X1c00c
#define L2_FWD_OVID_CFG					GENMASK(19,17)
#define L2_FWD_IVID_CFG					GENMASK(21,20)
#define L3_FWD_OVID0_DEL				BIT(25)
#define L3_FWD_IVID0_DEL				BIT(26)

enum iport_action {
	PACTION_DROP,
	PACTION_CPU,
	PACTION_FWD,
};

enum iport_cml {
	CML_LEARNING_RECV,      // enable learning and recv strange smac
	CML_NONE_LEARNING_RECV,
	CML_LEARNING_DROP,      // enable learning and send strange smac pkt to cpu
	CML_NONE_LEARNING_DROP, // enable learning and drop strange smac
};

enum vlan_action {
	ACTION_NONE,
	ACTION_REPLACE,
	ACTION_ADD,
	ACTION_DEL
};

enum vfp_search_mode {
	SEARCH_TUPLE,  // ivlan vfp table search by tuple, means use vfp table
	SEARCH_FIXED   /* ivlan vfp table search by fixed key:
			  {ivport,ovid,ivid}, means use xlt table */
};

enum vlan_l2_pfm {
	ALL_MCAST_FLOOD,
	UNKNOWN_MCAST_FLOOD, /* known mcast forward by dmac table, unknown mcast
				flood in vlan */
	UNKNOWN_MCAST_DROP,
};

enum sp_tree_action {
	SP_BLOCK,
	SP_LISTEN,
	SP_LEARN,
	SP_FORWARD
};

#define reg_read(p, reg)	\
	readl((p)->iobase + (reg))

#define reg_write(p, reg, val)	\
	writel((val), (p)->iobase + (reg))

#define reg_update(p, reg, mask, val)		\
	do {	\
		u32 tmp = readl((p)->iobase + (reg));		\
		tmp &= ~(mask);						\
		tmp |= (val);						\
		writel(tmp, (p)->iobase + (reg));			\
	}while(0)


int se_table_read(VLAN_t *priv,
	 u8 ram_id, u16 table_addr, u32* data, u32 size);
int se_table_write(VLAN_t *priv,
	 u8 ram_id, u16 table_addr, u32* data, u32 size);
#endif // _SE_COMMON_H_
