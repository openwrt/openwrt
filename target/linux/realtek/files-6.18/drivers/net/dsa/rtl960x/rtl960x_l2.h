/* SPDX-License-Identifier: GPL-2.0-only */
/* L2 table maintenance for the RTL960x switch core */

#ifndef __RTL960X_L2_H
#define __RTL960X_L2_H

#include "rtl960x_dsa.h"

int rtl960x_l2_flush(struct rtl960x_dsa *priv, int port);

#endif /* __RTL960X_L2_H */
