/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __LINUX_PCS_RTL_OTTO_H
#define __LINUX_PCS_RTL_OTTO_H

#include <linux/types.h>

struct phylink_pcs;

int rtl931x_pcs_validate_stack(struct phylink_pcs *pcs, int port);
int rtl931x_pcs_stack_rx_disable(struct phylink_pcs *pcs, int port,
				 bool disable);

#endif /* __LINUX_PCS_RTL_OTTO_H */
