/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_TC_H
#define _OTTO_TC_H

#include <linux/types.h>
#include <net/pkt_cls.h>

struct dsa_switch;
struct flow_action_entry;
struct flow_cls_offload;
struct otto_dsa_priv;

int otto_tc_init(struct otto_dsa_priv *priv);
void otto_tc_cleanup(struct otto_dsa_priv *priv);

int otto_tc_cls_flower_add(struct dsa_switch *ds, int port,
			   struct flow_cls_offload *cls, bool ingress);
int otto_tc_cls_flower_del(struct dsa_switch *ds, int port,
			   struct flow_cls_offload *cls, bool ingress);
int otto_tc_cls_flower_stats(struct dsa_switch *ds, int port,
			     struct flow_cls_offload *cls, bool ingress);

int otto_tc_930x_port_rate_police_add(struct dsa_switch *ds, int port,
				      const struct flow_action_entry *act,
				      bool ingress);
int otto_tc_930x_port_rate_police_del(struct dsa_switch *ds, int port,
				      struct flow_cls_offload *cls,
				      bool ingress);
int otto_tc_931x_port_rate_police_add(struct dsa_switch *ds, int port,
				      const struct flow_action_entry *act,
				      bool ingress);
int otto_tc_931x_port_rate_police_del(struct dsa_switch *ds, int port,
				      struct flow_cls_offload *cls,
				      bool ingress);

int otto_tc_port_setup_tc(struct dsa_switch *ds, int port, enum tc_setup_type type,
			  void *type_data);

#endif /* _OTTO_TC_H */
