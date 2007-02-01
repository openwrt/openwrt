#ifndef __COMPAT_HACKS
#define __COMPAT_HACKS

#include <linux/types.h>
#include <linux/workqueue.h>
#include <net/sch_generic.h>

struct delayed_work {
	struct work_struct work;
};

#define INIT_DELAYED_WORK(_work, _func) \
		INIT_WORK(&(_work)->work, _func, NULL)

#define INIT_WORK_NEW(_work, _func) INIT_WORK(_work, _func, NULL)

#define schedule_delayed_work(_work, interval) schedule_delayed_work(&(_work)->work, interval)
#define cancel_delayed_work(_work) cancel_delayed_work(&(_work)->work)

static inline struct Qdisc *_qdisc_create_dflt(struct net_device *dev, struct Qdisc_ops *ops)
{
	return qdisc_create_dflt(dev, ops);
}

#define qdisc_create_dflt(_dev, _ops, _handle) \
		_qdisc_create_dflt(_dev, _ops)

#endif
