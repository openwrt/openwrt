/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_LAG_H
#define _OTTO_LAG_H

#include <linux/types.h>

struct dsa_switch;
struct netdev_lag_upper_info;
struct rtl838x_switch_priv;
struct rtldsa_93xx_lag_entry;

#define MAX_LAGS 16

int otto_lag_add(struct dsa_switch *ds, int group, int port, struct netdev_lag_upper_info *info);
int otto_lag_del(struct dsa_switch *ds, int group, int port);
int otto_lag_93xx_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
					     int group, int algoidx, u32 algomsk);
int otto_lag_83xx_setup_algomask(struct rtl838x_switch_priv *priv, int group,
				 struct netdev_lag_upper_info *info);
int otto_lag_93xx_set_port_members(struct rtl838x_switch_priv *priv, int group,
				   u64 members, struct netdev_lag_upper_info *info);
void otto_lag_93xx_switch_init(struct rtl838x_switch_priv *priv);
int otto_lag_838x_trk_mbr_ctr(int group);
int otto_lag_838x_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
					     int group, int algoidx, u32 algomsk);
int otto_lag_838x_set_port_members(struct rtl838x_switch_priv *priv, int group,
				   u64 members, struct netdev_lag_upper_info *info);
int otto_lag_839x_trk_mbr_ctr(int group);
int otto_lag_839x_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
					     int group, int algoidx, u32 algomsk);
int otto_lag_839x_set_port_members(struct rtl838x_switch_priv *priv, int group,
				   u64 members, struct netdev_lag_upper_info *info);
int otto_lag_930x_trk_mbr_ctr(int group);
void otto_lag_930x_set_port2group(int group, int port, bool valid);
void otto_lag_930x_fill_data(u32 data[], struct rtldsa_93xx_lag_entry *e);
void otto_lag_930x_write_data(u32 data[], struct rtldsa_93xx_lag_entry *e);
void otto_lag_930x_set_local_group_id(int local_group, int global_group, bool valid);
void otto_lag_930x_set_local_port2group(int group, int port, bool valid);
void otto_lag_930x_sync_tables(void);
int otto_lag_930x_table(void);
int otto_lag_931x_trk_mbr_ctr(int group);
void otto_lag_931x_set_port2group(int group, int port, bool valid);
void otto_lag_931x_fill_data(u32 data[], struct rtldsa_93xx_lag_entry *e);
void otto_lag_931x_write_data(u32 data[], struct rtldsa_93xx_lag_entry *e);
void otto_lag_931x_set_local_group_id(int local_group, int global_group, bool valid);
void otto_lag_931x_set_local_port2group(int group, int port, bool valid);
void otto_lag_931x_sync_tables(void);
int otto_lag_931x_table(void);

#endif /* _OTTO_LAG_H */
