/*
 * Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#include <asm/stacktrace.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/smp.h>

#include "skbuff_debug.h"
#include "skbuff_notifier.h"
#include "skbuff_recycle.h"

static int skbuff_debugobj_enabled __read_mostly = 1;

static int skbuff_debug_event_handler(struct notifier_block *nb,
				      unsigned long action, void *data);
static struct notifier_block skbuff_debug_notify = {
	.notifier_call = skbuff_debug_event_handler,
	.priority = 0
};

inline u32 skbuff_debugobj_sum(struct sk_buff *skb)
{
	int pos = offsetof(struct sk_buff, free_addr);
	u32 sum = 0;

	while (pos--)
		sum += ((u8 *)skb)[pos];

	return sum;
}

struct skbuff_debugobj_walking {
	int pos;
	void **d;
};

#ifdef CONFIG_ARM
static int skbuff_debugobj_walkstack(struct stackframe *frame, void *p) {
	struct skbuff_debugobj_walking *w = (struct skbuff_debugobj_walking *)p;
	unsigned long pc = frame->pc;

	if (w->pos < DEBUG_OBJECTS_SKBUFF_STACKSIZE - 1) {
		w->d[w->pos++] = (void *)pc;
		return 0;
	}

	return -ENOENT;
}
#else
static bool skbuff_debugobj_walkstack(void *p, unsigned long pc)
{
	struct skbuff_debugobj_walking *w = (struct skbuff_debugobj_walking *)p;

	if (w->pos < DEBUG_OBJECTS_SKBUFF_STACKSIZE - 1) {
		w->d[w->pos++] = (void *)pc;
		return true;
	}

	return false;
}
#endif

#if defined(CONFIG_ARM) || defined(CONFIG_ARM64)
static void skbuff_debugobj_get_stack(void **ret)
{
	struct skbuff_debugobj_walking w = {0, ret};
	void *p = &w;

#ifdef CONFIG_ARM
	struct stackframe frame;
	register unsigned long current_sp asm ("sp");

	frame.lr = (unsigned long)__builtin_return_address(0);
	frame.fp = (unsigned long)__builtin_frame_address(0);
	frame.sp = current_sp;
	frame.pc = (unsigned long)skbuff_debugobj_get_stack;

	walk_stackframe(&frame, skbuff_debugobj_walkstack, p);
#else
	arch_stack_walk(skbuff_debugobj_walkstack, p, current, NULL);
#endif

	ret[w.pos] = NULL;
}
#else
#error
static void skbuff_debugobj_get_stack(void **ret)
{
	/* not supported */
	ret[0] = 0xdeadbeef;
}
#endif

void skbuff_debugobj_print_stack(void *const *stack)
{
	int i;

	for (i = 0; stack[i]; i++)
		pr_emerg("\t %pS (0x%p)\n", stack[i], stack[i]);
}

static const char *skbuff_debugobj_state_name(const struct sk_buff *skb)
{
	int obj_state;

	obj_state = debug_object_get_state((struct sk_buff *)skb);
	switch (obj_state) {
	case ODEBUG_STATE_NONE:
		return "none";
	case ODEBUG_STATE_INIT:
		return "init";
	case ODEBUG_STATE_INACTIVE:
		return "inactive";
	case ODEBUG_STATE_ACTIVE:
		return "active";
	case ODEBUG_STATE_DESTROYED:
		return "destroyed";
	case ODEBUG_STATE_NOTAVAILABLE:
		return "not available";
	default:
		return "invalid";
	}
}

void skbuff_debugobj_print_skb(const struct sk_buff *skb)
{
	pr_emerg("skb_debug: current process = %s (pid %i)\n",
		 current->comm, current->pid);
	pr_emerg("skb_debug: skb 0x%p, next 0x%p, prev 0x%p, state = %s\n", skb,
		 skb->next, skb->prev, skbuff_debugobj_state_name(skb));
	pr_emerg("skb_debug: free stack:\n");
	skbuff_debugobj_print_stack(skb->free_addr);
	pr_emerg("skb_debug: alloc stack:\n");
	skbuff_debugobj_print_stack(skb->alloc_addr);
}
EXPORT_SYMBOL(skbuff_debugobj_print_skb);

/* skbuff_debugobj_fixup():
 *	Called when an error is detected in the state machine for
 *	the objects
 */
static bool skbuff_debugobj_fixup(void *addr, enum debug_obj_state state)
{
	struct sk_buff *skb = (struct sk_buff *)addr;
	ftrace_dump(DUMP_ALL);
	WARN(1, "skb_debug: state = %d, skb = 0x%p sum = %d (now %d)\n",
	     state, skb, skb->sum, skbuff_debugobj_sum(skb));
	skb_recycler_notifier_send_event(SKB_RECYCLER_NOTIFIER_FSM, skb);

	return true;
}

static struct debug_obj_descr skbuff_debug_descr = {
	.name		= "sk_buff_struct",
	.fixup_init	= skbuff_debugobj_fixup,
	.fixup_activate	= skbuff_debugobj_fixup,
	.fixup_destroy	= skbuff_debugobj_fixup,
	.fixup_free	= skbuff_debugobj_fixup,
};

inline void skbuff_debugobj_activate(struct sk_buff *skb)
{
	int ret = 0;

	if (!skbuff_debugobj_enabled)
		return;

	skbuff_debugobj_get_stack(skb->alloc_addr);
	ret = debug_object_activate(skb, &skbuff_debug_descr);
	if (ret)
		goto err_act;

	skbuff_debugobj_sum_validate(skb);

	return;

err_act:
	ftrace_dump(DUMP_ALL);
	WARN(1, "skb_debug: failed to activate err = %d skb = 0x%p sum = %d (now %d)\n",
	     ret, skb, skb->sum, skbuff_debugobj_sum(skb));
	skb_recycler_notifier_send_event(SKB_RECYCLER_NOTIFIER_DBLALLOC, skb);
}

inline void skbuff_debugobj_init_and_activate(struct sk_buff *skb)
{
	if (!skbuff_debugobj_enabled)
		return;

	/* if we're coming from the slab, the skb->sum might
	 * be invalid anyways
	 */
	skb->sum = skbuff_debugobj_sum(skb);

	debug_object_init(skb, &skbuff_debug_descr);
	skbuff_debugobj_activate(skb);
}

inline void skbuff_debugobj_deactivate(struct sk_buff *skb)
{
	int obj_state;

	if (!skbuff_debugobj_enabled)
		return;

	skb->sum = skbuff_debugobj_sum(skb);

	obj_state = debug_object_get_state(skb);

	if (obj_state == ODEBUG_STATE_ACTIVE) {
		debug_object_deactivate(skb, &skbuff_debug_descr);
		skbuff_debugobj_get_stack(skb->free_addr);
		return;
	}

	ftrace_dump(DUMP_ALL);
	WARN(1, "skb_debug: deactivating inactive object skb=0x%p state=%d sum = %d (now %d)\n",
	     skb, obj_state, skb->sum, skbuff_debugobj_sum(skb));
	skb_recycler_notifier_send_event(SKB_RECYCLER_NOTIFIER_DBLFREE, skb);
}

inline void _skbuff_debugobj_sum_validate(struct sk_buff *skb,
					  const char *var, const char *src,
					  int line, const char *fxn)
{
	if (!skbuff_debugobj_enabled || !skb)
		return;

	if (skb->sum == skbuff_debugobj_sum(skb))
		return;

	ftrace_dump(DUMP_ALL);
	WARN(1, "skb_debug: skb sum changed skb = 0x%p sum = %d (now %d)\n",
	     skb, skb->sum, skbuff_debugobj_sum(skb));
	pr_emerg("skb_debug: %s() checking %s in %s:%d\n", fxn, var, src, line);
	skb_recycler_notifier_send_event(SKB_RECYCLER_NOTIFIER_SUMERR, skb);
}

inline void skbuff_debugobj_sum_update(struct sk_buff *skb)
{
	if (!skbuff_debugobj_enabled || !skb)
		return;

	skb->sum = skbuff_debugobj_sum(skb);
}

inline void skbuff_debugobj_destroy(struct sk_buff *skb)
{
	if (!skbuff_debugobj_enabled)
		return;

	debug_object_destroy(skb, &skbuff_debug_descr);
}

static int __init disable_object_debug(char *str)
{
	skbuff_debugobj_enabled = 0;

	pr_info("skb_debug: debug objects is disabled\n");
	return 0;
}

early_param("no_skbuff_debug_objects", disable_object_debug);

void skbuff_debugobj_print_skb_list(const struct sk_buff *skb_list,
				    const char *list_title, int cpu)
{
	int count;
	struct sk_buff *skb_i = (struct sk_buff *)skb_list;
	u32 sum_i, sum_now;
	int obj_state;

	if (cpu < 0) {
		cpu = get_cpu();
		put_cpu();
	}
	pr_emerg("skb_debug: start skb list '%s' [CPU#%d]\n", list_title, cpu);
	count = 0;
	if (skb_list) {
		do {
			obj_state =
				debug_object_get_state(skb_i);
			if (obj_state < ODEBUG_STATE_NOTAVAILABLE) {
				sum_i = skb_i->sum;
				sum_now = skbuff_debugobj_sum(skb_i);
			} else {
				sum_i = 0;
				sum_now = 0;
			}
			if (sum_i != sum_now) {
				pr_emerg("skb_debug: [%02d] skb 0x%p, next 0x%p, prev 0x%p, state %d (%s), sum %d (now %d)\n",
					 count, skb_i, skb_i->next, skb_i->prev,
					 obj_state, skbuff_debugobj_state_name(skb_i),
					 sum_i, sum_now);
			}
			skb_i = skb_i->next;
			count++;
		} while (skb_list != skb_i);
	}
	pr_emerg("skb_debug: end skb list '%s'. In total %d skbs iterated.\n", list_title, count);
}

void skbuff_debugobj_register_callback(void)
{
	skb_recycler_notifier_register(&skbuff_debug_notify);
}

int skbuff_debug_event_handler(struct notifier_block *nb, unsigned long action,
			       void *data)
{
	struct sk_buff *skb = (struct sk_buff *)data;

	pr_emerg("skb_debug: notifier event %lu\n", action);
	skbuff_debugobj_print_skb(skb);
	skb_recycler_print_all_lists();

	return NOTIFY_DONE;
}
