#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "flow_table.h"
#include "features.h"

/*
 * Two-phase classification:
 *
 * Phase 1 (heuristic): Port/protocol rules for obvious cases — DNS, VoIP,
 *   well-known ports. Catches ~40% of flows at zero cost.
 *
 * Phase 2 (ML model): Statistical features fed to a decision tree ensemble.
 *   Currently a heuristic stub. Will be replaced by a treelite-compiled
 *   XGBoost model (pure C, no runtime dependencies).
 */

struct classifier_ctx;

struct classifier_ctx *classifier_init(const char *model_path);
void classifier_destroy(struct classifier_ctx *ctx);

void classifier_classify_flow(struct classifier_ctx *ctx,
			      struct flow_entry *entry);

const char *classifier_class_name(enum traffic_class cls);

#endif
