#include "classifier.h"
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

struct classifier_ctx {
	char *model_path;
	bool model_loaded;
};

struct classifier_ctx *classifier_init(const char *model_path)
{
	struct classifier_ctx *ctx = calloc(1, sizeof(*ctx));
	if (!ctx)
		return NULL;

	if (model_path)
		ctx->model_path = strdup(model_path);

	/* TODO: Load treelite-compiled XGBoost model here */
	syslog(LOG_INFO, "classifier: using heuristic engine (model placeholder)");
	ctx->model_loaded = false;

	return ctx;
}

void classifier_destroy(struct classifier_ctx *ctx)
{
	if (!ctx)
		return;
	free(ctx->model_path);
	free(ctx);
}

static void classify_heuristic(struct flow_entry *entry, float features[NUM_FEATURES])
{
	uint16_t dst_port = entry->key.dst_port;
	uint16_t src_port = entry->key.src_port;
	uint8_t proto = entry->key.proto;

	float avg_pkt_size = features[6];
	float pkt_per_sec = features[12];
	float bytes_per_sec = features[13];
	float fwd_ratio = features[14];

	if (dst_port == 53 || src_port == 53) {
		entry->classification = CLASS_BROWSING;
		entry->confidence = 0.95f;
		return;
	}

	if (dst_port == 5060 || src_port == 5060 ||
	    (proto == FLOW_PROTO_UDP && avg_pkt_size < 300 &&
	     avg_pkt_size > 60 && pkt_per_sec > 20 && pkt_per_sec < 100)) {
		entry->classification = CLASS_VOIP;
		entry->confidence = 0.80f;
		return;
	}

	if (proto == FLOW_PROTO_UDP && avg_pkt_size < 200 &&
	    pkt_per_sec > 30 && fwd_ratio > 0.3f && fwd_ratio < 0.7f) {
		entry->classification = CLASS_GAMING;
		entry->confidence = 0.70f;
		return;
	}

	if (bytes_per_sec > 100000 && avg_pkt_size > 1000 && fwd_ratio < 0.15f) {
		entry->classification = CLASS_VIDEO;
		entry->confidence = 0.75f;
		return;
	}

	if (bytes_per_sec > 500000 && fwd_ratio < 0.05f) {
		entry->classification = CLASS_DOWNLOAD;
		entry->confidence = 0.70f;
		return;
	}

	if ((dst_port == 443 || src_port == 443) &&
	    bytes_per_sec > 5000 && bytes_per_sec < 100000 &&
	    avg_pkt_size > 200 && avg_pkt_size < 800) {
		entry->classification = CLASS_SOCIAL;
		entry->confidence = 0.50f;
		return;
	}

	if (dst_port == 443 || dst_port == 80 ||
	    src_port == 443 || src_port == 80) {
		entry->classification = CLASS_BROWSING;
		entry->confidence = 0.60f;
		return;
	}

	entry->classification = CLASS_OTHER;
	entry->confidence = 0.30f;
}

void classifier_classify_flow(struct classifier_ctx *ctx,
			      struct flow_entry *entry)
{
	float features[NUM_FEATURES];
	features_extract(entry, features);

	uint32_t total_pkts = entry->stats.total_pkts_fwd +
			      entry->stats.total_pkts_bwd;

	if (total_pkts < 5)
		return;

	if (ctx->model_loaded) {
		/* TODO: treelite/XGBoost inference path */
	}

	classify_heuristic(entry, features);
}

const char *classifier_class_name(enum traffic_class cls)
{
	if (cls < CLASSIFICATION_LABELS)
		return traffic_class_names[cls];
	return "unknown";
}
