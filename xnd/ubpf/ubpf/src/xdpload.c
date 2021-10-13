/* SPDX-License-Identifier: GPL-2.0 */

#include "uxdp.h"

static int unload;

static void
parse_cmdline(int argc, char **argv, struct xdp_ctx *ctx)
{
	int ch;

	while ((ch = getopt(argc, argv, "d:f:p:u")) != -1) {
		switch (ch) {
		case 'd':
			ctx->ifindex = if_nametoindex(optarg);
			break;
		case 'f':
			strncpy(ctx->file, optarg, sizeof(ctx->file));
			break;
		case 'p':
			strncpy(ctx->progname, optarg, sizeof(ctx->progname));
			break;
		case 'u':
			unload = 1;
			break;
		default:
			fprintf(stderr, "Invalid argument\n");
			exit(-1);
		}
	}
}

int
main(int argc, char **argv)
{
	struct xdp_ctx ctx = {
		.flags = XDP_FLAGS_UPDATE_IF_NOEXIST | XDP_FLAGS_SKB_MODE,
	};

	parse_cmdline(argc, argv, &ctx);

	if (!ctx.ifindex) {
		fprintf(stderr, "invalid device\n");
		return -1;
	}

	if (unload)
		return xdp_detach(&ctx);

	if (!ctx.file[0] || !ctx.progname[0]) {
		fprintf(stderr, "invalid program\n");
		return -1;
	}

	return xdp_load(&ctx);
}
