// SPDX-License-Identifier: GPL-2.0-only

#include <linux/dsa/tag_rtl_otto.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/rtl838x_eth.h>
#include <linux/rtnetlink.h>

#include "rtl-otto.h"

struct rtl931x_stack_cpu {
	struct rtl838x_switch_priv *priv;
	struct net_device *dev;
	u8 member_id;
	bool active;
};

static netdev_tx_t rtl931x_stack_cpu_xmit(struct sk_buff *skb,
					  struct net_device *dev)
{
	struct rtl931x_stack_cpu *cpu = netdev_priv(dev);
	struct rtl931x_stack_context *stack = &cpu->priv->stack;
	struct rtl_otto_tagger_data *tagger_data =
		cpu->priv->ds->tagger_data;
	struct net_device *conduit = stack->talk_conduit;
	unsigned int len = skb->len;
	int err;

	if (unlikely(!tagger_data || !conduit || len < ETH_HLEN)) {
		err = -ENETDOWN;
		goto drop;
	}

	read_lock_bh(&tagger_data->cpu_device_lock);
	if (tagger_data->cpu_device_changing || !cpu->active ||
	    !tagger_data->stack_cpu_active ||
	    tagger_data->stack_cpu_dev != dev ||
	    tagger_data->cpu_device != cpu->member_id ||
	    !READ_ONCE(stack->enabled) ||
	    READ_ONCE(stack->state) != RTL931X_STACK_STATE_CONFIGURED) {
		read_unlock_bh(&tagger_data->cpu_device_lock);
		err = -EBUSY;
		goto drop;
	}

	err = rtl838x_eth_stack_cpu_xmit(conduit, skb);
	read_unlock_bh(&tagger_data->cpu_device_lock);
	if (unlikely(err))
		dev_core_stats_tx_dropped_inc(dev);
	else
		dev_sw_netstats_tx_add(dev, 1, len);

	return NETDEV_TX_OK;

drop:
	dev_kfree_skb_any(skb);
	dev_core_stats_tx_dropped_inc(dev);
	return NETDEV_TX_OK;
}

static int rtl931x_stack_cpu_open(struct net_device *dev)
{
	struct rtl931x_stack_cpu *cpu = netdev_priv(dev);
	struct rtl_otto_tagger_data *tagger_data =
		cpu->priv->ds->tagger_data;

	read_lock_bh(&tagger_data->cpu_device_lock);
	if (cpu->active && tagger_data->stack_cpu_active &&
	    tagger_data->stack_cpu_dev == dev)
		netif_start_queue(dev);
	else
		netif_stop_queue(dev);
	read_unlock_bh(&tagger_data->cpu_device_lock);

	return 0;
}

static int rtl931x_stack_cpu_stop(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

static int rtl931x_stack_cpu_get_iflink(const struct net_device *dev)
{
	struct rtl931x_stack_cpu *cpu = netdev_priv(dev);
	struct rtl931x_stack_context *stack = &cpu->priv->stack;

	return READ_ONCE(stack->ifindex) ?: stack->talk_conduit->ifindex;
}

static const struct net_device_ops rtl931x_stack_cpu_netdev_ops = {
	.ndo_open = rtl931x_stack_cpu_open,
	.ndo_stop = rtl931x_stack_cpu_stop,
	.ndo_start_xmit = rtl931x_stack_cpu_xmit,
	.ndo_set_mac_address = eth_mac_addr,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_get_iflink = rtl931x_stack_cpu_get_iflink,
};

static void rtl931x_stack_cpu_setup(struct net_device *dev)
{
	ether_setup(dev);
	dev->netdev_ops = &rtl931x_stack_cpu_netdev_ops;
	dev->pcpu_stat_type = NETDEV_PCPU_STAT_TSTATS;
	dev->priv_flags |= IFF_NO_QUEUE | IFF_DISABLE_NETPOLL;
	dev->needed_tailroom = RTL_OTTO_TAG_LEN;
}

static int rtl931x_stack_cpu_register(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl_otto_tagger_data *tagger_data = priv->ds->tagger_data;
	struct rtl931x_stack_cpu *cpu;
	struct net_device *dev;
	int err;

	ASSERT_RTNL();
	if (!stack->talk_conduit || !tagger_data)
		return -ENODEV;

	dev = alloc_netdev(sizeof(*cpu), "stack%d", NET_NAME_ENUM,
			   rtl931x_stack_cpu_setup);
	if (!dev)
		return -ENOMEM;

	cpu = netdev_priv(dev);
	cpu->priv = priv;
	cpu->dev = dev;
	cpu->member_id = stack->member_id;
	SET_NETDEV_DEV(dev, priv->dev);
	dev_net_set(dev, dev_net(stack->talk_conduit));
	dev->netns_immutable = true;
	dev->min_mtu = ETH_MIN_MTU;
	dev->max_mtu = min_t(unsigned int, ETH_DATA_LEN,
			     stack->talk_conduit->mtu);
	dev->mtu = dev->max_mtu;
	eth_hw_addr_random(dev);
	ether_addr_copy(dev->perm_addr, dev->dev_addr);
	netif_carrier_off(dev);

	err = register_netdevice(dev);
	if (err) {
		free_netdev(dev);
		return err;
	}
	netif_tx_disable(dev);

	write_lock_bh(&tagger_data->cpu_device_lock);
	tagger_data->stack_cpu_dev = dev;
	tagger_data->stack_cpu_peer_device = stack->peer_id;
	tagger_data->stack_cpu_fabric_port = stack->port;
	tagger_data->stack_cpu_active = false;
	stack->cpu = cpu;
	write_unlock_bh(&tagger_data->cpu_device_lock);

	return 0;
}

void rtl931x_stack_cpu_unregister(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl_otto_tagger_data *tagger_data = priv->ds->tagger_data;
	struct rtl931x_stack_cpu *cpu = stack->cpu;

	ASSERT_RTNL();
	if (!cpu)
		return;

	write_lock_bh(&tagger_data->cpu_device_lock);
	cpu->active = false;
	tagger_data->stack_cpu_active = false;
	tagger_data->stack_cpu_dev = NULL;
	stack->cpu = NULL;
	write_unlock_bh(&tagger_data->cpu_device_lock);

	netif_carrier_off(cpu->dev);
	netif_tx_disable(cpu->dev);
	unregister_netdevice(cpu->dev);
	free_netdev(cpu->dev);
}

void rtl931x_stack_cpu_fence(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl_otto_tagger_data *tagger_data = priv->ds->tagger_data;
	struct rtl931x_stack_cpu *cpu;

	if (!tagger_data)
		return;

	write_lock_bh(&tagger_data->cpu_device_lock);
	cpu = stack->cpu;
	if (!cpu || tagger_data->stack_cpu_dev != cpu->dev)
		goto unlock;

	cpu->active = false;
	tagger_data->stack_cpu_active = false;
	netif_carrier_off(cpu->dev);
	netif_tx_stop_all_queues(cpu->dev);
unlock:
	write_unlock_bh(&tagger_data->cpu_device_lock);
}

void rtl931x_stack_cpu_update(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl_otto_tagger_data *tagger_data = priv->ds->tagger_data;
	struct rtl931x_stack_cpu *cpu;
	bool active;

	if (!tagger_data)
		return;

	active = READ_ONCE(stack->enabled) &&
		 READ_ONCE(stack->state) == RTL931X_STACK_STATE_CONFIGURED &&
		 READ_ONCE(stack->fabric_link_up);

	write_lock_bh(&tagger_data->cpu_device_lock);
	cpu = stack->cpu;
	if (!cpu || tagger_data->stack_cpu_dev != cpu->dev)
		goto unlock;

	cpu->active = active;
	tagger_data->stack_cpu_active = active;
	if (!active) {
		netif_carrier_off(cpu->dev);
		netif_tx_stop_all_queues(cpu->dev);
		goto unlock;
	}

	netif_carrier_on(cpu->dev);
	if (netif_running(cpu->dev))
		netif_tx_wake_all_queues(cpu->dev);
unlock:
	write_unlock_bh(&tagger_data->cpu_device_lock);
}

int rtl931x_stack_cpu_sync(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	int err;

	ASSERT_RTNL();
	if (!stack->enabled ||
	    stack->state != RTL931X_STACK_STATE_CONFIGURED) {
		rtl931x_stack_cpu_unregister(priv);
		return 0;
	}

	if (stack->cpu && stack->cpu->member_id != stack->member_id)
		rtl931x_stack_cpu_unregister(priv);
	if (!stack->cpu) {
		err = rtl931x_stack_cpu_register(priv);
		if (err)
			return err;
	}

	rtl931x_stack_cpu_update(priv);
	return 0;
}
