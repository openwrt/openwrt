#include <net/netfilter/nf_flow_table.h>
#include <linux/inetdevice.h>
#include <linux/platform_device.h>
#include <linux/rhashtable.h>
#include <linux/refcount.h>
#include <linux/slab.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/addrconf.h>
#include <linux/ppp_defs.h>
#include <net/ip6_tunnel.h>
#include <linux/inet.h>
#include <linux/module.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_acct.h>
#include <linux/netfilter_bridge.h>

#include <net/genetlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include <net/switchdev.h>

#include "dpns_common.h"
#include "sfxgmac-ext.h"
#include "nat.h"
#include "nat_ilkp.h"
#include "dpns_nat_genl.h"

static struct dpns_nat_priv *g_priv;
#define NAT_PRIVATE_IPV6_INFO_WR(_n, d0, d1, d2, d3)	\
	do {						\
		sf_writel(g_priv, NPU_NAT_PRIVATE_IPV6_INFO(_n) + 0x0, d0);	\
		sf_writel(g_priv, NPU_NAT_PRIVATE_IPV6_INFO(_n) + 0x4, d1);	\
		sf_writel(g_priv, NPU_NAT_PRIVATE_IPV6_INFO(_n) + 0x8, d2);	\
		sf_writel(g_priv, NPU_NAT_PRIVATE_IPV6_INFO(_n) + 0xC, d3);	\
	} while(0)

static struct flow_offload *dpns_nat_cookie_to_flow(unsigned long cookie)
{
	struct flow_offload_tuple *tuple;
	struct flow_offload_tuple_rhash *tuplehash;
	struct flow_offload *flow;

	tuple = (struct flow_offload_tuple *)cookie;
	tuplehash = container_of(tuple, struct flow_offload_tuple_rhash, tuple);
	flow = container_of(tuplehash, struct flow_offload, tuplehash[tuple->dir]);

	return flow;
}

const struct rhashtable_params dpns_nat_ht_params = {
	.head_offset = offsetof(struct dpns_nat_entry, node),
	.key_offset = offsetof(struct dpns_nat_entry, cookie),
	.key_len = sizeof_field(struct dpns_nat_entry, cookie),
	.automatic_shrinking = true,
};
static LIST_HEAD(dpns_nat_ft_cb_list);

/*
 * Flows without NAT ("LF" mode: IPv6, plain IPv4 routing). The NPU only
 * forwards them when LF lookup is enabled in SE_NAT_CONFIG1/5, which the
 * vendor driver never did on its own; without it the entries were installed
 * and the packets dropped. lf_mode 7 = full 5-tuple, i.e. per-connection
 * entries matching nf_flow_table semantics.
 *
 * IPv4 LF lookup cannot be combined with IPv4 NAT offload: once it is enabled
 * the NPU stops forwarding NAT44 flows. So by default only IPv6 LF is enabled
 * and routed (non-NAT) IPv4 flows stay on the software flowtable path.
 * lf4_offload=1 trades NAT44 offload for routed IPv4 offload (routers that do
 * no IPv4 NAT at all).
 *
 * The same holds for IPv6: with IPv6 LF lookup enabled, SNAT66 entries are
 * forwarded without translation. NAT66 flows are therefore left to software
 * unless lf_offload=0, which offloads NAT66 instead of routed IPv6.
 */
static bool lf_offload = true;
module_param(lf_offload, bool, 0444);
MODULE_PARM_DESC(lf_offload, "Offload routed IPv6 (no NAT); disables NAT66 offload, default on");

static bool lf4_offload;
module_param(lf4_offload, bool, 0444);
MODULE_PARM_DESC(lf4_offload, "Offload routed IPv4 (no NAT); disables NAT44 offload, default off");

static void dpns_nat_lf_init(struct dpns_nat_priv *priv)
{
	struct nat_genl_msg msg = { .is_lf = true, .lf_mode = 7 };

	if (lf_offload) {
		msg.is_v6_mode = true;
		dpns_nat_mode_set(priv, &msg);
	}
	if (lf4_offload) {
		msg.is_v6_mode = false;
		dpns_nat_mode_set(priv, &msg);
		pr_warn("dpns_nat: routed IPv4 offload enabled, NAT44 flows will not be offloaded\n");
	}
}

/*
 * One context per nf_flowtable flow block, shared by the per-port block
 * callbacks of that flowtable. Entries remember their block, so that when the
 * flowtable goes away (e.g. fw4 reload replaces the nftables table) its NPU
 * entries can be removed: the kernel unbinds the block before it tears down
 * the offloaded flows, so the FLOW_CLS_DESTROY commands never reach us.
 */
struct dpns_nat_block {
	struct list_head list;
	struct flow_block *block;
	struct dpns_nat_priv *priv;
	unsigned int users;
};

static LIST_HEAD(dpns_nat_blocks);
static DEFINE_MUTEX(dpns_nat_blocks_lock);

extern struct dpns_nat_subnet_info sf_lan_subnet[8];
extern struct dpns_nat_subnet_info sf_wan_subnet[8];

/*
 * Clear the valid bit of WAN table entry i. Entries are 18 bits wide and packed
 * back to back, so an entry can span two 32-bit registers; the valid bit
 * (WAN0_VLD, bit 17 of the entry) is at absolute bit i * 18 + 17.
 */
static void dpns_nat_wan_entry_invalidate(struct dpns_nat_priv *priv, int i)
{
	u32 pos = i * 18 + 17;

	sf_update(priv, WAN_TB_DATA(pos / 32), BIT(pos % 32), 0);
}

u16 crc16_custom(const u8 *buf, size_t len, u8 poly_sel)
{
	static const u16 poly[] = {
		0x1021, 0x8005, 0xA097, 0x8BB7, 0xC867, 0x3D65, 0x0589, 0x509D,
	};
	u16 crc = 0, arith = poly[poly_sel];
	int i;

	while(len--) {
		crc  = crc ^ (*buf++ << 8);
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x8000)
				crc = (crc << 1) ^ arith;
			else
				crc = crc << 1;
		}
	}

	return crc;
}

void dpns_nat_wait_rw(struct dpns_nat_priv *priv)
{
	unsigned long timeout = jiffies + HZ;

	do {
		if (!(sf_readl(priv, SE_NAT_TB_OP) & NAT_TB_OP_BUSY))
			return;

		cond_resched();
	} while (time_after(timeout, jiffies));

	NAT_DBG(ERR_LV, "timed out\n");
}

void dpns_nat_wait_lkp(struct dpns_nat_priv *priv)
{
	unsigned long timeout = jiffies + HZ;

	do {
		if (!(sf_readl(priv, SE_NAT_LKP_REQ) & BIT(0)))
			return;

		cond_resched();
	} while (time_after(timeout, jiffies));

	if (!(sf_readl(priv, SE_NAT_LKP_REQ) & BIT(0)))
		return;

	NAT_DBG(ERR_LV, "lkp timed out\n");
}

static void dpns_nat_mangle_eth(const struct flow_action_entry *act, void *eth)
{
	void *dest = eth + act->mangle.offset;
	const void *src = &act->mangle.val;

	if (act->mangle.offset > 8)
		return;

	if (act->mangle.mask == 0xffff) {
		src += 2;
		dest += 2;
	}

	memcpy(dest, src, act->mangle.mask ? 2 : 4);
}

static int dpns_nat_mangle_ipv4(const struct flow_action_entry *act, bool is_dnat, struct nat_ipv4_data *tb)
{
	if (!is_dnat) {
		tb->router_ip = ntohl(act->mangle.val);
	} else {
		tb->private_ip = ntohl(act->mangle.val);
	}
	return 0;
}

static void dpns_nat_add_private_ipv4(u32 ip, u8 masklen, u32 index)
{
	struct dpns_nat_priv *priv = g_priv;
	u64 reg = sf_readq(priv, NPU_NAT_IPV4_MASK_LEN0123);

	reg |= (u64)(NPU_NAT_IPV4_MASK_LEN_EN | masklen) << 8 * index;
	sf_writeq(priv, NPU_NAT_IPV4_MASK_LEN0123, reg);
	sf_writel(priv, NPU_NAT_PRIVATE_IPV4_INFO(index), ip);
}

static void dpns_nat_del_private_ipv4(u32 ip, u8 masklen, u32 index)
{
	struct dpns_nat_priv *priv = g_priv;
	u64 reg = sf_readq(priv, NPU_NAT_IPV4_MASK_LEN0123);

	reg &= ~(0xffULL << 8 * index);
	sf_writeq(priv, NPU_NAT_IPV4_MASK_LEN0123, reg);
	sf_writel(priv, NPU_NAT_PRIVATE_IPV4_INFO(index), 0);
}

static void dpns_nat_add_private_ipv6(struct in6_addr ip6, u8 masklen, u32 index)
{
	struct dpns_nat_priv *priv = g_priv;
	u32 reg67, data[4];
	int i;

	for (i = 0; i < 4; i++)
		data[i] = ntohl(ip6.s6_addr32[3 - i]);

	reg67 = sf_readl(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE);

	switch (index) {
	case 6:
		reg67 &= ~NAT_IPV6_MASK_LEN7;
		reg67 |= FIELD_PREP(NAT_IPV6_MASK_LEN7, NPU_NAT_IPV6_MASK_LEN_EN | masklen);
		sf_writel(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE, reg67);
		break;
	case 7:
		reg67 &= ~NAT_IPV6_MASK_LEN6;
		reg67 |= FIELD_PREP(NAT_IPV6_MASK_LEN6, NPU_NAT_IPV6_MASK_LEN_EN | masklen);
		sf_writel(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE, reg67);
		break;
	default:
		sf_writew(priv, NPU_NAT_IPV6_MASK_LEN(index), NPU_NAT_IPV6_MASK_LEN_EN | masklen);
	}

	NAT_PRIVATE_IPV6_INFO_WR(index, data[0], data[1], data[2], data[3]);
}

static void dpns_nat_del_private_ipv6(struct in6_addr ip6, u8 masklen, u32 index)
{
	struct dpns_nat_priv *priv = g_priv;
	u32 reg67;

	reg67 = sf_readl(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE);

	switch (index) {
	case 6:
		reg67 &= ~NAT_IPV6_MASK_LEN7;
		sf_writel(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE, reg67);
		break;
	case 7:
		reg67 &= ~NAT_IPV6_MASK_LEN6;
		sf_writel(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE, reg67);
		break;
	default:
		sf_writew(priv, NPU_NAT_IPV6_MASK_LEN(index), 0);
	}

	NAT_PRIVATE_IPV6_INFO_WR(index, 0, 0, 0, 0);
}

static bool is_pppoe_wan(char *devname)
{
	if (!strncmp("pppoe-wan", devname, strlen("pppoe-wan")))
		return true;

	return false;
}

static int dpns_nat_inetaddr_event(struct notifier_block *nb,
				   unsigned long event, void *ptr)
{
	struct dpns_nat_priv *priv = g_priv;
	struct in_ifaddr *ifa = ptr;
	struct net_device *dev = ifa->ifa_dev->dev;
	struct net_device *real_dev = dev;
	struct net_device_path_stack stack;
	struct net_device_path *path;
	COMMON_t *cpriv = priv->cpriv;
	u8 masklen, port_id;
	u16 vlan_id = DPA_UNTAGGED_VID;
	u32 ip, i, reg_offset, bit_offset;
	u64 reg, update_info;
	int ret;
	bool is_pppoewan;
	char pppoe_name[IFNAMSIZ];

	ip = ntohl(ifa->ifa_local);
	/* masklen is the number of trailing 0s, e.g. 255.255.255.0 -> 8 */
	masklen = __builtin_ctz(ntohl(ifa->ifa_mask));
	NAT_DBG(DBG_LV, "IPv4 dev:%s EVENT:%ld ip:%pI4 masklen:%u\n",
			real_dev->name, event, &ip, masklen);

	/* update lan subnet info */
	for (i = 0; i < 8; i++) {
		if (strncmp(sf_lan_subnet[i].ifname, real_dev->name, IFNAMSIZ))
			continue;

		switch (event) {
		case NETDEV_UP:
			dpns_nat_add_private_ipv4(ip, masklen, i);

			sf_lan_subnet[i].v4.ip = ip;
			sf_lan_subnet[i].v4.masklen = masklen;
			sf_lan_subnet[i].v4.valid = true;
			NAT_DBG(DBG_LV, "added %pI4/%pI4 to private IPv4 index %u\n",
					&ifa->ifa_local, &ifa->ifa_mask, i);
			return NOTIFY_OK;
		case NETDEV_DOWN:
			dpns_nat_del_private_ipv4(ip, masklen, i);

			sf_lan_subnet[i].v4.ip = 0;
			sf_lan_subnet[i].v4.masklen = 0;
			sf_lan_subnet[i].v4.valid = false;
			NAT_DBG(DBG_LV, "deleted %pI4/%pI4 from private IPv4\n",
					&ifa->ifa_local, &ifa->ifa_mask);
			return NOTIFY_OK;
		}
	}

	is_pppoewan = is_pppoe_wan(real_dev->name);
	if (is_pppoewan) {
		if (dpns_fill_forward_path(real_dev, real_dev->dev_addr, &stack))
			return NOTIFY_OK;

		for (i = 0; i < stack.num_paths; i++) {
			path = &stack.path[i];
			switch (path->type) {
			case DEV_PATH_VLAN:
				vlan_id = vlan_dev_vlan_id(path->dev);
				break;
			case DEV_PATH_ETHERNET:
				real_dev = (struct net_device *)path->dev;
				NAT_DBG(DBG_LV, "get pppoe real dev %s vid:%u\n",
						real_dev->name, vlan_id);
				break;
			default:
				break;
			}
		}
	}

	if (is_pppoewan == true && (vlan_id != DPA_UNTAGGED_VID))
		snprintf(pppoe_name, sizeof(pppoe_name), "%s.%u", real_dev->name, vlan_id);
	else if (is_pppoewan == true && (vlan_id == DPA_UNTAGGED_VID))
		strcpy(pppoe_name, real_dev->name);

	ret = cpriv->port_id_by_netdev(cpriv, real_dev, &port_id);
	if (ret)
		return -EOPNOTSUPP;

	if (is_vlan_dev(real_dev))
		vlan_id = vlan_dev_vlan_id(real_dev);

	/* update wan subnet info
	 * info include iport:bit[4:0] vlan_id:bit[16:5] valid:bit[17]
	 * */
	for (i = 0; i < 8; i++) {
		if (is_pppoewan) {
			if (strncmp(sf_wan_subnet[i].ifname, pppoe_name, IFNAMSIZ))
				continue;
		} else {
			if (strncmp(sf_wan_subnet[i].ifname, dev->name, IFNAMSIZ))
				continue;
		}

		switch (event) {
		case NETDEV_UP:
			update_info = FIELD_PREP(WAN0_IPORT, port_id) |
				FIELD_PREP(WAN0_VLAN_ID, vlan_id) | WAN0_VLD;

			reg_offset = (i * 18) / 32;
			bit_offset = (i * 18) % 32;
			reg = sf_readl(priv, WAN_TB_DATA(reg_offset));
			reg |= (u64)sf_readl(priv, WAN_TB_DATA(reg_offset + 1)) << 32;

			reg &= ~(0x3ffffULL << bit_offset);
			reg |= update_info << bit_offset;
			sf_writel(priv, WAN_TB_DATA(reg_offset), reg & 0xFFFFFFFF);
			sf_writel(priv, WAN_TB_DATA(reg_offset + 1), reg >> 32);

			sf_wan_subnet[i].v4.ip = ip;
			sf_wan_subnet[i].v4.masklen = masklen;
			sf_wan_subnet[i].v4.valid = true;
			NAT_DBG(DBG_LV, "added %pI4/%pI4 to private IPv4 index %u\n",
					&ifa->ifa_local, &ifa->ifa_mask, i);
			return NOTIFY_OK;
		case NETDEV_DOWN:
			dpns_nat_wan_entry_invalidate(priv, i);

			sf_wan_subnet[i].v4.ip = 0;
			sf_wan_subnet[i].v4.masklen = 0;
			sf_wan_subnet[i].v4.valid = false;
			NAT_DBG(DBG_LV, "deleted %pI4/%pI4 from private IPv4\n",
					&ifa->ifa_local, &ifa->ifa_mask);
			return NOTIFY_OK;
		}
	}

	return NOTIFY_DONE;
}

static int dpns_nat_inet6addr_event(struct notifier_block *nb,
				   unsigned long event, void *ptr)
{
	struct dpns_nat_priv *priv = g_priv;
	struct inet6_ifaddr *ifa = (struct inet6_ifaddr *) ptr;
	struct net_device *dev = ifa->idev->dev;
	struct net_device *real_dev = dev;
	struct net_device_path_stack stack;
	struct net_device_path *path;
	COMMON_t *cpriv = priv->cpriv;
	u8 masklen, port_id;
	u16 vlan_id = DPA_UNTAGGED_VID;
	u32 i, reg_offset, bit_offset;
	u64 reg, update_info;
	int ret;
	bool is_pppoewan;
	char pppoe_name[IFNAMSIZ];

	masklen = 128 - ifa->prefix_len;

	/* link-local addresses are not routed subnets; one slot per interface */
	if (ipv6_addr_type(&ifa->addr) & IPV6_ADDR_LINKLOCAL)
		return NOTIFY_DONE;

	NAT_DBG(DBG_LV, "IPv6 dev:%s EVENT:%ld ip:%pI6 masklen:%u\n",
			real_dev->name, event, &ifa->addr, masklen);
	/* update lan subnet info
	 * dpns support max 8ipv4 + 8ipv6 lan subnet
	 * */
	for (i = 0; i < 8; i++) {
		if (strncmp(sf_lan_subnet[i].ifname, real_dev->name, IFNAMSIZ))
			continue;

		switch (event) {
		case NETDEV_UP:
			dpns_nat_add_private_ipv6(ifa->addr, masklen, i);

			memcpy(sf_lan_subnet[i].v6.ip, ifa->addr.s6_addr, sizeof(struct in6_addr));
			sf_lan_subnet[i].v6.masklen = masklen;
			sf_lan_subnet[i].v6.valid = true;
			NAT_DBG(DBG_LV, "added %pI6, EVENT is UP\n", &ifa->addr);
			return NOTIFY_OK;
		case NETDEV_DOWN:
			dpns_nat_del_private_ipv6(ifa->addr, masklen, i);

			memset(sf_lan_subnet[i].v6.ip, 0, sizeof(struct in6_addr));
			sf_lan_subnet[i].v6.masklen = 0;
			sf_lan_subnet[i].v6.valid = false;
			NAT_DBG(DBG_LV, "deleted %pI6, EVENT is DOWN!\n", &ifa->addr);
			return NOTIFY_OK;
		}
	}

	is_pppoewan = is_pppoe_wan(real_dev->name);
	if (is_pppoewan) {
		if (dpns_fill_forward_path(real_dev, real_dev->dev_addr, &stack))
			return NOTIFY_OK;

		for (i = 0; i < stack.num_paths; i++) {
			path = &stack.path[i];
			switch (path->type) {
			case DEV_PATH_VLAN:
				vlan_id = vlan_dev_vlan_id(path->dev);
				break;
			case DEV_PATH_ETHERNET:
				real_dev = (struct net_device *)path->dev;
				NAT_DBG(DBG_LV, "get pppoe real dev %s vid:%u\n",
						real_dev->name, vlan_id);
				break;
			default:
				break;
			}
		}
	}

	if (is_pppoewan == true && (vlan_id != DPA_UNTAGGED_VID))
		snprintf(pppoe_name, sizeof(pppoe_name), "%s.%u", real_dev->name, vlan_id);
	else if (is_pppoewan == true && (vlan_id == DPA_UNTAGGED_VID))
		strcpy(pppoe_name, real_dev->name);

	ret = cpriv->port_id_by_netdev(cpriv, real_dev, &port_id);
	if (ret)
		return -EOPNOTSUPP;

	if (is_vlan_dev(real_dev))
		vlan_id = vlan_dev_vlan_id(real_dev);

	/* update wan subnet info
	 * info include iport:bit[4:0] vlan_id:bit[16:5] valid:bit[17]
	 * */
	for (i = 0; i < 8; i++) {
		if (is_pppoewan) {
			if (strncmp(sf_wan_subnet[i].ifname, pppoe_name, IFNAMSIZ))
				continue;
		} else {
			if (strncmp(sf_wan_subnet[i].ifname, dev->name, IFNAMSIZ))
				continue;
		}

		switch (event) {
		case NETDEV_UP:
			update_info = FIELD_PREP(WAN0_IPORT, port_id) |
				FIELD_PREP(WAN0_VLAN_ID, vlan_id) | WAN0_VLD;

			reg_offset = ((i + 8) * 18) / 32;
			bit_offset = ((i + 8) * 18) % 32;
			reg = sf_readl(priv, WAN_TB_DATA(reg_offset));
			reg |= (u64)sf_readl(priv, WAN_TB_DATA(reg_offset + 1)) << 32;

			reg &= ~(0x3ffffULL << bit_offset);
			reg |= update_info << bit_offset;
			sf_writel(priv, WAN_TB_DATA(reg_offset), reg & 0xFFFFFFFF);
			sf_writel(priv, WAN_TB_DATA(reg_offset + 1), reg >> 32);

			memcpy(sf_wan_subnet[i].v6.ip, ifa->addr.s6_addr, sizeof(struct in6_addr));
			sf_wan_subnet[i].v6.masklen = masklen;
			sf_wan_subnet[i].v6.valid = true;
			return NOTIFY_OK;
		case NETDEV_DOWN:
			/* IPv6 WAN entries follow the 8 IPv4 ones */
			dpns_nat_wan_entry_invalidate(priv, i + 8);

			memset(sf_wan_subnet[i].v6.ip, 0, sizeof(struct in6_addr));
			sf_wan_subnet[i].v6.masklen = 0;
			sf_wan_subnet[i].v6.valid = false;
			return NOTIFY_OK;
		}
	}

	return NOTIFY_DONE;
}

/*
 * The WAN subnet table is programmed from inetaddr events, which only cover
 * addresses added after the WAN interface name was configured. Replay the
 * IPv4 addresses that already exist on non-LAN devices so that the order of
 * "interface gets an address" and "interface is declared WAN" does not matter.
 */
void dpns_nat_wan_sync(void)
{
	struct net_device *dev;
	struct in_device *in_dev;
	struct in_ifaddr *ifa;
	bool is_lan;
	int i;

	rtnl_lock();
	for_each_netdev(&init_net, dev) {
		is_lan = false;
		for (i = 0; i < 8; i++)
			if (!strncmp(sf_lan_subnet[i].ifname, dev->name, IFNAMSIZ))
				is_lan = true;
		if (is_lan)
			continue;

		in_dev = __in_dev_get_rtnl(dev);
		if (!in_dev)
			continue;

		in_dev_for_each_ifa_rtnl(ifa, in_dev)
			dpns_nat_inetaddr_event(NULL, NETDEV_UP, ifa);
	}
	rtnl_unlock();
}

/* Withdraw WAN table entry index (before it is renamed or cleared). */
void dpns_nat_wan_clear(int index)
{
	if (!g_priv || index < 0 || index > 7)
		return;

	dpns_nat_wan_entry_invalidate(g_priv, index);
	dpns_nat_wan_entry_invalidate(g_priv, index + 8);
	memset(&sf_wan_subnet[index].v4, 0, sizeof(sf_wan_subnet[index].v4));
	memset(&sf_wan_subnet[index].v6, 0, sizeof(sf_wan_subnet[index].v6));
}

static struct notifier_block dpns_nat_inetaddr_notifier = {
	.notifier_call	= dpns_nat_inetaddr_event,
};

static struct notifier_block dpns_nat_inet6addr_notifier = {
	.notifier_call	= dpns_nat_inet6addr_event,
};

static void dpns_nat_init_subnet_info(struct dpns_nat_priv *priv)
{
	/* init lan subnet info */
	sprintf(sf_lan_subnet[0].ifname, "br-lan");
	sprintf(sf_lan_subnet[1].ifname, "br0");
	sprintf(sf_lan_subnet[2].ifname, "br1");
	sprintf(sf_lan_subnet[3].ifname, "br2");
	sprintf(sf_lan_subnet[4].ifname, "br3");
	sprintf(sf_lan_subnet[5].ifname, "br4");
}

void dpns_nat_hnat_tuple_set(u16 hnat_mode, struct nat_hash_tuple *tuple, bool is_dnat)
{
	switch (hnat_mode) {
	case NPU_HNAT_MODE_BASIC:
		if (!is_dnat) {
			tuple->l4_type = 0;
			tuple->sport = 0;
			tuple->dipv4 = 0;
			tuple->dipv6 = (struct in6_addr){0};
			tuple->dport = 0;
		} else {
			tuple->l4_type = 0;
			tuple->dport = 0;
			tuple->sipv4 = 0;
			tuple->sipv6 = (struct in6_addr){0};
			tuple->sport = 0;
		}
		break;
	case NPU_HNAT_MODE_SYMMETRIC:
		break;
	case NPU_HNAT_MODE_FULLCONE:
		if (!is_dnat) {
			tuple->dipv4 = 0;
			tuple->dipv6 = (struct in6_addr){0};
			tuple->dport = 0;
		} else {
			tuple->sipv4 = 0;
			tuple->sipv6 = (struct in6_addr){0};
			tuple->sport = 0;
		}
		break;
	case NPU_HNAT_MODE_HOST_RESTRICTED:
		if (!is_dnat) {
			tuple->dipv4 = 0;
			tuple->dipv6 = (struct in6_addr){0};
			tuple->dport = 0;
		} else {
			tuple->sport = 0;
		}
		break;
	case NPU_HNAT_MODE_PORT_RESTRICTED:
		if (!is_dnat) {
			tuple->dipv4 = 0;
			tuple->dipv6 = (struct in6_addr){0};
			tuple->dport = 0;
		}
		break;
	}
}

void dpns_nat_offload_tuple_set(u16 l2offload_mode, struct nat_hash_tuple *tuple)
{
	switch (l2offload_mode) {
	case 0:
		tuple->l4_type = 0;
		tuple->sport = 0;
		tuple->sipv4 = 0;
		tuple->sipv6 = (struct in6_addr){0};
		tuple->dport = 0;
		break;
	case 1:
		tuple->sport = 0;
		tuple->sipv4 = 0;
		tuple->sipv6 = (struct in6_addr){0};
		break;
	case 2:
		tuple->l4_type = 0;
		tuple->sport = 0;
		tuple->dport = 0;
		tuple->dipv4 = 0;
		tuple->dipv6 = (struct in6_addr){0};
		break;
	case 3:
		tuple->dport = 0;
		tuple->dipv4 = 0;
		tuple->dipv6 = (struct in6_addr){0};
		break;
	case 4:
		tuple->l4_type = 0;
		tuple->sport = 0;
		tuple->dport = 0;
		break;
	case 5:
		tuple->sport = 0;
		tuple->dport = 0;
		break;
	case 6:
		tuple->sport = 0;
		break;
	case 7:
		break;
	}
}

/* different HNAT modes(UDP or TCP) or LF modes use different 5-tuple to calculate */
static int dpns_nat_tuple_set(bool is_lf, bool is_dnat, bool is_ipv6,
					struct nat_hash_tuple *tuple)
{
	struct dpns_nat_priv *priv = g_priv;
	u32 reg_val, hnat_mode, lf_mode;

	/* HNAT mode set: v4 or v6, udp or tcp */
	if (!is_lf) {
		if (tuple->l4_type == 1) {
			reg_val = sf_readl(priv, SE_NAT_CONFIG0);
			hnat_mode = FIELD_GET(NAT_CONFIG0_UDP_HNAT_MODE, reg_val);
		} else {
			reg_val = sf_readl(priv, SE_NAT_CONFIG1);
			hnat_mode = FIELD_GET(NAT_CONFIG1_TCP_HNAT_MODE, reg_val);
		}
		dpns_nat_hnat_tuple_set(hnat_mode, tuple, is_dnat);
	}

	/* LF mode set: v4 or v6 */
	if (is_lf) {
		if (is_ipv6) {
			reg_val = sf_readl(priv, SE_NAT_CONFIG5);
			lf_mode = FIELD_GET(NAT_CONFIG5_V6LF_MODE, reg_val);
		} else {
			reg_val = sf_readl(priv, SE_NAT_CONFIG1);
			lf_mode = FIELD_GET(NAT_CONFIG1_V4LF_MODE, reg_val);
		}
		dpns_nat_offload_tuple_set(lf_mode, tuple);
	}
	return 0;
}

/*
 * Hardware flow counters. A NAPT entry can be tied to one of the counter pairs in
 * the NPU MIB RAM (stat_en/stat_index). With NAT_CONFIG5 MIB mode 1, counter
 * index * 2 counts the packets of the DNAT direction and index * 2 + 1 those of the
 * SNAT direction. Reading a packet counter clears it and latches the byte count
 * of the same counter (Ethernet frame without FCS). Both directions of a flow
 * share one NAPT entry, so they share one counter pair.
 */
struct dpns_nat_mib {
	refcount_t ref;
	u16 index;
};

/* caller holds mib_lock */
static void dpns_nat_mib_read(struct dpns_nat_priv *priv, unsigned int counter,
			      u64 *pkts, u64 *bytes)
{
	*pkts = sf_readl(priv, NPU_MIB_ADDR + counter * 4);
	*bytes = sf_readl(priv, NPU_MIB_BYTES_LO) |
		 (u64)sf_readl(priv, NPU_MIB_BYTES_HI) << 32;
}

/* Take the counter pair of the other direction of @flow, or a free one. */
static void dpns_nat_mib_get(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
			     struct flow_offload *flow, bool dir)
{
	unsigned long other_cookie = (unsigned long)&flow->tuplehash[!dir].tuple;
	struct dpns_nat_mib *mib = NULL, *new;
	struct dpns_nat_entry *other;
	unsigned int index;
	u64 pkts, bytes;

	new = kzalloc(sizeof(*new), GFP_KERNEL);

	rcu_read_lock();
	spin_lock_bh(&priv->mib_lock);
	other = rhashtable_lookup(&priv->flow_table, &other_cookie, dpns_nat_ht_params);
	if (other && other->mib && refcount_inc_not_zero(&other->mib->ref)) {
		mib = other->mib;
	} else if (new) {
		index = find_first_zero_bit(priv->mib_bitmap, DPNS_NAT_MIB_NUM);
		if (index < DPNS_NAT_MIB_NUM) {
			set_bit(index, priv->mib_bitmap);
			/* drop what the previous user of the pair left */
			dpns_nat_mib_read(priv, index * 2, &pkts, &bytes);
			dpns_nat_mib_read(priv, index * 2 + 1, &pkts, &bytes);
			refcount_set(&new->ref, 1);
			new->index = index;
			mib = new;
			new = NULL;
		}
	}
	entry->mib = mib;
	spin_unlock_bh(&priv->mib_lock);
	rcu_read_unlock();
	kfree(new);
}

static void dpns_nat_mib_put(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry)
{
	struct dpns_nat_mib *mib;

	spin_lock_bh(&priv->mib_lock);
	mib = entry->mib;
	entry->mib = NULL;
	if (mib && refcount_dec_and_test(&mib->ref))
		clear_bit(mib->index, priv->mib_bitmap);
	else
		mib = NULL;
	spin_unlock_bh(&priv->mib_lock);
	kfree(mib);
}

static int dpns_nat_add_ipv4(struct dpns_nat_priv *priv, struct flow_cls_offload *f,
			     bool is_dnat, struct flow_offload *flow,
			     bool is_lf, bool is_lf_reply)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct nat_ipv4_data tb = {};
	struct nat_hash_tuple tuple = {};
	struct flow_action_entry *act;
	struct ethhdr ethhdr = {};
	VLAN_t *vlan_priv = priv->cpriv->vlan_priv;
	struct vlan_vport_entry *pos;
	MAC_t *mac_priv = priv->cpriv->mac_priv;
	COMMON_t *cpriv = priv->cpriv;
	dpns_port_t *p;
	struct ipv6hdr ip6_hdr = {};
	struct dpns_nat_entry *entry = NULL;
	u64 port_bitmap;
	u32 result_data[2];
	u16 dmac_index;

	u16 vlan_id = DPA_UNTAGGED_VID, vid = 0;

	int ret, i, hit;
	u64 pppoe_hdr = 0, ip6_head[6] = {0};
	bool pppoe_en = is_dnat, tunnel_en = false;
	bool wan_flag = false;

	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_MANGLE:
			switch (act->mangle.htype) {
			case FLOW_ACT_MANGLE_HDR_TYPE_ETH:
				dpns_nat_mangle_eth(act, &ethhdr);
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_TCP:
			case FLOW_ACT_MANGLE_HDR_TYPE_UDP:
				if (!is_dnat)
					tb.router_port = ntohl(act->mangle.val) >> 16;
				else
					tb.private_port = (u16)ntohl(act->mangle.val);
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_IP4:
				ret = dpns_nat_mangle_ipv4(act, is_dnat, &tb);
				if (ret < 0)
					return ret;
				break;
			default:
				return -EOPNOTSUPP;
			}
			break;
		case FLOW_ACTION_REDIRECT: {
			if (act->dev->type == ARPHRD_TUNNEL6) {
				struct ip6_tnl *priv = netdev_priv(act->dev);
				struct net_device *netd;

				wan_flag = true;
				tunnel_en = true;
				netd = dev_get_by_index(priv->net, priv->parms.link);
				if (!netd)
					return -EINVAL;

				p = cpriv->port_by_netdev(cpriv, act->dev);
				if (!p)
					return -EOPNOTSUPP;

				if (!is_dnat)
					tb.soport_id = p->port_id;
				else
					tb.doport_id = p->port_id;

				ip6_hdr.saddr = priv->parms.laddr;
				ip6_hdr.daddr = priv->parms.raddr;
				ip6_hdr.hop_limit = priv->parms.hop_limit;
				ip6_hdr.flow_lbl[0] = ((u8 *)&priv->parms.flowinfo)[1];
				ip6_hdr.flow_lbl[1] = ((u8 *)&priv->parms.flowinfo)[2];
				ip6_hdr.flow_lbl[2] = ((u8 *)&priv->parms.flowinfo)[3];
				ip6_hdr.version = 6;
				ip6_hdr.nexthdr = IPPROTO_IPIP;

				dev_put(netd);
				break;
			} else {
				if(netif_is_bridge_master(act->dev))
					return -EINVAL;

				p = cpriv->port_by_netdev(cpriv, act->dev);
				if (!p)
					return -EOPNOTSUPP;

				/* FIXME: xgmac gets wrong dev */
				netdev_dbg(act->dev, "name: %s, oport id %u, is it corrupt?\n",
						 act->dev->name, p->port_id);

				if (!is_dnat)
					tb.soport_id = p->port_id;
				else
					tb.doport_id = p->port_id;
				break;
			}
		}
		case FLOW_ACTION_PPPOE_PUSH://write later
			wan_flag = true;
			pppoe_en = 1;
			pppoe_hdr = (0x1100ULL << 48) | ((u64)act->pppoe.sid << 32) | PPP_IP;

			break;
		case FLOW_ACTION_TUNNEL_ENCAP:/* LF mode only */
			if (act->tunnel->key.tun_id & IP_TUNNEL_INFO_IPV6) {
				tunnel_en = true;
				ip6_hdr.saddr = act->tunnel->key.u.ipv6.src;
				ip6_hdr.daddr = act->tunnel->key.u.ipv6.dst;
				ip6_hdr.flow_lbl[0] = ((u8 *)&act->tunnel->key.label)[1];
				ip6_hdr.flow_lbl[1] = ((u8 *)&act->tunnel->key.label)[2];
				ip6_hdr.flow_lbl[2] = ((u8 *)&act->tunnel->key.label)[3];
				ip6_hdr.hop_limit = act->tunnel->key.ttl;
				ip6_hdr.priority = act->tunnel->key.tos;
				ip6_hdr.version = 6;
				ip6_hdr.nexthdr = IPPROTO_IPIP;
			}
			break;
		case FLOW_ACTION_TUNNEL_DECAP:
			tunnel_en = true;
			break;
		case FLOW_ACTION_VLAN_PUSH:
			vlan_id = act->vlan.vid;
			vid = vlan_id;
			break;
		case FLOW_ACTION_CSUM:
		case FLOW_ACTION_VLAN_POP:
			break;
		default:
			return -EOPNOTSUPP;
		}
	}

	spin_lock_bh(&vlan_priv->vport_lock);
	list_for_each_entry(pos, &vlan_priv->vport_list, node) {
		if (pos->vlan_id == vlan_id && pos->port == tb.soport_id) {
			tb.soport_id = pos->vport;
			break;
		}
	}
	list_for_each_entry(pos, &vlan_priv->vport_list, node) {
		if (pos->vlan_id == vlan_id && pos->port == tb.doport_id) {
			tb.doport_id = pos->vport;
			break;
		}
	}
	spin_unlock_bh(&vlan_priv->vport_lock);

	if (!wan_flag) {
		pppoe_en = true;
		tunnel_en = true;
	}

	/* It's possible that only one of IP or port is changed */
	if (!is_dnat) {
		if (!tb.router_ip) {
			NAT_DBG(DBG_LV, "source ip not changed after snat\n");
			tb.router_ip = tb.private_ip;
		}
		if (!tb.router_port) {
			NAT_DBG(DBG_LV, "source port not changed after snat\n");
			tb.router_port = tb.private_port;
		}
	} else {
		if (!tb.private_ip) {
			NAT_DBG(DBG_LV, "dest ip not changed after dnat\n");
			tb.private_ip = tb.router_ip;
		}
		if (!tb.private_port) {
			NAT_DBG(DBG_LV, "dest port not changed after dnat\n");
			tb.private_port = tb.router_port;
		}
	}

	NAT_DBG(DBG_LV, "after nat: dest mac: %pM, src mac: %pM\n", ethhdr.h_dest, ethhdr.h_source);
	if (is_zero_ether_addr(ethhdr.h_dest) ||
	    is_zero_ether_addr(ethhdr.h_source))
		return -EINVAL;

	ip6_head[0] = 0;
	ip6_head[1] = be64_to_cpu(((u64 *)&ip6_hdr)[4]);
	ip6_head[2] = be64_to_cpu(((u64 *)&ip6_hdr)[3]);
	ip6_head[3] = be64_to_cpu(((u64 *)&ip6_hdr)[2]);
	ip6_head[4] = be64_to_cpu(((u64 *)&ip6_hdr)[1]);
	ip6_head[5] = be64_to_cpu(((u64 *)&ip6_hdr)[0]);

	if (!is_dnat) {
		hit = mac_priv->hw_search(mac_priv, ethhdr.h_dest, vlan_id, result_data);
		NAT_DBG(DBG_LV,"this is the snat ipv4 mac add!\n");
		NAT_DBG(DBG_LV, "the adding mac:%pM vlan_id:%d\n", ethhdr.h_dest, vlan_id);

		port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26,
				result_data[1]) <<19 |
				FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
				result_data[0]);

		if (!hit || ( hit && fls64(port_bitmap)-1 != tb.soport_id )) {
			NAT_DBG(DBG_LV, "pubmac_index add failed!\n");
			return -ENOSPC;
		} else {
			dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
			tb.pubmac_index = dmac_index;
		}

		//add srtmac_index , there is a search process in the process of adding
		ret = priv->cpriv->intf_add(priv->cpriv, vlan_id, pppoe_en, tunnel_en, wan_flag, ethhdr.h_source);
		if (ret < 0){
			NAT_DBG(ERR_LV, "srtmac_index not found! add failed\n");
			return -ENOSPC;
		} else {
			/* ip6 head for 4in6 mode use the same memory as pppoe_hdr */
			tb.srtmac_index = ret;
			if (pppoe_en) {
				priv->cpriv->table_write(priv->cpriv,
					16, tb.srtmac_index, (u32 *)&pppoe_hdr, sizeof(pppoe_hdr));
			} else {
				priv->cpriv->table_write(priv->cpriv,
					16, tb.srtmac_index, (u32 *)&ip6_head, sizeof(ip6_head));
			}
		}
	} else {//DNAT
		hit = mac_priv->hw_search(mac_priv, ethhdr.h_dest, vlan_id, result_data);
		NAT_DBG(DBG_LV,"this is the dnat ipv4 mac add!\n");
		NAT_DBG(DBG_LV, "the adding mac:%pM vlan_id:%d\n", ethhdr.h_dest, vlan_id);

		port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26,
				result_data[1]) <<19 |
				FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
				result_data[0]);

		if (!hit || ( hit && fls64(port_bitmap)-1 != tb.doport_id )) {
			ret = mac_priv->mac_table_update(mac_priv,
					ethhdr.h_dest,
					true,
					vlan_id,
					BIT(tb.doport_id),
					true,
					false,
					SA_CML,
					DA_CML,
					0,
					0,
					0);
			if (ret <= 0) {
				NAT_DBG(DBG_LV, "primac_index add failed!\n");
				return -ENOSPC;
			}

			tb.primac_index = ret;
		} else {
			dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
			tb.primac_index = dmac_index;
		}

		ret = priv->cpriv->intf_add(priv->cpriv, vlan_id, pppoe_en, tunnel_en, wan_flag, ethhdr.h_source);
		if (ret < 0){
			NAT_DBG(ERR_LV, "drtmac_index not found! add failed\n");
			return -ENOSPC;
		} else {
			tb.drtmac_index = ret;
		}
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_match_basic match;

		flow_rule_match_basic(rule, &match);
		switch (match.key->ip_proto) {
		case IPPROTO_TCP:
			tb.l4_type = 0;
			tuple.l4_type = 0;
			break;
		case IPPROTO_UDP:
			tb.l4_type = 1;
			tuple.l4_type = 1;
			break;
		default:
			return -EOPNOTSUPP;
		}
	} else {
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS)) {
		struct flow_match_ports ports;

		flow_rule_match_ports(rule, &ports);
		NAT_DBG(DBG_LV, "l4 src port: %u, dest port: %u\n",
			ntohs(ports.key->src), ntohs(ports.key->dst));

		tuple.sport = ports.key->src;
		tuple.dport = ports.key->dst;

		if (!is_dnat) {
			tb.private_port = ntohs(ports.key->src);
			tb.public_port = ntohs(ports.key->dst);
		} else {
			tb.public_port = ntohs(ports.key->src);
			tb.router_port = ntohs(ports.key->dst);
		}
	} else {
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS)) {
		struct flow_match_ipv4_addrs addrs;

		flow_rule_match_ipv4_addrs(rule, &addrs);
		NAT_DBG(DBG_LV, "src ip: %pI4, dest ip: %pI4\n", &addrs.key->src,
			&addrs.key->dst);

		tuple.sipv4 = addrs.key->src;
		tuple.dipv4 = addrs.key->dst;

		if (!is_dnat) {
			tb.private_ip = ntohl(addrs.key->src);
			tb.public_ip = ntohl(addrs.key->dst);
		} else {
			tb.public_ip = ntohl(addrs.key->src);
			tb.router_ip = ntohl(addrs.key->dst);
		}
	} else {
		return -EOPNOTSUPP;
	}

	ret = dpns_nat_tuple_set(is_lf, is_dnat, 0, &tuple);
	if (ret < 0)
		goto err_free;

	/* calculate 5-tuple crc16 using poly 0x8005 or 0x1021,
	 * depending on the sub-table chosen.
	 */
	for (i = 0; i < NPU_NAT_SUB_TB; i++)
		tb.crc16_poly[i] = crc16_custom((u8 *)&tuple, sizeof(tuple), i);

	entry = kmem_cache_zalloc(priv->swnapt_cache, GFP_KERNEL);
	if (!entry) {
		ret = -ENOMEM;
		goto err_free;
	}

	entry->cookie = f->cookie;
	if (!is_dnat)
		entry->sintf_index = tb.srtmac_index;
	else
		entry->dintf_index = tb.drtmac_index;

	for (i = 0; i < NPU_NAT_SUB_TB; i++)
		entry->crc16_poly[i] = tb.crc16_poly[i];

	ret = rhashtable_insert_fast(&priv->flow_table, &entry->node,
			dpns_nat_ht_params);
	if (ret)
		goto err_kfree;

	if (is_dnat)
		entry->is_dnat = true;

	/* routed (LF) flows are not counted yet */
	if (!is_lf) {
		dpns_nat_mib_get(priv, entry, flow, ((struct flow_offload_tuple *)f->cookie)->dir);
		if (entry->mib) {
			tb.stat_en = 1;
			tb.stat_index = entry->mib->index;
		}
	}

	mutex_lock(&priv->tbl_lock);
	ret = dpns_nat_add_napt4(priv, entry, is_lf, is_dnat, &tb);
	mutex_unlock(&priv->tbl_lock);
	if (ret)
		goto err_rm_hash;

	return 0;
err_rm_hash:
	dpns_nat_mib_put(priv, entry);
	rhashtable_remove_fast(&priv->flow_table, &entry->node,
				dpns_nat_ht_params);
err_kfree:
	kfree_rcu(entry, rcu);
err_free:
	if (!is_dnat)
		priv->cpriv->intf_del(priv->cpriv, tb.srtmac_index);
	else
		priv->cpriv->intf_del(priv->cpriv, tb.drtmac_index);
	return ret;
}

static int dpns_nat_mangle_ipv6(const struct flow_action_entry *act, bool is_dnat,
					struct nat_ipv6_data *tb6)
{
	switch (act->mangle.offset) {
	case offsetof(struct ipv6hdr, saddr) + 0x0:
		tb6->router_ip[3] = ntohl(act->mangle.val);
		break;
	case offsetof(struct ipv6hdr, saddr) + 0x4:
		tb6->router_ip[2] = ntohl(act->mangle.val);
		break;
	case offsetof(struct ipv6hdr, saddr) + 0x8:
		tb6->router_ip[1] = ntohl(act->mangle.val);
		break;
	case offsetof(struct ipv6hdr, saddr) + 0xc:
		tb6->router_ip[0] = ntohl(act->mangle.val);
		break;
	case offsetof(struct ipv6hdr, daddr) + 0x0:
		tb6->private_ip[3] = ntohl(act->mangle.val);
		break;
	case offsetof(struct ipv6hdr, daddr) + 0x4:
		tb6->private_ip[2] = ntohl(act->mangle.val);
		break;
	case offsetof(struct ipv6hdr, daddr) + 0x8:
		tb6->private_ip[1] = ntohl(act->mangle.val);
		break;
	case offsetof(struct ipv6hdr, daddr) + 0xc:
		tb6->private_ip[0] = ntohl(act->mangle.val);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int dpns_nat_add_ipv6(struct dpns_nat_priv *priv, struct flow_cls_offload *f,
			     bool is_dnat, struct flow_offload *flow,
			     bool is_lf, bool is_lf_reply)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct nat_ipv6_data tb6 = {};
	struct nat_hash_tuple tuple = {};
	struct flow_action_entry *act;
	struct ethhdr ethhdr = {};
	struct dpns_nat_entry *entry = NULL;
	MAC_t *mac_priv = priv->cpriv->mac_priv;
	COMMON_t *cpriv = priv->cpriv;
	dpns_port_t *p;
	u64 pppoe_hdr = 0;
	u64 port_bitmap;
	int hit;
	u32 result_data[2];
	u16 dmac_index;
	u16 vlan_id = DPA_UNTAGGED_VID, vid = 0;
	int ret, i, j;
	bool pppoe_en = is_dnat;

	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_MANGLE:
			switch (act->mangle.htype) {
			case FLOW_ACT_MANGLE_HDR_TYPE_ETH:
				dpns_nat_mangle_eth(act, &ethhdr);
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_TCP:
			case FLOW_ACT_MANGLE_HDR_TYPE_UDP:
				if (!is_dnat)
					tb6.router_port = ntohl(act->mangle.val) >> 16;
				else
					tb6.private_port =(u16)ntohl(act->mangle.val);
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_IP6:
				ret = dpns_nat_mangle_ipv6(act, is_dnat, &tb6);
				if (ret < 0)
					return ret;
				break;
			default:
				return -EOPNOTSUPP;
			}
			break;
		case FLOW_ACTION_REDIRECT: {
			if(netif_is_bridge_master(act->dev))
				return -EINVAL;

			p = cpriv->port_by_netdev(cpriv, act->dev);
			if (!p)
				return -EOPNOTSUPP;

			netdev_dbg(act->dev, "name: %s, oport id %u, is it corrupt?\n",
					act->dev->name, p->port_id);

			if (!is_dnat)
				tb6.soport_id = p->port_id;
			 else
				tb6.doport_id = p->port_id;
			break;
		}
		case FLOW_ACTION_PPPOE_PUSH:
			pppoe_en = 1;
			pppoe_hdr = (0x1100ULL << 48) | ((u64)act->pppoe.sid << 32) | PPP_IPV6;
			break;
		case FLOW_ACTION_VLAN_PUSH:
			vlan_id = act->vlan.vid;
			vid = vlan_id;
			break;
		case FLOW_ACTION_CSUM:
		case FLOW_ACTION_VLAN_POP:
			break;
		default:
			return -EOPNOTSUPP;
		}
	}

	NAT_DBG(DBG_LV, "after nat: dest mac: %pM, src mac: %pM\n", ethhdr.h_dest, ethhdr.h_source);
	if (is_zero_ether_addr(ethhdr.h_dest) ||
	    is_zero_ether_addr(ethhdr.h_source))
		return -EINVAL;

	if (!is_dnat) {
		hit = mac_priv->hw_search(mac_priv, ethhdr.h_dest, vlan_id, result_data);
		NAT_DBG(DBG_LV,"this is the snat ipv6 mac add!\n");
		NAT_DBG(DBG_LV, "the adding mac:%pM vlan_id:%d\n", ethhdr.h_dest, vlan_id);

		port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26,
				result_data[1]) <<19 |
				FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
				result_data[0]);

		if (!hit || ( hit && fls64(port_bitmap)-1 != tb6.soport_id )) {
			NAT_DBG(DBG_LV, "pubmac_index add failed!\n");
			return -ENOSPC;
		} else {
			dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
			tb6.pubmac_index = dmac_index;
		}

		//add srtmac_index , there is a search process in the process of adding
		ret = priv->cpriv->intf_add(priv->cpriv, vlan_id, pppoe_en, 0, !is_dnat, ethhdr.h_source);
		if (ret < 0){
			NAT_DBG(ERR_LV, "srtmac_index not found! add failed\n");
			return -ENOSPC;
		} else {
			tb6.srtmac_index = ret;
			priv->cpriv->table_write(priv->cpriv,
				16, tb6.srtmac_index, (u32 *)&pppoe_hdr, sizeof(pppoe_hdr));
		}
	} else {//DNAT
		hit = mac_priv->hw_search(mac_priv, ethhdr.h_dest, vlan_id, result_data);
		NAT_DBG(DBG_LV,"this is the dnat ipv6 mac add!\n");
		NAT_DBG(DBG_LV, "the adding mac:%pM vlan_id:%d\n", ethhdr.h_dest, vlan_id);

		port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26,
				result_data[1]) <<19 |
				FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
				result_data[0]);

		if (!hit || ( hit && fls64(port_bitmap)-1 != tb6.doport_id )) {
			ret = mac_priv->mac_table_update(mac_priv,
					ethhdr.h_dest,
					true,
					vlan_id,
					BIT(tb6.doport_id),
					true,
					false,
					SA_CML,
					DA_CML,
					0,
					0,
					0);
			if (ret <= 0) {
				NAT_DBG(DBG_LV, "primac_index add failed!\n");
				return -ENOSPC;
			}

			tb6.primac_index = ret;
		} else {
			dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
			tb6.primac_index = dmac_index;
		}

		ret = priv->cpriv->intf_add(priv->cpriv, vlan_id, pppoe_en, 0, !is_dnat, ethhdr.h_source);
		if (ret < 0){
			NAT_DBG(ERR_LV, "drtmac_index not found! add failed\n");
			return -ENOSPC;
		} else {
			tb6.drtmac_index = ret;
		}
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_match_basic match;

		flow_rule_match_basic(rule, &match);
		switch (match.key->ip_proto) {
		case IPPROTO_TCP:
			tb6.l4_type = 0;
			tuple.l4_type = 0;
			break;
		case IPPROTO_UDP:
			tb6.l4_type = 1;
			tuple.l4_type = 1;
			break;
		default:
			return -EOPNOTSUPP;
		}
	} else {
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS)) {
		struct flow_match_ports ports;

		flow_rule_match_ports(rule, &ports);

		tuple.sport = ports.key->src;
		tuple.dport = ports.key->dst;

		if (!is_dnat) {
			tb6.private_port = ntohs(ports.key->src);
			tb6.public_port = ntohs(ports.key->dst);
		} else {
			tb6.public_port = ntohs(ports.key->src);
			tb6.router_port = ntohs(ports.key->dst);
		}
	} else {
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV6_ADDRS)) {
		struct flow_match_ipv6_addrs addrs;

		flow_rule_match_ipv6_addrs(rule, &addrs);

		tuple.sipv6 = addrs.key->src;
		tuple.dipv6 = addrs.key->dst;
		if (!is_dnat) {
			for (j = 0; j < 4; j++) {
				tb6.private_ip[j] = ntohl(addrs.key->src.s6_addr32[3-j]);
				tb6.public_ip[j] = ntohl(addrs.key->dst.s6_addr32[3-j]);
			}
		} else {
			for (j = 0; j < 4; j++) {
				tb6.public_ip[j] = ntohl(addrs.key->src.s6_addr32[3-j]);
				tb6.router_ip[j] = ntohl(addrs.key->dst.s6_addr32[3-j]);
			}
		}
	} else {
		return -EOPNOTSUPP;
	}

	ret = dpns_nat_tuple_set(is_lf, is_dnat, 1, &tuple);
	if (ret < 0)
		goto err_free;

	/* calculate 5-tuple crc16 using poly 0x8005 or 0x1021,
	 * depending on the sub-table chosen.
	 */
	for (i = 0; i < NPU_NAT_SUB_TB; i++)
		tb6.crc16_poly[i] = crc16_custom((u8 *)&tuple, sizeof(tuple), i);

	entry = kmem_cache_zalloc(priv->swnapt_cache, GFP_KERNEL);
	if (!entry) {
		ret = -ENOMEM;
		goto err_free;
	}

	entry->v6_flag = true;
	entry->cookie = f->cookie;
	if (!is_dnat)
		entry->sintf_index = tb6.srtmac_index;
	else
		entry->dintf_index = tb6.drtmac_index;

	for (i = 0; i < NPU_NAT_SUB_TB; i++)
		entry->crc16_poly[i] = tb6.crc16_poly[i];

	ret = rhashtable_insert_fast(&priv->flow_table, &entry->node,
					dpns_nat_ht_params);
	if (ret)
		goto err_kfree;

	if (is_dnat)
		entry->is_dnat = true;

	/* routed (LF) flows are not counted yet */
	if (!is_lf) {
		dpns_nat_mib_get(priv, entry, flow, ((struct flow_offload_tuple *)f->cookie)->dir);
		if (entry->mib) {
			tb6.stat_en = 1;
			tb6.stat_index = entry->mib->index;
		}
	}

	/* first, find a vacant slot in the internal lookup table, if there is
	 * no more space, go to the external one.
	 */
	mutex_lock(&priv->tbl_lock);
	ret = dpns_nat_add_napt6(priv, entry, is_lf, is_dnat, &tb6);
	mutex_unlock(&priv->tbl_lock);
	if (ret)
		goto err_rm_hash;

	return 0;
err_rm_hash:
	dpns_nat_mib_put(priv, entry);
	rhashtable_remove_fast(&priv->flow_table, &entry->node,
			       dpns_nat_ht_params);
err_kfree:
	kfree_rcu(entry, rcu);
err_free:
	if (!is_dnat)
		priv->cpriv->intf_del(priv->cpriv, tb6.srtmac_index);
	else
		priv->cpriv->intf_del(priv->cpriv, tb6.drtmac_index);
	return ret;
}

static int dpns_nat_add(struct dpns_nat_priv *priv, struct flow_cls_offload *f)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct dpns_nat_entry *entry;
	struct flow_offload *flow;
	bool flow_snat = false, flow_dnat = false;
	bool dir = ((struct flow_offload_tuple *)f->cookie)->dir;
	bool is_dnat = false, is_lf = false, is_lf_reply = false;

	flow = dpns_nat_cookie_to_flow(f->cookie);
	flow_snat = test_bit(NF_FLOW_SNAT, &flow->flags);
	flow_dnat = test_bit(NF_FLOW_DNAT, &flow->flags);

	if (flow_snat && flow_dnat)
		return -EOPNOTSUPP;

	if ((flow_snat && !flow_dnat && dir) || (!flow_snat && flow_dnat && !dir))
		is_dnat = true;

	if (!(flow_snat || flow_dnat))
		is_lf = true;

	if (is_lf && dir)
		is_lf_reply = true;

	/* NAT44 and routed IPv4 offload are mutually exclusive, see dpns_nat_lf_init() */
	if (!is_lf && lf4_offload && flow->tuplehash[0].tuple.l3proto == NFPROTO_IPV4)
		return -EOPNOTSUPP;
	/* likewise NAT66 and routed IPv6 */
	if (!is_lf && lf_offload && flow->tuplehash[0].tuple.l3proto == NFPROTO_IPV6)
		return -EOPNOTSUPP;

	if (atomic_read(&priv->flow_table.nelems) >= NPU_HNAT_SIZE)
		return -ENOSPC;

	/*
	 * The NPU NAT entry carries a single 802.1Q tag. Stacked tags (QinQ) and
	 * 802.1ad tags were programmed as one 0x8100 tag, which black-holed the
	 * flow; keep such flows on the software path.
	 */
	{
		const struct flow_offload_tuple *t = (void *)f->cookie;
		struct flow_action_entry *a;
		int i, push = 0, tags = 0;

		flow_action_for_each(i, a, &rule->action) {
			if (a->id != FLOW_ACTION_VLAN_PUSH)
				continue;
			/* the NPU always inserts a 0x8100 tag */
			if (a->vlan.proto != htons(ETH_P_8021Q))
				return -EOPNOTSUPP;
			push++;
		}
		for (i = 0; i < t->encap_num; i++) {
			if (t->encap[i].proto == htons(ETH_P_8021AD))
				return -EOPNOTSUPP;
			if (t->encap[i].proto == htons(ETH_P_8021Q))
				tags++;
		}
		if (push > 1 || tags > 1)
			return -EOPNOTSUPP;
	}

	entry = rhashtable_lookup_fast(&priv->flow_table, &f->cookie, dpns_nat_ht_params);
	if (entry)
		return -EEXIST;

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL)) {
		struct flow_match_control match;

		flow_rule_match_control(rule, &match);
		switch (match.key->addr_type) {
		case FLOW_DISSECTOR_KEY_IPV4_ADDRS:
			if (is_lf && !lf4_offload)
				return -EOPNOTSUPP;
			return dpns_nat_add_ipv4(priv, f, is_dnat, flow, is_lf, is_lf_reply);
		case FLOW_DISSECTOR_KEY_IPV6_ADDRS:
			if (is_lf && !lf_offload)
				return -EOPNOTSUPP;
			return dpns_nat_add_ipv6(priv, f, is_dnat, flow, is_lf, is_lf_reply);
		}
	}

	return -EOPNOTSUPP;
}

static int dpns_nat_del_entry(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry)
{
	MAC_t *mac_priv;
	u16 crc_16;

	mac_priv = priv->cpriv->mac_priv;

	if (entry->v6_flag) {
		if (entry->nat_id < NPU_HNAT_INAPT_MAXID) {
			mutex_lock(&priv->tbl_lock);
			dpns_nat_rm_ilkp6_hw(priv, entry);
			mutex_unlock(&priv->tbl_lock);
		} else {
			union nat_table_u *t;
			crc_16 = crc16_custom((u8*)&(entry->nat_id), sizeof(int), 0);

			t = (!entry->is_dnat) ? priv->snat_table : priv->dnat_table;
			memset(&t[entry->index].v6, 0, sizeof(nat_ipv6_ext_table));
			if (test_and_change_bit(entry->nat_id - NPU_HNAT_INAPT_MAXID, priv->natid_odd_entries)) {
				clear_bit(entry->nat_id - NPU_HNAT_INAPT_MAXID, priv->natid_bitmap);
				mac_priv->sf_del_ts_info(mac_priv, NULL, 0, entry->nat_id, crc_16);
			}
		}
	} else {
		if (entry->nat_id < NPU_HNAT_INAPT_MAXID) {
			mutex_lock(&priv->tbl_lock);
			dpns_nat_rm_ilkp4_hw(priv, entry);
			mutex_unlock(&priv->tbl_lock);
		} else {
			union nat_table_u *t;
			crc_16 = crc16_custom((u8*)&(entry->nat_id), sizeof(int), 0);

			t = (!entry->is_dnat) ? priv->snat_table : priv->dnat_table;
			memset(&t[entry->index].v4[entry->second_slot], 0, sizeof(nat_ipv4_ext_table));
			if (test_and_change_bit(entry->nat_id - NPU_HNAT_INAPT_MAXID, priv->natid_odd_entries)) {
				clear_bit(entry->nat_id - NPU_HNAT_INAPT_MAXID, priv->natid_bitmap);
				mac_priv->sf_del_ts_info(mac_priv, NULL, 0, entry->nat_id, crc_16);
			}
		}
	}
	dpns_nat_mib_put(priv, entry);
	rhashtable_remove_fast(&priv->flow_table, &entry->node,
			dpns_nat_ht_params);
	kfree_rcu(entry, rcu);

	return 0;
}

/*
 * Read and clear the hardware counter of one direction of an entry. Returns the
 * packets, @bytes gets the IP bytes as conntrack counts them.
 */
static u64 dpns_nat_mib_collect(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
				u64 *bytes)
{
	u64 pkts = 0, frame_bytes = 0;

	spin_lock_bh(&priv->mib_lock);
	if (entry->mib)
		dpns_nat_mib_read(priv, entry->mib->index * 2 + !entry->is_dnat,
				  &pkts, &frame_bytes);
	spin_unlock_bh(&priv->mib_lock);

	*bytes = frame_bytes > pkts * ETH_HLEN ? frame_bytes - pkts * ETH_HLEN : 0;
	return pkts;
}

static int dpns_nat_del(struct dpns_nat_priv *priv, struct flow_cls_offload *f)
{
	const struct flow_offload_tuple *tuple = (void *)f->cookie;
	struct dpns_nat_entry *entry;
	struct flow_offload *flow;
	u64 pkts, bytes;

	entry = rhashtable_lookup_fast(&priv->flow_table, &f->cookie, dpns_nat_ht_params);
	if (!entry)
		return -ENOENT;

	/*
	 * The flowtable reads the counters only every few seconds and not when a
	 * flow is removed, so account what the NPU counted since the last read.
	 */
	pkts = dpns_nat_mib_collect(priv, entry, &bytes);
	flow = dpns_nat_cookie_to_flow(f->cookie);
	if (pkts && flow->ct)
		nf_ct_acct_add(flow->ct, tuple->dir, pkts, bytes);

	return dpns_nat_del_entry(priv, entry);
}

static void dpns_nat_visit_work(struct work_struct *work)
{
	struct dpns_nat_priv *priv = container_of(to_delayed_work(work), struct dpns_nat_priv, visit_dwork);
	const volatile long *base = (__force void *)priv->iobase + SE_NAT_VISIT(0);
	atomic_long_t *stats_cache = (atomic_long_t *)priv->stats_cache;
	unsigned long i;

	/* unroll the loop to reduce latency */
	for (i = 0; i < ARRAY_SIZE(priv->stats_cache) / 8; i++) {
		unsigned long reg0, reg1, reg2, reg3, reg4, reg5, reg6, reg7;

		reg0 = base[0];
		reg1 = base[1];
		reg2 = base[2];
		reg3 = base[3];
		reg4 = base[4];
		reg5 = base[5];
		reg6 = base[6];
		reg7 = base[7];
		base += 8;

		atomic_long_or(reg0, stats_cache + 0);
		atomic_long_or(reg1, stats_cache + 1);
		atomic_long_or(reg2, stats_cache + 2);
		atomic_long_or(reg3, stats_cache + 3);
		atomic_long_or(reg4, stats_cache + 4);
		atomic_long_or(reg5, stats_cache + 5);
		atomic_long_or(reg6, stats_cache + 6);
		atomic_long_or(reg7, stats_cache + 7);
		stats_cache += 8;
	}
}

static int dpns_nat_stats(struct dpns_nat_priv *priv, struct flow_cls_offload *f)
{
	struct dpns_nat_entry *entry;
	u64 pkts, bytes;

	rcu_read_lock();
	entry = rhashtable_lookup(&priv->flow_table, &f->cookie,
				  dpns_nat_ht_params);
	if (!entry) {
		rcu_read_unlock();
		return -ENOENT;
	}

	if (test_and_clear_bit(entry->nat_id, priv->stats_cache)) {
		f->stats.lastused = get_jiffies_64();
	} else {
		schedule_delayed_work(&priv->visit_dwork, 1 * HZ);
	}

	pkts = dpns_nat_mib_collect(priv, entry, &bytes);

	rcu_read_unlock();

	if (pkts)
		flow_stats_update(&f->stats, bytes, pkts, 0, get_jiffies_64(),
				  FLOW_ACTION_HW_STATS_DELAYED);

	return 0;
}


static int dpns_nat_tc_ft_cb(enum tc_setup_type type, void *type_data,
			     void *cb_priv)
{
	struct flow_cls_offload *f = type_data;
	struct dpns_nat_block *nb = cb_priv;
	struct dpns_nat_priv *priv = nb->priv;
	struct dpns_nat_entry *entry;
	int ret;

	if (type != TC_SETUP_CLSFLOWER)
		return -EOPNOTSUPP;

	switch (f->command) {
	case FLOW_CLS_REPLACE:
		ret = dpns_nat_add(priv, f);
		if (!ret) {
			entry = rhashtable_lookup_fast(&priv->flow_table, &f->cookie,
						       dpns_nat_ht_params);
			if (entry)
				entry->owner = nb;
		}
		break;
	case FLOW_CLS_DESTROY:
		ret = dpns_nat_del(priv, f);
		break;
	case FLOW_CLS_STATS:
		ret = dpns_nat_stats(priv, f);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	cond_resched();

	return ret;
}

int dpns_nat_show(struct dpns_nat_priv *priv)
{
	struct flow_offload_tuple *tuple, *other;
	struct dpns_nat_entry *entry;
	struct rhashtable_iter iter;
	bool snat, dnat;

	if (priv->refcnt == 0) {
		NAT_DBG(ERR_LV, "flow_table is destroyed\n");
		return 0;
	}

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry))
			continue;

		snat = !entry->is_dnat;
		dnat = entry->is_dnat;
		tuple = (struct flow_offload_tuple *)entry->cookie;
		other = &dpns_nat_cookie_to_flow(entry->cookie)
				 ->tuplehash[!tuple->dir].tuple;
		printk("NAPT%u: cookie %lx, NAT ID %u\n",
			   entry->second_slot, entry->cookie, entry->nat_id);
		if (entry->nat_id < NPU_HNAT_ILKP_SIZE) {
			if (snat)
				printk("ILKP SNAT hash %04x\n",
					   entry->hash_index);
			if (dnat)
				printk("ILKP DNAT hash %04x\n",
					   entry->hash_index);
		} else {
			printk("ELKP index %04x\n", entry->index);
		}
		printk("L4 protocol");
		switch (tuple->l4proto) {
		case IPPROTO_TCP:
			printk("TCP\n");
			break;
		case IPPROTO_UDP:
			printk("UDP\n");
			break;
		default:
			printk("unknown\n");
		}

		switch (tuple->l3proto) {
		case NFPROTO_IPV4:
			printk("Src %pI4:%u",
				   &tuple->src_v4, ntohs(tuple->src_port));
			if (snat) {
				printk("\t--> SNAT %pI4:%u", &other->dst_v4,
					   ntohs(other->dst_port));
			}
			printk("\n");
			printk("Dst %pI4:%u",
				   &tuple->dst_v4, ntohs(tuple->dst_port));
			if (dnat) {
				printk("\t--> DNAT %pI4:%u", &other->src_v4,
					   ntohs(other->src_port));
			}
			printk("\n");
			break;
		case NFPROTO_IPV6:
			printk("Src [%pI6]:%u",
				   &tuple->src_v6, ntohs(tuple->src_port));
			if (snat) {
				printk("\t--> SNAT [%pI6]:%u", &other->dst_v6,
					   ntohs(other->dst_port));
			}
			printk("\n");
			printk("Dst [%pI6]:%u",
				   &tuple->dst_v6, ntohs(tuple->dst_port));
			if (dnat) {
				printk("\t--> DNAT [%pI6]:%u", &other->src_v6,
					   ntohs(other->src_port));
			}
			printk("\n");
			break;
		default:
			continue;
		}
	}
	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);
	return 0;
}

static int dpns_nat_enable(struct dpns_nat_priv *priv)
{
	int ret;

	mutex_lock(&priv->tbl_lock);
	if (priv->refcnt) {
		priv->refcnt++;
		goto out;
	}

	ret = rhashtable_init(&priv->flow_table, &dpns_nat_ht_params);
	if (ret)
		goto err_rhashtable_init;

	priv->refcnt++;
out:
	mutex_unlock(&priv->tbl_lock);
	return 0;
err_rhashtable_init:
	mutex_unlock(&priv->tbl_lock);
	return ret;
}

static void dpns_nat_disable(struct dpns_nat_priv *priv)
{
	mutex_lock(&priv->tbl_lock);
	if (--priv->refcnt)
		goto out;
	rhashtable_destroy(&priv->flow_table);
out:
	mutex_unlock(&priv->tbl_lock);
}

/* Remove all NPU entries offloaded from flowtable block nb. */
static void dpns_nat_flush_block(struct dpns_nat_priv *priv, struct dpns_nat_block *nb)
{
	struct dpns_nat_entry *entry, *tmp;
	struct rhashtable_iter iter;
	LIST_HEAD(flush);
	unsigned int n = 0;

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry))
			continue;
		if (entry->owner == nb)
			list_add_tail(&entry->flush_node, &flush);
	}
	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

	list_for_each_entry_safe(entry, tmp, &flush, flush_node) {
		list_del(&entry->flush_node);
		dpns_nat_del_entry(priv, entry);
		n++;
	}
	if (n)
		pr_info("dpns_nat: flowtable unbound, removed %u NPU entries\n", n);
}

/*
 * Called for each per-port block callback when the kernel frees it, i.e.
 * after it has been taken off the flowtable callback list under the flow
 * block write lock, so no REPLACE/DESTROY/STATS can run concurrently.
 */
static void dpns_nat_block_release(void *cb_priv)
{
	struct dpns_nat_block *nb = cb_priv;
	struct dpns_nat_priv *priv = nb->priv;

	mutex_lock(&dpns_nat_blocks_lock);
	if (--nb->users == 0) {
		list_del(&nb->list);
		dpns_nat_flush_block(priv, nb);
		kfree(nb);
	}
	mutex_unlock(&dpns_nat_blocks_lock);

	dpns_nat_disable(priv);
}

static struct dpns_nat_block *dpns_nat_block_get(struct dpns_nat_priv *priv,
						 struct flow_block *block)
{
	struct dpns_nat_block *nb;

	mutex_lock(&dpns_nat_blocks_lock);
	list_for_each_entry(nb, &dpns_nat_blocks, list) {
		if (nb->block == block)
			goto found;
	}
	nb = kzalloc(sizeof(*nb), GFP_KERNEL);
	if (!nb)
		goto out;
	nb->block = block;
	nb->priv = priv;
	list_add_tail(&nb->list, &dpns_nat_blocks);
found:
	nb->users++;
out:
	mutex_unlock(&dpns_nat_blocks_lock);
	return nb;
}

static void dpns_nat_block_put(struct dpns_nat_block *nb)
{
	mutex_lock(&dpns_nat_blocks_lock);
	if (--nb->users == 0) {
		list_del(&nb->list);
		kfree(nb);
	}
	mutex_unlock(&dpns_nat_blocks_lock);
}

static int dpns_nat_tc_ft(struct net_device *dev, struct Qdisc *sch,
			  void *cb_priv, enum tc_setup_type type,
			  void *type_data, void *data,
			  void (*cleanup)(struct flow_block_cb *))
{
	struct dpns_nat_priv *priv = cb_priv;
	struct flow_block_cb *block_cb = NULL;
	struct flow_block_offload *f;
	COMMON_t *cpriv = priv->cpriv;
	flow_setup_cb_t *cb;
	dpns_port_t *p;
	int ret;

	if (!dev)
		return -EOPNOTSUPP;

	if (type != TC_SETUP_FT)
		return -EOPNOTSUPP;

	f = type_data;
	if (f->binder_type != FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS)
		return -EOPNOTSUPP;

	p = cpriv->port_by_netdev(cpriv, dev);
	if (!p)
		return -EOPNOTSUPP;

	cb = dpns_nat_tc_ft_cb;
	f->driver_block_list = &dpns_nat_ft_cb_list;

	switch (f->command) {
	case FLOW_BLOCK_BIND:
		if (f->block != NULL) {
			block_cb = flow_block_cb_lookup(f->block, cb, p);
			if (block_cb) {
				flow_block_cb_incref(block_cb);
				return 0;
			}
		}
		if (!block_cb) {
			struct dpns_nat_block *nb;

			ret = dpns_nat_enable(priv);
			if (ret)
				return ret;

			nb = dpns_nat_block_get(priv, f->block);
			if (!nb) {
				dpns_nat_disable(priv);
				return -ENOMEM;
			}

			block_cb = flow_indr_block_cb_alloc(cb, p, nb, dpns_nat_block_release,
							f, dev, sch, data, priv, cleanup);
			if (IS_ERR(block_cb)) {
				dpns_nat_block_put(nb);
				dpns_nat_disable(priv);
				return PTR_ERR(block_cb);
			}
			flow_block_cb_incref(block_cb);
			flow_block_cb_add(block_cb, f);
			list_add_tail(&block_cb->driver_list, f->driver_block_list);
		}
		return 0;
	case FLOW_BLOCK_UNBIND:
		block_cb = flow_block_cb_lookup(f->block, cb, p);
		if (!block_cb)
			return -ENOENT;

		if (flow_block_cb_decref(block_cb))
			return 0;

		/* entries are flushed and the table released in dpns_nat_block_release() */
		flow_indr_block_cb_remove(block_cb, f);
		list_del(&block_cb->driver_list);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

/* FIXME: Netfilter bridge conntrack unable to create table entry */
static unsigned int dpns_nat_hook(void *priv, struct sk_buff *skb,
				const struct nf_hook_state *state)
{
	enum ip_conntrack_info ctinfo;
	const struct nf_conn *ct;
	struct gmac_common *gmac_priv;
	union nat_table_u *t;
	struct nat_hash_tuple tuple = {};
	u32 index, reg_val, offset;
	u16 crc16_poly0, crc16_poly1;
	u16 i, j, l2offload_mode, nat_id;
	u8 sub_tb;
	bool second_slot = false, is_ipv6 = false;

	nat_ipv6_ext_table tb6 = {
		.valid = 1,
		.flag = 1,
		.dummy = -1,
	};

	nat_ipv4_ext_table tb = {
		.valid = 1,
		.dummy = -1
	};


	if (!g_priv->nat_offload_en)
		return NF_ACCEPT;

	ct = nf_ct_get(skb, &ctinfo);
	if (!ct)
		return NF_ACCEPT;

	if (ctinfo != IP_CT_ESTABLISHED) {
		return NF_ACCEPT;
	}

	if ((ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum != IPPROTO_UDP) &&
		ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum != IPPROTO_TCP)
		return NF_ACCEPT;

	if (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum == IPPROTO_UDP) {
		tb.l4_type = 1;
		tb6.l4_type = 1;
	}

	for (j = 0; j < 2; j++) {
		if (ct->tuplehash[j].tuple.src.l3num == AF_INET6) {
			is_ipv6 = true;
			tb6.public_ip[0] = ntohl(ct->tuplehash[j].tuple.dst.u3.ip6[3]);
			tb6.public_ip[1] = ntohl(ct->tuplehash[j].tuple.dst.u3.ip6[2]);
			tb6.public_ip[2] = ntohl(ct->tuplehash[j].tuple.dst.u3.ip6[1]);
			tb6.public_ip[3] = ntohl(ct->tuplehash[j].tuple.dst.u3.ip6[0]);
			tb6.private_ip[0] = ntohl(ct->tuplehash[j].tuple.src.u3.ip6[3]);
			tb6.private_ip[1] = ntohl(ct->tuplehash[j].tuple.src.u3.ip6[2]);
			tb6.private_ip[2] = ntohl(ct->tuplehash[j].tuple.src.u3.ip6[1]);
			tb6.private_ip[3] = ntohl(ct->tuplehash[j].tuple.src.u3.ip6[0]);
			tb6.public_port = ntohs(ct->tuplehash[j].tuple.dst.u.udp.port);
			tb6.private_port = ntohs(ct->tuplehash[j].tuple.src.u.udp.port);
			tuple.sipv6 = ct->tuplehash[j].tuple.src.u3.in6;
			tuple.dipv6 = ct->tuplehash[j].tuple.dst.u3.in6;
		}

		if (ct->tuplehash[j].tuple.src.l3num == AF_INET) {
			tb.public_ip = ntohl(ct->tuplehash[j].tuple.dst.u3.ip);
			tb.private_ip = ntohl(ct->tuplehash[j].tuple.src.u3.ip);
			tuple.sipv4 = ct->tuplehash[j].tuple.src.u3.ip;
			tuple.dipv4 = ct->tuplehash[j].tuple.dst.u3.ip;
			tb.public_port = ntohs(ct->tuplehash[j].tuple.dst.u.udp.port);
			tb.private_port = ntohs(ct->tuplehash[j].tuple.src.u.udp.port);
		}

		if (j == 0)
			gmac_priv = netdev_priv(state->out);
		else
			gmac_priv = netdev_priv(state->in);

		tb.oport_id = gmac_priv->id;
		tb6.oport_id = gmac_priv->id;

		tuple.l4_type = tb.l4_type;
		tuple.sport = ct->tuplehash[j].tuple.src.u.udp.port;
		tuple.dport = ct->tuplehash[j].tuple.dst.u.udp.port;

		reg_val = sf_readl(g_priv, SE_NAT_CONFIG5);
		if (is_ipv6)
			l2offload_mode = FIELD_GET(NAT_CONFIG5_V6_L2OFFLOAD_MODE, reg_val);
		else
			l2offload_mode = FIELD_GET(NAT_CONFIG5_V4_L2OFFLOAD_MODE, reg_val);

		switch (l2offload_mode) {
		case 0:
			tuple.l4_type = 0;
			tuple.sport = 0;
			tuple.sipv4 = 0;
			tuple.sipv6 = (struct in6_addr){0};
			tuple.dport = 0;
			break;
		case 1:
			tuple.sport = 0;
			tuple.sipv4 = 0;
			tuple.sipv6 = (struct in6_addr){0};
			break;
		case 2:
			tuple.l4_type = 0;
			tuple.sport = 0;
			tuple.dport = 0;
			tuple.dipv4 = 0;
			tuple.dipv6 = (struct in6_addr){0};
			break;
		case 3:
			tuple.dport = 0;
			tuple.dipv4 = 0;
			tuple.dipv6 = (struct in6_addr){0};
			break;
		case 4:
			tuple.l4_type = 0;
			tuple.sport = 0;
			tuple.dport = 0;
			break;
		case 5:
			tuple.sport = 0;
			tuple.dport = 0;
			break;
		case 6:
			tuple.sport = 0;
			break;
		case 7:
			break;
		}

		/* calculate 5-tuple crc16 using poly 0x8005 or 0x1021,
		* depending on the sub-table chosen.
		*/
		crc16_poly0 = crc16_custom((u8 *)&tuple, sizeof(tuple), 0);
		crc16_poly1 = crc16_custom((u8 *)&tuple, sizeof(tuple), 1);

		nat_id = find_first_zero_bit(g_priv->natid_bitmap,
			NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID) +
			NPU_HNAT_INAPT_MAXID;
		if (nat_id >= NPU_HNAT_VISIT_SIZE)
			return NF_ACCEPT;

		if (is_ipv6)
			tb6.nat_id = nat_id;
		else
			tb.nat_id = nat_id;

		t = g_priv->snat_table;
		/* for elkp, index is the lowest x bits of crc16,
		* x = log 2 (table entries / sub-tables count),
		* for 4M(64B*64K) size with 8 sub-tables, each sub-table has 8K
		* entries.
		* poly1 (0x8005) is used for even number of sub-tables, and poly0
		* (0x1021) is used for odd number ones.
		*/
		sub_tb = ELKP_SUB_TB(g_priv->elkp_v4_acs_times);
		offset = ELKP_OFFSET(g_priv->elkp_size, sub_tb);
		for (i = 0; i < sub_tb; i++) {
			index = i * offset;
			index += ((i % 2) ? crc16_poly0 : crc16_poly1) & (offset - 1);
			if (!is_ipv6) {
				if (t[index].v6.flag)
					continue;

				/* index is vacant */
				if (!t[index].v4[0].valid)
					break;

				if (!t[index].v4[1].valid) {
					second_slot = true;
					break;
				}
			} else {
				/* index is vacant */
				if (!t[index].v4[0].dummy && !t[index].v6.dummy)
					break;
			}
		}

		if (i == sub_tb)
			return NF_ACCEPT;

		if (!is_ipv6)
			memcpy(&t[index].v4[second_slot], &tb, sizeof(tb));
		else
			memcpy(&t[index].v6, &tb6, sizeof(tb6));
		/* Mark nat_id as used. (It's ok to do it twice) */
		set_bit(nat_id - NPU_HNAT_INAPT_MAXID, g_priv->natid_bitmap);
		/* nat_id refcnt += 1 */
		change_bit(nat_id - NPU_HNAT_INAPT_MAXID, g_priv->natid_odd_entries);
		NAT_DBG(DBG_LV, "adding nat id %u to external, l2 offload mode is %d\n",nat_id, l2offload_mode);
	}

	return NF_ACCEPT;
}

static const struct nf_hook_ops dpns_nat_ops = {
	.hook		= dpns_nat_hook,
	.pf		= NFPROTO_BRIDGE,
	.hooknum	= NF_BR_FORWARD,
	.priority	= NF_BR_PRI_FIRST,
};

int dpns_nat_probe(struct platform_device *pdev)
{
	COMMON_t *dpns = platform_get_drvdata(pdev);
	struct dpns_nat_priv *priv;
	u64 npu_clk = 0;
	u8 table_size;
	int ret;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	g_priv = priv;
	INIT_DELAYED_WORK(&priv->visit_dwork, dpns_nat_visit_work);
	priv->cpriv = dpns;
	priv->iobase = dpns->iobase;
	dpns->nat_priv = priv;
	priv->napt_add_mode = FIRST_ILKP;

	table_size = ELKP_TABLE_SIZE_2M;
	priv->elkp_size = SZ_512K << table_size;
	priv->elkp_v4_acs_times = 0;
	priv->elkp_v6_acs_times = 0;
	priv->dnat_table = dmam_alloc_coherent(&pdev->dev, priv->elkp_size,
					       &priv->dnat_phys, GFP_KERNEL);
	if (!priv->dnat_table)
		return -ENOMEM;

#ifndef CONFIG_ELK_DSNAT_SINGLE
	priv->snat_table = dmam_alloc_coherent(&pdev->dev, priv->elkp_size,
					       &priv->snat_phys, GFP_KERNEL);
	if (!priv->snat_table)
		return -ENOMEM;
#else
	priv->snat_table = priv->dnat_table;
	priv->snat_phys = priv->dnat_phys;
#endif

	priv->swnapt_cache = kmem_cache_create("dpns_nat_swnapt",
						sizeof(struct dpns_nat_entry), 0, 0, NULL);
	if (!priv->swnapt_cache)
		return -ENOMEM;

	priv->set_natmib_en = set_natmib_en;

	/* clear nat table */
	sf_writel(priv, SE_NAT_CLR, 0x7ff);

	/* set nat base addr */
	sf_writel(priv, SE_NAT_DNAT_BASE_ADDR, priv->dnat_phys);
	sf_writel(priv, SE_NAT_SNAT_BASE_ADDR, priv->snat_phys);
	NAT_DBG(INFO_LV, "use elkp at dnat %pad, snat %pad\n", &priv->dnat_phys, &priv->snat_phys);

	dpns_nat_init_subnet_info(priv);

	ret = register_inetaddr_notifier(&dpns_nat_inetaddr_notifier);
	if (ret)
		goto err_ip4_notifier;

	ret = register_inet6addr_notifier(&dpns_nat_inet6addr_notifier);
	if (ret)
		goto err_ip6_notifier;

	ret = nf_register_net_hook(&init_net, &dpns_nat_ops);
	if (ret)
		goto err_net_hook;

	/* nat mode symmetric , default nat66*/
	sf_update(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE,
			NPU_UDP_HNAT_MODE | NPU_TCP_HNAT_MODE | NPU_V6LF_MODE_SET,
			FIELD_PREP(NPU_UDP_HNAT_MODE, NPU_HNAT_MODE_SYMMETRIC) |
			FIELD_PREP(NPU_TCP_HNAT_MODE, NPU_HNAT_MODE_SYMMETRIC));

	/* symmetric, elkp size 2M, 1 elkp sub-tables */
	sf_update(priv, SE_NAT_CONFIG0,
		  NAT_CONFIG0_UDP_HNAT_MODE | NAT_CONFIG0_DIS_DDR_LKP |
		  NAT_CONFIG0_ELKP_TABLE_SIZE | NAT_CONFIG0_ELKP_V4_ACS_TIMES |
		  NAT_CONFIG0_ELKP_V6_ACS_TIMES,
		  FIELD_PREP(NAT_CONFIG0_UDP_HNAT_MODE , NPU_HNAT_MODE_SYMMETRIC) |
		  FIELD_PREP(NAT_CONFIG0_ELKP_TABLE_SIZE, table_size) |
		  FIELD_PREP(NAT_CONFIG0_ELKP_V4_ACS_TIMES, priv->elkp_v4_acs_times) |
		  FIELD_PREP(NAT_CONFIG0_ELKP_V6_ACS_TIMES, priv->elkp_v6_acs_times));

	/* default enable axi timing record */
	sf_update(priv, AXI_TIMING_RCD_CTRL, 0, AXI_RD_TIMING_MEASURE_EN);

	sf_update(priv, SE_NAT_CONFIG1, NAT_CONFIG1_TCP_HNAT_MODE | NAT_CONFIG1_V6LF_EN |
			NAT_HASH_POLY_SEL7 | NAT_HASH_POLY_SEL6 | NAT_HASH_POLY_SEL5 | NAT_HASH_POLY_SEL4 |
			NAT_HASH_POLY_SEL3 | NAT_HASH_POLY_SEL2 | NAT_HASH_POLY_SEL1 | NAT_HASH_POLY_SEL0,
			FIELD_PREP(NAT_CONFIG1_TCP_HNAT_MODE, NPU_HNAT_MODE_SYMMETRIC) |
			FIELD_PREP(NAT_HASH_POLY_SEL7,7) | FIELD_PREP(NAT_HASH_POLY_SEL6,6) |
			FIELD_PREP(NAT_HASH_POLY_SEL5,5) | FIELD_PREP(NAT_HASH_POLY_SEL4,4) |
			FIELD_PREP(NAT_HASH_POLY_SEL3,3) | FIELD_PREP(NAT_HASH_POLY_SEL2,2) |
			FIELD_PREP(NAT_HASH_POLY_SEL1,1) | FIELD_PREP(NAT_HASH_POLY_SEL0,0));

	sf_update(priv, SE_NAT_CONFIG5, NAT_SPL_CMPT_LEN | NAT_CONFIG5_MIB_MODE | NAT_CONFIG5_SPL_CNT_MODE | NAT_CONFIG5_SPL_SOURCE,
			FIELD_PREP(NAT_SPL_CMPT_LEN, 24) | FIELD_PREP(NAT_CONFIG5_MIB_MODE, 1) | FIELD_PREP(NAT_CONFIG5_SPL_CNT_MODE, SPL_BYTE) |
			FIELD_PREP(NAT_CONFIG5_SPL_SOURCE, UNIFORM_SNAT_DNAT));
	mutex_init(&priv->tbl_lock);
	spin_lock_init(&priv->mib_lock);

	/* after the default (NAT66) mode setup above, which clears the LF bits */
	dpns_nat_lf_init(priv);

	ret = dpns_nat_ilkp_init(priv);
	if (ret)
		goto err_ilkp_init;

	ret = dpns_nat_genl_init(priv);
	if (ret)
		goto err_genl_init;

	ret = flow_indr_dev_register(dpns_nat_tc_ft, priv);
	if (ret)
		goto err_flow_indr;

	npu_clk = clk_get_rate(priv->cpriv->clk);	//get NPU clock Hz
	npu_clk = npu_clk -1;
	sf_writel(priv, SE_NAT_CONFIG6, npu_clk);

	dpns_nat_proc_init(priv);

	printk("End %s\n", __func__);
	return 0;

err_flow_indr:
	dpns_nat_genl_exit();
err_genl_init:
	dpns_nat_ilkp_exit(priv);
err_ilkp_init:
	mutex_destroy(&priv->tbl_lock);
	nf_unregister_net_hook(&init_net, &dpns_nat_ops);
err_net_hook:
	unregister_inet6addr_notifier(&dpns_nat_inet6addr_notifier);
err_ip6_notifier:
	unregister_inetaddr_notifier(&dpns_nat_inetaddr_notifier);
err_ip4_notifier:
	kmem_cache_destroy(priv->swnapt_cache);
	return ret;
}
EXPORT_SYMBOL(dpns_nat_probe);

void dpns_nat_remove(struct platform_device *pdev)
{
	struct dpns_nat_priv *priv = g_priv;

	flow_indr_dev_unregister(dpns_nat_tc_ft, priv, NULL);
	dpns_nat_genl_exit();
	dpns_nat_ilkp_exit(priv);
	cancel_delayed_work_sync(&priv->visit_dwork);

	priv->dnat_table = NULL;
	priv->snat_table = NULL;

	nf_unregister_net_hook(&init_net, &dpns_nat_ops);
	unregister_inetaddr_notifier(&dpns_nat_inetaddr_notifier);
	unregister_inet6addr_notifier(&dpns_nat_inet6addr_notifier);
	kmem_cache_destroy(priv->swnapt_cache);

	/* disable elkp */
	sf_update(priv, SE_NAT_CONFIG0, 0, NAT_CONFIG0_DIS_DDR_LKP);

	/* clear nat table */
	sf_writel(priv, SE_NAT_CLR, 0x7ff);

	mutex_destroy(&priv->tbl_lock);
	dpns_nat_proc_exit();
	g_priv = NULL;
	printk("End %s\n", __func__);
}
EXPORT_SYMBOL(dpns_nat_remove);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("YouJia.min <youjia.min@siflower.com.cn>");
