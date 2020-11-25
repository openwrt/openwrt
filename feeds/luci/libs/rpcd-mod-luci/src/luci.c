/*
 * luci - LuCI core functions plugin for rpcd
 *
 *   Copyright (C) 2019 Jo-Philipp Wich <jo@mein.io>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <dlfcn.h>
#include <time.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/ether.h>
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>

#include <netlink/msg.h>
#include <netlink/attr.h>
#include <netlink/socket.h>

#include <libubus.h>
#include <libubox/avl.h>
#include <libubox/avl-cmp.h>
#include <libubox/usock.h>
#include <libubox/uloop.h>

#include <uci.h>

#include <iwinfo.h>

#include <rpcd/plugin.h>


static struct blob_buf blob;

struct reply_context {
	struct ubus_context *context;
	struct ubus_request_data request;
	struct uloop_timeout timeout;
	struct blob_buf blob;
	struct avl_tree avl;
	int pending;
};

struct invoke_context {
	struct ubus_request request;
	struct uloop_timeout timeout;
	struct ubus_context *context;
	void (*cb)(struct ubus_request *, int, struct blob_attr *);
	void *priv;
};

static const char **iw_modenames;
static struct iwinfo_ops *(*iw_backend)(const char *);
static void (*iw_close)(void);

static void
invoke_data_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct invoke_context *ictx =
		container_of(req, struct invoke_context, request);

	if (ictx->cb != NULL)
		ictx->cb(req, type, msg);

	ictx->cb = NULL;
}

static void
invoke_done_cb(struct ubus_request *req, int ret)
{
	struct invoke_context *ictx =
		container_of(req, struct invoke_context, request);

	if (ictx->cb != NULL)
		ictx->cb(req, -1, NULL);

	uloop_timeout_cancel(&ictx->timeout);
	free(ictx);
}

static void
invoke_timeout_cb(struct uloop_timeout *timeout)
{
	struct invoke_context *ictx =
		container_of(timeout, struct invoke_context, timeout);

	if (ictx->cb != NULL)
		ictx->cb(&ictx->request, -1, NULL);

	ubus_abort_request(ictx->context, &ictx->request);
	free(ictx);
}

static struct reply_context *
defer_request(struct ubus_context *ctx, struct ubus_request_data *req)
{
	struct reply_context *rctx;

	rctx = calloc(1, sizeof(*rctx));

	if (!rctx)
		return NULL;

	rctx->context = ctx;
	blob_buf_init(&rctx->blob, 0);
	ubus_defer_request(ctx, req, &rctx->request);

	return rctx;
}

static int
finish_request(struct reply_context *rctx, int status)
{
	if (status == UBUS_STATUS_OK)
		ubus_send_reply(rctx->context, &rctx->request, rctx->blob.head);

	ubus_complete_deferred_request(rctx->context, &rctx->request, status);
	blob_buf_free(&rctx->blob);
	free(rctx);

	return status;
}

static bool
invoke_ubus(struct ubus_context *ctx, const char *object, const char *method,
            struct blob_buf *req,
            void (*cb)(struct ubus_request *, int, struct blob_attr *),
            void *priv)
{
	struct invoke_context *ictx;
	struct blob_buf empty = {};
	uint32_t id;
	bool rv;

	if (ubus_lookup_id(ctx, object, &id))
		return false;

	if (req == NULL) {
		blob_buf_init(&empty, 0);
		req = &empty;
	}

	ictx = calloc(1, sizeof(*ictx));

	if (ictx == NULL)
		return false;

	ictx->context = ctx;
	rv = !ubus_invoke_async(ctx, id, method, req->head, &ictx->request);

	if (rv) {
		ictx->cb = cb;
		ictx->request.priv = priv;
		ictx->request.data_cb = invoke_data_cb;
		ictx->request.complete_cb = invoke_done_cb;
		ubus_complete_request_async(ctx, &ictx->request);

		ictx->timeout.cb = invoke_timeout_cb;
		uloop_timeout_set(&ictx->timeout, 2000);
	}
	else {
		if (cb != NULL)
			cb(&ictx->request, -1, NULL);

		free(ictx);
	}

	if (req == &empty)
		blob_buf_free(req);

	return rv;
}

static char *
readstr(const char *fmt, ...)
{
	static char data[128];
	char path[128];
	va_list ap;
	size_t n;
	FILE *f;

	va_start(ap, fmt);
	vsnprintf(path, sizeof(path), fmt, ap);
	va_end(ap);

	data[0] = 0;
	f = fopen(path, "r");

	if (f != NULL) {
		n = fread(data, 1, sizeof(data) - 1, f);
		data[n] = 0;

		while (n > 0 && isspace(data[n-1]))
			data[--n] = 0;

		fclose(f);
	}

	return data;
}

static char *
ea2str(struct ether_addr *ea)
{
	static char mac[18];

	if (!ea)
		return NULL;

	snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
	         ea->ether_addr_octet[0], ea->ether_addr_octet[1],
	         ea->ether_addr_octet[2], ea->ether_addr_octet[3],
	         ea->ether_addr_octet[4], ea->ether_addr_octet[5]);

	return mac;
}

static char *
sa2str(struct sockaddr *sa)
{
	static char buf[INET6_ADDRSTRLEN];
	union {
		struct sockaddr_in6 *in6;
		struct sockaddr_in *in;
		struct sockaddr_ll *ll;
		struct sockaddr *sa;
	} s;

	s.sa = sa;

	if (s.sa->sa_family == AF_INET)
		inet_ntop(sa->sa_family, &s.in->sin_addr, buf, sizeof(buf));
	else if (s.sa->sa_family == AF_INET6)
		inet_ntop(sa->sa_family, &s.in6->sin6_addr, buf, sizeof(buf));
	else if (s.sa->sa_family == AF_PACKET)
		strcpy(buf, ea2str((struct ether_addr *)s.ll->sll_addr));
	else
		buf[0] = 0;

	return buf;
}

static struct ether_addr *
duid2ea(const char *duid)
{
	static struct ether_addr ea;
	const char *p = NULL;
	size_t len;

	if (!duid)
		return NULL;

	for (len = 0; duid[len]; len++)
		if (!isxdigit(duid[len]))
			return NULL;

#define hex(x) \
	(((x) <= '9') ? ((x) - '0') : \
		(((x) <= 'F') ? ((x) - 'A' + 10) : \
			((x) - 'a' + 10)))

	switch (len) {
	case 28:
		if (!strncmp(duid, "00010001", 8))
			p = duid + 8;

		break;

	case 20:
		if (!strncmp(duid, "00030001", 8))
			p = duid + 8;

		break;

	case 12:
		p = duid;
		break;
	}

	if (!p)
		return NULL;

	ea.ether_addr_octet[0] = hex(p[0]) * 16 + hex(p[1]);
	ea.ether_addr_octet[1] = hex(p[2]) * 16 + hex(p[3]);
	ea.ether_addr_octet[2] = hex(p[4]) * 16 + hex(p[5]);
	ea.ether_addr_octet[3] = hex(p[6]) * 16 + hex(p[7]);
	ea.ether_addr_octet[4] = hex(p[8]) * 16 + hex(p[9]);
	ea.ether_addr_octet[5] = hex(p[10]) * 16 + hex(p[11]);

	return &ea;
}


static struct {
	FILE *dnsmasq_file;
	FILE *odhcpd_file;
	time_t now;
} lease_state = { };

struct lease_entry {
	int af, n_addr;
	char buf[512];
	int32_t expire;
	struct ether_addr mac;
	char *hostname;
	char *duid;
	union {
		struct in_addr in;
		struct in6_addr in6;
	} addr[10];
};

static char *
find_leasefile(struct uci_context *uci, const char *section)
{
	struct uci_ptr ptr = { .package = "dhcp" };
	struct uci_package *pkg = NULL;
	struct uci_section *s;
	struct uci_element *e;

	pkg = uci_lookup_package(uci, ptr.package);

	if (!pkg) {
		uci_load(uci, ptr.package, &pkg);

		if (!pkg)
			return NULL;
	}

	uci_foreach_element(&pkg->sections, e) {
		s = uci_to_section(e);

		if (strcmp(s->type, section))
			continue;

		ptr.flags = 0;

		ptr.section = s->e.name;
		ptr.s = NULL;

		ptr.option = "leasefile";
		ptr.o = NULL;

		if (uci_lookup_ptr(uci, &ptr, NULL, true) || ptr.o == NULL)
			continue;

		if (ptr.o->type != UCI_TYPE_STRING)
			continue;

		return ptr.o->v.string;
	}

	return NULL;
}

static void
lease_close(void)
{
	if (lease_state.dnsmasq_file) {
		fclose(lease_state.dnsmasq_file);
		lease_state.dnsmasq_file = NULL;
	}

	if (lease_state.odhcpd_file) {
		fclose(lease_state.odhcpd_file);
		lease_state.odhcpd_file = NULL;
	}
}

static void
lease_open(void)
{
	struct uci_context *uci;
	char *p;

	lease_close();

	uci = uci_alloc_context();

	if (!uci)
		return;

	lease_state.now = time(NULL);

	p = find_leasefile(uci, "dnsmasq");
	lease_state.dnsmasq_file = fopen(p ? p : "/tmp/dhcp.leases", "r");

	p = find_leasefile(uci, "odhcpd");
	lease_state.odhcpd_file = fopen(p ? p : "/tmp/hosts/odhcpd", "r");

	uci_free_context(uci);
}

static struct lease_entry *
lease_next(void)
{
	static struct lease_entry e;
	struct ether_addr *ea;
	char *p;
	int n;

	memset(&e, 0, sizeof(e));

	if (lease_state.dnsmasq_file) {
		while (fgets(e.buf, sizeof(e.buf), lease_state.dnsmasq_file)) {
			p = strtok(e.buf, " \t\n");

			if (!p)
				continue;

			n = strtol(p, NULL, 10);

			if (n > lease_state.now)
				e.expire = n - lease_state.now;
			else if (n > 0)
				e.expire = 0;
			else
				e.expire = -1;

			p = strtok(NULL, " \t\n");

			if (!p)
				continue;

			ea = ether_aton(p);

			p = strtok(NULL, " \t\n");

			if (p && inet_pton(AF_INET6, p, &e.addr[0].in6)) {
				e.af = AF_INET6;
				e.n_addr = 1;
			}
			else if (p && inet_pton(AF_INET, p, &e.addr[0].in)) {
				e.af = AF_INET;
				e.n_addr = 1;
			}
			else {
				continue;
			}

			if (!ea && e.af != AF_INET6)
				continue;

			e.hostname = strtok(NULL, " \t\n");
			e.duid     = strtok(NULL, " \t\n");

			if (!e.hostname || !e.duid)
				continue;

			if (!strcmp(e.hostname, "*"))
				e.hostname = NULL;

			if (!strcmp(e.duid, "*"))
				e.duid = NULL;

			if (!ea && e.duid)
				ea = duid2ea(e.duid);

			if (ea)
				e.mac = *ea;

			return &e;
		}

		fclose(lease_state.dnsmasq_file);
		lease_state.dnsmasq_file = NULL;
	}

	if (lease_state.odhcpd_file) {
		while (fgets(e.buf, sizeof(e.buf), lease_state.odhcpd_file)) {
			strtok(e.buf, " \t\n"); /* # */
			strtok(NULL, " \t\n"); /* iface */

			e.duid = strtok(NULL, " \t\n"); /* duid */

			if (!e.duid)
				continue;

			p = strtok(NULL, " \t\n"); /* iaid */

			if (p)
				e.af = strcmp(p, "ipv4") ? AF_INET6 : AF_INET;
			else
				continue;

			e.hostname = strtok(NULL, " \t\n"); /* name */

			if (!e.hostname)
				continue;

			p = strtok(NULL, " \t\n"); /* ts */

			if (!p)
				continue;

			n = strtol(p, NULL, 10);

			if (n > lease_state.now)
				e.expire = n - lease_state.now;
			else if (n >= 0)
				e.expire = 0;
			else
				e.expire = -1;

			strtok(NULL, " \t\n"); /* id */
			strtok(NULL, " \t\n"); /* length */

			for (e.n_addr = 0, p = strtok(NULL, "/ \t\n");
			     e.n_addr < ARRAY_SIZE(e.addr) && p != NULL;
			     p = strtok(NULL, "/ \t\n")) {
				if (inet_pton(e.af, p, &e.addr[e.n_addr].in6))
					e.n_addr++;
			}

			ea = duid2ea(e.duid);

			if (ea)
				e.mac = *ea;

			if (!strcmp(e.hostname, "-"))
				e.hostname = NULL;

			if (!strcmp(e.duid, "-"))
				e.duid = NULL;

			return &e;
		}

		fclose(lease_state.odhcpd_file);
		lease_state.odhcpd_file = NULL;
	}

	return NULL;
}


static void
rpc_luci_parse_network_device_sys(const char *name, struct ifaddrs *ifaddr)
{
	char link[64], buf[512], *p;
	unsigned int ifa_flags = 0;
	struct sockaddr_ll *sll;
	struct ifaddrs *ifa;
	struct dirent *e;
	void *o, *o2, *a;
	ssize_t len;
	uint64_t v;
	int n, af;
	DIR *d;

	const char *stats[] = {
		"rx_bytes", "tx_bytes", "tx_errors", "rx_errors", "tx_packets",
		"rx_packets", "multicast", "collisions", "rx_dropped", "tx_dropped"
	};

	o = blobmsg_open_table(&blob, name);

	blobmsg_add_string(&blob, "name", name);

	snprintf(buf, sizeof(buf), "/sys/class/net/%s/brif", name);

	d = opendir(buf);

	if (d) {
		blobmsg_add_u8(&blob, "bridge", 1);

		a = blobmsg_open_array(&blob, "ports");

		while (true) {
			e = readdir(d);

			if (e == NULL)
				break;

			if (strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
				blobmsg_add_string(&blob, NULL, e->d_name);
		}

		blobmsg_close_array(&blob, a);

		closedir(d);

		p = readstr("/sys/class/net/%s/bridge/bridge_id", name);
		blobmsg_add_string(&blob, "id", p);

		p = readstr("/sys/class/net/%s/bridge/stp_state", name);
		blobmsg_add_u8(&blob, "stp", strcmp(p, "0") ? 1 : 0);
	}

	snprintf(buf, sizeof(buf), "/sys/class/net/%s/master", name);
	len = readlink(buf, link, sizeof(link) - 1);

	if (len > 0) {
		link[len] = 0;
		blobmsg_add_string(&blob, "master", basename(link));
	}

	p = readstr("/sys/class/net/%s/phy80211/index", name);
	blobmsg_add_u8(&blob, "wireless", *p ? 1 : 0);

	p = readstr("/sys/class/net/%s/operstate", name);
	blobmsg_add_u8(&blob, "up", !strcmp(p, "up") || !strcmp(p, "unknown"));

	n = atoi(readstr("/sys/class/net/%s/mtu", name));
	if (n > 0)
		blobmsg_add_u32(&blob, "mtu", n);

	n = atoi(readstr("/sys/class/net/%s/tx_queue_len", name));
	if (n > 0)
		blobmsg_add_u32(&blob, "qlen", n);

	p = readstr("/sys/class/net/%s/master", name);
	if (*p)
		blobmsg_add_string(&blob, "master", p);

	for (af = AF_INET; af != 0; af = (af == AF_INET) ? AF_INET6 : 0) {
		a = blobmsg_open_array(&blob,
		                       (af == AF_INET) ? "ipaddrs" : "ip6addrs");

		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != af)
				continue;

			if (strcmp(ifa->ifa_name, name))
				continue;

			o2 = blobmsg_open_table(&blob, NULL);

			blobmsg_add_string(&blob, "address",
			                   sa2str(ifa->ifa_addr));

			blobmsg_add_string(&blob, "netmask",
			                   sa2str(ifa->ifa_netmask));

			if (ifa->ifa_dstaddr && (ifa->ifa_flags & IFF_POINTOPOINT))
				blobmsg_add_string(&blob, "remote",
				                   sa2str(ifa->ifa_dstaddr));
			else if (ifa->ifa_broadaddr && (ifa->ifa_flags & IFF_BROADCAST))
				blobmsg_add_string(&blob, "broadcast",
				                   sa2str(ifa->ifa_broadaddr));

			blobmsg_close_table(&blob, o2);

			ifa_flags |= ifa->ifa_flags;
		}

		blobmsg_close_array(&blob, a);
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_PACKET)
			continue;

		if (strcmp(ifa->ifa_name, name))
			continue;

		sll = (struct sockaddr_ll *)ifa->ifa_addr;

		if (sll->sll_hatype == 1)
			blobmsg_add_string(&blob, "mac", sa2str(ifa->ifa_addr));

		blobmsg_add_u32(&blob, "type", sll->sll_hatype);
		blobmsg_add_u32(&blob, "ifindex", sll->sll_ifindex);

		ifa_flags |= ifa->ifa_flags;
		break;
	}

	o2 = blobmsg_open_table(&blob, "stats");

	for (n = 0; n < ARRAY_SIZE(stats); n++) {
        v = strtoull(readstr("/sys/class/net/%s/statistics/%s",
                                      name, stats[n]), NULL, 10);

		blobmsg_add_u64(&blob, stats[n], v);
	}

	blobmsg_close_table(&blob, o2);

	o2 = blobmsg_open_table(&blob, "flags");
	blobmsg_add_u8(&blob, "up", ifa_flags & IFF_UP);
	blobmsg_add_u8(&blob, "broadcast", ifa_flags & IFF_BROADCAST);
	blobmsg_add_u8(&blob, "promisc", ifa_flags & IFF_PROMISC);
	blobmsg_add_u8(&blob, "loopback", ifa_flags & IFF_LOOPBACK);
	blobmsg_add_u8(&blob, "noarp", ifa_flags & IFF_NOARP);
	blobmsg_add_u8(&blob, "multicast", ifa_flags & IFF_MULTICAST);
	blobmsg_add_u8(&blob, "pointtopoint", ifa_flags & IFF_POINTOPOINT);
	blobmsg_close_table(&blob, o2);

	blobmsg_close_table(&blob, o);
}

static int
rpc_luci_get_network_devices(struct ubus_context *ctx,
                             struct ubus_object *obj,
                             struct ubus_request_data *req,
                             const char *method,
                             struct blob_attr *msg)
{
	struct ifaddrs *ifaddr;
	struct dirent *e;
	DIR *d;

	blob_buf_init(&blob, 0);

	d = opendir("/sys/class/net");

	if (d != NULL) {
		if (getifaddrs(&ifaddr) == 1)
			ifaddr = NULL;

		while (true) {
			e = readdir(d);

			if (e == NULL)
				break;

			if (strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
				rpc_luci_parse_network_device_sys(e->d_name, ifaddr);
		}

		if (ifaddr != NULL)
			freeifaddrs(ifaddr);

		closedir(d);
	}

	ubus_send_reply(ctx, req, blob.head);
	return 0;
}


static void
iw_call_str(int (*method)(const char *, char *), const char *dev,
            struct blob_buf *blob, const char *field)
{
	char buf[IWINFO_BUFSIZE] = {};

	if (method(dev, buf) == 0)
		blobmsg_add_string(blob, field, buf);
}

static void
iw_call_num(int (*method)(const char *, int *), const char *dev,
            struct blob_buf *blob, const char *field)
{
	int val = 0;

	if (method(dev, &val) == 0)
		blobmsg_add_u32(blob, field, val);
}

static bool rpc_luci_get_iwinfo(struct blob_buf *buf, const char *devname,
                                bool phy_only)
{
	struct iwinfo_crypto_entry crypto = {};
	struct iwinfo_hardware_id ids = {};
	const struct iwinfo_ops *iw;
	void *iwlib, *o, *o2, *a;
	int nret;

	if (!iw_backend || !iw_close || !iw_modenames) {
		iwlib = dlopen("libiwinfo.so", RTLD_LOCAL);

		if (!iwlib)
			return false;

		iw_backend = dlsym(iwlib, "iwinfo_backend");
		iw_close = dlsym(iwlib, "iwinfo_close");
		iw_modenames = dlsym(iwlib, "IWINFO_OPMODE_NAMES");

		if (!iw_backend || !iw_close || !iw_modenames)
			return false;
	}

	iw = iw_backend(devname);

	if (!iw)
		return false;

	o = blobmsg_open_table(buf, "iwinfo");

	iw_call_num(iw->signal, devname, buf, "signal");
	iw_call_num(iw->noise, devname, buf, "noise");
	iw_call_num(iw->channel, devname, buf, "channel");
	iw_call_str(iw->country, devname, buf, "country");
	iw_call_str(iw->phyname, devname, buf, "phy");
	iw_call_num(iw->txpower, devname, buf, "txpower");
	iw_call_num(iw->txpower_offset, devname, buf, "txpower_offset");
	iw_call_num(iw->frequency, devname, buf, "frequency");
	iw_call_num(iw->frequency_offset, devname, buf, "frequency_offset");

	if (!iw->hwmodelist(devname, &nret)) {
		a = blobmsg_open_array(buf, "hwmodes");

		if (nret & IWINFO_80211_AC)
			blobmsg_add_string(buf, NULL, "ac");

		if (nret & IWINFO_80211_A)
			blobmsg_add_string(buf, NULL, "a");

		if (nret & IWINFO_80211_B)
			blobmsg_add_string(buf, NULL, "b");

		if (nret & IWINFO_80211_G)
			blobmsg_add_string(buf, NULL, "g");

		if (nret & IWINFO_80211_N)
			blobmsg_add_string(buf, NULL, "n");

		blobmsg_close_array(buf, a);
	}

	if (!iw->htmodelist(devname, &nret)) {
		a = blobmsg_open_array(buf, "htmodes");

		if (nret & IWINFO_HTMODE_HT20)
			blobmsg_add_string(buf, NULL, "HT20");

		if (nret & IWINFO_HTMODE_HT40)
			blobmsg_add_string(buf, NULL, "HT40");

		if (nret & IWINFO_HTMODE_VHT20)
			blobmsg_add_string(buf, NULL, "VHT20");

		if (nret & IWINFO_HTMODE_VHT40)
			blobmsg_add_string(buf, NULL, "VHT40");

		if (nret & IWINFO_HTMODE_VHT80)
			blobmsg_add_string(buf, NULL, "VHT80");

		if (nret & IWINFO_HTMODE_VHT80_80)
			blobmsg_add_string(buf, NULL, "VHT80+80");

		if (nret & IWINFO_HTMODE_VHT160)
			blobmsg_add_string(buf, NULL, "VHT160");

		blobmsg_close_array(buf, a);
	}

	if (!iw->hardware_id(devname, (char *)&ids)) {
		o2 = blobmsg_open_table(buf, "hardware");

		a = blobmsg_open_array(buf, "id");
		blobmsg_add_u32(buf, NULL, ids.vendor_id);
		blobmsg_add_u32(buf, NULL, ids.device_id);
		blobmsg_add_u32(buf, NULL, ids.subsystem_vendor_id);
		blobmsg_add_u32(buf, NULL, ids.subsystem_device_id);
		blobmsg_close_array(buf, a);

		iw_call_str(iw->hardware_name, devname, buf, "name");

		blobmsg_close_table(buf, o2);
	}

	if (!phy_only) {
		iw_call_num(iw->quality, devname, buf, "quality");
		iw_call_num(iw->quality_max, devname, buf, "quality_max");
		iw_call_num(iw->bitrate, devname, buf, "bitrate");

		if (!iw->mode(devname, &nret))
			blobmsg_add_string(buf, "mode", iw_modenames[nret]);

		iw_call_str(iw->ssid, devname, buf, "ssid");
		iw_call_str(iw->bssid, devname, buf, "bssid");

		if (!iw->encryption(devname, (char *)&crypto)) {
			o2 = blobmsg_open_table(buf, "encryption");

			blobmsg_add_u8(buf, "enabled", crypto.enabled);

			if (crypto.enabled) {
				if (!crypto.wpa_version) {
					a = blobmsg_open_array(buf, "wep");

					if (crypto.auth_algs & IWINFO_AUTH_OPEN)
					    blobmsg_add_string(buf, NULL, "open");

					if (crypto.auth_algs & IWINFO_AUTH_SHARED)
					    blobmsg_add_string(buf, NULL, "shared");

					blobmsg_close_array(buf, a);
				}
				else {
					a = blobmsg_open_array(buf, "wpa");

					for (nret = 1; nret <= 3; nret++)
						if (crypto.wpa_version & (1 << (nret - 1)))
							blobmsg_add_u32(buf, NULL, nret);

					blobmsg_close_array(buf, a);

					a = blobmsg_open_array(buf, "authentication");

					if (crypto.auth_suites & IWINFO_KMGMT_PSK)
						blobmsg_add_string(buf, NULL, "psk");

					if (crypto.auth_suites & IWINFO_KMGMT_8021x)
						blobmsg_add_string(buf, NULL, "802.1x");

					if (crypto.auth_suites & IWINFO_KMGMT_SAE)
						blobmsg_add_string(buf, NULL, "sae");

					if (crypto.auth_suites & IWINFO_KMGMT_OWE)
						blobmsg_add_string(buf, NULL, "owe");

					if (!crypto.auth_suites ||
					    (crypto.auth_suites & IWINFO_KMGMT_NONE))
						blobmsg_add_string(buf, NULL, "none");

					blobmsg_close_array(buf, a);
				}

				a = blobmsg_open_array(buf, "ciphers");
				nret = crypto.pair_ciphers | crypto.group_ciphers;

				if (nret & IWINFO_CIPHER_WEP40)
					blobmsg_add_string(buf, NULL, "wep-40");

				if (nret & IWINFO_CIPHER_WEP104)
					blobmsg_add_string(buf, NULL, "wep-104");

				if (nret & IWINFO_CIPHER_TKIP)
					blobmsg_add_string(buf, NULL, "tkip");

				if (nret & IWINFO_CIPHER_CCMP)
					blobmsg_add_string(buf, NULL, "ccmp");

				if (nret & IWINFO_CIPHER_WRAP)
					blobmsg_add_string(buf, NULL, "wrap");

				if (nret & IWINFO_CIPHER_AESOCB)
					blobmsg_add_string(buf, NULL, "aes-ocb");

				if (nret & IWINFO_CIPHER_CKIP)
					blobmsg_add_string(buf, NULL, "ckip");

				if (!nret || (nret & IWINFO_CIPHER_NONE))
					blobmsg_add_string(buf, NULL, "none");

                blobmsg_close_array(buf, a);
			}

			blobmsg_close_table(buf, o2);
		}
	}

	blobmsg_close_table(buf, o);

	iw_close();

	return true;
}

static void rpc_luci_get_wireless_devices_cb(struct ubus_request *req,
                                             int type, struct blob_attr *msg)
{
	struct blob_attr *wifi, *cur, *iface, *cur2;
	struct reply_context *rctx = req->priv;
	const char *name, *first_ifname;
	int rem, rem2, rem3, rem4;
	void *o, *a, *o2;

	blob_for_each_attr(wifi, msg, rem) {
		if (blobmsg_type(wifi) != BLOBMSG_TYPE_TABLE ||
		    blobmsg_name(wifi) == NULL)
			continue;

		o = blobmsg_open_table(&rctx->blob, blobmsg_name(wifi));

		rem2 = blobmsg_data_len(wifi);
		first_ifname = NULL;

		__blob_for_each_attr(cur, blobmsg_data(wifi), rem2) {
			name = blobmsg_name(cur);

			if (!name || !strcmp(name, "iwinfo")) {
				continue;
			}
			else if (!strcmp(name, "interfaces")) {
				if (blobmsg_type(cur) != BLOBMSG_TYPE_ARRAY)
					continue;

				a = blobmsg_open_array(&rctx->blob, "interfaces");

				rem3 = blobmsg_data_len(cur);

				__blob_for_each_attr(iface, blobmsg_data(cur), rem3) {
					if (blobmsg_type(iface) != BLOBMSG_TYPE_TABLE)
						continue;

					o2 = blobmsg_open_table(&rctx->blob, NULL);

					rem4 = blobmsg_data_len(iface);
					name = NULL;

					__blob_for_each_attr(cur2, blobmsg_data(iface), rem4) {
						if (!strcmp(blobmsg_name(cur2), "ifname"))
							name = blobmsg_get_string(cur2);
						else if (!strcmp(blobmsg_name(cur2), "iwinfo"))
							continue;

						blobmsg_add_blob(&rctx->blob, cur2);
					}

					if (name)
						if (rpc_luci_get_iwinfo(&rctx->blob, name, false))
							first_ifname = first_ifname ? first_ifname : name;

					blobmsg_close_table(&rctx->blob, o2);
				}

				blobmsg_close_array(&rctx->blob, a);
			}
			else {
				blobmsg_add_blob(&rctx->blob, cur);
			}
		}

		rpc_luci_get_iwinfo(&rctx->blob,
		                    first_ifname ? first_ifname : blobmsg_name(wifi),
		                    true);

		blobmsg_close_table(&rctx->blob, o);
	}

	finish_request(rctx, UBUS_STATUS_OK);
}

static int
rpc_luci_get_wireless_devices(struct ubus_context *ctx,
                              struct ubus_object *obj,
                              struct ubus_request_data *req,
                              const char *method,
                              struct blob_attr *msg)
{
	struct reply_context *rctx = defer_request(ctx, req);

	if (!rctx)
		return UBUS_STATUS_UNKNOWN_ERROR;

	if (!invoke_ubus(ctx, "network.wireless", "status", NULL,
	                 rpc_luci_get_wireless_devices_cb, rctx))
		return finish_request(rctx, UBUS_STATUS_NOT_FOUND);

	return UBUS_STATUS_OK;
}

struct host_hint {
	struct avl_node avl;
	char *hostname;
	struct in_addr ip;
	struct in6_addr ip6;
};

static int
nl_cb_done(struct nl_msg *msg, void *arg)
{
	struct reply_context *rctx = arg;
	rctx->pending = 0;
	return NL_STOP;
}

static int
nl_cb_error(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
	struct reply_context *rctx = arg;
	rctx->pending = 0;
	return NL_STOP;
}

static struct host_hint *
rpc_luci_get_host_hint(struct reply_context *rctx, struct ether_addr *ea)
{
	struct host_hint *hint;
	char *p, *mac;

	if (!ea)
		return NULL;

	mac = ea2str(ea);
	hint = avl_find_element(&rctx->avl, mac, hint, avl);

	if (!hint) {
		hint = calloc_a(sizeof(*hint), &p, strlen(mac) + 1);

		if (!hint)
			return NULL;

		hint->avl.key = strcpy(p, mac);
		avl_insert(&rctx->avl, &hint->avl);
	}

	return hint;
}

static int nl_cb_dump_neigh(struct nl_msg *msg, void *arg)
{
	struct reply_context *rctx = arg;
	struct ether_addr *mac;
	struct in6_addr *dst;
	struct nlmsghdr *hdr = nlmsg_hdr(msg);
	struct ndmsg *nd = NLMSG_DATA(hdr);
	struct nlattr *tb[NDA_MAX+1];
	struct host_hint *hint;

	rctx->pending = !!(hdr->nlmsg_flags & NLM_F_MULTI);

	if (hdr->nlmsg_type != RTM_NEWNEIGH ||
	    (nd->ndm_family != AF_INET && nd->ndm_family != AF_INET6))
		return NL_SKIP;

	if (!(nd->ndm_state & (0xFF & ~NUD_NOARP)))
		return NL_SKIP;

	nlmsg_parse(hdr, sizeof(*nd), tb, NDA_MAX, NULL);

	mac = tb[NDA_LLADDR] ? RTA_DATA(tb[NDA_LLADDR]) : NULL;
	dst = tb[NDA_DST]    ? RTA_DATA(tb[NDA_DST])    : NULL;

	if (!mac || !dst)
		return NL_SKIP;

	hint = rpc_luci_get_host_hint(rctx, mac);

	if (!hint)
		return NL_SKIP;

	if (nd->ndm_family == AF_INET)
		hint->ip = *(struct in_addr *)dst;
	else
		hint->ip6 = *(struct in6_addr *)dst;

	return NL_SKIP;
}

static void
rpc_luci_get_host_hints_nl(struct reply_context *rctx)
{
	struct nl_sock *sock = NULL;
	struct nl_msg *msg = NULL;
	struct nl_cb *cb = NULL;
	struct ndmsg ndm = {};

	sock = nl_socket_alloc();

	if (!sock)
		goto out;

	if (nl_connect(sock, NETLINK_ROUTE))
		goto out;

	cb = nl_cb_alloc(NL_CB_DEFAULT);

	if (!cb)
		goto out;

	msg = nlmsg_alloc_simple(RTM_GETNEIGH, NLM_F_REQUEST | NLM_F_DUMP);

	if (!msg)
		goto out;

	nlmsg_append(msg, &ndm, sizeof(ndm), 0);

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, nl_cb_dump_neigh, rctx);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, nl_cb_done, rctx);
	nl_cb_err(cb, NL_CB_CUSTOM, nl_cb_error, rctx);

	avl_init(&rctx->avl, avl_strcmp, false, NULL);

	rctx->pending = 1;

	nl_send_auto_complete(sock, msg);

	while (rctx->pending)
		nl_recvmsgs(sock, cb);

out:
	if (sock)
		nl_socket_free(sock);

	if (cb)
		nl_cb_put(cb);

	if (msg)
		nlmsg_free(msg);
}

static void
rpc_luci_get_host_hints_ether(struct reply_context *rctx)
{
	struct host_hint *hint;
	struct in_addr in;
	char buf[512], *p;
	FILE *f;

	f = fopen("/etc/ethers", "r");

	if (!f)
		return;

	while (fgets(buf, sizeof(buf), f)) {
		p = strtok(buf, " \t\n");
		hint = rpc_luci_get_host_hint(rctx, p ? ether_aton(p) : NULL);

		if (!hint)
			continue;

		p = strtok(NULL, " \t\n");

		if (!p)
			continue;

		if (inet_pton(AF_INET, p, &in) == 1) {
			if (hint->ip.s_addr == 0)
				hint->ip = in;
		}
		else if (*p && !hint->hostname) {
			hint->hostname = strdup(p);
		}
	}

	fclose(f);
}

static void
rpc_luci_get_host_hints_uci(struct reply_context *rctx)
{
	struct uci_ptr ptr = { .package = "dhcp" };
	struct uci_context *uci = NULL;
	struct uci_package *pkg = NULL;
	struct in6_addr empty = {};
	struct lease_entry *lease;
	struct host_hint *hint;
	struct uci_element *e, *l;
	struct uci_section *s;
	struct in_addr in;
	char *p, *n;

	uci = uci_alloc_context();

	if (!uci)
		goto out;

	uci_load(uci, ptr.package, &pkg);

	if (!pkg)
		goto out;

	uci_foreach_element(&pkg->sections, e)
	{
		s = uci_to_section(e);

		if (strcmp(s->type, "host"))
			continue;

		ptr.section = s->e.name;
		ptr.s = NULL;

		ptr.option = "ip";
		ptr.o = NULL;

		if (!uci_lookup_ptr(uci, &ptr, NULL, true) && ptr.o != NULL &&
			ptr.o->type != UCI_TYPE_STRING)
			n = ptr.o->v.string;
		else
			n = NULL;

		if (!n || inet_pton(AF_INET, n, &in) != 1)
			in.s_addr = 0;

		ptr.option = "name";
		ptr.o = NULL;

		if (!uci_lookup_ptr(uci, &ptr, NULL, true) && ptr.o != NULL &&
		    ptr.o->type == UCI_TYPE_STRING)
		    n = ptr.o->v.string;
		else
			n = NULL;

		ptr.option = "mac";
		ptr.o = NULL;

		if (uci_lookup_ptr(uci, &ptr, NULL, true) || ptr.o == NULL)
			continue;

		if (ptr.o->type == UCI_TYPE_STRING) {
			for (p = strtok(ptr.o->v.string, " \t");
			     p != NULL;
			     p = strtok(NULL, " \t")) {
				hint = rpc_luci_get_host_hint(rctx, ether_aton(p));

				if (!hint)
					continue;

				if (hint->ip.s_addr == 0 && in.s_addr != 0)
					hint->ip = in;

				if (n && !hint->hostname)
					hint->hostname = strdup(n);
			}
		}
		else if (ptr.o->type == UCI_TYPE_LIST) {
			uci_foreach_element(&ptr.o->v.list, l) {
				hint = rpc_luci_get_host_hint(rctx, ether_aton(l->name));

				if (!hint)
					continue;

				if (hint->ip.s_addr == 0 && in.s_addr != 0)
					hint->ip = in;

				if (n && !hint->hostname)
					hint->hostname = strdup(n);
			}
		}
	}

	lease_open();

	while ((lease = lease_next()) != NULL) {
		hint = rpc_luci_get_host_hint(rctx, &lease->mac);

		if (!hint)
			continue;

		if (lease->af == AF_INET && lease->n_addr && hint->ip.s_addr == 0)
			hint->ip = lease->addr[0].in;
		else if (lease->af == AF_INET6 && lease->n_addr &&
		         !memcmp(&hint->ip6, &empty, sizeof(empty)))
			hint->ip6 = lease->addr[0].in6;

		if (lease->hostname && !hint->hostname)
			hint->hostname = strdup(lease->hostname);
	}

	lease_close();

out:
	if (uci)
		uci_free_context(uci);
}

static void
rpc_luci_get_host_hints_ifaddrs(struct reply_context *rctx)
{
	struct ether_addr empty_ea = {};
	struct in6_addr empty_in6 = {};
	struct ifaddrs *ifaddr, *ifa;
	struct sockaddr_ll *sll;
	struct avl_tree devices;
	struct host_hint *hint;
	struct {
		struct avl_node avl;
		struct ether_addr ea;
		struct in6_addr in6;
		struct in_addr in;
	} *device, *nextdevice;
	char *p;

	avl_init(&devices, avl_strcmp, false, NULL);

	if (getifaddrs(&ifaddr) == -1)
		return;

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr)
			continue;

		device = avl_find_element(&devices, ifa->ifa_name, device, avl);

		if (!device) {
			device = calloc_a(sizeof(*device), &p, strlen(ifa->ifa_name) + 1);

			if (!device)
				continue;

			device->avl.key = strcpy(p, ifa->ifa_name);
			avl_insert(&devices, &device->avl);
		}

		switch (ifa->ifa_addr->sa_family) {
		case AF_PACKET:
			sll = (struct sockaddr_ll *)ifa->ifa_addr;

			if (sll->sll_halen == 6)
				memcpy(&device->ea, sll->sll_addr, 6);

			break;

		case AF_INET6:
			device->in6 = ((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
			break;

		case AF_INET:
			device->in = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			break;
		}
	}

	freeifaddrs(ifaddr);

	avl_remove_all_elements(&devices, device, avl, nextdevice) {
		if (memcmp(&device->ea, &empty_ea, sizeof(empty_ea)) &&
		    (memcmp(&device->in6, &empty_in6, sizeof(empty_in6)) ||
		     device->in.s_addr != 0)) {
			hint = rpc_luci_get_host_hint(rctx, &device->ea);

			if (hint) {
				if (hint->ip.s_addr == 0 && device->in.s_addr != 0)
					hint->ip = device->in;

				if (memcmp(&hint->ip6, &empty_in6, sizeof(empty_in6)) == 0 &&
				    memcmp(&device->in6, &empty_in6, sizeof(empty_in6)) != 0)
					hint->ip6 = device->in6;
			}
		}

		free(device);
	}
}

static int
rpc_luci_get_host_hints_finish(struct reply_context *rctx);

static void
rpc_luci_get_host_hints_rrdns_cb(struct ubus_request *req, int type,
                                 struct blob_attr *msg)
{
	struct reply_context *rctx = req->priv;
	struct host_hint *hint;
	struct blob_attr *cur;
	struct in6_addr in6;
	struct in_addr in;
	int rem;

	if (msg) {
		blob_for_each_attr(cur, msg, rem) {
			if (blobmsg_type(cur) != BLOBMSG_TYPE_STRING)
				continue;

			if (inet_pton(AF_INET6, blobmsg_name(cur), &in6) == 1) {
				avl_for_each_element(&rctx->avl, hint, avl) {
					if (!memcmp(&hint->ip6, &in6, sizeof(in6))) {
						if (hint->hostname)
							free(hint->hostname);

						hint->hostname = strdup(blobmsg_get_string(cur));
						break;
					}
				}
			}
			else if (inet_pton(AF_INET, blobmsg_name(cur), &in) == 1) {
				avl_for_each_element(&rctx->avl, hint, avl) {
					if (!memcmp(&hint->ip, &in, sizeof(in))) {
						if (hint->hostname)
							free(hint->hostname);

						hint->hostname = strdup(blobmsg_get_string(cur));
						break;
					}
				}
			}
		}
	}

	rpc_luci_get_host_hints_finish(rctx);
}

static void
rpc_luci_get_host_hints_rrdns(struct reply_context *rctx)
{
	struct in6_addr empty_in6 = {};
	char buf[INET6_ADDRSTRLEN];
	struct blob_buf req = {};
	struct host_hint *hint;
	int n = 0;
	void *a;

	blob_buf_init(&req, 0);

	a = blobmsg_open_array(&req, "addrs");

	avl_for_each_element(&rctx->avl, hint, avl) {
		if (hint->ip.s_addr != 0) {
			inet_ntop(AF_INET, &hint->ip, buf, sizeof(buf));
			blobmsg_add_string(&req, NULL, buf);
			n++;
		}
		else if (memcmp(&hint->ip6, &empty_in6, sizeof(empty_in6))) {
			inet_ntop(AF_INET6, &hint->ip6, buf, sizeof(buf));
			blobmsg_add_string(&req, NULL, buf);
			n++;
		}
	}

	blobmsg_close_array(&req, a);

	if (n > 0) {
		blobmsg_add_u32(&req, "timeout", 250);
		blobmsg_add_u32(&req, "limit", n);

		if (!invoke_ubus(rctx->context, "network.rrdns", "lookup", &req,
		                 rpc_luci_get_host_hints_rrdns_cb, rctx))
			rpc_luci_get_host_hints_finish(rctx);
	}
	else {
		rpc_luci_get_host_hints_finish(rctx);
	}

	blob_buf_free(&req);
}

static int
rpc_luci_get_host_hints_finish(struct reply_context *rctx)
{
	struct host_hint *hint, *nexthint;
	char buf[INET6_ADDRSTRLEN];
	struct in6_addr in6 = {};
	struct in_addr in = {};
	void *o;

	avl_remove_all_elements(&rctx->avl, hint, avl, nexthint) {
		o = blobmsg_open_table(&rctx->blob, hint->avl.key);

		if (memcmp(&hint->ip, &in, sizeof(in))) {
			inet_ntop(AF_INET, &hint->ip, buf, sizeof(buf));
			blobmsg_add_string(&rctx->blob, "ipv4", buf);
		}

		if (memcmp(&hint->ip6, &in6, sizeof(in6))) {
			inet_ntop(AF_INET6, &hint->ip6, buf, sizeof(buf));
			blobmsg_add_string(&rctx->blob, "ipv6", buf);
		}

		if (hint->hostname)
			blobmsg_add_string(&rctx->blob, "name", hint->hostname);

		blobmsg_close_table(&rctx->blob, o);

		if (hint->hostname)
			free(hint->hostname);

		free(hint);
	}

	return finish_request(rctx, UBUS_STATUS_OK);
}

static int
rpc_luci_get_host_hints(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
	struct reply_context *rctx = defer_request(ctx, req);

	if (!rctx)
		return UBUS_STATUS_UNKNOWN_ERROR;

	rpc_luci_get_host_hints_nl(rctx);
	rpc_luci_get_host_hints_uci(rctx);
	rpc_luci_get_host_hints_ether(rctx);
	rpc_luci_get_host_hints_ifaddrs(rctx);
	rpc_luci_get_host_hints_rrdns(rctx);

	return UBUS_STATUS_OK;
}

static int
rpc_luci_get_duid_hints(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
	struct { struct avl_node avl; } *e, *next;
	char s[INET6_ADDRSTRLEN], *p;
	struct ether_addr empty = {};
	struct lease_entry *lease;
	struct avl_tree avl;
	void *o, *a;
	int n;

	avl_init(&avl, avl_strcmp, false, NULL);
	blob_buf_init(&blob, 0);

	lease_open();

	while ((lease = lease_next()) != NULL) {
		if (lease->af != AF_INET6 || lease->duid == NULL)
			continue;

		e = avl_find_element(&avl, lease->duid, e, avl);

		if (e)
			continue;

		e = calloc_a(sizeof(*e), &p, strlen(lease->duid) + 1);

		if (!e)
			continue;

		o = blobmsg_open_table(&blob, lease->duid);

		inet_ntop(AF_INET6, &lease->addr[0].in6, s, sizeof(s));
		blobmsg_add_string(&blob, "ip6addr", s);

		a = blobmsg_open_array(&blob, "ip6addrs");

		for (n = 0; n < lease->n_addr; n++) {
			inet_ntop(AF_INET6, &lease->addr[n].in6, s, sizeof(s));
			blobmsg_add_string(&blob, NULL, s);
		}

		blobmsg_close_array(&blob, a);

		if (lease->hostname)
			blobmsg_add_string(&blob, "hostname", lease->hostname);

		if (memcmp(&lease->mac, &empty, sizeof(empty)))
			blobmsg_add_string(&blob, "macaddr", ea2str(&lease->mac));

		blobmsg_close_table(&blob, o);

		e->avl.key = strcpy(p, lease->duid);
		avl_insert(&avl, &e->avl);
	}

	lease_close();

	avl_remove_all_elements(&avl, e, avl, next) {
		free(e);
	}

	ubus_send_reply(ctx, req, blob.head);

	return UBUS_STATUS_OK;
}

static int
rpc_luci_get_board_json(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
	blob_buf_init(&blob, 0);

	if (!blobmsg_add_json_from_file(&blob, "/etc/board.json"))
		return UBUS_STATUS_UNKNOWN_ERROR;

	ubus_send_reply(ctx, req, blob.head);
	return UBUS_STATUS_OK;
}

static int
rpc_luci_get_dsl_status(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
	char line[128], *p, *s;
	FILE *cmd;

	cmd = popen("/etc/init.d/dsl_control lucistat", "r");

	if (!cmd)
		return UBUS_STATUS_NOT_FOUND;

	blob_buf_init(&blob, 0);

	while (fgets(line, sizeof(line), cmd)) {
		if (strncmp(line, "dsl.", 4))
			continue;

		p = strchr(line, '=');

		if (!p)
			continue;

		s = p + strlen(p) - 1;

		while (s >= p && isspace(*s))
			*s-- = 0;

		*p++ = 0;

		if (!strcmp(p, "nil"))
			continue;

		if (isdigit(*p)) {
			blobmsg_add_u32(&blob, line + 4, strtoul(p, NULL, 0));
		}
		else if (*p == '"') {
			s = p + strlen(p) - 1;

			if (s >= p && *s == '"')
				*s = 0;

			blobmsg_add_string(&blob, line + 4, p + 1);
		}
	}

	fclose(cmd);

	ubus_send_reply(ctx, req, blob.head);
	return UBUS_STATUS_OK;
}


enum {
	RPC_L_FAMILY,
	__RPC_L_MAX,
};

static const struct blobmsg_policy rpc_get_leases_policy[__RPC_L_MAX] = {
	[RPC_L_FAMILY] = { .name = "family", .type = BLOBMSG_TYPE_INT32 }
};

static int
rpc_luci_get_dhcp_leases(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
	struct blob_attr *tb[__RPC_L_MAX];
	struct ether_addr emptymac = {};
	struct lease_entry *lease;
	char s[INET6_ADDRSTRLEN];
	int af, family = 0;
	void *a, *a2, *o;
	int n;

	blobmsg_parse(rpc_get_leases_policy, __RPC_L_MAX, tb,
	              blob_data(msg), blob_len(msg));

	switch (tb[RPC_L_FAMILY] ? blobmsg_get_u32(tb[RPC_L_FAMILY]) : 0) {
	case 0:
		family = 0;
		break;

	case 4:
		family = AF_INET;
		break;

	case 6:
		family = AF_INET6;
		break;

	default:
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	blob_buf_init(&blob, 0);

	for (af = family ? family : AF_INET;
	     af != 0;
	     af = (family == 0) ? (af == AF_INET ? AF_INET6 : 0) : 0) {

		a = blobmsg_open_array(&blob, (af == AF_INET) ? "dhcp_leases"
		                                              : "dhcp6_leases");

		lease_open();

		while ((lease = lease_next()) != NULL) {
			if (lease->af != af)
				continue;

			o = blobmsg_open_table(&blob, NULL);

			if (lease->expire == -1)
				blobmsg_add_u8(&blob, "expires", 0);
			else
				blobmsg_add_u32(&blob, "expires", lease->expire);

			if (lease->hostname)
				blobmsg_add_string(&blob, "hostname", lease->hostname);

			if (memcmp(&lease->mac, &emptymac, sizeof(emptymac)))
				blobmsg_add_string(&blob, "macaddr", ea2str(&lease->mac));

			if (lease->duid)
				blobmsg_add_string(&blob, "duid", lease->duid);

			inet_ntop(lease->af, &lease->addr[0].in6, s, sizeof(s));
			blobmsg_add_string(&blob, (af == AF_INET) ? "ipaddr" : "ip6addr",
			                   s);

			if (af == AF_INET6) {
				a2 = blobmsg_open_array(&blob, "ip6addrs");

				for (n = 0; n < lease->n_addr; n++) {
					inet_ntop(lease->af, &lease->addr[n].in6, s, sizeof(s));
					blobmsg_add_string(&blob, NULL, s);
				}

				blobmsg_close_array(&blob, a2);
			}

			blobmsg_close_table(&blob, o);
		}

		lease_close();

		blobmsg_close_array(&blob, a);
	}

	ubus_send_reply(ctx, req, blob.head);

	return UBUS_STATUS_OK;
}

static int
rpc_luci_api_init(const struct rpc_daemon_ops *o, struct ubus_context *ctx)
{
	static const struct ubus_method luci_methods[] = {
		UBUS_METHOD_NOARG("getNetworkDevices", rpc_luci_get_network_devices),
		UBUS_METHOD_NOARG("getWirelessDevices", rpc_luci_get_wireless_devices),
		UBUS_METHOD_NOARG("getHostHints", rpc_luci_get_host_hints),
		UBUS_METHOD_NOARG("getDUIDHints", rpc_luci_get_duid_hints),
		UBUS_METHOD_NOARG("getBoardJSON", rpc_luci_get_board_json),
		UBUS_METHOD_NOARG("getDSLStatus", rpc_luci_get_dsl_status),
		UBUS_METHOD("getDHCPLeases", rpc_luci_get_dhcp_leases, rpc_get_leases_policy)
	};

	static struct ubus_object_type luci_type =
		UBUS_OBJECT_TYPE("rpcd-luci", luci_methods);

	static struct ubus_object obj = {
		.name = "luci-rpc",
		.type = &luci_type,
		.methods = luci_methods,
		.n_methods = ARRAY_SIZE(luci_methods),
	};

	return ubus_add_object(ctx, &obj);
}

struct rpc_plugin rpc_plugin = {
	.init = rpc_luci_api_init
};
