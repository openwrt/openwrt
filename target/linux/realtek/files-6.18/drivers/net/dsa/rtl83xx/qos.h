/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_QOS_H
#define _OTTO_QOS_H

#include <linux/types.h>

struct dsa_switch;
struct otto_dsa_priv;

#define RTL930X_PORT_TBL_IDX_CTRL(port)		(0x9B20 + (((port) / 16) * 4))
#define RTL931X_PORT_TBL_IDX_CTRL(port)		(0x9064 + (((port) / 16) * 4))
#define RTL93XX_PORT_TBL_IDX_CTRL_IDX_OFFSET(port) \
						(((port) & 0xF) << 1)
#define RTL93XX_PORT_TBL_IDX_CTRL_IDX_MASK(port) \
						(0x3 << RTL93XX_PORT_TBL_IDX_CTRL_IDX_OFFSET(port))

#define RTL930X_REMAP_DSCP(p)			(0x9B04 + (((p) / 10) * 4))
#define RTL931X_REMAP_DSCP(p)			(0x9034 + (((p) / 10) * 4))
#define RTL93XX_REMAP_DSCP_INTPRI_DSCP_OFFSET(p) \
						(((p) % 10) * 3)
#define RTL93XX_REMAP_DSCP_INTPRI_DSCP_MASK(index) \
						(0x7 << RTL93XX_REMAP_DSCP_INTPRI_DSCP_OFFSET(index))

u32 otto_qos_838x_get_egress_rate(struct otto_dsa_priv *priv, int port);
int otto_qos_838x_set_egress_rate(struct otto_dsa_priv *priv, int port, u32 rate);
u32 otto_qos_839x_get_egress_rate(struct otto_dsa_priv *priv, int port);
int otto_qos_839x_set_egress_rate(struct otto_dsa_priv *priv, int port, u32 rate);

void otto_qos_838x_init(struct otto_dsa_priv *priv);
void otto_qos_839x_init(struct otto_dsa_priv *priv);
void otto_qos_930x_init(struct otto_dsa_priv *priv);
void otto_qos_931x_init(struct otto_dsa_priv *priv);
void otto_qos_930x_queue_sched_set(struct otto_dsa_priv *priv, int port, int queue, u32 weight,
				   bool strict);
void otto_qos_queue_sched_reset(struct otto_dsa_priv *priv, int port);

void otto_qos_setup(struct dsa_switch *ds);
int otto_qos_port_get_default_prio(struct dsa_switch *ds, int port);
int otto_qos_port_set_default_prio(struct dsa_switch *ds, int port, u8 prio);
int otto_qos_port_get_dscp_prio(struct dsa_switch *ds, int port, u8 dscp);
int otto_qos_port_add_dscp_prio(struct dsa_switch *ds, int port, u8 dscp, u8 prio);
int otto_qos_port_del_dscp_prio(struct dsa_switch *ds, int port, u8 dscp, u8 prio);
int otto_qos_port_get_apptrust(struct dsa_switch *ds, int port, u8 *sel, int *nsel);
int otto_qos_port_set_apptrust(struct dsa_switch *ds, int port, const u8 *sel, int nsel);

#endif /* _OTTO_QOS_H */
