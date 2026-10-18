/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_QOS_H
#define _OTTO_QOS_H

#include <linux/types.h>

struct otto_dsa_priv;

u32 otto_qos_838x_get_egress_rate(struct otto_dsa_priv *priv, int port);
int otto_qos_838x_set_egress_rate(struct otto_dsa_priv *priv, int port, u32 rate);
u32 otto_qos_839x_get_egress_rate(struct otto_dsa_priv *priv, int port);
int otto_qos_839x_set_egress_rate(struct otto_dsa_priv *priv, int port, u32 rate);

void otto_qos_838x_init(struct otto_dsa_priv *priv);
void otto_qos_839x_init(struct otto_dsa_priv *priv);
void otto_qos_930x_init(struct otto_dsa_priv *priv);
void otto_qos_931x_init(struct otto_dsa_priv *priv);

#endif /* _OTTO_QOS_H */
