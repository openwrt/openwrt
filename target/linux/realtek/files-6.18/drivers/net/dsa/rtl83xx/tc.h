/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _RTL83XX_TC_H
#define _RTL83XX_TC_H

#include <linux/types.h>

struct dsa_switch;
struct flow_action_entry;
struct flow_cls_offload;
struct rtl838x_switch_priv;

int rtldsa_tc_init(struct rtl838x_switch_priv *priv);
void rtldsa_tc_cleanup(struct rtl838x_switch_priv *priv);

int rtldsa_cls_flower_add(struct dsa_switch *ds, int port,
			  struct flow_cls_offload *cls, bool ingress);
int rtldsa_cls_flower_del(struct dsa_switch *ds, int port,
			  struct flow_cls_offload *cls, bool ingress);
int rtldsa_cls_flower_stats(struct dsa_switch *ds, int port,
			    struct flow_cls_offload *cls, bool ingress);

int rtldsa_930x_port_rate_police_add(struct dsa_switch *ds, int port,
				     const struct flow_action_entry *act,
				     bool ingress);
int rtldsa_930x_port_rate_police_del(struct dsa_switch *ds, int port,
				     struct flow_cls_offload *cls,
				     bool ingress);
int rtldsa_931x_port_rate_police_add(struct dsa_switch *ds, int port,
				     const struct flow_action_entry *act,
				     bool ingress);
int rtldsa_931x_port_rate_police_del(struct dsa_switch *ds, int port,
				     struct flow_cls_offload *cls,
				     bool ingress);

#endif /* _RTL83XX_TC_H */
