// SPDX-License-Identifier: GPL-2.0
/*
 * IP host config data (ME 134), TCP/UDP config data (ME 136) and the
 * virtual Ethernet interface point (ME 329, VEIP): the router side of
 * OMCI (HGU mode).
 *
 * ME 134 (SDK omci_me_layer3_data_services_nodetable.c:74-94,
 * omci_me_layer3_data_services_func.c):
 *   1 ip options       bit 0 DHCP (IP_OPTION_DHCP_MASK, :20; the SDK
 *                      setter picks the ISP type from it, :2256), bit 1/2
 *                      respond to ping / traceroute (:21-22) - reported
 *                      only, see below, bit 3 IP stack (:23, unused by
 *                      the SDK)
 *                      -> proto dhcp, or static with attributes 4..8
 *                      (proto none while the address is 0.0.0.0); bits
 *                      1/2 are not written anywhere
 *   2 mac addr (R)     the MAC of the interface device (SDK :2314-2366
 *                      reads the WAN interface MAC): netifd
 *                      network.device status "macaddr" (device.c:1572)
 *   3 onu identifier   stored only (generalGetSetFunc)
 *   4..8 address, mask, gateway, primary/secondary DNS: the static
 *                      configuration; checked as the SDK does
 *                      (checkInternalClientIp() :971-1018,
 *                      checkSubnetLegal() :1022-1040, setters :2397-2600)
 *   9..13 current ...  DHCP off: the configured value (:2631), DHCP on:
 *                      what the interface got, here the netifd interface
 *                      status "ipv4-address", "route" (default route
 *                      nexthop) and "dns-server" (iface_status_cb() in ubus.c);
 *                      AVC (SDK avcId OMCI_AVC_ID_IP_HOST_CUR_*)
 *   14 domain name, 15 host name (R, AVC): first "dns-search" entry of the
 *                      interface status, the system host name
 * The ONU creates the instances (SDK omci_me.c: ME access ONU,
 * omciInitInstForIPHostConfigData() creates 0 and 1 with
 * TCSUPPORT_PON_IP_HOST): onu.ip_hosts of them here, ids 0..n-1.  IPv6
 * host config data (G.988 ME 347) is not in the SDK table (ME 347 there is
 * the vendor "reserved 347 ME", TCSUPPORT_VNPTT only): not implemented.
 *
 * Ping / traceroute response (setIPHostConfigDataIPOptions(), SDK :2238-2302):
 * the SDK has the device answer by default and, while the bit is clear, drops
 * the answers itself with iptables rules on the WAN interface of the host:
 *   bit 1 clear   -A OUTPUT -p icmp --icmp-type 0 -o <if> -j DROP   (echo reply)
 *   bit 2 clear   -A OUTPUT -p icmp --icmp-type 11 -o <if> -j DROP  (time exceeded)
 *                 -A INPUT -p udp --dport 32768: -i <if> -j DROP    (probes; not
 *                 with TCSUPPORT_CMCC)
 * and "-D" of the same rules while it is set.
 *
 * Neither direction is imitated: gpond does not read or write
 * /etc/config/firewall, so the firewall of the system has the last word - a
 * host the OLT asks to answer pings answers only if the owner of the system
 * let the answer through, and one it asks to stay silent stays silent
 * whatever the rules of the daemon say.  The ask is only reported:
 * once in the log, in eth_l3_applied(), and in "ubus call xpon eth" with
 * every dump.
 *
 * ME 136: stored (SDK setTcpUdpDataIpHostPtrFunc(), :3197-3243, only
 * stores and notifies the TR-069 client); no Linux meaning.
 *
 * ME 329 (SDK omci_me_ethernet_services_func.c:2087-2330):
 *   1 admin state      0 unlock, 1 lock (other values refused, :2189-2220);
 *                      lock -> its netifd interfaces get option auto 0
 *   2 operational state (R, AVC)  the SDK mirrors the admin state (:2258);
 *                      here also 1 when the interfaces are all down
 *   3 interdomain name, 4 tcp/udp pointer   stored
 *   5 iana assigned port (R)   the SDK reads the TR-069 connection request
 *                      port (:2301); here the port id of the ME 136 the
 *                      tcp/udp pointer names, 0xffff without
 *   alarm 0 connecting function fail: raised with the lock (SDK :2236-2250)
 *                      and when every interface of the VEIP is down
 * The ONU creates the VEIP 0x0a01 (VEIP_SLOT/VEIP_INST_ID, omci_me.h:54,93;
 * omciInitInstForVirtualEthernetInterfacePoint(), :2175).
 * Its WAN VLAN devices (ME 47 type 11 bridge port / ME 171
 * association type 10) become netifd interfaces xpon_wan<N>, proto
 * onu.veip_proto (default dhcp), plus xpon_wan<N>_6 (dhcpv6) with
 * onu.veip_ipv6.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <uci.h>

#include "gpond.h"
#include "eth.h"

/* ME 134 attributes */
#define IPH_OPTIONS		1
#define IPH_MAC			2
#define IPH_ADDR		4
#define IPH_MASK		5
#define IPH_GW			6
#define IPH_DNS1		7
#define IPH_DNS2		8
#define IPH_CUR_ADDR		9
#define IPH_CUR_MASK		10
#define IPH_CUR_GW		11
#define IPH_CUR_DNS1		12
#define IPH_CUR_DNS2		13
#define IPH_DOMAIN		14
#define IPH_HOST		15
#define IPH_OPT_DHCP		0x01
#define IPH_OPT_PING		0x02	/* IP_OPTION_ENABLE_PING_RESPONSE_MASK */
#define IPH_OPT_TRACE		0x04	/* IP_OPTION_ENABLE_TRACERT_RESPONSE_MASK */

/* ME 329 attributes */
#define VEIP_ADMIN		1
#define VEIP_OPER		2
#define VEIP_TCPUDP		4
#define VEIP_IANA		5
#define VEIP_ALM_FAIL		0

#ifndef L3_POLL_MS
#define L3_POLL_MS		2000
#endif
#define L3_DOWN_POLLS		3	/* VEIP down this long before the alarm */

static struct eth_l3 live[ETH_MAX_L3];
static int nlive;
static struct uloop_timeout poll_tmo;
static bool warned_response;	/* the ping/traceroute ask was logged once */
static int veip_down_polls;

/* device an IP host / VEIP instance sits on, NULL if unknown */
const char *eth_l3_dev(uint16_t class_id, uint16_t me)
{
	int i;

	for (i = 0; i < nlive; i++)
		if (live[i].class_id == class_id && live[i].me == me)
			return live[i].dev;

	return NULL;
}

/* ---------------------------------------------------------------------
 * Set value checks (SDK checkInternalClientIp() / checkSubnetLegal())
 * ------------------------------------------------------------------- */

/* 0.0.0.0, or a unicast host address: first octet 1..223 except 127,
 * last octet not 255 (omci_me_layer3_data_services_func.c:971-1018) */
static bool ip_ok(const uint8_t *a)
{
	if (!a[0] && !a[1] && !a[2] && !a[3])
		return true;

	return a[0] && a[0] != 127 && a[0] <= 223 && a[3] != 255;
}

/* a contiguous mask (:1022-1040) */
static bool mask_ok(const uint8_t *m)
{
	uint32_t v = (uint32_t)m[0] << 24 | m[1] << 16 | m[2] << 8 | m[3];

	return !(~v & (~v + 1));	/* ~v + 1 is a power of two or 0 */
}

bool eth_l3_check(struct omci_inst *in, uint8_t idx, const uint8_t *v)
{
	switch (in->def->class_id) {
	case CLASS_IP_HOST:
		switch (idx) {
		case IPH_ADDR:
		case IPH_GW:
		case IPH_DNS1:
		case IPH_DNS2:
			return ip_ok(v);
		case IPH_MASK:
			return mask_ok(v);
		default:
			return true;
		}
	case CLASS_VEIP:
	case CLASS_TR069:
		return idx != VEIP_ADMIN || v[0] <= 1;
	default:
		return true;
	}
}

/* ---------------------------------------------------------------------
 * model
 * ------------------------------------------------------------------- */

/* a netifd proto name: lower case, digits, '_' */
static bool proto_ok(const char *s)
{
	if (!*s)
		return false;
	for (; *s; s++)
		if (!(*s >= 'a' && *s <= 'z') && !(*s >= '0' && *s <= '9') &&
		    *s != '_')
			return false;

	return true;
}

/* an existing admin-defined interface in /etc/config/network for @dev */
static bool find_admin_iface(const char *dev, char *name, size_t nlen,
			     char *proto, size_t plen)
{
	struct uci_context *ctx;
	struct uci_package *pkg = NULL;
	struct uci_element *e;
	bool found = false;

	ctx = uci_alloc_context();
	if (!ctx)
		return false;
	if (uci_load(ctx, "network", &pkg)) {
		uci_free_context(ctx);
		return false;
	}
	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);
		const char *d, *p;

		if (strcmp(s->type, "interface") || persist_ours(ctx, s))
			continue;
		d = uci_lookup_option_string(ctx, s, "device");
		if (!d)
			d = uci_lookup_option_string(ctx, s, "ifname");
		if (!d || strcmp(d, dev))
			continue;
		p = uci_lookup_option_string(ctx, s, "proto");
		if (p && !strcmp(p, "dhcpv6"))
			continue;
		snprintf(name, nlen, "%s", s->e.name);
		if (proto && plen)
			snprintf(proto, plen, "%s", p ? p : "none");
		found = true;
		break;
	}
	uci_unload(ctx, pkg);
	uci_free_context(ctx);

	return found;
}

struct eth_l3 *eth_l3_add(struct eth_model *m, uint16_t class_id, uint16_t me,
			  const char *dev)
{
	struct omci_inst *in = mib_find(class_id, me);
	struct eth_l3 *l;
	int i, idx = 0;

	if (!in || m->nl3 >= ETH_MAX_L3)
		return NULL;
	/* only one L3 entry per device */
	for (i = 0; i < m->nl3; i++)
		if (!strcmp(m->l3[i].dev, dev))
			return &m->l3[i];
	for (i = 0; i < m->nl3; i++)
		idx += m->l3[i].class_id == class_id &&
		       (class_id == CLASS_VEIP || m->l3[i].me == me);

	l = &m->l3[m->nl3++];
	memset(l, 0, sizeof(*l));
	l->class_id = class_id;
	l->me = me;
	snprintf(l->dev, sizeof(l->dev), "%s", dev);

	if (class_id == CLASS_VEIP) {
		l->disabled = mib_get_uint(in, VEIP_ADMIN) == 1;
		if (find_admin_iface(dev, l->name, sizeof(l->name),
				     l->proto, sizeof(l->proto)))
			return l;
		snprintf(l->name, sizeof(l->name), "gpon_wan%d", idx);
		snprintf(l->proto, sizeof(l->proto), "%s",
			 proto_ok(cfg.veip_proto) ? cfg.veip_proto : "dhcp");
		l->ipv6 = cfg.veip_ipv6;
		return l;
	}

	if (!find_admin_iface(dev, l->name, sizeof(l->name), NULL, 0))
		snprintf(l->name, sizeof(l->name),
			 idx ? "gpon_iph%x_%d" : "gpon_iph%x", me, idx);
	l->ping = mib_get_uint(in, IPH_OPTIONS) & IPH_OPT_PING;
	l->trace = mib_get_uint(in, IPH_OPTIONS) & IPH_OPT_TRACE;
	if (mib_get_uint(in, IPH_OPTIONS) & IPH_OPT_DHCP) {
		snprintf(l->proto, sizeof(l->proto), "dhcp");
		return l;
	}
	memcpy(l->addr, mib_val(in, IPH_ADDR), 4);
	memcpy(l->mask, mib_val(in, IPH_MASK), 4);
	memcpy(l->gw, mib_val(in, IPH_GW), 4);
	memcpy(l->dns[0], mib_val(in, IPH_DNS1), 4);
	memcpy(l->dns[1], mib_val(in, IPH_DNS2), 4);
	snprintf(l->proto, sizeof(l->proto), "%s",
		 is_zero4(l->addr) ? "none" : "static");

	return l;
}

static int add_ip4(struct uci_context *ctx, const char *sn, const char *opt,
		   const uint8_t *a, bool list)
{
	char s[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, a, s, sizeof(s));

	return list ? persist_add_list(ctx, "network", sn, opt, s) :
		      persist_set(ctx, "network", sn, opt, s);
}

/* netifd interface sections (netifd interface.c iface_attrs[], proto.c
 * OPT_IPADDR/NETMASK/GATEWAY, interface "dns") */
int eth_l3_uci(struct uci_context *ctx, struct uci_package *pkg,
	       const struct eth_l3 *l)
{
	struct uci_section *existing;
	char sn6[sizeof(l->name) + 4];
	bool admin_sect;
	int ret = 0;

	existing = uci_lookup_section(ctx, pkg, l->name);
	admin_sect = existing && !persist_ours(ctx, existing);

	/* Never overwrite an interface section defined by the admin (e.g.
	 * 'wan' with PPPoE/DHCP): leave it completely alone and only
	 * monitor its status for VEIP / IP host */
	if (admin_sect)
		return 0;

#define SET(_n, _o, _v)	ret = ret ? ret : persist_set(ctx, "network", _n, _o, _v)
	SET(l->name, NULL, "interface");
	ret = ret ? ret : persist_comment(ctx, "network", l->name,
					  "gpond me %u 0x%04x",
					  l->class_id, l->me);
	SET(l->name, "device", l->dev);
	SET(l->name, "proto", l->proto);
	if (l->disabled)
		SET(l->name, "auto", "0");
	if (!ret && !strcmp(l->proto, "static")) {
		uint32_t m = (uint32_t)l->mask[0] << 24 | l->mask[1] << 16 |
			     l->mask[2] << 8 | l->mask[3];
		char s[INET_ADDRSTRLEN];
		int i;

		ret = add_ip4(ctx, l->name, "ipaddr", l->addr, true);
		if (!ret && m) {
			snprintf(s, sizeof(s), "%u.%u.%u.%u", l->mask[0],
				 l->mask[1], l->mask[2], l->mask[3]);
			SET(l->name, "netmask", s);
		}
		if (!ret && !is_zero4(l->gw))
			ret = add_ip4(ctx, l->name, "gateway", l->gw, false);
		for (i = 0; i < 2 && !ret; i++)
			if (!is_zero4(l->dns[i]))
				ret = add_ip4(ctx, l->name, "dns", l->dns[i], true);
	}
	if (!ret && l->ipv6) {
		snprintf(sn6, sizeof(sn6), "%s_6", l->name);
		SET(sn6, NULL, "interface");
		ret = ret ? ret : persist_comment(ctx, "network", sn6,
						  "gpond me %u 0x%04x ipv6",
						  l->class_id, l->me);
		SET(sn6, "device", l->dev);
		SET(sn6, "proto", "dhcpv6");
		if (l->disabled)
			SET(sn6, "auto", "0");
	}
#undef SET

	return ret;
}

/* ---------------------------------------------------------------------
 * TR-069 / CWMP (ME 340 -> ME 137 -> ME 157 + ME 148) -> /etc/config/easycwmp
 * ------------------------------------------------------------------- */

static void read_large_string(struct omci_inst *ls, char *out, size_t maxlen)
{
	uint8_t nparts = ls ? mib_get_uint(ls, 1) : 0;
	size_t off = 0;
	unsigned int p, n;

	out[0] = 0;
	if (!ls)
		return;
	if (nparts > 15)
		nparts = 15;
	for (p = 0; p < (nparts ? nparts : 15); p++) {
		const uint8_t *part = mib_val(ls, 2 + p);

		if (!part)
			break;
		if (!nparts && !part[0])
			break;
		for (n = 0; n < 25 && part[n] && off + 1 < maxlen; n++)
			out[off++] = (char)part[n];
		if (n < 25)
			break;
	}
	out[off] = 0;
}

void eth_cwmp_build(struct eth_model *m)
{
	struct eth_cwmp *cw = &m->cwmp;
	struct omci_inst *tr = NULL, *in, *na, *ls, *au, *veip, *tu;
	uint16_t na_id, tag;
	char suffix[16];
	int i;

	memset(cw, 0, sizeof(*cw));
	list_for_each_entry(in, &omci_mib, list) {
		if (in->def->class_id != CLASS_TR069)
			continue;
		if (in->by_olt || in->olt_mask) {
			tr = in;
			break;
		}
	}
	if (!tr)
		return;

	cw->present = true;
	cw->me = tr->id;
	cw->enabled = mib_get_uint(tr, 1) == 0;
	na_id = mib_get_uint(tr, 2);
	tag = mib_get_uint(tr, 3);
	if (tag && tag != 0xffff) {
		cw->vid = tag & 0xfff;
		cw->pbit = (tag >> 13) & 7;
	}

	na = (na_id && na_id != 0xffff) ? mib_find(CLASS_NET_ADDR, na_id) : NULL;
	if (na) {
		uint16_t sec_id = mib_get_uint(na, 1);
		uint16_t str_id = mib_get_uint(na, 2);

		ls = (str_id && str_id != 0xffff) ?
		     mib_find(CLASS_LARGE_STR, str_id) : NULL;
		read_large_string(ls, cw->url, sizeof(cw->url));

		au = (sec_id && sec_id != 0xffff) ?
		     mib_find(CLASS_AUTH_METHOD, sec_id) : NULL;
		if (au) {
			const uint8_t *u1 = mib_val(au, 2), *u2 = mib_val(au, 5);
			const uint8_t *pw = mib_val(au, 3), *rm = mib_val(au, 4);
			size_t l1 = 0;

			cw->auth_scheme = mib_get_uint(au, 1);
			if (u1) {
				for (l1 = 0; l1 < 25 && u1[l1]; l1++)
					cw->username[l1] = (char)u1[l1];
				cw->username[l1] = 0;
			}
			if (l1 == 25 && u2 && u2[0]) {
				size_t l2;

				for (l2 = 0; l2 < 25 && u2[l2]; l2++)
					cw->username[25 + l2] = (char)u2[l2];
				cw->username[25 + l2] = 0;
			}
			if (pw) {
				memcpy(cw->password, pw, 25);
				cw->password[25] = 0;
			}
			if (rm) {
				memcpy(cw->realm, rm, 25);
				cw->realm[25] = 0;
			}
		}
	}

	veip = mib_find(CLASS_VEIP, tr->id);
	if (veip) {
		uint16_t tu_id = mib_get_uint(veip, VEIP_TCPUDP);

		tu = (tu_id && tu_id != 0xffff) ?
		     mib_find(CLASS_TCP_UDP, tu_id) : NULL;
		if (tu)
			cw->port = mib_get_uint(tu, 1);
	} else {
		tu = mib_find(CLASS_TCP_UDP, tr->id);
		if (tu)
			cw->port = mib_get_uint(tu, 1);
	}

	snprintf(suffix, sizeof(suffix), ".%u", cw->vid);
	for (i = 0; i < m->nl3; i++) {
		const struct eth_l3 *l = &m->l3[i];
		size_t dl = strlen(l->dev), sl = strlen(suffix);

		if (!cw->iface[0])
			snprintf(cw->iface, sizeof(cw->iface), "%s", l->name);
		if (cw->vid && dl >= sl && !strcmp(l->dev + dl - sl, suffix)) {
			snprintf(cw->iface, sizeof(cw->iface), "%s", l->name);
			break;
		}
	}
}

static const char *cwmp_sect(struct uci_context *ctx, struct uci_package *pkg,
			     const char *type, uint16_t me, int *ret)
{
	struct uci_element *e;

	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);

		if (!strcmp(s->type, type) && !persist_ours(ctx, s))
			return s->e.name;
	}
	*ret = *ret ? *ret : persist_set(ctx, CWMP_PKG, type, NULL, type);
	*ret = *ret ? *ret : persist_comment(ctx, CWMP_PKG, type,
					     "gpond me 340 0x%04x", me);
	return type;
}

int eth_cwmp_write(const struct eth_model *m, bool *changed)
{
	const struct eth_cwmp *cw = &m->cwmp;
	struct uci_package *pkg;
	struct uci_context *ctx;
	struct uci_element *e, *tmp;
	char *before, *after;
	bool ch;
	int ret = 0;

	ctx = persist_open(CWMP_PKG, &pkg);
	if (!ctx)
		return -EIO;
	before = persist_snapshot(ctx, pkg);

	uci_foreach_element_safe(&pkg->sections, tmp, e) {
		struct uci_section *s = uci_to_section(e);

		if (persist_ours(ctx, s))
			persist_set(ctx, CWMP_PKG, s->e.name, NULL, NULL);
	}

	if (cw->present) {
		const char *loc = cwmp_sect(ctx, pkg, "local", cw->me, &ret);
		const char *acs = cwmp_sect(ctx, pkg, "acs", cw->me, &ret);
		char pbuf[16];

		ret = ret ? ret : persist_set(ctx, CWMP_PKG, loc, "enable",
					      cw->enabled ? "1" : "0");
		if (!ret && cw->iface[0])
			ret = persist_set(ctx, CWMP_PKG, loc, "interface",
					  cw->iface);
		if (!ret && cw->port) {
			snprintf(pbuf, sizeof(pbuf), "%u", cw->port);
			ret = persist_set(ctx, CWMP_PKG, loc, "port", pbuf);
		}
		if (!ret && cw->url[0])
			ret = persist_set(ctx, CWMP_PKG, acs, "url", cw->url);
		if (!ret && cw->username[0])
			ret = persist_set(ctx, CWMP_PKG, acs, "username",
					  cw->username);
		if (!ret && cw->password[0])
			ret = persist_set(ctx, CWMP_PKG, acs, "password",
					  cw->password);
	}

	after = persist_snapshot(ctx, pkg);
	ch = !before || !after || strcmp(before, after);
	free(before);
	free(after);
	if (changed)
		*changed = ch;

	if (ret || !ch) {
		persist_close(ctx, pkg, false);
		return ret;
	}

	ret = persist_close(ctx, pkg, true);
	if (!ret && cw->present && cw->enabled &&
	    !access("/etc/init.d/easycwmp", X_OK)) {
		if (system("/etc/init.d/easycwmp reload >/dev/null 2>&1 || "
			   "/etc/init.d/easycwmp start >/dev/null 2>&1"))
			WARN("eth: starting /etc/init.d/easycwmp failed");
	}

	return ret;
}

/* ---------------------------------------------------------------------
 * ping / traceroute response (ME 134 IP options bits 1 and 2)
 *
 * Nothing is written for it.  The vendor (SDK :2238-2302) drops or restores
 * the answers itself with iptables rules on the WAN interface of the host:
 *   bit 1 clear   -A OUTPUT -p icmp --icmp-type 0 -o <if> -j DROP   (echo reply)
 *   bit 2 clear   -A OUTPUT -p icmp --icmp-type 11 -o <if> -j DROP  (time exceeded)
 *                 -A INPUT -p udp --dport 32768: -i <if> -j DROP    (probes)
 * Here neither direction is imitated: gpond does not read or write
 * /etc/config/firewall, so a host that the OLT asks to answer pings answers
 * only if the firewall of the system lets it through.  The rules of the
 * system come first, and the ask is only reported - in the log once, in
 * eth_l3_applied(), and in "ubus call xpon eth" on every dump.
 * ------------------------------------------------------------------- */

/* reported only: no firewall is read and none is written */

void eth_l3_dump(struct blob_buf *b)
{
	const struct eth_model *m = eth_current();
	void *a, *t;
	int i;

	a = blobmsg_open_array(b, "l3");
	for (i = 0; i < m->nl3; i++) {
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_string(b, "interface", m->l3[i].name);
		blobmsg_add_string(b, "device", m->l3[i].dev);
		blobmsg_add_string(b, "proto", m->l3[i].proto);
		blobmsg_add_u32(b, "class", m->l3[i].class_id);
		blobmsg_add_u32(b, "me", m->l3[i].me);
		blobmsg_add_u8(b, "enabled", !m->l3[i].disabled);
		if (m->l3[i].class_id == CLASS_IP_HOST) {
			blobmsg_add_u8(b, "ping_response", m->l3[i].ping);
			blobmsg_add_u8(b, "traceroute_response", m->l3[i].trace);
		}
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);
	if (m->cwmp.present) {
		t = blobmsg_open_table(b, "cwmp");
		blobmsg_add_u32(b, "me", m->cwmp.me);
		blobmsg_add_u8(b, "enabled", m->cwmp.enabled);
		if (m->cwmp.iface[0])
			blobmsg_add_string(b, "interface", m->cwmp.iface);
		if (m->cwmp.port)
			blobmsg_add_u32(b, "port", m->cwmp.port);
		if (m->cwmp.vid)
			blobmsg_add_u32(b, "vid", m->cwmp.vid);
		if (m->cwmp.url[0])
			blobmsg_add_string(b, "url", m->cwmp.url);
		if (m->cwmp.username[0])
			blobmsg_add_string(b, "username", m->cwmp.username);
		blobmsg_close_table(b, t);
	}
}

/* ---------------------------------------------------------------------
 * state back into the MIB
 * ------------------------------------------------------------------- */

static void set_ip(struct omci_inst *in, uint8_t idx, const uint8_t *a)
{
	omci_onu_set(in, idx, a, 4);
}

static void ip_host_update(struct omci_inst *in, const struct eth_l3 *l)
{
	struct gpon_l3_status st;
	struct gpon_link lk;
	char host[26] = "";
	bool dhcp = mib_get_uint(in, IPH_OPTIONS) & IPH_OPT_DHCP;

	memset(&st, 0, sizeof(st));
	if (ubus_device_status(l->dev, &lk) == 0 && lk.mac_valid)
		mib_set_raw(in, IPH_MAC, lk.mac, 6);
	if (dhcp)
		ubus_iface_status(l->name, &st);

	if (!dhcp) {
		/* SDK: current values are the configured ones */
		set_ip(in, IPH_CUR_ADDR, mib_val(in, IPH_ADDR));
		set_ip(in, IPH_CUR_MASK, mib_val(in, IPH_MASK));
		set_ip(in, IPH_CUR_GW, mib_val(in, IPH_GW));
		set_ip(in, IPH_CUR_DNS1, mib_val(in, IPH_DNS1));
		set_ip(in, IPH_CUR_DNS2, mib_val(in, IPH_DNS2));
	} else if (st.valid) {
		set_ip(in, IPH_CUR_ADDR, st.addr);
		set_ip(in, IPH_CUR_MASK, st.mask);
		set_ip(in, IPH_CUR_GW, st.gw);
		set_ip(in, IPH_CUR_DNS1, st.dns[0]);
		set_ip(in, IPH_CUR_DNS2, st.dns[1]);
	}
	if (st.valid)
		omci_onu_set_str(in, IPH_DOMAIN, st.domain);
	if (!gethostname(host, sizeof(host) - 1))
		omci_onu_set_str(in, IPH_HOST, host);
}

static void veip_alarm(struct omci_inst *in, bool fail, bool notify)
{
	omci_alarm_set(CLASS_VEIP, in->id, VEIP_ALM_FAIL, fail, notify);
	omci_onu_set_uint(in, VEIP_OPER, fail);
}

static void veip_update(struct omci_inst *in)
{
	struct omci_inst *tu = mib_find(CLASS_TCP_UDP, mib_get_uint(in, VEIP_TCPUDP));
	int i, n = 0, up = 0;
	bool down;

	/* iana assigned port: the port of the ME 136 it points to */
	mib_set_uint(in, VEIP_IANA, tu ? mib_get_uint(tu, 1) : 0xffff);

	if (mib_get_uint(in, VEIP_ADMIN) == 1) {
		veip_down_polls = 0;
		return;				/* eth_l3_admin() did it */
	}
	for (i = 0; i < nlive; i++) {
		struct gpon_l3_status st;

		if (live[i].class_id != CLASS_VEIP || live[i].me != in->id)
			continue;
		n++;
		if (!ubus_iface_status(live[i].name, &st) &&
		    st.valid && (st.up || st.pending))
			up++;
	}
	down = n && !up;
	veip_down_polls = down ? veip_down_polls + 1 : 0;
	veip_alarm(in, down && veip_down_polls >= L3_DOWN_POLLS, true);
}

/* the OLT changed the admin state of a VEIP: the SDK mirrors it at once */
void eth_l3_admin(struct omci_inst *in)
{
	if (in->def->class_id != CLASS_VEIP)
		return;
	veip_down_polls = 0;
	/* what the OLT does not see while the VEIP is locked is not reported
	 * at the unlock either (the unlock walk sends the active alarms) */
	veip_alarm(in, mib_get_uint(in, VEIP_ADMIN) == 1, false);
}

static void poll_cb(struct uloop_timeout *t)
{
	struct omci_inst *in;
	int i;

	uloop_timeout_set(t, L3_POLL_MS);
	for (i = 0; i < nlive; i++) {
		if (live[i].class_id != CLASS_IP_HOST)
			continue;
		/* the first interface of an instance */
		if (i && live[i - 1].class_id == CLASS_IP_HOST &&
		    live[i - 1].me == live[i].me)
			continue;
		in = mib_find(CLASS_IP_HOST, live[i].me);
		if (in)
			ip_host_update(in, &live[i]);
	}
	in = mib_find(CLASS_VEIP, VEIP_ME_ID);
	if (in)
		veip_update(in);
}

void eth_l3_applied(const struct eth_model *m)
{
	memcpy(live, m->l3, sizeof(live));
	nlive = m->nl3;

	/*
	 * The OLT asks the host to answer ping / traceroute (ME 134, IP
	 * options bits 1 and 2).  gpond does not arrange that: it does not
	 * write /etc/config/firewall at all, neither to answer nor to stay
	 * silent.  What the firewall of the system does is what the host does,
	 * and the owner of the system decides.  Say it out loud when the ask
	 * cannot be honoured here, so a host that does not answer has a
	 * reason in the log; the state itself is in "ubus call xpon eth"
	 * (ping_response / traceroute_response).
	 */
	if (!warned_response) {
		int i;

		for (i = 0; i < m->nl3; i++)
			if (m->l3[i].class_id == CLASS_IP_HOST &&
			    (m->l3[i].ping || m->l3[i].trace)) {
				warned_response = true;
				WARN("eth: IP host %s (%s) is asked to answer %s, "
				     "but gpond does not touch /etc/config/"
				     "firewall: whether it answers is the "
				     "firewall configuration of the system",
				     m->l3[i].name, m->l3[i].dev,
				     m->l3[i].ping
				     ? (m->l3[i].trace ? "ping and traceroute"
						        : "ping")
				     : "traceroute");
				break;
			}
	}
}

void eth_l3_mib_reset(void)
{
	nlive = 0;
	veip_down_polls = 0;
}

void eth_l3_init(void)
{
	poll_tmo.cb = poll_cb;
	uloop_timeout_set(&poll_tmo, L3_POLL_MS);
}
