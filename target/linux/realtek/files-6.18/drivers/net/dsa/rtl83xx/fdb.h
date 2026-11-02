/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_FDB_H
#define _OTTO_FDB_H

#include <net/dsa.h>

struct otto_dsa_priv;
struct rtldsa_l2_uc;
struct otto_l3_nexthop;

int rtldsa_port_fdb_add(struct dsa_switch *ds, int port,
			       const unsigned char *addr, u16 vid,
			       const struct dsa_db db);
int rtldsa_port_fdb_del(struct dsa_switch *ds, int port,
			       const unsigned char *addr, u16 vid,
			       const struct dsa_db db);
int rtldsa_port_fdb_dump(struct dsa_switch *ds, int port,
				dsa_fdb_dump_cb_t *cb, void *data);

struct rtldsa_l2_uc *otto_l2_uc_lookup(struct otto_dsa_priv *priv, int idx);

int otto_l2_nexthop_add(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh);
int otto_l2_nexthop_del(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh);
bool otto_l2_nexthop_current(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh);

void otto_l2_dbgfs_init(struct otto_dsa_priv *priv);

#endif /* _OTTO_FDB_H */
