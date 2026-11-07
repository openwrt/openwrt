/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_FLOW_H
#define _OTTO_FLOW_H

#include <linux/netdevice.h>

struct dsa_switch;
struct otto_dsa_priv;

int otto_flow_init(struct otto_dsa_priv *priv);
void otto_flow_cleanup(struct otto_dsa_priv *priv);
void otto_flow_flush_all(struct otto_dsa_priv *priv);
/* Runtime transitions hold RTNL, but no L3 resource or hardware locks. */
int otto_flow_prepare(struct otto_dsa_priv *priv);
int otto_flow_set_enabled(struct otto_dsa_priv *priv, bool enabled);
int otto_flow_setup_tc(struct dsa_switch *ds, int port,
		       enum tc_setup_type type, void *type_data);

#endif
