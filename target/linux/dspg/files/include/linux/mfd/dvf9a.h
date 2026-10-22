/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#ifndef __LINUX_MFD_DVF9A_H
#define __LINUX_MFD_DVF9A_H

struct dvf9a;

typedef int (*dvf9a_acquire_t)(void *priv);

int dvf9a_acquire_hook_set(struct dvf9a *dvf9a, dvf9a_acquire_t acquire,
			   void *priv);
void dvf9a_owner_release(struct dvf9a *dvf9a);
int dvf9a_irq_get(struct dvf9a *dvf9a, unsigned int hwirq);

#endif
