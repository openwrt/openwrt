#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>

#include <net/if.h>

#include <linux/limits.h>
#include <linux/if_xdp.h>
#include <linux/if_link.h>
#include <linux/if_xdp.h>

#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#define PROG_MAX	64
struct xdp_ctx {
	char file[PATH_MAX];
	char progname[PROG_MAX];
	__u32 flags;
	char ifindex;
	struct bpf_object *obj;
	struct bpf_program *prog;
	int fd;
};

struct xdp_map {
	char *net;
	char *prog;

	char *map;
	int map_fd;
	int map_id;
	struct bpf_map_info map_info;
	struct bpf_map_info map_want;
};

int xdp_load(struct xdp_ctx *ctx);
int xdp_detach(struct xdp_ctx *ctx);

int map_verify(struct bpf_map_info *map, struct bpf_map_info *exp);
int map_lookup(struct xdp_map *lookup);

void map_find(struct xdp_map *lookup, __u32 map);
void prog_find(struct xdp_map *lookup, __u32 prog);
void net_find(struct xdp_map *lookup);
