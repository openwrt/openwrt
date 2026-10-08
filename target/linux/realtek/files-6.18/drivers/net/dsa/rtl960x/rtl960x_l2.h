/* SPDX-License-Identifier: GPL-2.0-only */
/* Forwarding database interface for the RTL960x switch core */

#ifndef __RTL960X_L2_H
#define __RTL960X_L2_H

#include <linux/if_ether.h>
#include <linux/types.h>
#include <net/dsa.h>

#include "rtl960x_dsa.h"

int rtl960x_l2_flush(struct rtl960x_dsa *priv, int port);

int rtl960x_port_fdb_add(struct dsa_switch *ds, int port,
			 const unsigned char *addr, u16 vid,
			 const struct dsa_db db);
int rtl960x_port_fdb_del(struct dsa_switch *ds, int port,
			 const unsigned char *addr, u16 vid,
			 const struct dsa_db db);
int rtl960x_port_fdb_dump(struct dsa_switch *ds, int port,
			  dsa_fdb_dump_cb_t *cb, void *data);

#endif /* __RTL960X_L2_H */
