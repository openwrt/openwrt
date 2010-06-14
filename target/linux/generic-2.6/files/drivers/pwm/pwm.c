/*
 * drivers/pwm/pwm.c
 *
 * Copyright (C) 2010 Bill Gatliff <bgat@billgatliff.com>
 *
 * This program is free software; you may redistribute and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/fs.h>
#include <linux/completion.h>
#include <linux/workqueue.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/slab.h>    /*kcalloc, kfree since 2.6.34 */
#include <linux/pwm/pwm.h>

static int __pwm_create_sysfs(struct pwm_device *pwm);

static const char *REQUEST_SYSFS = "sysfs";
static LIST_HEAD(pwm_device_list);
static DEFINE_MUTEX(device_list_mutex);
static struct class pwm_class;
static struct workqueue_struct *pwm_handler_workqueue;

int pwm_register(struct pwm_device *pwm)
{
	struct pwm_channel *p;
	int wchan;
	int ret;

	spin_lock_init(&pwm->list_lock);

	p = kcalloc(pwm->nchan, sizeof(*p), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	for (wchan = 0; wchan < pwm->nchan; wchan++) {
		spin_lock_init(&p[wchan].lock);
		init_completion(&p[wchan].complete);
		p[wchan].chan = wchan;
		p[wchan].pwm = pwm;
	}

	pwm->channels = p;

	mutex_lock(&device_list_mutex);

	list_add_tail(&pwm->list, &pwm_device_list);
	ret = __pwm_create_sysfs(pwm);
	if (ret) {
		mutex_unlock(&device_list_mutex);
		goto err_create_sysfs;
	}

	mutex_unlock(&device_list_mutex);

	dev_info(pwm->dev, "%d channel%s\n", pwm->nchan,
		 pwm->nchan > 1 ? "s" : "");
	return 0;

err_create_sysfs:
	kfree(p);

	return ret;
}
EXPORT_SYMBOL(pwm_register);

static int __match_device(struct device *dev, void *data)
{
	return dev_get_drvdata(dev) == data;
}

int pwm_unregister(struct pwm_device *pwm)
{
	int wchan;
	struct device *dev;

	mutex_lock(&device_list_mutex);

	for (wchan = 0; wchan < pwm->nchan; wchan++) {
	  if (pwm->channels[wchan].flags & BIT(FLAG_REQUESTED)) {
			mutex_unlock(&device_list_mutex);
			return -EBUSY;
		}
	}

	for (wchan = 0; wchan < pwm->nchan; wchan++) {
		dev = class_find_device(&pwm_class, NULL,
					&pwm->channels[wchan],
					__match_device);
		if (dev) {
			put_device(dev);
			device_unregister(dev);
		}
	}

	kfree(pwm->channels);
	list_del(&pwm->list);
	mutex_unlock(&device_list_mutex);

	return 0;
}
EXPORT_SYMBOL(pwm_unregister);

static struct pwm_device *
__pwm_find_device(const char *bus_id)
{
	struct pwm_device *p;

	list_for_each_entry(p, &pwm_device_list, list) {
		if (!strcmp(bus_id, p->bus_id))
			return p;
	}
	return NULL;
}

static int
__pwm_request_channel(struct pwm_channel *p,
		      const char *requester)
{
	int ret;

	if (test_and_set_bit(FLAG_REQUESTED, &p->flags))
		return -EBUSY;

	if (p->pwm->request) {
		ret = p->pwm->request(p);
		if (ret) {
			clear_bit(FLAG_REQUESTED, &p->flags);
			return ret;
		}
	}

	p->requester = requester;
	if (!strcmp(requester, REQUEST_SYSFS))
		p->pid = current->pid;

	return 0;
}

struct pwm_channel *
pwm_request(const char *bus_id,
	    int chan,
	    const char *requester)
{
	struct pwm_device *p;
	int ret;

	mutex_lock(&device_list_mutex);

	p = __pwm_find_device(bus_id);
	if (!p || chan >= p->nchan)
		goto err_no_device;

	if (!try_module_get(p->owner))
		goto err_module_get_failed;

	ret = __pwm_request_channel(&p->channels[chan], requester);
	if (ret)
		goto err_request_failed;

	mutex_unlock(&device_list_mutex);
	return &p->channels[chan];

err_request_failed:
	module_put(p->owner);
err_module_get_failed:
err_no_device:
	mutex_unlock(&device_list_mutex);
	return NULL;
}
EXPORT_SYMBOL(pwm_request);

void pwm_free(struct pwm_channel *p)
{
	mutex_lock(&device_list_mutex);

	if (!test_and_clear_bit(FLAG_REQUESTED, &p->flags))
		goto done;

	pwm_stop(p);
	pwm_unsynchronize(p, NULL);
	pwm_set_handler(p, NULL, NULL);

	if (p->pwm->free)
		p->pwm->free(p);
	module_put(p->pwm->owner);
done:
	mutex_unlock(&device_list_mutex);
}
EXPORT_SYMBOL(pwm_free);

unsigned long pwm_ns_to_ticks(struct pwm_channel *p,
			      unsigned long nsecs)
{
	unsigned long long ticks;

	ticks = nsecs;
	ticks *= p->tick_hz;
	do_div(ticks, 1000000000);
	return ticks;
}
EXPORT_SYMBOL(pwm_ns_to_ticks);

unsigned long pwm_ticks_to_ns(struct pwm_channel *p,
			      unsigned long ticks)
{
	unsigned long long ns;

	if (!p->tick_hz)
		return 0;

	ns = ticks;
	ns *= 1000000000UL;
	do_div(ns, p->tick_hz);
	return ns;
}
EXPORT_SYMBOL(pwm_ticks_to_ns);

static void
pwm_config_ns_to_ticks(struct pwm_channel *p,
		       struct pwm_channel_config *c)
{
	if (c->config_mask & PWM_CONFIG_PERIOD_NS) {
		c->period_ticks = pwm_ns_to_ticks(p, c->period_ns);
		c->config_mask &= ~PWM_CONFIG_PERIOD_NS;
		c->config_mask |= PWM_CONFIG_PERIOD_TICKS;
	}

	if (c->config_mask & PWM_CONFIG_DUTY_NS) {
		c->duty_ticks = pwm_ns_to_ticks(p, c->duty_ns);
		c->config_mask &= ~PWM_CONFIG_DUTY_NS;
		c->config_mask |= PWM_CONFIG_DUTY_TICKS;
	}
}

static void
pwm_config_percent_to_ticks(struct pwm_channel *p,
			    struct pwm_channel_config *c)
{
	if (c->config_mask & PWM_CONFIG_DUTY_PERCENT) {
		if (c->config_mask & PWM_CONFIG_PERIOD_TICKS)
			c->duty_ticks = c->period_ticks;
		else
			c->duty_ticks = p->period_ticks;

		c->duty_ticks *= c->duty_percent;
		c->duty_ticks /= 100;
		c->config_mask &= ~PWM_CONFIG_DUTY_PERCENT;
		c->config_mask |= PWM_CONFIG_DUTY_TICKS;
	}
}

int pwm_config_nosleep(struct pwm_channel *p,
		       struct pwm_channel_config *c)
{
	if (!p->pwm->config_nosleep)
		return -EINVAL;

	pwm_config_ns_to_ticks(p, c);
	pwm_config_percent_to_ticks(p, c);

	return p->pwm->config_nosleep(p, c);
}
EXPORT_SYMBOL(pwm_config_nosleep);

int pwm_config(struct pwm_channel *p,
	       struct pwm_channel_config *c)
{
	int ret = 0;

	if (unlikely(!p->pwm->config))
		return -EINVAL;

	pwm_config_ns_to_ticks(p, c);
	pwm_config_percent_to_ticks(p, c);

	switch (c->config_mask & (PWM_CONFIG_PERIOD_TICKS
				  | PWM_CONFIG_DUTY_TICKS)) {
	case PWM_CONFIG_PERIOD_TICKS:
		if (p->duty_ticks > c->period_ticks) {
			ret = -EINVAL;
			goto err;
		}
		break;
	case PWM_CONFIG_DUTY_TICKS:
		if (p->period_ticks < c->duty_ticks) {
			ret = -EINVAL;
			goto err;
		}
		break;
	case PWM_CONFIG_DUTY_TICKS | PWM_CONFIG_PERIOD_TICKS:
		if (c->duty_ticks > c->period_ticks) {
			ret = -EINVAL;
			goto err;
		}
		break;
	default:
		break;
	}

err:
	dev_dbg(p->pwm->dev, "%s: config_mask %d period_ticks %lu duty_ticks %lu"
		" polarity %d duty_ns %lu period_ns %lu duty_percent %d\n",
		__func__, c->config_mask, c->period_ticks, c->duty_ticks,
		c->polarity, c->duty_ns, c->period_ns, c->duty_percent);

	if (ret)
		return ret;
	return p->pwm->config(p, c);
}
EXPORT_SYMBOL(pwm_config);

int pwm_set_period_ns(struct pwm_channel *p,
		      unsigned long period_ns)
{
	struct pwm_channel_config c = {
		.config_mask = PWM_CONFIG_PERIOD_TICKS,
		.period_ticks = pwm_ns_to_ticks(p, period_ns),
	};

	return pwm_config(p, &c);
}
EXPORT_SYMBOL(pwm_set_period_ns);

unsigned long pwm_get_period_ns(struct pwm_channel *p)
{
	return pwm_ticks_to_ns(p, p->period_ticks);
}
EXPORT_SYMBOL(pwm_get_period_ns);

int pwm_set_duty_ns(struct pwm_channel *p,
		    unsigned long duty_ns)
{
	struct pwm_channel_config c = {
		.config_mask = PWM_CONFIG_DUTY_TICKS,
		.duty_ticks = pwm_ns_to_ticks(p, duty_ns),
	};
	return pwm_config(p, &c);
}
EXPORT_SYMBOL(pwm_set_duty_ns);

unsigned long pwm_get_duty_ns(struct pwm_channel *p)
{
	return pwm_ticks_to_ns(p, p->duty_ticks);
}
EXPORT_SYMBOL(pwm_get_duty_ns);

int pwm_set_duty_percent(struct pwm_channel *p,
			 int percent)
{
	struct pwm_channel_config c = {
		.config_mask = PWM_CONFIG_DUTY_PERCENT,
		.duty_percent = percent,
	};
	return pwm_config(p, &c);
}
EXPORT_SYMBOL(pwm_set_duty_percent);

int pwm_set_polarity(struct pwm_channel *p,
		     int active_high)
{
	struct pwm_channel_config c = {
		.config_mask = PWM_CONFIG_POLARITY,
		.polarity = active_high,
	};
	return pwm_config(p, &c);
}
EXPORT_SYMBOL(pwm_set_polarity);

int pwm_start(struct pwm_channel *p)
{
	struct pwm_channel_config c = {
		.config_mask = PWM_CONFIG_START,
	};
	return pwm_config(p, &c);
}
EXPORT_SYMBOL(pwm_start);

int pwm_stop(struct pwm_channel *p)
{
	struct pwm_channel_config c = {
		.config_mask = PWM_CONFIG_STOP,
	};
	return pwm_config(p, &c);
}
EXPORT_SYMBOL(pwm_stop);

int pwm_synchronize(struct pwm_channel *p,
		    struct pwm_channel *to_p)
{
	if (p->pwm != to_p->pwm) {
		/* TODO: support cross-device synchronization */
		return -EINVAL;
	}

	if (!p->pwm->synchronize)
		return -EINVAL;

	return p->pwm->synchronize(p, to_p);
}
EXPORT_SYMBOL(pwm_synchronize);

int pwm_unsynchronize(struct pwm_channel *p,
		      struct pwm_channel *from_p)
{
	if (from_p && (p->pwm != from_p->pwm)) {
		/* TODO: support cross-device synchronization */
		return -EINVAL;
	}

	if (!p->pwm->unsynchronize)
		return -EINVAL;

	return p->pwm->unsynchronize(p, from_p);
}
EXPORT_SYMBOL(pwm_unsynchronize);

static void pwm_handler(struct work_struct *w)
{
	struct pwm_channel *p = container_of(w, struct pwm_channel,
					     handler_work);
	if (p->handler && p->handler(p, p->handler_data))
		pwm_stop(p);
}

static void __pwm_callback(struct pwm_channel *p)
{
	queue_work(pwm_handler_workqueue, &p->handler_work);
	dev_dbg(p->pwm->dev, "handler %p scheduled with data %p\n",
		p->handler, p->handler_data);
}

int pwm_set_handler(struct pwm_channel *p,
		    pwm_handler_t handler,
		    void *data)
{
	if (p->pwm->set_callback) {
		p->handler_data = data;
		p->handler = handler;
		INIT_WORK(&p->handler_work, pwm_handler);
		return p->pwm->set_callback(p, handler ? __pwm_callback : NULL);
	}
	return -EINVAL;
}
EXPORT_SYMBOL(pwm_set_handler);

static ssize_t pwm_run_store(struct device *dev,
			     struct device_attribute *attr,
			     const char *buf,
			     size_t len)
{
	struct pwm_channel *p = dev_get_drvdata(dev);
	if (sysfs_streq(buf, "1"))
		pwm_start(p);
	else if (sysfs_streq(buf, "0"))
		pwm_stop(p);
	return len;
}
static DEVICE_ATTR(run, 0200, NULL, pwm_run_store);

static ssize_t pwm_duty_ns_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct pwm_channel *p = dev_get_drvdata(dev);
	return sprintf(buf, "%lu\n", pwm_get_duty_ns(p));
}

static ssize_t pwm_duty_ns_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf,
				 size_t len)
{
	unsigned long duty_ns;
	struct pwm_channel *p = dev_get_drvdata(dev);

	if (1 == sscanf(buf, "%lu", &duty_ns))
		pwm_set_duty_ns(p, duty_ns);
	return len;
}
static DEVICE_ATTR(duty_ns, 0644, pwm_duty_ns_show, pwm_duty_ns_store);

static ssize_t pwm_period_ns_show(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	struct pwm_channel *p = dev_get_drvdata(dev);
	return sprintf(buf, "%lu\n", pwm_get_period_ns(p));
}

static ssize_t pwm_period_ns_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf,
				   size_t len)
{
	unsigned long period_ns;
	struct pwm_channel *p = dev_get_drvdata(dev);

	if (1 == sscanf(buf, "%lu", &period_ns))
		pwm_set_period_ns(p, period_ns);
	return len;
}
static DEVICE_ATTR(period_ns, 0644, pwm_period_ns_show, pwm_period_ns_store);

static ssize_t pwm_polarity_show(struct device *dev,
				 struct device_attribute *attr,
				 char *buf)
{
	struct pwm_channel *p = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", p->active_high ? 1 : 0);
}

static ssize_t pwm_polarity_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf,
				  size_t len)
{
	int polarity;
	struct pwm_channel *p = dev_get_drvdata(dev);

	if (1 == sscanf(buf, "%d", &polarity))
		pwm_set_polarity(p, polarity);
	return len;
}
static DEVICE_ATTR(polarity, 0644, pwm_polarity_show, pwm_polarity_store);

static ssize_t pwm_request_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct pwm_channel *p = dev_get_drvdata(dev);
	mutex_lock(&device_list_mutex);
	__pwm_request_channel(p, REQUEST_SYSFS);
	mutex_unlock(&device_list_mutex);

	if (p->pid)
		return sprintf(buf, "%s %d\n", p->requester, p->pid);
	else
		return sprintf(buf, "%s\n", p->requester);
}

static ssize_t pwm_request_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf,
				 size_t len)
{
	struct pwm_channel *p = dev_get_drvdata(dev);
	pwm_free(p);
	return len;
}
static DEVICE_ATTR(request, 0644, pwm_request_show, pwm_request_store);

static const struct attribute *pwm_attrs[] =
{
	&dev_attr_run.attr,
	&dev_attr_polarity.attr,
	&dev_attr_duty_ns.attr,
	&dev_attr_period_ns.attr,
	&dev_attr_request.attr,
	NULL,
};

static const struct attribute_group pwm_device_attr_group = {
	.attrs = (struct attribute **)pwm_attrs,
};

static int __pwm_create_sysfs(struct pwm_device *pwm)
{
	int ret = 0;
	struct device *dev;
	int wchan;

	for (wchan = 0; wchan < pwm->nchan; wchan++) {
		dev = device_create(&pwm_class, pwm->dev, MKDEV(0, 0),
				    pwm->channels + wchan,
				    "%s:%d", pwm->bus_id, wchan);
		if (!dev)
			goto err_dev_create;
		ret = sysfs_create_group(&dev->kobj, &pwm_device_attr_group);
		if (ret)
			goto err_dev_create;
	}

	return ret;

err_dev_create:
	for (wchan = 0; wchan < pwm->nchan; wchan++) {
		dev = class_find_device(&pwm_class, NULL,
					&pwm->channels[wchan],
					__match_device);
		if (dev) {
			put_device(dev);
			device_unregister(dev);
		}
	}

	return ret;
}

static struct class_attribute pwm_class_attrs[] = {
	__ATTR_NULL,
};

static struct class pwm_class = {
	.name = "pwm",
	.owner = THIS_MODULE,

	.class_attrs = pwm_class_attrs,
};

static int __init pwm_init(void)
{
	int ret;

	/* TODO: how to deal with devices that register very early? */
	pr_err("%s\n", __func__);
	ret = class_register(&pwm_class);
	if (ret < 0)
		return ret;

	pwm_handler_workqueue = create_workqueue("pwmd");

	return 0;
}
postcore_initcall(pwm_init);
