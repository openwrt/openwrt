// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <net/genetlink.h>

#include "rtl-otto.h"

static LIST_HEAD(rtl931x_stack_list);
static DEFINE_MUTEX(rtl931x_stack_lock);

static struct genl_family rtl931x_stack_family;

struct rtl931x_stack_target {
	struct rtl838x_switch_priv *priv;
	struct net_device *dev;
	int port;
};

static const struct nla_policy
rtl931x_stack_policy[RTL931X_STACK_ATTR_MAX + 1] = {
	[RTL931X_STACK_ATTR_UNSPEC] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_API_VERSION] = { .type = NLA_U16 },
	[RTL931X_STACK_ATTR_IFINDEX] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_ENABLED] = NLA_POLICY_MAX(NLA_U8, 1),
	[RTL931X_STACK_ATTR_MEMBER_ID] =
		NLA_POLICY_MAX(NLA_U8, RTL931X_STACK_MAX_DEVICES - 1),
	[RTL931X_STACK_ATTR_PEER_ID] =
		NLA_POLICY_MAX(NLA_U8, RTL931X_STACK_MAX_DEVICES - 1),
	[RTL931X_STACK_ATTR_MASTER_ID] =
		NLA_POLICY_MAX(NLA_U8, RTL931X_STACK_MAX_DEVICES - 1),
	[RTL931X_STACK_ATTR_FLAGS] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_GENERATION] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_STATE] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_LINK_UP] = { .type = NLA_REJECT },
};

static int rtl931x_stack_check_version(struct genl_info *info)
{
	struct nlattr *version = info->attrs[RTL931X_STACK_ATTR_API_VERSION];

	if (!version) {
		NL_SET_ERR_MSG_MOD(info->extack, "API version is required");
		return -EINVAL;
	}

	if (nla_get_u16(version) != RTL931X_STACK_GENL_VERSION) {
		NL_SET_ERR_MSG_MOD(info->extack, "unsupported API version");
		return -EPROTONOSUPPORT;
	}

	return 0;
}

static int rtl931x_stack_get_target(struct genl_info *info,
				    struct rtl931x_stack_target *target)
{
	struct rtl931x_stack_context *stack;
	struct nlattr *ifindex;
	int i;

	ASSERT_RTNL();
	lockdep_assert_held(&rtl931x_stack_lock);

	ifindex = info->attrs[RTL931X_STACK_ATTR_IFINDEX];
	if (!ifindex) {
		NL_SET_ERR_MSG_MOD(info->extack, "stack port ifindex is required");
		return -EINVAL;
	}

	target->dev = dev_get_by_index(genl_info_net(info),
				       nla_get_u32(ifindex));
	if (!target->dev) {
		NL_SET_ERR_MSG_MOD(info->extack, "stack port does not exist");
		return -ENODEV;
	}

	list_for_each_entry(stack, &rtl931x_stack_list, list) {
		target->priv = container_of(stack, struct rtl838x_switch_priv,
					    stack);

		for (i = 0; i < target->priv->ds->num_ports; i++) {
			if (!target->priv->ports[i].dp ||
			    target->priv->ports[i].dp->user != target->dev)
				continue;

			target->port = i;
			return 0;
		}
	}

	dev_put(target->dev);
	target->dev = NULL;
	NL_SET_ERR_MSG_MOD(info->extack, "interface is not an RTL931x user port");

	return -ENODEV;
}

static int rtl931x_stack_put_reply(struct genl_info *info,
				   struct rtl931x_stack_target *target)
{
	struct rtl931x_stack_context *stack = &target->priv->stack;
	struct sk_buff *skb;
	void *hdr;
	u32 ifindex;

	ifindex = stack->enabled ? stack->ifindex : target->dev->ifindex;
	skb = genlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	hdr = genlmsg_put_reply(skb, info, &rtl931x_stack_family, 0,
				info->genlhdr->cmd);
	if (!hdr)
		goto nla_put_failure;

	if (nla_put_u16(skb, RTL931X_STACK_ATTR_API_VERSION,
			RTL931X_STACK_GENL_VERSION) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_IFINDEX, ifindex) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_ENABLED, stack->enabled) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_MEMBER_ID, stack->member_id) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_PEER_ID, stack->peer_id) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_MASTER_ID, stack->master_id) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_FLAGS, stack->flags) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_GENERATION,
			stack->generation) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_STATE, stack->state) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_LINK_UP,
		       netif_carrier_ok(target->dev))) {
		genlmsg_cancel(skb, hdr);
		goto nla_put_failure;
	}

	genlmsg_end(skb, hdr);
	return genlmsg_reply(skb, info);

nla_put_failure:
	nlmsg_free(skb);
	return -EMSGSIZE;
}

static int rtl931x_stack_get(struct sk_buff *skb, struct genl_info *info)
{
	struct rtl931x_stack_target target = {};
	int err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_target(info, &target);
	if (err)
		goto out_unlock;

	if (target.priv->stack.enabled &&
	    target.port != target.priv->stack.port) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "interface is not the active stack port");
		err = -ENODEV;
		goto out_put;
	}

	err = rtl931x_stack_put_reply(info, &target);

out_put:
	dev_put(target.dev);
out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
	return err;
}

static bool
rtl931x_stack_request_matches(struct rtl931x_stack_context *stack, int port,
			      u8 member, u8 peer, u8 master, u32 flags,
			      bool enabled)
{
	if (!enabled)
		return !stack->enabled && !stack->saved_valid &&
		       stack->state == RTL931X_STACK_STATE_DISABLED;

	return stack->enabled && stack->saved_valid &&
	       stack->state == RTL931X_STACK_STATE_CONFIGURED &&
	       stack->port == port && stack->member_id == member &&
	       stack->peer_id == peer && stack->master_id == master &&
	       stack->flags == flags;
}

static int rtl931x_stack_set_two_member(struct sk_buff *skb,
					struct genl_info *info)
{
	struct rtl931x_stack_target target = {};
	struct rtl931x_stack_context *stack;
	struct nlattr **attrs = info->attrs;
	u32 flags, generation;
	u8 member, peer, master;
	bool enabled;
	int err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;

	if (!attrs[RTL931X_STACK_ATTR_ENABLED] ||
	    !attrs[RTL931X_STACK_ATTR_GENERATION]) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "enabled and generation are required");
		return -EINVAL;
	}

	enabled = nla_get_u8(attrs[RTL931X_STACK_ATTR_ENABLED]);
	generation = nla_get_u32(attrs[RTL931X_STACK_ATTR_GENERATION]);

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_target(info, &target);
	if (err)
		goto out_unlock;

	stack = &target.priv->stack;
	member = stack->member_id;
	peer = stack->peer_id;
	master = stack->master_id;
	flags = stack->flags;

	if (stack->enabled && target.port != stack->port) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "interface is not the active stack port");
		err = -EINVAL;
		goto out_put;
	}

	if (enabled) {
		if (!attrs[RTL931X_STACK_ATTR_MEMBER_ID] ||
		    !attrs[RTL931X_STACK_ATTR_PEER_ID] ||
		    !attrs[RTL931X_STACK_ATTR_MASTER_ID] ||
		    !attrs[RTL931X_STACK_ATTR_FLAGS]) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "member, peer, master and flags are required");
			err = -EINVAL;
			goto out_put;
		}

		member = nla_get_u8(attrs[RTL931X_STACK_ATTR_MEMBER_ID]);
		peer = nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_ID]);
		master = nla_get_u8(attrs[RTL931X_STACK_ATTR_MASTER_ID]);
		flags = nla_get_u32(attrs[RTL931X_STACK_ATTR_FLAGS]);

		if (member == peer) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "member and peer IDs must differ");
			err = -EINVAL;
			goto out_put;
		}

		if (master != member && master != peer) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "master ID must identify a stack member");
			err = -EINVAL;
			goto out_put;
		}

		if (flags & ~RTL931X_STACK_F_MASK) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "unsupported stacking flags");
			err = -EOPNOTSUPP;
			goto out_put;
		}

		if (netdev_has_any_upper_dev(target.dev)) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "stack port must not have an upper device");
			err = -EBUSY;
			goto out_put;
		}

	}

	if (stack->generation_valid && generation < stack->generation) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "configuration generation is stale");
		err = -ESTALE;
		goto out_put;
	}

	if (stack->generation_valid && generation == stack->generation) {
		if (!rtl931x_stack_request_matches(stack, target.port, member,
						   peer, master, flags,
						   enabled)) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "configuration generation is already committed");
			err = -EALREADY;
			goto out_put;
		}

		err = rtl931x_stack_put_reply(info, &target);
		goto out_put;
	}

	mutex_lock(&target.priv->reg_mutex);
	if (enabled && target.priv->lagmembers) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "remove all LAGs before enabling stacking");
		err = -EBUSY;
		goto out_reg_unlock;
	}

	err = rtl931x_stack_configure(target.priv, target.port, member, peer,
				       master, flags, generation, enabled,
				       info->extack);
	if (!err || stack->saved_valid)
		stack->ifindex = stack->enabled ? target.dev->ifindex : 0;

out_reg_unlock:
	mutex_unlock(&target.priv->reg_mutex);
	if (err)
		goto out_put;

	err = rtl931x_stack_put_reply(info, &target);

out_put:
	dev_put(target.dev);
out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
	return err;
}

static const struct genl_small_ops rtl931x_stack_ops[] = {
	{
		.cmd = RTL931X_STACK_CMD_GET,
		.doit = rtl931x_stack_get,
	},
	{
		.cmd = RTL931X_STACK_CMD_SET_TWO_MEMBER,
		.doit = rtl931x_stack_set_two_member,
		.flags = GENL_ADMIN_PERM,
	},
};

static struct genl_family rtl931x_stack_family __ro_after_init = {
	.name = RTL931X_STACK_GENL_NAME,
	.version = RTL931X_STACK_GENL_VERSION,
	.maxattr = RTL931X_STACK_ATTR_MAX,
	.resv_start_op = __RTL931X_STACK_CMD_MAX,
	.policy = rtl931x_stack_policy,
	.module = THIS_MODULE,
	.small_ops = rtl931x_stack_ops,
	.n_small_ops = ARRAY_SIZE(rtl931x_stack_ops),
};

void rtl931x_stack_register(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;

	INIT_LIST_HEAD(&stack->list);
	stack->state = RTL931X_STACK_STATE_DISABLED;

	mutex_lock(&rtl931x_stack_lock);
	list_add_tail(&stack->list, &rtl931x_stack_list);
	stack->registered = true;
	mutex_unlock(&rtl931x_stack_lock);
}

void rtl931x_stack_unregister(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;

	mutex_lock(&rtl931x_stack_lock);
	if (!stack->registered)
		goto out_unlock;

	mutex_lock(&priv->reg_mutex);
	if (stack->saved_valid &&
	    rtl931x_stack_configure(priv, stack->port, stack->member_id,
				    stack->peer_id, stack->master_id,
				    stack->flags, stack->generation, false,
				    NULL))
		dev_err(priv->dev, "failed to restore standalone switch state\n");
	mutex_unlock(&priv->reg_mutex);

	list_del_init(&stack->list);
	stack->registered = false;

out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
}

int rtl931x_stack_init(void)
{
	return genl_register_family(&rtl931x_stack_family);
}

void rtl931x_stack_exit(void)
{
	WARN_ON_ONCE(!list_empty(&rtl931x_stack_list));
	genl_unregister_family(&rtl931x_stack_family);
}
