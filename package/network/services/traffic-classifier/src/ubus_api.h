#ifndef UBUS_API_H
#define UBUS_API_H

#include <libubus.h>
#include "flow_table.h"
#include "classifier.h"
#include "sta_tracker.h"

struct tc_ubus_ctx {
	struct ubus_context *ubus;
	struct ubus_object obj;
	struct flow_table *ft;
	struct classifier_ctx *classifier;
	struct sta_tracker *sta;
};

int tc_ubus_init(struct tc_ubus_ctx *ctx);
void tc_ubus_cleanup(struct tc_ubus_ctx *ctx);

#endif
