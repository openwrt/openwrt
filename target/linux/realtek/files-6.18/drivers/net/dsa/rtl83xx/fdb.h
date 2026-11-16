/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_FDB_H
#define _OTTO_FDB_H

#include <net/dsa.h>

struct otto_dsa_priv;
struct rtldsa_l2_uc;
struct otto_l3_nexthop;

int otto_fdb_port_add(struct dsa_switch *ds, int port,
		      const unsigned char *addr, u16 vid,
		      const struct dsa_db db);
int otto_fdb_port_del(struct dsa_switch *ds, int port,
		      const unsigned char *addr, u16 vid,
		      const struct dsa_db db);
int otto_fdb_port_dump(struct dsa_switch *ds, int port,
		       dsa_fdb_dump_cb_t *cb, void *data);

struct rtldsa_l2_uc *otto_fdb_uc_lookup(struct otto_dsa_priv *priv, int idx);

int otto_fdb_nexthop_add(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh);
int otto_fdb_nexthop_del(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh);
bool otto_fdb_nexthop_current(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh);

void otto_fdb_dbgfs_init(struct otto_dsa_priv *priv);

#endif /* _OTTO_FDB_H */
