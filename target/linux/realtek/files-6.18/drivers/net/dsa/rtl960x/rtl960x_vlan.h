/* SPDX-License-Identifier: GPL-2.0-only */
/* VLAN table and port VLAN configuration of the RTL960x switch core */

#ifndef __RTL960X_VLAN_H
#define __RTL960X_VLAN_H

#include <linux/types.h>
#include <net/dsa.h>

#include "rtl960x_dsa.h"

int rtl960x_vlan_setup(struct dsa_switch *ds);

#endif /* __RTL960X_VLAN_H */
