#include "ubus_api.h"

#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <syslog.h>
#include <stdio.h>

struct flows_dump_ctx {
	struct blob_buf *b;
	struct sta_tracker *sta;
};

static int dump_flow_cb(struct flow_entry *entry, void *arg)
{
	struct flows_dump_ctx *ctx = (struct flows_dump_ctx *)arg;
	struct blob_buf *b = ctx->b;
	char addr_buf[INET6_ADDRSTRLEN];
	char mac_str[18];

	void *flow = blobmsg_open_table(b, NULL);

	if (entry->key.is_ipv6)
		inet_ntop(AF_INET6, &entry->key.src_ip, addr_buf, sizeof(addr_buf));
	else
		inet_ntop(AF_INET, &entry->key.src_ip.s6_addr[12], addr_buf, sizeof(addr_buf));
	blobmsg_add_string(b, "src_ip", addr_buf);

	if (entry->key.is_ipv6)
		inet_ntop(AF_INET6, &entry->key.dst_ip, addr_buf, sizeof(addr_buf));
	else
		inet_ntop(AF_INET, &entry->key.dst_ip.s6_addr[12], addr_buf, sizeof(addr_buf));
	blobmsg_add_string(b, "dst_ip", addr_buf);

	blobmsg_add_u32(b, "src_port", entry->key.src_port);
	blobmsg_add_u32(b, "dst_port", entry->key.dst_port);
	blobmsg_add_u32(b, "proto", entry->key.proto);

	blobmsg_add_string(b, "class", classifier_class_name(entry->classification));
	blobmsg_add_u32(b, "confidence", (uint32_t)(entry->confidence * 100));

	blobmsg_add_u64(b, "bytes_fwd", entry->stats.total_bytes_fwd);
	blobmsg_add_u64(b, "bytes_bwd", entry->stats.total_bytes_bwd);
	blobmsg_add_u32(b, "pkts_fwd", entry->stats.total_pkts_fwd);
	blobmsg_add_u32(b, "pkts_bwd", entry->stats.total_pkts_bwd);

	snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
		 entry->src_mac[0], entry->src_mac[1], entry->src_mac[2],
		 entry->src_mac[3], entry->src_mac[4], entry->src_mac[5]);
	blobmsg_add_string(b, "src_mac", mac_str);

	const struct sta_entry *sta = sta_tracker_find_mac(ctx->sta, entry->src_mac);
	if (sta) {
		blobmsg_add_string(b, "ssid", sta->ssid);
		blobmsg_add_string(b, "ifname", sta->ifname);
		blobmsg_add_u32(b, "signal", (uint32_t)sta->signal);
	}

	blobmsg_close_table(b, flow);
	return 0;
}

static int handle_get_flows(struct ubus_context *ctx, struct ubus_object *obj,
			    struct ubus_request_data *req, const char *method,
			    struct blob_attr *msg)
{
	struct tc_ubus_ctx *tc = container_of(obj, struct tc_ubus_ctx, obj);
	struct blob_buf b = {};

	blob_buf_init(&b, 0);

	void *arr = blobmsg_open_array(&b, "flows");
	struct flows_dump_ctx dump_ctx = { .b = &b, .sta = tc->sta };
	flow_table_for_each(tc->ft, dump_flow_cb, &dump_ctx);
	blobmsg_close_array(&b, arr);

	blobmsg_add_u32(&b, "total_flows", tc->ft->count);
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return UBUS_STATUS_OK;
}

struct client_summary {
	uint8_t mac[6];
	char ssid[MAX_SSID_LEN];
	uint32_t class_counts[CLASSIFICATION_LABELS];
	uint64_t total_bytes;
	uint32_t total_flows;
};

struct client_agg_ctx {
	struct client_summary clients[MAX_STATIONS];
	int count;
	struct sta_tracker *sta;
};

static int agg_flow_cb(struct flow_entry *entry, void *arg)
{
	struct client_agg_ctx *ctx = (struct client_agg_ctx *)arg;

	int idx = -1;
	for (int i = 0; i < ctx->count; i++) {
		if (memcmp(ctx->clients[i].mac, entry->src_mac, 6) == 0) {
			idx = i;
			break;
		}
	}

	if (idx < 0) {
		if (ctx->count >= MAX_STATIONS)
			return 0;
		idx = ctx->count++;
		memcpy(ctx->clients[idx].mac, entry->src_mac, 6);

		const struct sta_entry *sta = sta_tracker_find_mac(ctx->sta, entry->src_mac);
		if (sta)
			snprintf(ctx->clients[idx].ssid, MAX_SSID_LEN, "%s", sta->ssid);
	}

	struct client_summary *cs = &ctx->clients[idx];
	if (entry->classification < CLASSIFICATION_LABELS)
		cs->class_counts[entry->classification]++;
	cs->total_bytes += entry->stats.total_bytes_fwd + entry->stats.total_bytes_bwd;
	cs->total_flows++;

	return 0;
}

static int handle_get_clients(struct ubus_context *ctx, struct ubus_object *obj,
			      struct ubus_request_data *req, const char *method,
			      struct blob_attr *msg)
{
	struct tc_ubus_ctx *tc = container_of(obj, struct tc_ubus_ctx, obj);
	struct blob_buf b = {};
	struct client_agg_ctx agg = { .sta = tc->sta };

	flow_table_for_each(tc->ft, agg_flow_cb, &agg);

	blob_buf_init(&b, 0);
	void *arr = blobmsg_open_array(&b, "clients");

	for (int i = 0; i < agg.count; i++) {
		struct client_summary *cs = &agg.clients[i];
		char mac_str[18];
		snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
			 cs->mac[0], cs->mac[1], cs->mac[2],
			 cs->mac[3], cs->mac[4], cs->mac[5]);

		void *client = blobmsg_open_table(&b, NULL);
		blobmsg_add_string(&b, "mac", mac_str);
		if (cs->ssid[0])
			blobmsg_add_string(&b, "ssid", cs->ssid);
		blobmsg_add_u64(&b, "total_bytes", cs->total_bytes);
		blobmsg_add_u32(&b, "total_flows", cs->total_flows);

		void *classes = blobmsg_open_table(&b, "app_usage");
		for (int c = 0; c < CLASSIFICATION_LABELS; c++) {
			if (cs->class_counts[c] > 0)
				blobmsg_add_u32(&b, traffic_class_names[c],
						cs->class_counts[c]);
		}
		blobmsg_close_table(&b, classes);
		blobmsg_close_table(&b, client);
	}

	blobmsg_close_array(&b, arr);
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return UBUS_STATUS_OK;
}

static int handle_get_stats(struct ubus_context *ctx, struct ubus_object *obj,
			    struct ubus_request_data *req, const char *method,
			    struct blob_attr *msg)
{
	struct tc_ubus_ctx *tc = container_of(obj, struct tc_ubus_ctx, obj);
	struct blob_buf b = {};

	uint32_t class_counts[CLASSIFICATION_LABELS] = {};
	uint64_t total_bytes = 0;

	struct flow_entry *e;
	for (int i = 0; i < FLOW_TABLE_SIZE; i++) {
		e = tc->ft->buckets[i];
		while (e) {
			if (e->classification < CLASSIFICATION_LABELS)
				class_counts[e->classification]++;
			total_bytes += e->stats.total_bytes_fwd +
				       e->stats.total_bytes_bwd;
			e = e->hash_next;
		}
	}

	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "active_flows", tc->ft->count);
	blobmsg_add_u32(&b, "tracked_stations", tc->sta->count);
	blobmsg_add_u64(&b, "total_bytes", total_bytes);

	void *classes = blobmsg_open_table(&b, "classification");
	for (int c = 0; c < CLASSIFICATION_LABELS; c++)
		blobmsg_add_u32(&b, traffic_class_names[c], class_counts[c]);
	blobmsg_close_table(&b, classes);

	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return UBUS_STATUS_OK;
}

static int handle_status(struct ubus_context *ctx, struct ubus_object *obj,
			 struct ubus_request_data *req, const char *method,
			 struct blob_attr *msg)
{
	struct tc_ubus_ctx *tc = container_of(obj, struct tc_ubus_ctx, obj);
	struct blob_buf b = {};

	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "version", "0.1.0");
	blobmsg_add_string(&b, "status", "running");
	blobmsg_add_u32(&b, "active_flows", tc->ft->count);
	blobmsg_add_u32(&b, "max_flows", tc->ft->max_entries);
	blobmsg_add_u32(&b, "tracked_stations", tc->sta->count);
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return UBUS_STATUS_OK;
}

static const struct ubus_method tc_methods[] = {
	UBUS_METHOD_NOARG("get_flows", handle_get_flows),
	UBUS_METHOD_NOARG("get_clients", handle_get_clients),
	UBUS_METHOD_NOARG("get_stats", handle_get_stats),
	UBUS_METHOD_NOARG("status", handle_status),
};

static struct ubus_object_type tc_obj_type =
	UBUS_OBJECT_TYPE("traffic-classifier", tc_methods);

int tc_ubus_init(struct tc_ubus_ctx *ctx)
{
	ctx->obj.name = "traffic-classifier";
	ctx->obj.type = &tc_obj_type;
	ctx->obj.methods = tc_methods;
	ctx->obj.n_methods = ARRAY_SIZE(tc_methods);

	int ret = ubus_add_object(ctx->ubus, &ctx->obj);
	if (ret) {
		syslog(LOG_ERR, "ubus_add_object failed: %s", ubus_strerror(ret));
		return ret;
	}

	syslog(LOG_INFO, "ubus: registered object 'traffic-classifier'");
	return 0;
}

void tc_ubus_cleanup(struct tc_ubus_ctx *ctx)
{
	ubus_remove_object(ctx->ubus, &ctx->obj);
}
