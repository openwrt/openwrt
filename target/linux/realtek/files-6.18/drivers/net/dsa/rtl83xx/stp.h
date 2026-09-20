/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_STP_H
#define _OTTO_STP_H

#include <linux/kref.h>
#include <linux/types.h>

struct dsa_switch;
struct rtl838x_switch_priv;
struct switchdev_mst_state;

struct rtldsa_mst {
	/** @msti: MSTI mapped to this slot. 0 == unused */
	u16 msti;

	/** @refcount: number of vlans currently using this msti, undefined when unused */
	struct kref refcount;
};

bool rtldsa_mst_put_slot(struct rtl838x_switch_priv *priv, u16 mst_slot);
int rtldsa_mst_replace(struct rtl838x_switch_priv *priv, u16 msti, u16 old_mst_slot);
int rtldsa_port_get_stp_state(struct rtl838x_switch_priv *priv, int port);
void rtldsa_port_stp_state_set(struct dsa_switch *ds, int port, u8 state);
int rtldsa_port_mst_state_set(struct dsa_switch *ds, int port,
			      const struct switchdev_mst_state *st);

/* Caller must hold priv->reg_mutex. */
void rtldsa_port_non_cist_states_set(struct rtl838x_switch_priv *priv, int port, u8 state);

int rtldsa_838x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port);
void rtl838x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state);
void rtldsa_838x_stp_init(void);
int rtldsa_839x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port);
void rtl839x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state);
void rtldsa_839x_stp_init(void);
int rtldsa_930x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port);
void rtl930x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state);
void rtldsa_930x_stp_init(void);
int rtldsa_931x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port);
void rtl931x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state);
void rtldsa_931x_stp_init(void);

#endif /* _OTTO_STP_H */
