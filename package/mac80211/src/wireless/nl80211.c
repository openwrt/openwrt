/*
 * This is the new netlink-based wireless configuration interface.
 *
 * Copyright 2006 Johannes Berg <johannes@sipsolutions.net>
 */

#include <linux/if.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/if_ether.h>
#include <linux/ieee80211.h>
#include <linux/nl80211.h>
#include <linux/rtnetlink.h>
#include <net/genetlink.h>
#include <net/cfg80211.h>
#include "core.h"
#include "nl80211.h"

/* the netlink family */
static struct genl_family nl80211_fam = {
	.id = GENL_ID_GENERATE,	/* don't bother with a hardcoded ID */
	.name = "nl80211",	/* have users key off the name instead */
	.hdrsize = 0,		/* no private header */
	.version = 1,		/* no particular meaning now */
	.maxattr = NL80211_ATTR_MAX,
};

/* internal helper: validate an information element attribute */
static int check_information_element(struct nlattr *nla)
{
	int len = nla_len(nla);
	u8 *data = nla_data(nla);
	int elementlen;

	while (len >= 2) {
		/* 1 byte ID, 1 byte len, `len' bytes data */
		elementlen = *(data+1) + 2;
		data += elementlen;
		len -= elementlen;
	}
	return len ? -EINVAL : 0;
}

/* internal helper: get drv and dev */
static int get_drv_dev_by_info_ifindex(struct genl_info *info,
				       struct cfg80211_registered_device **drv,
				       struct net_device **dev)
{
	int ifindex;

	if (!info->attrs[NL80211_ATTR_IFINDEX])
		return -EINVAL;

	ifindex = nla_get_u32(info->attrs[NL80211_ATTR_IFINDEX]);
	*dev = dev_get_by_index(ifindex);
	if (!dev)
		return -ENODEV;

	*drv = cfg80211_get_dev_from_ifindex(ifindex);
	if (IS_ERR(*drv)) {
		dev_put(*dev);
		return PTR_ERR(*drv);
	}

	return 0;
}

/* policy for the attributes */
static struct nla_policy nl80211_policy[NL80211_ATTR_MAX+1] __read_mostly = {
	[NL80211_ATTR_IFINDEX] = { .type = NLA_U32 },
	[NL80211_ATTR_IFNAME] = { .type = NLA_NUL_STRING, .len = IFNAMSIZ-1 },
	[NL80211_ATTR_WIPHY] = { .type = NLA_U32 },
	[NL80211_ATTR_WIPHY_NAME] = { .type = NLA_NUL_STRING,
				      .len = BUS_ID_SIZE-1 },
	[NL80211_ATTR_IFTYPE] = { .type = NLA_U32 },
	[NL80211_ATTR_BSSID] = { .len = ETH_ALEN },
	[NL80211_ATTR_SSID] = { .type = NLA_BINARY,
				.len = IEEE80211_MAX_SSID_LEN },
	[NL80211_ATTR_CHANNEL] = { .type = NLA_U32 },
	[NL80211_ATTR_PHYMODE] = { .type = NLA_U32 },
	[NL80211_ATTR_CHANNEL_LIST] = { .type = NLA_NESTED },
	[NL80211_ATTR_BSS_LIST] = { .type = NLA_NESTED },
	[NL80211_ATTR_BSSTYPE] = { .type = NLA_U32 },
	[NL80211_ATTR_BEACON_PERIOD] = { .type = NLA_U32 },
	[NL80211_ATTR_DTIM_PERIOD] = { .type = NLA_U32 },
	[NL80211_ATTR_TIMESTAMP] = { .type = NLA_U64 },
	[NL80211_ATTR_IE] = { .type = NLA_BINARY, .len = NL80211_MAX_IE_LEN },
	[NL80211_ATTR_AUTH_ALGORITHM] = { .type = NLA_U32 },
	[NL80211_ATTR_TIMEOUT_TU] = { .type = NLA_U32 },
	[NL80211_ATTR_REASON_CODE] = { .type = NLA_U32 },
	[NL80211_ATTR_ASSOCIATION_ID] = { .type = NLA_U16 },
	[NL80211_ATTR_DEAUTHENTICATED] = { .type = NLA_FLAG },
	[NL80211_ATTR_RX_SENSITIVITY] = { .type = NLA_U32 },
	[NL80211_ATTR_TRANSMIT_POWER] = { .type = NLA_U32 },
	[NL80211_ATTR_FRAG_THRESHOLD] = { .type = NLA_U32 },
	[NL80211_ATTR_FLAG_SCAN_ACTIVE] = { .type = NLA_FLAG },
	[NL80211_ATTR_BEACON_HEAD] = { .type = NLA_BINARY },
	[NL80211_ATTR_BEACON_TAIL] = { .type = NLA_BINARY },
	[NL80211_ATTR_KEY_DATA] = { .type = NLA_BINARY,
				    .len = WLAN_MAX_KEY_LEN },
	[NL80211_ATTR_KEY_ID] = { .type = NLA_U32 },
	[NL80211_ATTR_KEY_TYPE] = { .type = NLA_U32 },
	[NL80211_ATTR_MAC] = { .len = ETH_ALEN },
	[NL80211_ATTR_KEY_CIPHER] = { .type = NLA_U32 },
};

/* netlink command implementations */

#define CHECK_CMD(ptr, cmd)				\
	if (drv->ops->ptr)				\
		NLA_PUT_FLAG(msg, NL80211_CMD_##cmd);

static int nl80211_get_cmdlist(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	struct sk_buff *msg;
	void *hdr;
	int err;
	struct nlattr *start;

	drv = cfg80211_get_dev_from_info(info);
	if (IS_ERR(drv))
		return PTR_ERR(drv);

	hdr = nl80211msg_new(&msg, info->snd_pid, info->snd_seq, 0,
			     NL80211_CMD_NEW_CMDLIST);
	if (IS_ERR(hdr)) {
		err = PTR_ERR(hdr);
		goto put_drv;
	}

	NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, drv->idx);

	start = nla_nest_start(msg, NL80211_ATTR_CMDS);
	if (!start)
		goto nla_put_failure;

	/* unconditionally allow some common commands we handle centrally
	 * or where we require the implementation */
	NLA_PUT_FLAG(msg, NL80211_CMD_GET_CMDLIST);
	NLA_PUT_FLAG(msg, NL80211_CMD_GET_WIPHYS);
	NLA_PUT_FLAG(msg, NL80211_CMD_GET_INTERFACES);
	NLA_PUT_FLAG(msg, NL80211_CMD_RENAME_WIPHY);

	CHECK_CMD(add_virtual_intf, ADD_VIRTUAL_INTERFACE);
	CHECK_CMD(del_virtual_intf, DEL_VIRTUAL_INTERFACE);
	CHECK_CMD(associate, ASSOCIATE);
	CHECK_CMD(disassociate, DISASSOCIATE);
	CHECK_CMD(deauth, DEAUTH);
	CHECK_CMD(initiate_scan, INITIATE_SCAN);
	CHECK_CMD(get_association, GET_ASSOCIATION);
	CHECK_CMD(get_auth_list, GET_AUTH_LIST);
	CHECK_CMD(add_key, ADD_KEY);
	CHECK_CMD(del_key, DEL_KEY);

	nla_nest_end(msg, start);

	genlmsg_end(msg, hdr);

	err = genlmsg_unicast(msg, info->snd_pid);
	goto put_drv;

 nla_put_failure:
	err = -ENOBUFS;
	nlmsg_free(msg);
 put_drv:
	cfg80211_put_dev(drv);
	return err;
}
#undef CHECK_CMD

static int nl80211_get_wiphys(struct sk_buff *skb, struct genl_info *info)
{
	struct sk_buff *msg;
	void *hdr;
	struct nlattr *start, *indexstart;
	struct cfg80211_registered_device *drv;
	int idx = 1;

	hdr = nl80211msg_new(&msg, info->snd_pid, info->snd_seq, 0,
			     NL80211_CMD_NEW_WIPHYS);
	if (IS_ERR(hdr))
		return PTR_ERR(hdr);

	start = nla_nest_start(msg, NL80211_ATTR_WIPHY_LIST);
	if (!start)
		goto nla_outer_nest_failure;

	mutex_lock(&cfg80211_drv_mutex);
	list_for_each_entry(drv, &cfg80211_drv_list, list) {
		indexstart = nla_nest_start(msg, idx++);
		if (!indexstart)
			goto nla_put_failure;
		NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, drv->idx);
		nla_nest_end(msg, indexstart);
	}
	mutex_unlock(&cfg80211_drv_mutex);

	nla_nest_end(msg, start);

	genlmsg_end(msg, hdr);

	return genlmsg_unicast(msg, info->snd_pid);

 nla_put_failure:
	mutex_unlock(&cfg80211_drv_mutex);
 nla_outer_nest_failure:
	nlmsg_free(msg);
	return -ENOBUFS;
}

static int addifidx(struct net_device *dev, struct sk_buff *skb, int *idx)
{
	int err = -ENOBUFS;
	struct nlattr *start;

	dev_hold(dev);

	start = nla_nest_start(skb, *idx++);
	if (!start)
		goto nla_put_failure;

	NLA_PUT_U32(skb, NL80211_ATTR_IFINDEX, dev->ifindex);
	NLA_PUT_STRING(skb, NL80211_ATTR_IFNAME, dev->name);

	nla_nest_end(skb, start);
	err = 0;

 nla_put_failure:
	dev_put(dev);
	return err;
}

static int nl80211_get_intfs(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	struct sk_buff *msg;
	void *hdr;
	int err, array_idx;
	struct nlattr *start;
	struct wireless_dev *wdev;

	drv = cfg80211_get_dev_from_info(info);
	if (IS_ERR(drv))
		return PTR_ERR(drv);

	hdr = nl80211msg_new(&msg, info->snd_pid, info->snd_seq, 0,
			     NL80211_CMD_NEW_INTERFACES);
	if (IS_ERR(hdr)) {
		err = PTR_ERR(hdr);
		goto put_drv;
	}

	NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, drv->idx);

	start = nla_nest_start(msg, NL80211_ATTR_INTERFACE_LIST);
	if (!start) {
		err = -ENOBUFS;
		goto msg_free;
	}

	array_idx = 1;
	err = 0;
	mutex_lock(&drv->devlist_mtx);
	list_for_each_entry(wdev, &drv->netdev_list, list) {
		err = addifidx(wdev->netdev, msg, &array_idx);
		if (err)
			break;
	}
	mutex_unlock(&drv->devlist_mtx);
	if (err)
		goto msg_free;

	nla_nest_end(msg, start);

	genlmsg_end(msg, hdr);

	err = genlmsg_unicast(msg, info->snd_pid);
	goto put_drv;

 nla_put_failure:
	err = -ENOBUFS;
 msg_free:
	nlmsg_free(msg);
 put_drv:
	cfg80211_put_dev(drv);
	return err;
}

static int nl80211_add_virt_intf(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	int err;
	enum nl80211_iftype type = NL80211_IFTYPE_UNSPECIFIED;

	if (!info->attrs[NL80211_ATTR_IFNAME])
		return -EINVAL;

	if (info->attrs[NL80211_ATTR_IFTYPE]) {
		type = nla_get_u32(info->attrs[NL80211_ATTR_IFTYPE]);
		if (type > NL80211_IFTYPE_MAX)
			return -EINVAL;
	}

	drv = cfg80211_get_dev_from_info(info);
	if (IS_ERR(drv))
		return PTR_ERR(drv);

	if (!drv->ops->add_virtual_intf) {
		err = -EOPNOTSUPP;
		goto unlock;
	}

	rtnl_lock();
	err = drv->ops->add_virtual_intf(&drv->wiphy,
		nla_data(info->attrs[NL80211_ATTR_IFNAME]), type);
	rtnl_unlock();

 unlock:
	cfg80211_put_dev(drv);
	return err;
}

static int nl80211_del_virt_intf(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	int ifindex, err;
	struct net_device *dev;

	err = get_drv_dev_by_info_ifindex(info, &drv, &dev);
	if (err)
		return err;
	ifindex = dev->ifindex;
	dev_put(dev);

	if (!drv->ops->del_virtual_intf) {
		err = -EOPNOTSUPP;
		goto out;
	}

	rtnl_lock();
	err = drv->ops->del_virtual_intf(&drv->wiphy, ifindex);
	rtnl_unlock();

 out:
	cfg80211_put_dev(drv);
	return err;
}

static int nl80211_change_virt_intf(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	int err, ifindex;
	enum nl80211_iftype type;
	struct net_device *dev;

	if (info->attrs[NL80211_ATTR_IFTYPE]) {
		type = nla_get_u32(info->attrs[NL80211_ATTR_IFTYPE]);
		if (type > NL80211_IFTYPE_MAX)
			return -EINVAL;
	} else
		return -EINVAL;

	err = get_drv_dev_by_info_ifindex(info, &drv, &dev);
	if (err)
		return err;
	ifindex = dev->ifindex;
	dev_put(dev);

	if (!drv->ops->change_virtual_intf) {
		err = -EOPNOTSUPP;
		goto unlock;
	}

	rtnl_lock();
	err = drv->ops->change_virtual_intf(&drv->wiphy, ifindex, type);
	rtnl_unlock();

 unlock:
	cfg80211_put_dev(drv);
	return err;
}

static int nl80211_get_association(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	int err;
	struct net_device *dev;
	struct sk_buff *msg;
	void *hdr;
	u8 bssid[ETH_ALEN];

	err = get_drv_dev_by_info_ifindex(info, &drv, &dev);
	if (err)
		return err;

	if (!drv->ops->get_association) {
		err = -EOPNOTSUPP;
		goto out_put_drv;
	}

	rtnl_lock();
	err = drv->ops->get_association(&drv->wiphy, dev, bssid);
	rtnl_unlock();
	if (err < 0)
		goto out_put_drv;

	hdr = nl80211msg_new(&msg, info->snd_pid, info->snd_seq, 0,
			     NL80211_CMD_ASSOCIATION_CHANGED);

	if (IS_ERR(hdr)) {
		err = PTR_ERR(hdr);
		goto out_put_drv;
	}

	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, dev->ifindex);
	if (err == 1)
		NLA_PUT(msg, NL80211_ATTR_BSSID, ETH_ALEN, bssid);

	genlmsg_end(msg, hdr);
	err = genlmsg_unicast(msg, info->snd_pid);
	goto out_put_drv;

 nla_put_failure:
	err = -ENOBUFS;
	nlmsg_free(msg);
 out_put_drv:
	cfg80211_put_dev(drv);
	dev_put(dev);
	return err;
}

static int nl80211_associate(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	int err;
	struct net_device *dev;
	struct association_params assoc_params;

	memset(&assoc_params, 0, sizeof(assoc_params));

	err = get_drv_dev_by_info_ifindex(info, &drv, &dev);
	if (err)
		return err;

	if (!drv->ops->associate) {
		err = -EOPNOTSUPP;
		goto out;
	}

	if (!info->attrs[NL80211_ATTR_SSID])
		return -EINVAL;

	assoc_params.ssid = nla_data(info->attrs[NL80211_ATTR_SSID]);
	assoc_params.ssid_len = nla_len(info->attrs[NL80211_ATTR_SSID]);

	if (info->attrs[NL80211_ATTR_BSSID])
		assoc_params.bssid = nla_data(info->attrs[NL80211_ATTR_BSSID]);

	if (info->attrs[NL80211_ATTR_IE]) {
		err = check_information_element(info->attrs[NL80211_ATTR_IE]);
		if (err)
			goto out;
		assoc_params.ie = nla_data(info->attrs[NL80211_ATTR_IE]);
		assoc_params.ie_len = nla_len(info->attrs[NL80211_ATTR_IE]);
	}

	if (info->attrs[NL80211_ATTR_TIMEOUT_TU]) {
		assoc_params.timeout =
			nla_get_u32(info->attrs[NL80211_ATTR_TIMEOUT_TU]);
		assoc_params.valid |= ASSOC_PARAMS_TIMEOUT;
	}

	rtnl_lock();
	err = drv->ops->associate(&drv->wiphy, dev, &assoc_params);
	rtnl_unlock();

 out:
	cfg80211_put_dev(drv);
	dev_put(dev);
	return err;
}

static int nl80211_disassoc_deauth(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	int err;
	struct net_device *dev;
	int (*act)(struct wiphy *wiphy, struct net_device *dev);

	err = get_drv_dev_by_info_ifindex(info, &drv, &dev);
	if (err)
		return err;

	switch (info->genlhdr->cmd) {
	case NL80211_CMD_DISASSOCIATE:
		act = drv->ops->disassociate;
		break;
	case NL80211_CMD_DEAUTH:
		act = drv->ops->deauth;
		break;
	default:
		act = NULL;
	}

	if (!act) {
		err = -EOPNOTSUPP;
		goto out;
	}

	rtnl_lock();
	err = act(&drv->wiphy, dev);
	rtnl_unlock();
 out:
	cfg80211_put_dev(drv);
	dev_put(dev);
	return err;
}

struct add_cb_data {
	int idx;
	struct sk_buff *skb;
};

static int add_bssid(void *data, u8 *bssid)
{
	struct add_cb_data *cb = data;
	int err = -ENOBUFS;
	struct nlattr *start;

	start = nla_nest_start(cb->skb, cb->idx++);
	if (!start)
		goto nla_put_failure;

	NLA_PUT(cb->skb, NL80211_ATTR_BSSID, ETH_ALEN, bssid);

	nla_nest_end(cb->skb, start);
	err = 0;

 nla_put_failure:
	return err;
}

static int nl80211_get_auth_list(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	struct net_device *dev;
	struct sk_buff *msg;
	void *hdr;
	int err;
	struct nlattr *start;
	struct add_cb_data cb;

	err = get_drv_dev_by_info_ifindex(info, &drv, &dev);
	if (err)
		return err;

	if (!drv->ops->get_auth_list) {
		err = -EOPNOTSUPP;
		goto put_drv;
	}

	hdr = nl80211msg_new(&msg, info->snd_pid, info->snd_seq, 0,
			     NL80211_CMD_NEW_AUTH_LIST);
	if (IS_ERR(hdr)) {
		err = PTR_ERR(hdr);
		goto put_drv;
	}

	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, dev->ifindex);

	start = nla_nest_start(msg, NL80211_ATTR_BSS_LIST);
	if (!start) {
		err = -ENOBUFS;
		goto msg_free;
	}

	cb.skb = msg;
	cb.idx = 1;
	rtnl_lock();
	err = drv->ops->get_auth_list(&drv->wiphy, dev, &cb, add_bssid);
	rtnl_unlock();
	if (err)
		goto msg_free;

	nla_nest_end(msg, start);

	genlmsg_end(msg, hdr);

	err = genlmsg_unicast(msg, info->snd_pid);
	goto put_drv;

 nla_put_failure:
	err = -ENOBUFS;
 msg_free:
	nlmsg_free(msg);
 put_drv:
	cfg80211_put_dev(drv);
	dev_put(dev);
	return err;
}

static int nl80211_initiate_scan(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	int err;
	struct net_device *dev;
	struct scan_params params;
	struct scan_channel *channels = NULL;
	int count = -1;

	if (info->attrs[NL80211_ATTR_PHYMODE])
		params.phymode = nla_get_u32(info->attrs[NL80211_ATTR_PHYMODE]);

	if (params.phymode > NL80211_PHYMODE_MAX)
		return -EINVAL;

	err = get_drv_dev_by_info_ifindex(info, &drv, &dev);
	if (err)
		return err;

	if (!drv->ops->initiate_scan) {
		err = -EOPNOTSUPP;
		goto out;
	}

	params.active = nla_get_flag(info->attrs[NL80211_ATTR_FLAG_SCAN_ACTIVE]);

	if (info->attrs[NL80211_ATTR_CHANNEL_LIST]) {
		struct nlattr *attr = info->attrs[NL80211_ATTR_CHANNEL_LIST];
		struct nlattr *nla;
		int rem;
		struct nlattr **tb;

		/* let's count first */
		count = 0;
		nla_for_each_attr(nla, nla_data(attr), nla_len(attr), rem)
			count++;

		if (count == 0) {
			/* assume we should actually scan all channels,
			 * scanning no channels make no sense */
			count = -1;
			goto done_channels;
		}

		if (count > NL80211_MAX_CHANNEL_LIST_ITEM) {
			err = -EINVAL;
			goto out;
		}

		channels = kmalloc(count * sizeof(struct scan_channel),
				   GFP_KERNEL);
		tb = kmalloc((NL80211_ATTR_MAX+1) * sizeof(struct nlattr),
			     GFP_KERNEL);

		count = 0;
		nla_for_each_attr(nla, nla_data(attr), nla_len(attr), rem) {
			err = nla_parse(tb, NL80211_ATTR_MAX, nla_data(nla),
					nla_len(nla), nl80211_policy);

			if (err || !tb[NL80211_ATTR_CHANNEL]) {
				err = -EINVAL;
				kfree(tb);
				kfree(channels);
				goto out;
			}

			channels[count].phymode = params.phymode;

			if (tb[NL80211_ATTR_PHYMODE])
				channels[count].phymode =
					nla_get_u32(tb[NL80211_ATTR_PHYMODE]);

			if (channels[count].phymode > NL80211_PHYMODE_MAX) {
				err = -EINVAL;
				kfree(tb);
				kfree(channels);
				goto out;
			}

			channels[count].channel =
				nla_get_u32(tb[NL80211_ATTR_CHANNEL]);

			channels[count].active =
				nla_get_flag(tb[NL80211_ATTR_FLAG_SCAN_ACTIVE]);
			count++;
		}
		kfree(tb);
	}

 done_channels:
	params.channels = channels;
	params.n_channels = count;

	rtnl_lock();
	err = drv->ops->initiate_scan(&drv->wiphy, dev, &params);
	rtnl_unlock();

	kfree(channels);
 out:
	cfg80211_put_dev(drv);
	dev_put(dev);
	return err;
}

static int nl80211_rename_wiphy(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev;
	int result;

	if (!info->attrs[NL80211_ATTR_WIPHY_NAME])
		return -EINVAL;

	rdev = cfg80211_get_dev_from_info(info);
	if (IS_ERR(rdev))
		return PTR_ERR(rdev);

	result = cfg80211_dev_rename(rdev, nla_data(info->attrs[NL80211_ATTR_WIPHY_NAME]));

	cfg80211_put_dev(rdev);
	return result;
}

static int nl80211_key_cmd(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *drv;
	int err, del;
	struct net_device *dev;
	struct key_params params;
	int (*act)(struct wiphy *wiphy, struct net_device *dev,
		   struct key_params *params);

	memset(&params, 0, sizeof(params));

	if (!info->attrs[NL80211_ATTR_KEY_TYPE])
		return -EINVAL;

	if (!info->attrs[NL80211_ATTR_KEY_CIPHER])
		return -EINVAL;

	params.key_type = nla_get_u32(info->attrs[NL80211_ATTR_KEY_TYPE]);
	if (params.key_type > NL80211_KEYTYPE_MAX)
		return -EINVAL;

	err = get_drv_dev_by_info_ifindex(info, &drv, &dev);
	if (err)
		return err;

	switch (info->genlhdr->cmd) {
	case NL80211_CMD_ADD_KEY:
		act = drv->ops->add_key;
		del = 0;
		break;
	case NL80211_CMD_DEL_KEY:
		act = drv->ops->del_key;
		del = 1;
		break;
	default:
		act = NULL;
	}

	if (!act) {
		err = -EOPNOTSUPP;
		goto out;
	}

	if (info->attrs[NL80211_ATTR_KEY_DATA]) {
		params.key = nla_data(info->attrs[NL80211_ATTR_KEY_DATA]);
		params.key_len = nla_len(info->attrs[NL80211_ATTR_KEY_DATA]);
	}

	if (info->attrs[NL80211_ATTR_KEY_ID]) {
		params.key_id = nla_get_u32(info->attrs[NL80211_ATTR_KEY_ID]);
	} else {
		params.key_id = -1;
	}

	params.cipher = nla_get_u32(info->attrs[NL80211_ATTR_KEY_CIPHER]);

	if (info->attrs[NL80211_ATTR_MAC]) {
		params.macaddress = nla_data(info->attrs[NL80211_ATTR_MAC]);
	} else {
		params.macaddress = NULL;
	}

	rtnl_lock();
	err = act(&drv->wiphy, dev, &params);
	rtnl_unlock();

 out:
	cfg80211_put_dev(drv);
	dev_put(dev);
	return err;
}

static struct genl_ops nl80211_ops[] = {
	{
		.cmd = NL80211_CMD_RENAME_WIPHY,
		.doit = nl80211_rename_wiphy,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_GET_CMDLIST,
		.doit = nl80211_get_cmdlist,
		.policy = nl80211_policy,
		/* can be retrieved by unprivileged users */
	},
	{
		.cmd = NL80211_CMD_ADD_VIRTUAL_INTERFACE,
		.doit = nl80211_add_virt_intf,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_DEL_VIRTUAL_INTERFACE,
		.doit = nl80211_del_virt_intf,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_CHANGE_VIRTUAL_INTERFACE,
		.doit = nl80211_change_virt_intf,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_GET_WIPHYS,
		.doit = nl80211_get_wiphys,
		.policy = nl80211_policy,
		/* can be retrieved by unprivileged users */
	},
	{
		.cmd = NL80211_CMD_GET_INTERFACES,
		.doit = nl80211_get_intfs,
		.policy = nl80211_policy,
		/* can be retrieved by unprivileged users */
	},
	{
		.cmd = NL80211_CMD_INITIATE_SCAN,
		.doit = nl80211_initiate_scan,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_GET_ASSOCIATION,
		.doit = nl80211_get_association,
		.policy = nl80211_policy,
		/* can be retrieved by unprivileged users */
	},
	{
		.cmd = NL80211_CMD_ASSOCIATE,
		.doit = nl80211_associate,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_DISASSOCIATE,
		.doit = nl80211_disassoc_deauth,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_DEAUTH,
		.doit = nl80211_disassoc_deauth,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_GET_AUTH_LIST,
		.doit = nl80211_get_auth_list,
		.policy = nl80211_policy,
		/* can be retrieved by unprivileged users */
	},
/*
	{
		.cmd = NL80211_CMD_AP_SET_BEACON,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_AP_ADD_STA,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_AP_UPDATE_STA,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_AP_GET_STA_INFO,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_AP_SET_RATESETS,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
*/
	{
		.cmd = NL80211_CMD_ADD_KEY,
		.doit = nl80211_key_cmd,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NL80211_CMD_DEL_KEY,
		.doit = nl80211_key_cmd,
		.policy = nl80211_policy,
		.flags = GENL_ADMIN_PERM,
	},
};


/* exported functions */

void *nl80211hdr_put(struct sk_buff *skb, u32 pid, u32 seq, int flags, u8 cmd)
{
	/* since there is no private header just add the generic one */
	return genlmsg_put(skb, pid, seq, &nl80211_fam, flags, cmd);
}
EXPORT_SYMBOL_GPL(nl80211hdr_put);

void *nl80211msg_new(struct sk_buff **skb, u32 pid, u32 seq, int flags, u8 cmd)
{
	void *hdr;

	*skb = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!*skb)
		return ERR_PTR(-ENOBUFS);

	hdr = nl80211hdr_put(*skb, pid, seq, flags, cmd);
	if (!hdr) {
		nlmsg_free(*skb);
		return ERR_PTR(-ENOBUFS);
	}

	return hdr;
}
EXPORT_SYMBOL_GPL(nl80211msg_new);

/* notification functions */

void nl80211_notify_dev_rename(struct cfg80211_registered_device *rdev)
{
	struct sk_buff *msg;
	void *hdr;

	hdr = nl80211msg_new(&msg, 0, 0, 0, NL80211_CMD_WIPHY_NEWNAME);
	if (IS_ERR(hdr))
		return;

	NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, rdev->idx);
	NLA_PUT_STRING(msg, NL80211_ATTR_WIPHY_NAME, wiphy_name(&rdev->wiphy));

	genlmsg_end(msg, hdr);
	genlmsg_multicast(msg, 0, NL80211_GROUP_CONFIG, GFP_KERNEL);

	return;

 nla_put_failure:
	nlmsg_free(msg);
}

/* initialisation/exit functions */

int nl80211_init(void)
{
	int err, i;

	err = genl_register_family(&nl80211_fam);
	if (err)
		return err;

	for (i = 0; i < ARRAY_SIZE(nl80211_ops); i++) {
		err = genl_register_ops(&nl80211_fam, &nl80211_ops[i]);
		if (err)
			goto err_out;
	}
	return 0;
 err_out:
	genl_unregister_family(&nl80211_fam);
	return err;
}

void nl80211_exit(void)
{
	genl_unregister_family(&nl80211_fam);
}
