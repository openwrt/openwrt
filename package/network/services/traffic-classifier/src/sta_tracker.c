#include "sta_tracker.h"

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>

struct sta_tracker *sta_tracker_init(struct ubus_context *ubus)
{
	struct sta_tracker *st = calloc(1, sizeof(*st));
	if (!st)
		return NULL;
	st->ubus = ubus;
	return st;
}

void sta_tracker_destroy(struct sta_tracker *st)
{
	free(st);
}

static int parse_mac(const char *str, uint8_t mac[6])
{
	unsigned int m[6];
	if (sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
		   &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]) != 6)
		return -1;
	for (int i = 0; i < 6; i++)
		mac[i] = (uint8_t)m[i];
	return 0;
}

enum {
	STATUS_SSID,
	__STATUS_MAX
};

static const struct blobmsg_policy status_policy[] = {
	[STATUS_SSID] = { .name = "ssid", .type = BLOBMSG_TYPE_STRING },
};

enum {
	CLIENTS_FREQ,
	CLIENTS_TABLE,
	__CLIENTS_MAX
};

static const struct blobmsg_policy clients_policy[] = {
	[CLIENTS_FREQ]  = { .name = "freq",    .type = BLOBMSG_TYPE_INT32 },
	[CLIENTS_TABLE] = { .name = "clients", .type = BLOBMSG_TYPE_TABLE },
};

static void status_cb(struct ubus_request *req, int type,
		      struct blob_attr *msg)
{
	char *ssid_out = (char *)req->priv;
	struct blob_attr *tb[__STATUS_MAX];

	blobmsg_parse(status_policy, __STATUS_MAX,
		      tb, blob_data(msg), blob_len(msg));

	if (tb[STATUS_SSID])
		snprintf(ssid_out, MAX_SSID_LEN, "%s",
			 blobmsg_get_string(tb[STATUS_SSID]));
}

struct client_parse_ctx {
	struct sta_tracker *st;
	const char *ssid;
	const char *ifname;
};

static void clients_cb(struct ubus_request *req, int type,
		       struct blob_attr *msg)
{
	struct client_parse_ctx *pctx = (struct client_parse_ctx *)req->priv;
	struct sta_tracker *st = pctx->st;
	struct blob_attr *tb[__CLIENTS_MAX];

	blobmsg_parse(clients_policy, __CLIENTS_MAX,
		      tb, blob_data(msg), blob_len(msg));

	if (!tb[CLIENTS_TABLE])
		return;

	struct blob_attr *cur;
	int rem;

	blobmsg_for_each_attr(cur, tb[CLIENTS_TABLE], rem) {
		if (st->count >= MAX_STATIONS)
			break;

		const char *mac_str = blobmsg_name(cur);
		struct sta_entry *e = &st->stations[st->count];

		if (parse_mac(mac_str, e->mac) < 0)
			continue;

		snprintf(e->ssid, sizeof(e->ssid), "%s", pctx->ssid);
		snprintf(e->ifname, sizeof(e->ifname), "%s", pctx->ifname);
		e->active = true;

		if (blobmsg_type(cur) == BLOBMSG_TYPE_TABLE) {
			struct blob_attr *field;
			int frem;
			blobmsg_for_each_attr(field, cur, frem) {
				const char *name = blobmsg_name(field);
				if (!strcmp(name, "signal") &&
				    blobmsg_type(field) == BLOBMSG_TYPE_INT32)
					e->signal = blobmsg_get_u32(field);
			}
		}

		st->count++;
	}
}

int sta_tracker_refresh(struct sta_tracker *st)
{
	struct ubus_context *ubus = st->ubus;
	uint32_t id;
	struct blob_buf b = {};

	st->count = 0;

	const char *iface_patterns[] = {
		"hostapd.wlan0", "hostapd.wlan1",
		"hostapd.wlan0-1", "hostapd.wlan0-2",
		"hostapd.wlan1-1", "hostapd.wlan1-2",
		NULL
	};

	for (const char **p = iface_patterns; *p; p++) {
		if (ubus_lookup_id(ubus, *p, &id) != 0)
			continue;

		const char *ifname = *p + strlen("hostapd.");

		char ssid[MAX_SSID_LEN] = {};
		blob_buf_init(&b, 0);
		ubus_invoke(ubus, id, "get_status", b.head,
			    status_cb, ssid, 1000);

		if (!ssid[0])
			continue;

		struct client_parse_ctx pctx = {
			.st = st,
			.ssid = ssid,
			.ifname = ifname,
		};

		blob_buf_init(&b, 0);
		ubus_invoke(ubus, id, "get_clients", b.head,
			    clients_cb, &pctx, 1000);
	}

	blob_buf_free(&b);

	syslog(LOG_DEBUG, "sta_tracker: %d stations across all BSSes", st->count);
	return st->count;
}

const struct sta_entry *sta_tracker_find_mac(const struct sta_tracker *st,
					     const uint8_t *mac)
{
	for (int i = 0; i < st->count; i++) {
		if (memcmp(st->stations[i].mac, mac, 6) == 0)
			return &st->stations[i];
	}
	return NULL;
}
