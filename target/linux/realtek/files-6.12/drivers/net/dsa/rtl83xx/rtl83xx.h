/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _NET_DSA_RTL83XX_H
#define _NET_DSA_RTL83XX_H

#include <net/dsa.h>
#include "rtl838x.h"

struct fdb_update_work {
	struct work_struct work;
	struct net_device *ndev;
	u64 macs[];
};

enum mib_reg {
	MIB_REG_INVALID = 0,
	MIB_REG_STD,
	MIB_REG_PRV,
	MIB_TBL_STD,
	MIB_TBL_PRV,
};

#define MIB_ITEM(_reg, _offset, _size) \
		{.reg = _reg, .offset = _offset, .size = _size}

#define MIB_LIST_ITEM(_name, _item) \
		{.name = _name, .item = _item}

struct rtldsa_mib_item {
	enum mib_reg reg;
	unsigned int offset;
	unsigned int size;
};

struct rtldsa_mib_list_item {
	const char *name;
	struct rtldsa_mib_item item;
};

struct rtldsa_mib_desc {
	struct rtldsa_mib_item symbol_errors;

	struct rtldsa_mib_item if_in_octets;
	struct rtldsa_mib_item if_out_octets;
	struct rtldsa_mib_item if_in_ucast_pkts;
	struct rtldsa_mib_item if_in_mcast_pkts;
	struct rtldsa_mib_item if_in_bcast_pkts;
	struct rtldsa_mib_item if_out_ucast_pkts;
	struct rtldsa_mib_item if_out_mcast_pkts;
	struct rtldsa_mib_item if_out_bcast_pkts;
	struct rtldsa_mib_item if_out_discards;
	struct rtldsa_mib_item single_collisions;
	struct rtldsa_mib_item multiple_collisions;
	struct rtldsa_mib_item deferred_transmissions;
	struct rtldsa_mib_item late_collisions;
	struct rtldsa_mib_item excessive_collisions;
	struct rtldsa_mib_item crc_align_errors;
	struct rtldsa_mib_item rx_pkts_over_max_octets;

	struct rtldsa_mib_item unsupported_opcodes;

	struct rtldsa_mib_item rx_undersize_pkts;
	struct rtldsa_mib_item rx_oversize_pkts;
	struct rtldsa_mib_item rx_fragments;
	struct rtldsa_mib_item rx_jabbers;

	struct rtldsa_mib_item tx_pkts[ETHTOOL_RMON_HIST_MAX];
	struct rtldsa_mib_item rx_pkts[ETHTOOL_RMON_HIST_MAX];
	struct ethtool_rmon_hist_range rmon_ranges[ETHTOOL_RMON_HIST_MAX];

	struct rtldsa_mib_item drop_events;
	struct rtldsa_mib_item collisions;

	struct rtldsa_mib_item rx_pause_frames;
	struct rtldsa_mib_item tx_pause_frames;

	size_t list_count;
	const struct rtldsa_mib_list_item *list;
};

/* API for switch table access */
struct table_reg {
	u16 addr;
	u16 data;
	u8  max_data;
	u8 c_bit;
	u8 t_bit;
	u8 rmode;
	u8 tbl;
	struct mutex lock;
};

#define TBL_DESC(_addr, _data, _max_data, _c_bit, _t_bit, _rmode) \
		{  .addr = _addr, .data = _data, .max_data = _max_data, .c_bit = _c_bit, \
		    .t_bit = _t_bit, .rmode = _rmode \
		}

typedef enum {
	RTL8380_TBL_L2 = 0,
	RTL8380_TBL_0,
	RTL8380_TBL_1,
	RTL8390_TBL_L2,
	RTL8390_TBL_0,
	RTL8390_TBL_1,
	RTL8390_TBL_2,
	RTL9300_TBL_L2,
	RTL9300_TBL_0,
	RTL9300_TBL_1,
	RTL9300_TBL_2,
	RTL9300_TBL_HSB,
	RTL9300_TBL_HSA,
	RTL9310_TBL_0,
	RTL9310_TBL_1,
	RTL9310_TBL_2,
	RTL9310_TBL_3,
	RTL9310_TBL_4,
	RTL9310_TBL_5,
	RTL_TBL_END
} rtl838x_tbl_reg_t;

void rtl_table_init(void);
struct table_reg *rtl_table_get(rtl838x_tbl_reg_t r, int t);
void rtl_table_release(struct table_reg *r);
int rtl_table_read(struct table_reg *r, int idx);
int rtl_table_write(struct table_reg *r, int idx);
inline u16 rtl_table_data(struct table_reg *r, int i);
inline u32 rtl_table_data_r(struct table_reg *r, int i);
inline void rtl_table_data_w(struct table_reg *r, u32 v, int i);

void rtldsa_838x_qos_init(struct rtl838x_switch_priv *priv);
void rtldsa_839x_qos_init(struct rtl838x_switch_priv *priv);

void rtl83xx_fast_age(struct dsa_switch *ds, int port);
int rtl83xx_packet_cntr_alloc(struct rtl838x_switch_priv *priv);
int rtldsa_port_get_stp_state(struct rtl838x_switch_priv *priv, int port);
int rtl83xx_port_is_under(const struct net_device *dev, struct rtl838x_switch_priv *priv);
void rtl83xx_port_stp_state_set(struct dsa_switch *ds, int port, u8 state);
int rtl83xx_setup_tc(struct net_device *dev, enum tc_setup_type type, void *type_data);

/* Port register accessor functions for the RTL839x and RTL931X SoCs */
void rtl839x_mask_port_reg_be(u64 clear, u64 set, int reg);
u32 rtl839x_get_egress_rate(struct rtl838x_switch_priv *priv, int port);
u64 rtl839x_get_port_reg_be(int reg);
void rtl839x_set_port_reg_be(u64 set, int reg);
void rtl839x_mask_port_reg_le(u64 clear, u64 set, int reg);
int rtl839x_set_egress_rate(struct rtl838x_switch_priv *priv, int port, u32 rate);
void rtl839x_set_port_reg_le(u64 set, int reg);
u64 rtl839x_get_port_reg_le(int reg);

/* Port register accessor functions for the RTL838x and RTL930X SoCs */
void rtl838x_mask_port_reg(u64 clear, u64 set, int reg);
void rtl838x_set_port_reg(u64 set, int reg);
u32 rtl838x_get_egress_rate(struct rtl838x_switch_priv *priv, int port);
u64 rtl838x_get_port_reg(int reg);
int rtl838x_set_egress_rate(struct rtl838x_switch_priv *priv, int port, u32 rate);

/* RTL838x-specific */
u32 rtl838x_hash(struct rtl838x_switch_priv *priv, u64 seed);
irqreturn_t rtl838x_switch_irq(int irq, void *dev_id);
void rtl838x_vlan_profile_dump(int index);
void rtl838x_print_matrix(void);

/* RTL839x-specific */
u32 rtl839x_hash(struct rtl838x_switch_priv *priv, u64 seed);
irqreturn_t rtl839x_switch_irq(int irq, void *dev_id);
void rtl839x_vlan_profile_dump(int index);
void rtl839x_exec_tbl2_cmd(u32 cmd);
void rtl839x_print_matrix(void);

/* RTL930x-specific */
u32 rtl930x_hash(struct rtl838x_switch_priv *priv, u64 seed);
irqreturn_t rtldsa_930x_switch_irq(int irq, void *dev_id);
irqreturn_t rtl839x_switch_irq(int irq, void *dev_id);
void rtl930x_vlan_profile_dump(int index);
void rtl930x_print_matrix(void);

/* RTL931x-specific */
irqreturn_t rtl931x_switch_irq(int irq, void *dev_id);
void rtl931x_print_matrix(void);
void rtldsa_931x_config_phy_ability_source(struct rtl838x_switch_priv *priv);

int rtl83xx_lag_add(struct dsa_switch *ds, int group, int port, struct netdev_lag_upper_info *info);
int rtl83xx_lag_del(struct dsa_switch *ds, int group, int port);

/*
 * TODO: The following functions are currently not in use. So compiler will complain if
 * they are static and not made available externally. To preserve them for future use
 * collect them in this section.
 */

void rtl838x_egress_rate_queue_limit(struct rtl838x_switch_priv *priv, int port,
				     int queue, u32 rate);

void rtl839x_pie_rule_dump(struct  pie_rule *pr);
void rtl839x_set_egress_queue(int port, int queue);

void rtl9300_dump_debug(void);
void rtl930x_pie_rule_dump_raw(u32 r[]);

void rtl931x_print_matrix(void);

extern const struct dsa_switch_ops rtl83xx_switch_ops;
extern const struct dsa_switch_ops rtl93xx_switch_ops;

extern const struct rtl838x_reg rtl838x_reg;
extern const struct rtl838x_reg rtl839x_reg;
extern const struct rtl838x_reg rtl930x_reg;
extern const struct rtl838x_reg rtl931x_reg;

/* TODO actually from arch/mips/rtl838x/prom.c */
extern struct rtl83xx_soc_info soc_info;

#endif /* _NET_DSA_RTL83XX_H */
