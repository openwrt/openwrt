#ifndef CAPTURE_H
#define CAPTURE_H

#include "flow_table.h"

struct capture_ctx;

struct capture_ctx *capture_init(const char *ifname, struct flow_table *ft);
void capture_destroy(struct capture_ctx *ctx);

int capture_get_fd(struct capture_ctx *ctx);

int capture_process(struct capture_ctx *ctx);

#endif
