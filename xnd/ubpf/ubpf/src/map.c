#include "uxdp.h"

void
map_find(struct xdp_map *lookup, __u32 map)
{
	__u32 id = 0;
	int err;

	while (true) {
		struct bpf_map_info info = {};
		__u32 len = sizeof(info);
		int fd;

		err = bpf_map_get_next_id(id, &id);
		if (err) {
			if (errno == ENOENT)
				break;
			fprintf(stderr, "bpf_map_get_next_id failed\n");
			break;
		}

		if (id != map)
			continue;

		fd = bpf_map_get_fd_by_id(id);
		if (fd < 0) {
			if (errno == ENOENT)
				continue;
			fprintf(stderr, "bpf_map_get_fd_by_id failed\n");
			break;
		}

		err = bpf_obj_get_info_by_fd(fd, &info, &len);
		if (err) {
			fprintf(stderr, "bpf_obj_get_info_by_fd failed\n");
			close(fd);
			break;
		}
		if (!lookup) {
			fprintf(stderr, "\t\t%u: %s\n", info.id, *info.name ? info.name : "(unknown)");
			goto next;
		}
		if (!*info.name)
			goto next;
		if (strcmp(lookup->map, info.name))
			goto next;
		memcpy(&lookup->map_info, &info, sizeof(info));
		lookup->map_fd = fd;
		lookup->map_id = id;
		continue;
next:
		close(fd);
	}

}

int
map_lookup(struct xdp_map *lookup)
{
	net_find(lookup);
	if (!lookup->map_fd)
		return -1;
	if (map_verify(&lookup->map_info, &lookup->map_want))
		return -1;
	return 0;
}

int
map_verify(struct bpf_map_info *map, struct bpf_map_info *exp)
{
	if (exp->key_size && exp->key_size != map->key_size)
		return -1;
	if (exp->value_size && exp->value_size != map->value_size)
		return -1;
	if (exp->max_entries && exp->max_entries != map->max_entries)
		return -1;
	if (exp->type && exp->type  != map->type)
		return -1;

	return 0;
}


