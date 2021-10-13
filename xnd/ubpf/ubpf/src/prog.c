#include "uxdp.h"

static inline __u64 ptr_to_u64(const void *ptr)
{
	return (__u64)(unsigned long)ptr;
}

void
prog_find(struct xdp_map *lookup, __u32 prog)
{
	__u32 id = 0;

	while(true) {
		struct bpf_prog_info info = {};
		__u32 len = sizeof(info);
		int err;
		int fd;

		if (bpf_prog_get_next_id(id, &id)) {
			if (errno != ENOENT)
				fprintf(stderr, "bpf_prog_get_next_id failed\n");
			break;
		}
		if (id != prog)
			continue;
		fd = bpf_prog_get_fd_by_id(id);
		if (fd < 0) {
			if (errno == ENOENT)
				continue;
			fprintf(stderr, "bpf_prog_get_fd_by_id failed\n");
			break;
		}

		err = bpf_obj_get_info_by_fd(fd, &info, &len);
		if (err) {
			fprintf(stderr, "bpf_obj_get_info_by_fd failed\n");
			close(fd);
			break;
		}
		if (info.name && !lookup)
			fprintf(stderr, "\t%d: %s\n", id, info.name);
		if (info.nr_map_ids) {
			struct bpf_prog_info maps = {};
			__u32 len = sizeof(info);
			__u32 map_ids[info.nr_map_ids];
			unsigned int i;
			int err;

			maps.nr_map_ids = info.nr_map_ids;
			maps.map_ids = ptr_to_u64(map_ids);

			err = bpf_obj_get_info_by_fd(fd, &maps, &len);
			if (err || !maps.nr_map_ids) {
				fprintf(stderr, "failed to get maps\n");
				break;
			}
			for (i = 0; i < info.nr_map_ids; i++)
				map_find(lookup, map_ids[i]);
		}

		if (!lookup)
			fprintf(stderr, "\n");
		close(fd);
	}
}


