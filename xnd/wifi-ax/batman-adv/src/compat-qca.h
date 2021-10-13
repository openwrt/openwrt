#include <linux/version.h>	/* LINUX_VERSION_CODE */
#include <linux/types.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0)

#define dev_get_iflink(_net_dev) ((_net_dev)->iflink)

#endif /* < KERNEL_VERSION(4, 1, 0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)

#include <linux/netdevice.h>

#define netdev_master_upper_dev_link(dev, upper_dev, upper_priv, upper_info, extack) ({\
	BUILD_BUG_ON(upper_priv != NULL); \
	BUILD_BUG_ON(upper_info != NULL); \
	BUILD_BUG_ON(extack != NULL); \
	netdev_master_upper_dev_link(dev, upper_dev); \
})

#elif LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)

#include <linux/netdevice.h>

#define netdev_master_upper_dev_link(dev, upper_dev, upper_priv, upper_info, extack) ({\
	BUILD_BUG_ON(extack != NULL); \
	netdev_master_upper_dev_link(dev, upper_dev, upper_priv, upper_info); \
})

#endif /* < KERNEL_VERSION(4, 5, 0) */


#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)

/* wild hack for batadv_getlink_net only */
#define get_link_net get_xstats_size || 1 ? fallback_net : (struct net*)netdev->rtnl_link_ops->get_xstats_size

#endif /* < KERNEL_VERSION(4, 0, 0) */


#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 2, 0)

struct sk_buff *skb_checksum_trimmed(struct sk_buff *skb,
				     unsigned int transport_len,
				     __sum16(*skb_chkf)(struct sk_buff *skb));

int ip_mc_check_igmp(struct sk_buff *skb);
int ipv6_mc_check_mld(struct sk_buff *skb);

#elif LINUX_VERSION_CODE < KERNEL_VERSION(5, 1, 0)

#include_next <linux/igmp.h>
#include_next <net/addrconf.h>

static inline int batadv_ip_mc_check_igmp1(struct sk_buff *skb)
{
	return ip_mc_check_igmp(skb, NULL);
}

static inline int batadv_ip_mc_check_igmp2(struct sk_buff *skb,
					   struct sk_buff **skb_trimmed)
{
	return ip_mc_check_igmp(skb, skb_trimmed);
}

#define ip_mc_check_igmp_get(_1, _2, ip_mc_check_igmp_name, ...) ip_mc_check_igmp_name
#define ip_mc_check_igmp(...) \
	ip_mc_check_igmp_get(__VA_ARGS__, batadv_ip_mc_check_igmp2, batadv_ip_mc_check_igmp1)(__VA_ARGS__)

#endif /* < KERNEL_VERSION(4, 2, 0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 3, 0)

#define IFF_NO_QUEUE	0; dev->tx_queue_len = 0

static inline bool hlist_fake(struct hlist_node *h)
{
	return h->pprev == &h->next;
}

#endif /* < KERNEL_VERSION(4, 3, 0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 6, 0)

#include <linux/ethtool.h>

#define ethtool_link_ksettings batadv_ethtool_link_ksettings

struct batadv_ethtool_link_ksettings {
	struct {
		__u32	speed;
		__u8	duplex;
		__u8    autoneg;
	} base;
};

#define __ethtool_get_link_ksettings(__dev, __link_settings) \
	batadv_ethtool_get_link_ksettings(__dev, __link_settings)

static inline int
batadv_ethtool_get_link_ksettings(struct net_device *dev,
				  struct ethtool_link_ksettings *link_ksettings)
{
	struct ethtool_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(cmd));
	ret = __ethtool_get_settings(dev, &cmd);

	if (ret != 0)
		return ret;

	link_ksettings->base.duplex = cmd.duplex;
	link_ksettings->base.speed = ethtool_cmd_speed(&cmd);
	link_ksettings->base.autoneg = cmd.autoneg;

	return 0;
}

#endif /* < KERNEL_VERSION(4, 6, 0) */


#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0)

#include_next <linux/cache.h>

/* hack for netlink.c which marked the family ops as ro */
#ifdef __ro_after_init
#undef __ro_after_init
#endif
#define __ro_after_init

#endif /* < KERNEL_VERSION(4, 10, 0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 9)

#include <linux/netdevice.h>

/* work around missing attribute needs_free_netdev and priv_destructor in
 * net_device
 */
#define ether_setup(dev) \
	void batadv_softif_free2(struct net_device *dev) \
	{ \
		batadv_softif_free(dev); \
		free_netdev(dev); \
	} \
	void (*t1)(struct net_device *dev) __attribute__((unused)); \
	bool t2 __attribute__((unused)); \
	ether_setup(dev)
#define needs_free_netdev destructor = batadv_softif_free2; t2
#define priv_destructor destructor = batadv_softif_free2; t1

#endif /* < KERNEL_VERSION(4, 11, 9) */


#if LINUX_VERSION_IS_LESS(4, 13, 0)

#define batadv_softif_validate(__tb, __data, __extack) \
	batadv_softif_validate(__tb, __data)

#define batadv_softif_newlink(__src_net, __dev, __tb, __data, __extack) \
	batadv_softif_newlink(__src_net, __dev, __tb, __data)

#endif /* LINUX_VERSION_IS_LESS(4, 13, 0) */


#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)

#define batadv_softif_slave_add(__dev, __slave_dev, __extack) \
	batadv_softif_slave_add(__dev, __slave_dev)

#endif /* < KERNEL_VERSION(4, 15, 0) */


#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 16, 0)

#ifndef sizeof_field
#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))
#endif

#endif /* < KERNEL_VERSION(4, 16, 0) */


#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)

static inline int batadv_access_ok(int type, const void __user *p,
				   unsigned long size)
{
	return access_ok(type, p, size);
}

#ifdef access_ok
#undef access_ok
#endif

#define access_ok_get(_1, _2, _3 , access_ok_name, ...) access_ok_name
#define access_ok(...) \
	access_ok_get(__VA_ARGS__, access_ok3, access_ok2)(__VA_ARGS__)

#define access_ok2(addr, size)	batadv_access_ok(VERIFY_WRITE, (addr), (size))
#define access_ok3(type, addr, size)	batadv_access_ok((type), (addr), (size))

#endif /* < KERNEL_VERSION(5, 0, 0) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)

#ifndef fallthrough
#if __GNUC__ > 7 && !defined(__CHECKER__)
# define fallthrough                    __attribute__((__fallthrough__))
#else
# define fallthrough                    do {} while (0)  /* fallthrough */
#endif
#endif

#endif /* < KERNEL_VERSION(5, 4, 0) */
