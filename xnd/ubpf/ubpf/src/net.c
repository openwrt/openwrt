/* SPDX-License-Identifier: GPL-2.0 */

#define _GNU_SOURCE

#include <linux/rtnetlink.h>

#include <unl.h>

#include "uxdp.h"

static struct unl unl;

static int
getlink_recv(struct nl_msg *msg, void *arg)
{
	struct xdp_map *lookup = (struct xdp_map *) arg;
	struct ifinfomsg *ifi = nlmsg_data(msg->nm_nlh);
	struct nlattr *tb[IFLA_MAX + 1] = {};
	struct nlattr *xdp[IFLA_XDP_MAX + 1] = {};
	unsigned char mode;
	char *ifname;

	if (!nlmsg_valid_hdr(msg->nm_nlh, sizeof(*ifi)) || ifi->ifi_family != AF_UNSPEC)
		return NL_SKIP;

	nlmsg_parse(msg->nm_nlh, sizeof(*ifi), tb, __IFLA_MAX - 1, NULL);
	if (!tb[IFLA_IFNAME] || !tb[IFLA_XDP])
		return NL_SKIP;
	ifname = nla_get_string(tb[IFLA_IFNAME]);
	if (nla_parse_nested(xdp, IFLA_XDP_MAX, tb[IFLA_XDP], NULL))
		return NL_SKIP;

	if (!xdp[IFLA_XDP_ATTACHED])
		return NL_SKIP;

	mode = nla_get_u8(xdp[IFLA_XDP_ATTACHED]);
	if (mode == XDP_ATTACHED_NONE)
		return NL_SKIP;
	switch(mode) {
	case XDP_ATTACHED_MULTI:
		fprintf(stderr, "%s:%s[%d]XDP_ATTACHED_MULTI not supported yet\n", __FILE__, __func__, __LINE__);
		break;
	case XDP_ATTACHED_DRV:
	case XDP_ATTACHED_SKB:
	case XDP_ATTACHED_HW:
		if (!lookup)
			fprintf(stderr, "%s\n", ifname);
		prog_find(lookup, nla_get_u32(xdp[IFLA_XDP_PROG_ID]));
		break;
	}
	return NL_OK;
}

void
net_find(struct xdp_map *lookup)
{
	struct nl_msg *msg;

	msg = unl_rtnl_msg(&unl, RTM_GETLINK, true);
	msg->nm_nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	unl_request(&unl, msg, getlink_recv, lookup);
}

static void __attribute__((constructor))
rtnl_init(void)
{
	if (unl_rtnl_init(&unl))
		fprintf(stderr, "failed to open rtnl\n");
}
