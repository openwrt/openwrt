/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_LAG_H
#define _OTTO_LAG_H

#include "rtl-otto.h"

int rtldsa_lag_add(struct dsa_switch *ds, int group, int port, struct netdev_lag_upper_info *info);
int rtldsa_lag_del(struct dsa_switch *ds, int group, int port);
int rtldsa_93xx_lag_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
					       int group, int algoidx, u32 algomsk);
int rtldsa_83xx_lag_setup_algomask(struct rtl838x_switch_priv *priv, int group,
				   struct netdev_lag_upper_info *info);
int rtldsa_93xx_lag_set_port_members(struct rtl838x_switch_priv *priv, int group,
				     u64 members, struct netdev_lag_upper_info *info);
void rtldsa_93xx_lag_switch_init(struct rtl838x_switch_priv *priv);
int rtl838x_trk_mbr_ctr(int group);
int rtldsa_838x_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
					   int group, int algoidx, u32 algomsk);
int rtldsa_838x_lag_set_port_members(struct rtl838x_switch_priv *priv, int group,
				     u64 members, struct netdev_lag_upper_info *info);
int rtl839x_trk_mbr_ctr(int group);
int rtldsa_839x_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
					   int group, int algoidx, u32 algomsk);
int rtldsa_839x_lag_set_port_members(struct rtl838x_switch_priv *priv, int group,
				     u64 members, struct netdev_lag_upper_info *info);
int rtl930x_trk_mbr_ctr(int group);
void rtldsa_930x_lag_set_port2group(int group, int port, bool valid);
void rtldsa_930x_lag_fill_data(u32 data[], struct rtldsa_93xx_lag_entry *e);
void rtldsa_930x_lag_write_data(u32 data[], struct rtldsa_93xx_lag_entry *e);
void rtldsa_930x_lag_set_local_group_id(int local_group, int global_group, bool valid);
void rtldsa_930x_lag_set_local_port2group(int group, int port, bool valid);
void rtldsa_930x_lag_sync_tables(void);
int rtldsa_930x_lag_table(void);
int rtldsa_931x_trk_mbr_ctr(int group);
void rtldsa_931x_lag_set_port2group(int group, int port, bool valid);
void rtldsa_931x_lag_fill_data(u32 data[], struct rtldsa_93xx_lag_entry *e);
void rtldsa_931x_lag_write_data(u32 data[], struct rtldsa_93xx_lag_entry *e);
void rtldsa_931x_lag_set_local_group_id(int local_group, int global_group, bool valid);
void rtldsa_931x_lag_set_local_port2group(int group, int port, bool valid);
void rtldsa_931x_lag_sync_tables(void);
int rtldsa_931x_lag_table(void);

#endif /* _OTTO_LAG_H */
