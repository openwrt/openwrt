/*
 * swconfig.c: Switch configuration API
 *
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/capability.h>
#include <linux/skbuff.h>
#include <linux/switch.h>

//#define DEBUG 1
#ifdef DEBUG
#define DPRINTF(format, ...) printk("%s: " format, __func__, ##__VA_ARGS__)
#else
#define DPRINTF(...) do {} while(0)
#endif

#define SWCONFIG_DEVNAME	"switch%d"

MODULE_AUTHOR("Felix Fietkau <nbd@openwrt.org>");
MODULE_LICENSE("GPL");

static int swdev_id = 0;
static struct list_head swdevs;
static DEFINE_SPINLOCK(swdevs_lock);
struct swconfig_callback;

struct swconfig_callback
{
	struct sk_buff *msg;
	struct genlmsghdr *hdr;
	struct genl_info *info;
	int cmd;

	/* callback for filling in the message data */
	int (*fill)(struct swconfig_callback *cb, void *arg);

	/* callback for closing the message before sending it */
	int (*close)(struct swconfig_callback *cb, void *arg);

	struct nlattr *nest[4];
	int args[4];
};

/* defaults */

static int
swconfig_get_vlan_ports(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	int ret;
	if (val->port_vlan >= dev->vlans)
		return -EINVAL;

	if (!dev->ops->get_vlan_ports)
		return -EOPNOTSUPP;

	ret = dev->ops->get_vlan_ports(dev, val);
	return ret;
}

static int
swconfig_set_vlan_ports(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	struct switch_port *ports = val->value.ports;
	const struct switch_dev_ops *ops = dev->ops;
	int i;

	if (val->port_vlan >= dev->vlans)
		return -EINVAL;

	/* validate ports */
	if (val->len > dev->ports)
		return -EINVAL;

	if (!ops->set_vlan_ports)
		return -EOPNOTSUPP;

	for (i = 0; i < val->len; i++) {
		if (ports[i].id >= dev->ports)
			return -EINVAL;

		if (ops->set_port_pvid &&
		    !(ports[i].flags & (1 << SWITCH_PORT_FLAG_TAGGED)))
			ops->set_port_pvid(dev, ports[i].id, val->port_vlan);
	}

	return ops->set_vlan_ports(dev, val);
}

static int
swconfig_set_pvid(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	if (val->port_vlan >= dev->ports)
		return -EINVAL;

	if (!dev->ops->set_port_pvid)
		return -EOPNOTSUPP;

	return dev->ops->set_port_pvid(dev, val->port_vlan, val->value.i);
}

static int
swconfig_get_pvid(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	if (val->port_vlan >= dev->ports)
		return -EINVAL;

	if (!dev->ops->get_port_pvid)
		return -EOPNOTSUPP;

	return dev->ops->get_port_pvid(dev, val->port_vlan, &val->value.i);
}

static int
swconfig_apply_config(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	/* don't complain if not supported by the switch driver */
	if (!dev->ops->apply_config)
		return 0;

	return dev->ops->apply_config(dev);
}

static int
swconfig_reset_switch(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val)
{
	/* don't complain if not supported by the switch driver */
	if (!dev->ops->reset_switch)
		return 0;

	return dev->ops->reset_switch(dev);
}

enum global_defaults {
	GLOBAL_APPLY,
	GLOBAL_RESET,
};

enum vlan_defaults {
	VLAN_PORTS,
};

enum port_defaults {
	PORT_PVID,
};

static struct switch_attr default_global[] = {
	[GLOBAL_APPLY] = {
		.type = SWITCH_TYPE_NOVAL,
		.name = "apply",
		.description = "Activate changes in the hardware",
		.set = swconfig_apply_config,
	},
	[GLOBAL_RESET] = {
		.type = SWITCH_TYPE_NOVAL,
		.name = "reset",
		.description = "Reset the switch",
		.set = swconfig_reset_switch,
	}
};

static struct switch_attr default_port[] = {
	[PORT_PVID] = {
		.type = SWITCH_TYPE_INT,
		.name = "pvid",
		.description = "Primary VLAN ID",
		.set = swconfig_set_pvid,
		.get = swconfig_get_pvid,
	}
};

static struct switch_attr default_vlan[] = {
	[VLAN_PORTS] = {
		.type = SWITCH_TYPE_PORTS,
		.name = "ports",
		.description = "VLAN port mapping",
		.set = swconfig_set_vlan_ports,
		.get = swconfig_get_vlan_ports,
	},
};


static void swconfig_defaults_init(struct switch_dev *dev)
{
	const struct switch_dev_ops *ops = dev->ops;

	dev->def_global = 0;
	dev->def_vlan = 0;
	dev->def_port = 0;

	if (ops->get_vlan_ports || ops->set_vlan_ports)
		set_bit(VLAN_PORTS, &dev->def_vlan);

	if (ops->get_port_pvid || ops->set_port_pvid)
		set_bit(PORT_PVID, &dev->def_port);

	/* always present, can be no-op */
	set_bit(GLOBAL_APPLY, &dev->def_global);
	set_bit(GLOBAL_RESET, &dev->def_global);
}


static struct genl_family switch_fam = {
	.id = GENL_ID_GENERATE,
	.name = "switch",
	.hdrsize = 0,
	.version = 1,
	.maxattr = SWITCH_ATTR_MAX,
};

static const struct nla_policy switch_policy[SWITCH_ATTR_MAX+1] = {
	[SWITCH_ATTR_ID] = { .type = NLA_U32 },
	[SWITCH_ATTR_OP_ID] = { .type = NLA_U32 },
	[SWITCH_ATTR_OP_PORT] = { .type = NLA_U32 },
	[SWITCH_ATTR_OP_VLAN] = { .type = NLA_U32 },
	[SWITCH_ATTR_OP_VALUE_INT] = { .type = NLA_U32 },
	[SWITCH_ATTR_OP_VALUE_STR] = { .type = NLA_NUL_STRING },
	[SWITCH_ATTR_OP_VALUE_PORTS] = { .type = NLA_NESTED },
	[SWITCH_ATTR_TYPE] = { .type = NLA_U32 },
};

static const struct nla_policy port_policy[SWITCH_PORT_ATTR_MAX+1] = {
	[SWITCH_PORT_ID] = { .type = NLA_U32 },
	[SWITCH_PORT_FLAG_TAGGED] = { .type = NLA_FLAG },
};

static inline void
swconfig_lock(void)
{
	spin_lock(&swdevs_lock);
}

static inline void
swconfig_unlock(void)
{
	spin_unlock(&swdevs_lock);
}

static struct switch_dev *
swconfig_get_dev(struct genl_info *info)
{
	struct switch_dev *dev = NULL;
	struct switch_dev *p;
	int id;

	if (!info->attrs[SWITCH_ATTR_ID])
		goto done;

	id = nla_get_u32(info->attrs[SWITCH_ATTR_ID]);
	swconfig_lock();
	list_for_each_entry(p, &swdevs, dev_list) {
		if (id != p->id)
			continue;

		dev = p;
		break;
	}
	if (dev)
		spin_lock(&dev->lock);
	else
		DPRINTF("device %d not found\n", id);
	swconfig_unlock();
done:
	return dev;
}

static inline void
swconfig_put_dev(struct switch_dev *dev)
{
	spin_unlock(&dev->lock);
}

static int
swconfig_dump_attr(struct swconfig_callback *cb, void *arg)
{
	struct switch_attr *op = arg;
	struct genl_info *info = cb->info;
	struct sk_buff *msg = cb->msg;
	int id = cb->args[0];
	void *hdr;

	hdr = genlmsg_put(msg, info->snd_pid, info->snd_seq, &switch_fam,
			NLM_F_MULTI, SWITCH_CMD_NEW_ATTR);
	if (IS_ERR(hdr))
		return -1;

	NLA_PUT_U32(msg, SWITCH_ATTR_OP_ID, id);
	NLA_PUT_U32(msg, SWITCH_ATTR_OP_TYPE, op->type);
	NLA_PUT_STRING(msg, SWITCH_ATTR_OP_NAME, op->name);
	if (op->description)
		NLA_PUT_STRING(msg, SWITCH_ATTR_OP_DESCRIPTION,
			op->description);

	return genlmsg_end(msg, hdr);
nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}

/* spread multipart messages across multiple message buffers */
static int
swconfig_send_multipart(struct swconfig_callback *cb, void *arg)
{
	struct genl_info *info = cb->info;
	int restart = 0;
	int err;

	do {
		if (!cb->msg) {
			cb->msg = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
			if (cb->msg == NULL)
				goto error;
		}

		if (!(cb->fill(cb, arg) < 0))
			break;

		/* fill failed, check if this was already the second attempt */
		if (restart)
			goto error;

		/* try again in a new message, send the current one */
		restart = 1;
		if (cb->close) {
			if (cb->close(cb, arg) < 0)
				goto error;
		}
		err = genlmsg_reply(cb->msg, info);
		cb->msg = NULL;
		if (err < 0)
			goto error;

	} while (restart);

	return 0;

error:
	if (cb->msg)
		nlmsg_free(cb->msg);
	return -1;
}

static int
swconfig_list_attrs(struct sk_buff *skb, struct genl_info *info)
{
	struct genlmsghdr *hdr = nlmsg_data(info->nlhdr);
	const struct switch_attrlist *alist;
	struct switch_dev *dev;
	struct swconfig_callback cb;
	int err = -EINVAL;
	int i;

	/* defaults */
	struct switch_attr *def_list;
	unsigned long *def_active;
	int n_def;

	dev = swconfig_get_dev(info);
	if (!dev)
		return -EINVAL;

	switch(hdr->cmd) {
	case SWITCH_CMD_LIST_GLOBAL:
		alist = &dev->ops->attr_global;
		def_list = default_global;
		def_active = &dev->def_global;
		n_def = ARRAY_SIZE(default_global);
		break;
	case SWITCH_CMD_LIST_VLAN:
		alist = &dev->ops->attr_vlan;
		def_list = default_vlan;
		def_active = &dev->def_vlan;
		n_def = ARRAY_SIZE(default_vlan);
		break;
	case SWITCH_CMD_LIST_PORT:
		alist = &dev->ops->attr_port;
		def_list = default_port;
		def_active = &dev->def_port;
		n_def = ARRAY_SIZE(default_port);
		break;
	default:
		WARN_ON(1);
		goto out;
	}

	memset(&cb, 0, sizeof(cb));
	cb.info = info;
	cb.fill = swconfig_dump_attr;
	for (i = 0; i < alist->n_attr; i++) {
		if (alist->attr[i].disabled)
			continue;
		cb.args[0] = i;
		err = swconfig_send_multipart(&cb, (void *) &alist->attr[i]);
		if (err < 0)
			goto error;
	}

	/* defaults */
	for (i = 0; i < n_def; i++) {
		if (!test_bit(i, def_active))
			continue;
		cb.args[0] = SWITCH_ATTR_DEFAULTS_OFFSET + i;
		err = swconfig_send_multipart(&cb, (void *) &def_list[i]);
		if (err < 0)
			goto error;
	}
	swconfig_put_dev(dev);

	if (!cb.msg)
		return 0;

	return genlmsg_reply(cb.msg, info);

error:
	if (cb.msg)
		nlmsg_free(cb.msg);
out:
	swconfig_put_dev(dev);
	return err;
}

static const struct switch_attr *
swconfig_lookup_attr(struct switch_dev *dev, struct genl_info *info,
		struct switch_val *val)
{
	struct genlmsghdr *hdr = nlmsg_data(info->nlhdr);
	const struct switch_attrlist *alist;
	const struct switch_attr *attr = NULL;
	int attr_id;

	/* defaults */
	struct switch_attr *def_list;
	unsigned long *def_active;
	int n_def;

	if (!info->attrs[SWITCH_ATTR_OP_ID])
		goto done;

	switch(hdr->cmd) {
	case SWITCH_CMD_SET_GLOBAL:
	case SWITCH_CMD_GET_GLOBAL:
		alist = &dev->ops->attr_global;
		def_list = default_global;
		def_active = &dev->def_global;
		n_def = ARRAY_SIZE(default_global);
		break;
	case SWITCH_CMD_SET_VLAN:
	case SWITCH_CMD_GET_VLAN:
		alist = &dev->ops->attr_vlan;
		def_list = default_vlan;
		def_active = &dev->def_vlan;
		n_def = ARRAY_SIZE(default_vlan);
		if (!info->attrs[SWITCH_ATTR_OP_VLAN])
			goto done;
		val->port_vlan = nla_get_u32(info->attrs[SWITCH_ATTR_OP_VLAN]);
		if (val->port_vlan >= dev->vlans)
			goto done;
		break;
	case SWITCH_CMD_SET_PORT:
	case SWITCH_CMD_GET_PORT:
		alist = &dev->ops->attr_port;
		def_list = default_port;
		def_active = &dev->def_port;
		n_def = ARRAY_SIZE(default_port);
		if (!info->attrs[SWITCH_ATTR_OP_PORT])
			goto done;
		val->port_vlan = nla_get_u32(info->attrs[SWITCH_ATTR_OP_PORT]);
		if (val->port_vlan >= dev->ports)
			goto done;
		break;
	default:
		WARN_ON(1);
		goto done;
	}

	if (!alist)
		goto done;

	attr_id = nla_get_u32(info->attrs[SWITCH_ATTR_OP_ID]);
	if (attr_id >= SWITCH_ATTR_DEFAULTS_OFFSET) {
		attr_id -= SWITCH_ATTR_DEFAULTS_OFFSET;
		if (attr_id >= n_def)
			goto done;
		if (!test_bit(attr_id, def_active))
			goto done;
		attr = &def_list[attr_id];
	} else {
		if (attr_id >= alist->n_attr)
			goto done;
		attr = &alist->attr[attr_id];
	}

	if (attr->disabled)
		attr = NULL;

done:
	if (!attr)
		DPRINTF("attribute lookup failed\n");
	val->attr = attr;
	return attr;
}

static int
swconfig_parse_ports(struct sk_buff *msg, struct nlattr *head,
		struct switch_val *val, int max)
{
	struct nlattr *nla;
	int rem;

	val->len = 0;
	nla_for_each_nested(nla, head, rem) {
		struct nlattr *tb[SWITCH_PORT_ATTR_MAX+1];
		struct switch_port *port = &val->value.ports[val->len];

		if (val->len >= max)
			return -EINVAL;

		if (nla_parse_nested(tb, SWITCH_PORT_ATTR_MAX, nla,
				port_policy))
			return -EINVAL;

		if (!tb[SWITCH_PORT_ID])
			return -EINVAL;

		port->id = nla_get_u32(tb[SWITCH_PORT_ID]);
		if (tb[SWITCH_PORT_FLAG_TAGGED])
			port->flags |= (1 << SWITCH_PORT_FLAG_TAGGED);
		val->len++;
	}

	return 0;
}

static int
swconfig_set_attr(struct sk_buff *skb, struct genl_info *info)
{
	const struct switch_attr *attr;
	struct switch_dev *dev;
	struct switch_val val;
	int err = -EINVAL;

	dev = swconfig_get_dev(info);
	if (!dev)
		return -EINVAL;

	memset(&val, 0, sizeof(val));
	attr = swconfig_lookup_attr(dev, info, &val);
	if (!attr || !attr->set)
		goto error;

	val.attr = attr;
	switch(attr->type) {
	case SWITCH_TYPE_NOVAL:
		break;
	case SWITCH_TYPE_INT:
		if (!info->attrs[SWITCH_ATTR_OP_VALUE_INT])
			goto error;
		val.value.i =
			nla_get_u32(info->attrs[SWITCH_ATTR_OP_VALUE_INT]);
		break;
	case SWITCH_TYPE_STRING:
		if (!info->attrs[SWITCH_ATTR_OP_VALUE_STR])
			goto error;
		val.value.s =
			nla_data(info->attrs[SWITCH_ATTR_OP_VALUE_STR]);
		break;
	case SWITCH_TYPE_PORTS:
		val.value.ports = dev->portbuf;
		memset(dev->portbuf, 0,
			sizeof(struct switch_port) * dev->ports);

		/* TODO: implement multipart? */
		if (info->attrs[SWITCH_ATTR_OP_VALUE_PORTS]) {
			err = swconfig_parse_ports(skb,
				info->attrs[SWITCH_ATTR_OP_VALUE_PORTS], &val, dev->ports);
			if (err < 0)
				goto error;
		} else {
			val.len = 0;
			err = 0;
		}
		break;
	default:
		goto error;
	}

	err = attr->set(dev, attr, &val);
error:
	swconfig_put_dev(dev);
	return err;
}

static int
swconfig_close_portlist(struct swconfig_callback *cb, void *arg)
{
	if (cb->nest[0])
		nla_nest_end(cb->msg, cb->nest[0]);
	return 0;
}

static int
swconfig_send_port(struct swconfig_callback *cb, void *arg)
{
	const struct switch_port *port = arg;
	struct nlattr *p = NULL;

	if (!cb->nest[0]) {
		cb->nest[0] = nla_nest_start(cb->msg, cb->cmd);
		if (!cb->nest[0])
			return -1;
	}

	p = nla_nest_start(cb->msg, SWITCH_ATTR_PORT);
	if (!p)
		goto error;

	NLA_PUT_U32(cb->msg, SWITCH_PORT_ID, port->id);
	if (port->flags & (1 << SWITCH_PORT_FLAG_TAGGED))
		NLA_PUT_FLAG(cb->msg, SWITCH_PORT_FLAG_TAGGED);

	nla_nest_end(cb->msg, p);
	return 0;

nla_put_failure:
		nla_nest_cancel(cb->msg, p);
error:
	nla_nest_cancel(cb->msg, cb->nest[0]);
	return -1;
}

static int
swconfig_send_ports(struct sk_buff **msg, struct genl_info *info, int attr,
		const struct switch_val *val)
{
	struct swconfig_callback cb;
	int err = 0;
	int i;

	if (!val->value.ports)
		return -EINVAL;

	memset(&cb, 0, sizeof(cb));
	cb.cmd = attr;
	cb.msg = *msg;
	cb.info = info;
	cb.fill = swconfig_send_port;
	cb.close = swconfig_close_portlist;

	cb.nest[0] = nla_nest_start(cb.msg, cb.cmd);
	for (i = 0; i < val->len; i++) {
		err = swconfig_send_multipart(&cb, &val->value.ports[i]);
		if (err)
			goto done;
	}
	err = val->len;
	swconfig_close_portlist(&cb, NULL);
	*msg = cb.msg;

done:
	return err;
}

static int
swconfig_get_attr(struct sk_buff *skb, struct genl_info *info)
{
	struct genlmsghdr *hdr = nlmsg_data(info->nlhdr);
	const struct switch_attr *attr;
	struct switch_dev *dev;
	struct sk_buff *msg = NULL;
	struct switch_val val;
	int err = -EINVAL;
	int cmd = hdr->cmd;

	dev = swconfig_get_dev(info);
	if (!dev)
		return -EINVAL;

	memset(&val, 0, sizeof(val));
	attr = swconfig_lookup_attr(dev, info, &val);
	if (!attr || !attr->get)
		goto error;

	if (attr->type == SWITCH_TYPE_PORTS) {
		val.value.ports = dev->portbuf;
		memset(dev->portbuf, 0,
			sizeof(struct switch_port) * dev->ports);
	}

	err = attr->get(dev, attr, &val);
	if (err)
		goto error;

	msg = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!msg)
		goto error;

	hdr = genlmsg_put(msg, info->snd_pid, info->snd_seq, &switch_fam,
			0, cmd);
	if (IS_ERR(hdr))
		goto nla_put_failure;

	switch(attr->type) {
	case SWITCH_TYPE_INT:
		NLA_PUT_U32(msg, SWITCH_ATTR_OP_VALUE_INT, val.value.i);
		break;
	case SWITCH_TYPE_STRING:
		NLA_PUT_STRING(msg, SWITCH_ATTR_OP_VALUE_STR, val.value.s);
		break;
	case SWITCH_TYPE_PORTS:
		err = swconfig_send_ports(&msg, info,
				SWITCH_ATTR_OP_VALUE_PORTS, &val);
		if (err < 0)
			goto nla_put_failure;
		break;
	default:
		DPRINTF("invalid type in attribute\n");
		err = -EINVAL;
		goto error;
	}
	err = genlmsg_end(msg, hdr);
	if (err < 0)
		goto nla_put_failure;

	swconfig_put_dev(dev);
	return genlmsg_reply(msg, info);

nla_put_failure:
	if (msg)
		nlmsg_free(msg);
error:
	swconfig_put_dev(dev);
	if (!err)
		err = -ENOMEM;
	return err;
}

static int
swconfig_send_switch(struct sk_buff *msg, u32 pid, u32 seq, int flags,
		const struct switch_dev *dev)
{
	void *hdr;

	hdr = genlmsg_put(msg, pid, seq, &switch_fam, flags,
			SWITCH_CMD_NEW_ATTR);
	if (IS_ERR(hdr))
		return -1;

	NLA_PUT_U32(msg, SWITCH_ATTR_ID, dev->id);
	NLA_PUT_STRING(msg, SWITCH_ATTR_DEV_NAME, dev->devname);
	NLA_PUT_STRING(msg, SWITCH_ATTR_ALIAS, dev->alias);
	NLA_PUT_STRING(msg, SWITCH_ATTR_NAME, dev->name);
	NLA_PUT_U32(msg, SWITCH_ATTR_VLANS, dev->vlans);
	NLA_PUT_U32(msg, SWITCH_ATTR_PORTS, dev->ports);
	NLA_PUT_U32(msg, SWITCH_ATTR_CPU_PORT, dev->cpu_port);

	return genlmsg_end(msg, hdr);
nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}

static int swconfig_dump_switches(struct sk_buff *skb,
		struct netlink_callback *cb)
{
	struct switch_dev *dev;
	int start = cb->args[0];
	int idx = 0;

	swconfig_lock();
	list_for_each_entry(dev, &swdevs, dev_list) {
		if (++idx <= start)
			continue;
		if (swconfig_send_switch(skb, NETLINK_CB(cb->skb).pid,
				cb->nlh->nlmsg_seq, NLM_F_MULTI,
				dev) < 0)
			break;
	}
	swconfig_unlock();
	cb->args[0] = idx;

	return skb->len;
}

static int
swconfig_done(struct netlink_callback *cb)
{
	return 0;
}

static struct genl_ops swconfig_ops[] = {
	{
		.cmd = SWITCH_CMD_LIST_GLOBAL,
		.doit = swconfig_list_attrs,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_LIST_VLAN,
		.doit = swconfig_list_attrs,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_LIST_PORT,
		.doit = swconfig_list_attrs,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_GET_GLOBAL,
		.doit = swconfig_get_attr,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_GET_VLAN,
		.doit = swconfig_get_attr,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_GET_PORT,
		.doit = swconfig_get_attr,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_SET_GLOBAL,
		.doit = swconfig_set_attr,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_SET_VLAN,
		.doit = swconfig_set_attr,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_SET_PORT,
		.doit = swconfig_set_attr,
		.policy = switch_policy,
	},
	{
		.cmd = SWITCH_CMD_GET_SWITCH,
		.dumpit = swconfig_dump_switches,
		.policy = switch_policy,
		.done = swconfig_done,
	}
};

int
register_switch(struct switch_dev *dev, struct net_device *netdev)
{
	struct switch_dev *sdev;
	const int max_switches = 8 * sizeof(unsigned long);
	unsigned long in_use = 0;
	int i;

	INIT_LIST_HEAD(&dev->dev_list);
	if (netdev) {
		dev->netdev = netdev;
		if (!dev->alias)
			dev->alias = netdev->name;
	}
	BUG_ON(!dev->alias);

	if (dev->ports > 0) {
		dev->portbuf = kzalloc(sizeof(struct switch_port) * dev->ports,
				GFP_KERNEL);
		if (!dev->portbuf)
			return -ENOMEM;
	}
	swconfig_defaults_init(dev);
	spin_lock_init(&dev->lock);
	swconfig_lock();
	dev->id = ++swdev_id;

	list_for_each_entry(sdev, &swdevs, dev_list) {
		if (!sscanf(sdev->devname, SWCONFIG_DEVNAME, &i))
			continue;
		if (i < 0 || i > max_switches)
			continue;

		set_bit(i, &in_use);
	}
	i = find_first_zero_bit(&in_use, max_switches);

	if (i == max_switches)
		return -ENFILE;

	/* fill device name */
	snprintf(dev->devname, IFNAMSIZ, SWCONFIG_DEVNAME, i);

	list_add(&dev->dev_list, &swdevs);
	swconfig_unlock();

	return 0;
}
EXPORT_SYMBOL_GPL(register_switch);

void
unregister_switch(struct switch_dev *dev)
{
	kfree(dev->portbuf);
	spin_lock(&dev->lock);
	swconfig_lock();
	list_del(&dev->dev_list);
	swconfig_unlock();
	spin_unlock(&dev->lock);
}
EXPORT_SYMBOL_GPL(unregister_switch);


static int __init
swconfig_init(void)
{
	int i, err;

	INIT_LIST_HEAD(&swdevs);
	err = genl_register_family(&switch_fam);
	if (err)
		return err;

	for (i = 0; i < ARRAY_SIZE(swconfig_ops); i++) {
		err = genl_register_ops(&switch_fam, &swconfig_ops[i]);
		if (err)
			goto unregister;
	}

	return 0;

unregister:
	genl_unregister_family(&switch_fam);
	return err;
}

static void __exit
swconfig_exit(void)
{
	genl_unregister_family(&switch_fam);
}

module_init(swconfig_init);
module_exit(swconfig_exit);

