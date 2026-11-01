/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_STP_H
#define _OTTO_STP_H

#include <linux/kref.h>
#include <linux/types.h>

struct dsa_switch;
struct otto_dsa_priv;
struct switchdev_mst_state;

struct rtldsa_mst {
	/** @msti: MSTI mapped to this slot. 0 == unused */
	u16 msti;

	/** @refcount: number of vlans currently using this msti, undefined when unused */
	struct kref refcount;
};

bool otto_stp_slot_put(struct otto_dsa_priv *priv, u16 mst_slot);
int otto_stp_slot_replace(struct otto_dsa_priv *priv, u16 msti, u16 old_mst_slot);
int otto_stp_port_state_get(struct otto_dsa_priv *priv, int port);
void otto_stp_port_state_set(struct dsa_switch *ds, int port, u8 state);
int otto_stp_port_mst_state_set(struct dsa_switch *ds, int port,
				const struct switchdev_mst_state *st);

/* Caller must hold priv->reg_mutex. */
void otto_stp_port_non_cist_states_set(struct otto_dsa_priv *priv, int port, u8 state);

int otto_stp_838x_get(struct otto_dsa_priv *priv, u16 msti, int port);
void otto_stp_838x_set(struct otto_dsa_priv *priv, u16 msti, int port, int state);
void otto_stp_838x_init(void);
int otto_stp_839x_get(struct otto_dsa_priv *priv, u16 msti, int port);
void otto_stp_839x_set(struct otto_dsa_priv *priv, u16 msti, int port, int state);
void otto_stp_839x_init(void);
int otto_stp_930x_get(struct otto_dsa_priv *priv, u16 msti, int port);
void otto_stp_930x_set(struct otto_dsa_priv *priv, u16 msti, int port, int state);
void otto_stp_930x_init(void);
int otto_stp_931x_get(struct otto_dsa_priv *priv, u16 msti, int port);
void otto_stp_931x_set(struct otto_dsa_priv *priv, u16 msti, int port, int state);
void otto_stp_931x_init(void);

#endif /* _OTTO_STP_H */
