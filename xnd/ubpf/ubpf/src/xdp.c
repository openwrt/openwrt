#include "uxdp.h"

int
xdp_load(struct xdp_ctx *ctx)
{
	struct bpf_prog_load_attr attr = {
		.prog_type = BPF_PROG_TYPE_XDP,
		.file = ctx->file,
	};
	int err;
	int fd = -1;

	err = bpf_prog_load_xattr(&attr, &ctx->obj, &fd);
	if (err) {
		fprintf(stderr, "failed to load %s %s\n", ctx->file, strerror(-err));
		return -1;
	}

	ctx->prog = bpf_object__find_program_by_title(ctx->obj, ctx->progname);
	if (!ctx->prog) {
		fprintf(stderr, "failed to find '%s'\n", ctx->progname);
		return -1;
	}

	ctx->fd = bpf_program__fd(ctx->prog);
	if (ctx->fd <= 0) {
		fprintf(stderr, "failed to get fd %s\n", ctx->progname);
		return -1;
	}

	err = bpf_set_link_xdp_fd(ctx->ifindex, ctx->fd, ctx->flags);
	if (err < 0) {
		switch(err) {
		case EBUSY:
		case EEXIST:
			fprintf(stderr, "program has already been loaded\n");
			break;
		default:
			fprintf(stderr, "failed to load program\n");
			break;
		}
		return -1;
	}

	return 0;
}

int
xdp_detach(struct xdp_ctx *ctx)
{
	__u32 id;

	if (bpf_get_link_xdp_id(ctx->ifindex, &id, ctx->flags)) {
		fprintf(stderr, "xdp: failed to get link\n");
		return -1;
	}

	if (!id)
		return 0;

	if (bpf_set_link_xdp_fd(ctx->ifindex, -1, ctx->flags)) {
		fprintf(stderr, "xdp: failed to detach\n");
		return -1;
	}

	return 0;
}
