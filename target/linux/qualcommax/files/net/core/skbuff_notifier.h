/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SKBUFF_NOTIFIER_H
#define SKBUFF_NOTIFIER_H

#include <linux/notifier.h>
#include <linux/skbuff.h>

/* notifier events */
#define SKB_RECYCLER_NOTIFIER_SUMERR   0x0001
#define SKB_RECYCLER_NOTIFIER_DBLFREE  0x0002
#define SKB_RECYCLER_NOTIFIER_DBLALLOC 0x0004
#define SKB_RECYCLER_NOTIFIER_FSM      0x0008

#if defined(CONFIG_DEBUG_OBJECTS_SKBUFF)
int skb_recycler_notifier_register(struct notifier_block *nb);
int skb_recycler_notifier_unregister(struct notifier_block *nb);
int skb_recycler_notifier_send_event(unsigned long action,
				     struct sk_buff *skb);
#else
static inline int skb_recycler_notifier_register(struct notifier_block *nb)
{
	return 0;
}

static inline int skb_recycler_notifier_unregister(struct notifier_block *nb)
{
	return 0;
}

static inline int skb_recycler_notifier_send_event(unsigned long action,
						   struct sk_buff *skb)
{
	return 1;
}
#endif /* CONFIG_DEBUG_OBJECTS_SKBUFF */

#endif /* SKBUFF_NOTIFIER_H */
