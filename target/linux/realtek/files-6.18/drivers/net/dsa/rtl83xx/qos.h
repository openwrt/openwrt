/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_QOS_H
#define _OTTO_QOS_H

#include <linux/types.h>

struct otto_dsa_priv;

u32 rtldsa_838x_get_egress_rate(struct otto_dsa_priv *priv, int port);
int rtldsa_838x_set_egress_rate(struct otto_dsa_priv *priv, int port, u32 rate);
u32 rtldsa_839x_get_egress_rate(struct otto_dsa_priv *priv, int port);
int rtldsa_839x_set_egress_rate(struct otto_dsa_priv *priv, int port, u32 rate);

void rtldsa_838x_qos_init(struct otto_dsa_priv *priv);
void rtldsa_839x_qos_init(struct otto_dsa_priv *priv);
void rtldsa_930x_qos_init(struct otto_dsa_priv *priv);
void rtldsa_931x_qos_init(struct otto_dsa_priv *priv);

#endif /* _OTTO_QOS_H */
