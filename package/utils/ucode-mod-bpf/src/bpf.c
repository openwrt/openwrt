#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#include "ucode/module.h"

#define err_return_int(err, ...) do { set_error(err, __VA_ARGS__); return -1; } while(0)
#define err_return(err, ...) do { set_error(err, __VA_ARGS__); return NULL; } while(0)
#define TRUE ucv_boolean_new(true)

static uc_value_t *registry;
static uc_vm_t *debug_vm;

static struct {
	int code;
	char *msg;
} last_error;

struct uc_bpf_fd {
	int fd;
	bool close;
};

struct uc_bpf_map {
	struct uc_bpf_fd fd; /* must be first */
	unsigned int type;
	unsigned int key_size, val_size;
	unsigned int max_entries;
};

struct uc_bpf_map_iter {
	int fd;
	unsigned int key_size;
	bool has_next;
	uint8_t key[];
};

__attribute__((format(printf, 2, 3))) static void
set_error(int errcode, const char *fmt, ...)
{
	va_list ap;

	free(last_error.msg);

	last_error.code = errcode;
	last_error.msg = NULL;

	if (fmt) {
		va_start(ap, fmt);
		xvasprintf(&last_error.msg, fmt, ap);
		va_end(ap);
	}
}

static void init_env(void)
{
	static bool init_done = false;
	struct rlimit limit = {
		.rlim_cur = RLIM_INFINITY,
		.rlim_max = RLIM_INFINITY,
	};

	if (init_done)
		return;

	setrlimit(RLIMIT_MEMLOCK, &limit);
	init_done = true;
}

static uc_value_t *
uc_bpf_error(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *numeric = uc_fn_arg(0);
	const char *msg = last_error.msg;
	int code = last_error.code;
	uc_stringbuf_t *buf;
	const char *s;

	if (last_error.code == 0)
		return NULL;

	set_error(0, NULL);

	if (ucv_is_truish(numeric))
		return ucv_int64_new(code);

	buf = ucv_stringbuf_new();
	if (code < 0 && msg) {
		ucv_stringbuf_addstr(buf, msg, strlen(msg));
	} else {
		s = strerror(code);
		ucv_stringbuf_addstr(buf, s, strlen(s));
		if (msg)
			ucv_stringbuf_printf(buf, ": %s", msg);
	}

	return ucv_stringbuf_finish(buf);
}

static int
uc_bpf_module_set_opts(struct bpf_object *obj, uc_value_t *opts)
{
	uc_value_t *val;

	if (!opts)
		return 0;

	if (ucv_type(opts) != UC_OBJECT)
		err_return_int(EINVAL, "options argument");

	if ((val = ucv_object_get(opts, "rodata", NULL)) != NULL) {
		struct bpf_map *map = NULL;

		if (ucv_type(val) != UC_STRING)
			err_return_int(EINVAL, "rodata type");

		while ((map = bpf_object__next_map(obj, map)) != NULL) {
			if (!strstr(bpf_map__name(map), ".rodata"))
				continue;

			break;
		}

		if (!map)
			err_return_int(ENOENT, "rodata map");

		if (bpf_map__set_initial_value(map, ucv_string_get(val),
					       ucv_string_length(val)))
			err_return_int(errno, "rodata");
	}

	if ((val = ucv_object_get(opts, "program-type", NULL)) != NULL) {
		if (ucv_type(val) != UC_OBJECT)
			err_return_int(EINVAL, "prog_types argument");

		ucv_object_foreach(val, name, type) {
			struct bpf_program *prog;

			if (ucv_type(type) != UC_INTEGER)
				err_return_int(EINVAL, "program %s type", name);

			prog = bpf_object__find_program_by_name(obj, name);
			if (!prog)
				err_return_int(-1, "program %s not found", name);

			bpf_program__set_type(prog, ucv_int64_get(type));
		}
	}

	return 0;
}

static uc_value_t *
uc_bpf_open_module(uc_vm_t *vm, size_t nargs)
{
	DECLARE_LIBBPF_OPTS(bpf_object_open_opts, bpf_opts);
	uc_value_t *path = uc_fn_arg(0);
	uc_value_t *opts = uc_fn_arg(1);
	struct bpf_object *obj;

	if (ucv_type(path) != UC_STRING)
		err_return(EINVAL, "module path");

	init_env();
	obj = bpf_object__open_file(ucv_string_get(path), &bpf_opts);
	if (libbpf_get_error(obj))
		err_return(errno, NULL);

	if (uc_bpf_module_set_opts(obj, opts)) {
		bpf_object__close(obj);
		return NULL;
	}

	if (bpf_object__load(obj)) {
		bpf_object__close(obj);
		err_return(errno, NULL);
	}

	return ucv_resource_create(vm, "bpf.module", obj);
}

static uc_value_t *
uc_bpf_map_create(uc_vm_t *vm, uc_value_t *mod, int fd, unsigned int type,
		  unsigned int key_size, unsigned int val_size,
		  unsigned int max_entries, bool close)
{
	struct uc_bpf_map *uc_map;
	uc_value_t *res;

	res = ucv_resource_create_ex(vm, "bpf.map", (void **)&uc_map, 1, sizeof(*uc_map));
	ucv_resource_value_set(res, 0, ucv_get(mod));
	uc_map->fd.fd = fd;
	uc_map->type = type;
	uc_map->key_size = key_size;
	uc_map->val_size = val_size;
	uc_map->max_entries = max_entries;
	uc_map->fd.close = close;

	return res;
}

static uc_value_t *
uc_bpf_prog_create(uc_vm_t *vm, uc_value_t *mod, int fd, bool close)
{
	struct uc_bpf_fd *uc_fd;
	uc_value_t *res;

	res = ucv_resource_create_ex(vm, "bpf.program", (void **)&uc_fd, 1, sizeof(*uc_fd));
	ucv_resource_value_set(res, 0, ucv_get(mod));
	uc_fd->fd = fd;
	uc_fd->close = close;

	return res;
}

static uc_value_t *
uc_bpf_open_map_fd(uc_vm_t *vm, int fd)
{
	struct bpf_map_info info;
	__u32 len = sizeof(info);
	int err;

	err = bpf_obj_get_info_by_fd(fd, &info, &len);
	if (err) {
		close(fd);
		err_return(errno, NULL);
	}

	return uc_bpf_map_create(vm, NULL, fd, info.type, info.key_size,
				 info.value_size, info.max_entries, true);
}

static uc_value_t *
uc_bpf_open_map(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *path = uc_fn_arg(0);
	int fd;

	if (ucv_type(path) != UC_STRING)
		err_return(EINVAL, "module path");

	fd = bpf_obj_get(ucv_string_get(path));
	if (fd < 0)
		err_return(errno, NULL);

	return uc_bpf_open_map_fd(vm, fd);
}

static uc_value_t *
uc_bpf_open_map_id(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *id = uc_fn_arg(0);
	int fd;

	if (ucv_type(id) != UC_INTEGER)
		err_return(EINVAL, "map id");

	fd = bpf_map_get_fd_by_id(ucv_int64_get(id));
	if (fd < 0)
		err_return(errno, NULL);

	return uc_bpf_open_map_fd(vm, fd);
}

static uc_value_t *
uc_bpf_open_program(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *path = uc_fn_arg(0);
	int fd;

	if (ucv_type(path) != UC_STRING)
		err_return(EINVAL, "module path");

	fd = bpf_obj_get(ucv_string_get(path));
	if (fd < 0)
		err_return(errno, NULL);

	return uc_bpf_prog_create(vm, NULL, fd, true);
}

static uc_value_t *
uc_bpf_open_program_id(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *id = uc_fn_arg(0);
	int fd;

	if (ucv_type(id) != UC_INTEGER)
		err_return(EINVAL, "program id");

	fd = bpf_prog_get_fd_by_id(ucv_int64_get(id));
	if (fd < 0)
		err_return(errno, NULL);

	return uc_bpf_prog_create(vm, NULL, fd, true);
}

static uc_value_t *
uc_bpf_module_get_maps(uc_vm_t *vm, size_t nargs)
{
	struct bpf_object *obj = uc_fn_thisval("bpf.module");
	struct bpf_map *map = NULL;
	uc_value_t *rv;
	int i = 0;

	if (!obj)
		err_return(EINVAL, NULL);

	rv = ucv_array_new(vm);
	bpf_object__for_each_map(map, obj)
		ucv_array_set(rv, i++, ucv_string_new(bpf_map__name(map)));

	return rv;
}

static uc_value_t *
uc_bpf_module_get_map(uc_vm_t *vm, size_t nargs)
{
	struct bpf_object *obj = uc_fn_thisval("bpf.module");
	struct bpf_map *map;
	uc_value_t *name = uc_fn_arg(0);
	int fd;

	if (!obj || ucv_type(name) != UC_STRING)
		err_return(EINVAL, NULL);

	map = bpf_object__find_map_by_name(obj, ucv_string_get(name));
	if (!map)
		err_return(errno, NULL);

	fd = bpf_map__fd(map);
	if (fd < 0)
		err_return(EINVAL, NULL);

	return uc_bpf_map_create(vm, _uc_fn_this_res(vm), fd, bpf_map__type(map),
				 bpf_map__key_size(map), bpf_map__value_size(map),
				 bpf_map__max_entries(map), false);
}

static uc_value_t *
uc_bpf_module_get_programs(uc_vm_t *vm, size_t nargs)
{
	struct bpf_object *obj = uc_fn_thisval("bpf.module");
	struct bpf_program *prog = NULL;
	uc_value_t *rv;
	int i = 0;

	if (!obj)
		err_return(EINVAL, NULL);

	rv = ucv_array_new(vm);
	bpf_object__for_each_program(prog, obj)
		ucv_array_set(rv, i++, ucv_string_new(bpf_program__name(prog)));

	return rv;
}

static uc_value_t *
uc_bpf_module_get_program(uc_vm_t *vm, size_t nargs)
{
	struct bpf_object *obj = uc_fn_thisval("bpf.module");
	struct bpf_program *prog;
	uc_value_t *name = uc_fn_arg(0);
	int fd;

	if (!obj || !name || ucv_type(name) != UC_STRING)
		err_return(EINVAL, NULL);

	prog = bpf_object__find_program_by_name(obj, ucv_string_get(name));
	if (!prog)
		err_return(errno, NULL);

	fd = bpf_program__fd(prog);
	if (fd < 0)
		err_return(EINVAL, NULL);

	return uc_bpf_prog_create(vm, _uc_fn_this_res(vm), fd, false);
}

static void *
uc_bpf_map_arg(uc_value_t *val, const char *kind, unsigned int size,
	       uint64_t *val_int)
{
	switch (ucv_type(val)) {
	case UC_INTEGER:
		if (size == 4)
			*(uint32_t *)val_int = ucv_int64_get(val);
		else if (size == 8)
			*val_int = ucv_int64_get(val);
		else
			break;

		return val_int;
	case UC_STRING:
		if (size != ucv_string_length(val))
			break;

		return ucv_string_get(val);
	case UC_RESOURCE: {
		struct uc_bpf_fd *f;

		f = ucv_resource_data(val, "bpf.map");
		if (!f)
			f = ucv_resource_data(val, "bpf.program");
		if (!f)
			err_return(EINVAL, "%s type", kind);

		if (size != 4)
			break;

		*(uint32_t *)val_int = f->fd;

		return val_int;
	}
	default:
		err_return(EINVAL, "%s type", kind);
	}

	err_return(EINVAL, "%s size mismatch (expected: %d)", kind, size);
}

static bool
uc_bpf_map_key_arg(struct uc_bpf_map *map, uc_value_t *a_key, uint64_t *buf,
		   void **key)
{
	if (!map->key_size) {
		if (a_key) {
			set_error(EINVAL, "map has no key");
			return false;
		}

		*key = NULL;

		return true;
	}

	*key = uc_bpf_map_arg(a_key, "key", map->key_size, buf);

	return *key != NULL;
}

static bool
uc_bpf_map_is_percpu(struct uc_bpf_map *map)
{
	switch (map->type) {
	case BPF_MAP_TYPE_PERCPU_HASH:
	case BPF_MAP_TYPE_PERCPU_ARRAY:
	case BPF_MAP_TYPE_LRU_PERCPU_HASH:
	case BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE:
		return true;
	default:
		return false;
	}
}

static unsigned int
uc_bpf_map_val_stride(struct uc_bpf_map *map)
{
	return (map->val_size + 7) & ~7U;
}

static bool
uc_bpf_map_val_len(struct uc_bpf_map *map, unsigned int *len, int *num_cpus)
{
	*len = map->val_size;
	*num_cpus = 1;

	if (!uc_bpf_map_is_percpu(map))
		return true;

	*num_cpus = libbpf_num_possible_cpus();
	if (*num_cpus <= 0) {
		set_error(errno, "number of CPUs");
		return false;
	}

	*len = uc_bpf_map_val_stride(map) * *num_cpus;

	return true;
}

static uc_value_t *
uc_bpf_map_val_read(uc_vm_t *vm, struct uc_bpf_map *map, void *val,
		    int num_cpus)
{
	unsigned int stride = uc_bpf_map_val_stride(map);
	uc_value_t *rv;
	int i;

	if (!uc_bpf_map_is_percpu(map))
		return ucv_string_new_length(val, map->val_size);

	rv = ucv_array_new(vm);
	for (i = 0; i < num_cpus; i++)
		ucv_array_set(rv, i, ucv_string_new_length((char *)val + i * stride,
							   map->val_size));

	return rv;
}

static bool
uc_bpf_map_val_copy(struct uc_bpf_map *map, uc_value_t *a_val, void *dest)
{
	uint64_t val_int;
	void *val;

	val = uc_bpf_map_arg(a_val, "value", map->val_size, &val_int);
	if (!val)
		return false;

	memcpy(dest, val, map->val_size);

	return true;
}

static void *
uc_bpf_map_val_arg(struct uc_bpf_map *map, uc_value_t *a_val, void *buf,
		   int num_cpus)
{
	unsigned int stride = uc_bpf_map_val_stride(map);
	int i;

	if (!uc_bpf_map_is_percpu(map))
		return uc_bpf_map_arg(a_val, "value", map->val_size, buf);

	memset(buf, 0, stride * num_cpus);
	if (ucv_type(a_val) != UC_ARRAY) {
		if (!uc_bpf_map_val_copy(map, a_val, buf))
			return NULL;

		for (i = 1; i < num_cpus; i++)
			memcpy((char *)buf + i * stride, buf, map->val_size);

		return buf;
	}

	if (ucv_array_length(a_val) != (size_t)num_cpus)
		err_return(EINVAL, "value count mismatch (expected: %d)", num_cpus);

	for (i = 0; i < num_cpus; i++)
		if (!uc_bpf_map_val_copy(map, ucv_array_get(a_val, i),
					 (char *)buf + i * stride))
			return NULL;

	return buf;
}

static uc_value_t *
uc_bpf_map_info(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	uc_value_t *rv;

	if (!map)
		err_return(EINVAL, NULL);

	rv = ucv_object_new(vm);
	ucv_object_add(rv, "type", ucv_int64_new(map->type));
	ucv_object_add(rv, "key_size", ucv_int64_new(map->key_size));
	ucv_object_add(rv, "value_size", ucv_int64_new(map->val_size));
	ucv_object_add(rv, "max_entries", ucv_int64_new(map->max_entries));

	return rv;
}

static uc_value_t *
uc_bpf_map_get(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	uc_value_t *a_key = uc_fn_arg(0);
	unsigned int val_len;
	uint64_t key_int;
	void *key, *val;
	int num_cpus;

	if (!map)
		err_return(EINVAL, NULL);

	if (!uc_bpf_map_key_arg(map, a_key, &key_int, &key))
		return NULL;

	if (!uc_bpf_map_val_len(map, &val_len, &num_cpus))
		return NULL;

	val = alloca(val_len);
	if (bpf_map_lookup_elem(map->fd.fd, key, val))
		return NULL;

	return uc_bpf_map_val_read(vm, map, val, num_cpus);
}

static uc_value_t *
uc_bpf_map_set(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	uc_value_t *a_key = uc_fn_arg(0);
	uc_value_t *a_val = uc_fn_arg(1);
	uc_value_t *a_flags = uc_fn_arg(2);
	unsigned int val_len;
	uint64_t key_int;
	uint64_t flags;
	void *key, *val;
	int num_cpus;

	if (!map)
		err_return(EINVAL, NULL);

	if (!uc_bpf_map_key_arg(map, a_key, &key_int, &key))
		return NULL;

	if (!uc_bpf_map_val_len(map, &val_len, &num_cpus))
		return NULL;

	val = alloca(val_len);
	val = uc_bpf_map_val_arg(map, a_val, val, num_cpus);
	if (!val)
		return NULL;

	if (!a_flags)
		flags = BPF_ANY;
	else if (ucv_type(a_flags) != UC_INTEGER)
		err_return(EINVAL, "flags");
	else
		flags = ucv_int64_get(a_flags);

	if (bpf_map_update_elem(map->fd.fd, key, val, flags))
		return NULL;

	return uc_bpf_map_val_read(vm, map, val, num_cpus);
}

static uc_value_t *
uc_bpf_map_delete(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	uc_value_t *a_key = uc_fn_arg(0);
	uc_value_t *a_return = uc_fn_arg(1);
	unsigned int val_len;
	void *key, *val = NULL;
	uint64_t key_int;
	int num_cpus;
	int ret;

	if (!map)
		err_return(EINVAL, NULL);

	if (!uc_bpf_map_key_arg(map, a_key, &key_int, &key))
		return NULL;

	if (!ucv_is_truish(a_return)) {
		ret = bpf_map_delete_elem(map->fd.fd, key);

		return ucv_boolean_new(ret == 0);
	}

	if (!uc_bpf_map_val_len(map, &val_len, &num_cpus))
		return NULL;

	val = alloca(val_len);
	if (bpf_map_lookup_and_delete_elem(map->fd.fd, key, val))
		return NULL;

	return uc_bpf_map_val_read(vm, map, val, num_cpus);
}

static uc_value_t *
uc_bpf_map_delete_all(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	uc_value_t *filter = uc_fn_arg(0);
	bool has_next;
	void *key, *next;

	if (!map)
		err_return(EINVAL, NULL);

	key = alloca(map->key_size);
	next = alloca(map->key_size);
	has_next = !bpf_map_get_next_key(map->fd.fd, NULL, next);
	while (has_next) {
		bool skip = false;

		memcpy(key, next, map->key_size);
		has_next = !bpf_map_get_next_key(map->fd.fd, next, next);

		if (ucv_is_callable(filter)) {
			uc_value_t *rv;

			uc_value_push(ucv_get(filter));
			uc_value_push(ucv_string_new_length((const char *)key, map->key_size));
			if (uc_call(1) != EXCEPTION_NONE)
				break;

			rv = uc_vm_stack_pop(vm);
			if (!rv)
				break;

			skip = !ucv_is_truish(rv);
			ucv_put(rv);
		}

		if (!skip)
			bpf_map_delete_elem(map->fd.fd, key);
	}

	return TRUE;
}

static uc_value_t *
uc_bpf_map_iterator(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	struct uc_bpf_map_iter *iter;
	uc_value_t *res;

	if (!map)
		err_return(EINVAL, NULL);

	res = ucv_resource_create_ex(vm, "bpf.map_iter", (void **)&iter, 1, sizeof(*iter) + map->key_size);
	ucv_resource_value_set(res, 0, ucv_get(_uc_fn_this_res(vm)));
	iter->fd = map->fd.fd;
	iter->key_size = map->key_size;
	iter->has_next = !bpf_map_get_next_key(iter->fd, NULL, &iter->key);

	return res;
}

static uc_value_t *
uc_bpf_map_iter_next(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map_iter *iter = uc_fn_thisval("bpf.map_iter");
	uc_value_t *rv;

	if (!iter)
		err_return(EINVAL, NULL);

	if (!iter->has_next)
		return NULL;

	rv = ucv_string_new_length((const char *)iter->key, iter->key_size);
	iter->has_next = !bpf_map_get_next_key(iter->fd, &iter->key, &iter->key);

	return rv;
}

static uc_value_t *
uc_bpf_map_iter_next_int(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map_iter *iter = uc_fn_thisval("bpf.map_iter");
	uint64_t intval;
	uc_value_t *rv;

	if (!iter)
		err_return(EINVAL, NULL);

	if (!iter->has_next)
		return NULL;

	if (iter->key_size == 4) {
		uint32_t val32;

		memcpy(&val32, iter->key, sizeof(val32));
		intval = val32;
	} else if (iter->key_size == 8) {
		memcpy(&intval, iter->key, sizeof(intval));
	} else {
		return NULL;
	}

	rv = ucv_int64_new(intval);
	iter->has_next = !bpf_map_get_next_key(iter->fd, &iter->key, &iter->key);

	return rv;
}

static uc_value_t *
uc_bpf_map_foreach(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	uc_value_t *func = uc_fn_arg(0);
	bool has_next;
	void *key, *next;
	bool ret = false;

	if (!map)
		err_return(EINVAL, NULL);

	key = alloca(map->key_size);
	next = alloca(map->key_size);
	has_next = !bpf_map_get_next_key(map->fd.fd, NULL, next);

	while (has_next) {
		uc_value_t *rv;
		bool stop;

		memcpy(key, next, map->key_size);
		has_next = !bpf_map_get_next_key(map->fd.fd, next, next);

		uc_value_push(ucv_get(func));
		uc_value_push(ucv_string_new_length((const char *)key, map->key_size));

		if (uc_call(1) != EXCEPTION_NONE)
			break;

		rv = uc_vm_stack_pop(vm);
		stop = (ucv_type(rv) == UC_BOOLEAN && !ucv_boolean_get(rv));
		ucv_put(rv);

		if (stop)
			break;

		ret = true;
	}

	return ucv_boolean_new(ret);
}

enum {
	UC_BPF_BUF_CB,
	UC_BPF_BUF_LOST_CB,
	UC_BPF_BUF_MAP,
	__UC_BPF_BUF_MAX
};

struct uc_bpf_buffer {
	uc_vm_t *vm;
	uc_value_t *res;
	bool perf;
	bool exception;
	union {
		struct ring_buffer *rb;
		struct perf_buffer *pb;
	};
};

static uc_value_t *
uc_bpf_buffer_call(struct uc_bpf_buffer *buf, size_t slot, uc_value_t *arg,
		   int cpu)
{
	uc_vm_t *vm = buf->vm;
	size_t fn_nargs = 1;

	uc_vm_stack_push(vm, ucv_get(ucv_resource_value_get(buf->res, slot)));
	uc_vm_stack_push(vm, arg);
	if (cpu >= 0) {
		uc_vm_stack_push(vm, ucv_int64_new(cpu));
		fn_nargs++;
	}

	if (uc_vm_call(vm, false, fn_nargs) != EXCEPTION_NONE) {
		buf->exception = true;
		return NULL;
	}

	return uc_vm_stack_pop(vm);
}

static int
uc_bpf_ringbuf_sample(void *ctx, void *data, size_t size)
{
	struct uc_bpf_buffer *buf = ctx;
	uc_value_t *rv;
	int ret = 0;

	if (buf->exception)
		return -1;

	rv = uc_bpf_buffer_call(buf, UC_BPF_BUF_CB,
				ucv_string_new_length(data, size), -1);
	if (buf->exception)
		return -1;

	if (ucv_type(rv) == UC_INTEGER)
		ret = ucv_int64_get(rv);
	ucv_put(rv);

	return ret;
}

static void
uc_bpf_perf_sample(void *ctx, int cpu, void *data, __u32 size)
{
	struct uc_bpf_buffer *buf = ctx;

	if (buf->exception)
		return;

	ucv_put(uc_bpf_buffer_call(buf, UC_BPF_BUF_CB,
				   ucv_string_new_length(data, size), cpu));
}

static void
uc_bpf_perf_lost(void *ctx, int cpu, __u64 cnt)
{
	struct uc_bpf_buffer *buf = ctx;

	if (buf->exception || !ucv_resource_value_get(buf->res, UC_BPF_BUF_LOST_CB))
		return;

	ucv_put(uc_bpf_buffer_call(buf, UC_BPF_BUF_LOST_CB, ucv_int64_new(cnt),
				   cpu));
}

static uc_value_t *
uc_bpf_map_buffer_create(uc_vm_t *vm, struct uc_bpf_map *map, bool perf,
			 size_t pages, uc_value_t *cb, uc_value_t *lost_cb)
{
	struct uc_bpf_buffer *buf;
	uc_value_t *res;

	res = ucv_resource_create_ex(vm, "bpf.buffer", (void **)&buf,
				     __UC_BPF_BUF_MAX, sizeof(*buf));
	buf->vm = vm;
	buf->res = res;
	buf->perf = perf;
	if (perf)
		buf->pb = perf_buffer__new(map->fd.fd, pages, uc_bpf_perf_sample,
					   uc_bpf_perf_lost, buf, NULL);
	else
		buf->rb = ring_buffer__new(map->fd.fd, uc_bpf_ringbuf_sample,
					   buf, NULL);
	if (!buf->rb) {
		ucv_put(res);
		err_return(errno, NULL);
	}

	ucv_resource_value_set(res, UC_BPF_BUF_CB, ucv_get(cb));
	ucv_resource_value_set(res, UC_BPF_BUF_LOST_CB, ucv_get(lost_cb));
	ucv_resource_value_set(res, UC_BPF_BUF_MAP, ucv_get(_uc_fn_this_res(vm)));

	return res;
}

static uc_value_t *
uc_bpf_map_ringbuf(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	uc_value_t *cb = uc_fn_arg(0);

	if (!map)
		err_return(EINVAL, NULL);

	if (map->type != BPF_MAP_TYPE_RINGBUF)
		err_return(EINVAL, "map type");

	if (!ucv_is_callable(cb))
		err_return(EINVAL, "callback");

	return uc_bpf_map_buffer_create(vm, map, false, 0, cb, NULL);
}

static uc_value_t *
uc_bpf_map_perf_buffer(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_map *map = uc_fn_thisval("bpf.map");
	uc_value_t *cb = uc_fn_arg(0);
	uc_value_t *a_pages = uc_fn_arg(1);
	uc_value_t *lost_cb = uc_fn_arg(2);
	size_t pages = 8;

	if (!map)
		err_return(EINVAL, NULL);

	if (map->type != BPF_MAP_TYPE_PERF_EVENT_ARRAY)
		err_return(EINVAL, "map type");

	if (!ucv_is_callable(cb))
		err_return(EINVAL, "callback");

	if (a_pages) {
		if (ucv_type(a_pages) != UC_INTEGER || ucv_int64_get(a_pages) <= 0)
			err_return(EINVAL, "page count");

		pages = ucv_int64_get(a_pages);
	}

	if (lost_cb && !ucv_is_callable(lost_cb))
		err_return(EINVAL, "lost callback");

	return uc_bpf_map_buffer_create(vm, map, true, pages, cb, lost_cb);
}

static uc_value_t *
uc_bpf_buffer_fileno(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_buffer *buf = uc_fn_thisval("bpf.buffer");
	int fd;

	if (!buf)
		err_return(EINVAL, NULL);

	fd = buf->perf ? perf_buffer__epoll_fd(buf->pb) :
			 ring_buffer__epoll_fd(buf->rb);
	if (fd < 0)
		err_return(errno, NULL);

	return ucv_int64_new(fd);
}

static uc_value_t *
uc_bpf_buffer_result(uc_vm_t *vm, struct uc_bpf_buffer *buf, int ret)
{
	if (buf->exception) {
		buf->exception = false;
		return NULL;
	}

	if (ret < 0)
		err_return(errno, NULL);

	return ucv_int64_new(ret);
}

static uc_value_t *
uc_bpf_buffer_poll(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_buffer *buf = uc_fn_thisval("bpf.buffer");
	uc_value_t *a_timeout = uc_fn_arg(0);
	int timeout = 0;
	int ret;

	if (!buf)
		err_return(EINVAL, NULL);

	if (a_timeout) {
		if (ucv_type(a_timeout) != UC_INTEGER)
			err_return(EINVAL, "timeout");

		timeout = ucv_int64_get(a_timeout);
	}

	ret = buf->perf ? perf_buffer__poll(buf->pb, timeout) :
			  ring_buffer__poll(buf->rb, timeout);

	return uc_bpf_buffer_result(vm, buf, ret);
}

static uc_value_t *
uc_bpf_buffer_consume(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_buffer *buf = uc_fn_thisval("bpf.buffer");
	int ret;

	if (!buf)
		err_return(EINVAL, NULL);

	ret = buf->perf ? perf_buffer__consume(buf->pb) :
			  ring_buffer__consume(buf->rb);

	return uc_bpf_buffer_result(vm, buf, ret);
}

static uc_value_t *
uc_bpf_obj_pin(uc_vm_t *vm, size_t nargs, const char *type)
{
	struct uc_bpf_fd *f = uc_fn_thisval(type);
	uc_value_t *path = uc_fn_arg(0);

	if (!f)
		err_return(EINVAL, NULL);

	if (ucv_type(path) != UC_STRING)
		err_return(EINVAL, NULL);

	if (bpf_obj_pin(f->fd, ucv_string_get(path)))
		err_return(errno, NULL);

	return TRUE;
}

static uc_value_t *
uc_bpf_program_pin(uc_vm_t *vm, size_t nargs)
{
	return uc_bpf_obj_pin(vm, nargs, "bpf.program");
}

static uc_value_t *
uc_bpf_map_pin(uc_vm_t *vm, size_t nargs)
{
	return uc_bpf_obj_pin(vm, nargs, "bpf.map");
}

static uc_value_t *
uc_bpf_set_tc_hook(uc_value_t *ifname, uc_value_t *type, uc_value_t *prio,
		   uc_value_t *classid, int fd)
{
	DECLARE_LIBBPF_OPTS(bpf_tc_hook, hook);
	DECLARE_LIBBPF_OPTS(bpf_tc_opts, attach_tc,
			    .handle = 1);
	const char *type_str;
	uint64_t prio_val;

	if (ucv_type(ifname) != UC_STRING || ucv_type(type) != UC_STRING ||
	    ucv_type(prio) != UC_INTEGER)
		err_return(EINVAL, NULL);

	prio_val = ucv_int64_get(prio);
	if (prio_val > 0xffff)
		err_return(EINVAL, NULL);

	type_str = ucv_string_get(type);
	if (!strcmp(type_str, "ingress"))
		hook.attach_point = BPF_TC_INGRESS;
	else if (!strcmp(type_str, "egress"))
		hook.attach_point = BPF_TC_EGRESS;
	else
		err_return(EINVAL, NULL);

	hook.ifindex = if_nametoindex(ucv_string_get(ifname));
	if (!hook.ifindex)
		goto error;

	bpf_tc_hook_create(&hook);
	attach_tc.priority = prio_val;
	if (bpf_tc_detach(&hook, &attach_tc) < 0 && fd < 0)
		goto error;

	if (fd < 0)
		goto out;

	attach_tc.prog_fd = fd;
	attach_tc.classid = ucv_int64_get(classid);
	if (bpf_tc_attach(&hook, &attach_tc) < 0)
		goto error;

out:
	return TRUE;

error:
	err_return(errno, NULL);
}

static uc_value_t *
uc_bpf_program_tc_attach(uc_vm_t *vm, size_t nargs)
{
	struct uc_bpf_fd *f = uc_fn_thisval("bpf.program");
	uc_value_t *ifname = uc_fn_arg(0);
	uc_value_t *type = uc_fn_arg(1);
	uc_value_t *prio = uc_fn_arg(2);
	uc_value_t *classid = uc_fn_arg(3);

	if (!f)
		err_return(EINVAL, NULL);

	return uc_bpf_set_tc_hook(ifname, type, prio, classid, f->fd);
}

static uc_value_t *
uc_bpf_tc_detach(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *ifname = uc_fn_arg(0);
	uc_value_t *type = uc_fn_arg(1);
	uc_value_t *prio = uc_fn_arg(2);

	return uc_bpf_set_tc_hook(ifname, type, prio, NULL, -1);
}

static int
uc_bpf_debug_print(enum libbpf_print_level level, const char *format,
		   va_list args)
{
	char buf[256], *str = NULL;
	uc_value_t *val;
	va_list ap;
	int size;

	va_copy(ap, args);
	size = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);

	if (size > 0 && (unsigned long)size < ARRAY_SIZE(buf) - 1) {
		val = ucv_string_new(buf);
		goto out;
	}

	if (vasprintf(&str, format, args) < 0)
		return 0;

	val = ucv_string_new(str);
	free(str);

out:
	uc_vm_stack_push(debug_vm, ucv_get(ucv_array_get(registry, 0)));
	uc_vm_stack_push(debug_vm, ucv_int64_new(level));
	uc_vm_stack_push(debug_vm, val);
	if (uc_vm_call(debug_vm, false, 2) == EXCEPTION_NONE)
		ucv_put(uc_vm_stack_pop(debug_vm));

	return 0;
}

static uc_value_t *
uc_bpf_set_debug_handler(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *handler = uc_fn_arg(0);

	if (handler && !ucv_is_callable(handler))
		err_return(EINVAL, NULL);

	debug_vm = vm;
	libbpf_set_print(handler ? uc_bpf_debug_print : NULL);

	ucv_array_set(registry, 0, ucv_get(handler));

	return NULL;
}

static void
register_constants(uc_vm_t *vm, uc_value_t *scope)
{
#define ADD_CONST(x) ucv_object_add(scope, #x, ucv_int64_new(x))
	ADD_CONST(BPF_PROG_TYPE_SCHED_CLS);
	ADD_CONST(BPF_PROG_TYPE_SCHED_ACT);

	ADD_CONST(BPF_MAP_TYPE_HASH);
	ADD_CONST(BPF_MAP_TYPE_ARRAY);
	ADD_CONST(BPF_MAP_TYPE_PROG_ARRAY);
	ADD_CONST(BPF_MAP_TYPE_PERF_EVENT_ARRAY);
	ADD_CONST(BPF_MAP_TYPE_PERCPU_HASH);
	ADD_CONST(BPF_MAP_TYPE_PERCPU_ARRAY);
	ADD_CONST(BPF_MAP_TYPE_LRU_HASH);
	ADD_CONST(BPF_MAP_TYPE_LRU_PERCPU_HASH);
	ADD_CONST(BPF_MAP_TYPE_LPM_TRIE);
	ADD_CONST(BPF_MAP_TYPE_ARRAY_OF_MAPS);
	ADD_CONST(BPF_MAP_TYPE_HASH_OF_MAPS);
	ADD_CONST(BPF_MAP_TYPE_DEVMAP);
	ADD_CONST(BPF_MAP_TYPE_SOCKMAP);
	ADD_CONST(BPF_MAP_TYPE_QUEUE);
	ADD_CONST(BPF_MAP_TYPE_STACK);
	ADD_CONST(BPF_MAP_TYPE_RINGBUF);

	ADD_CONST(BPF_ANY);
	ADD_CONST(BPF_NOEXIST);
	ADD_CONST(BPF_EXIST);
	ADD_CONST(BPF_F_LOCK);
}

static const uc_function_list_t module_fns[] = {
	{ "get_map",			uc_bpf_module_get_map },
	{ "get_maps",			uc_bpf_module_get_maps },
	{ "get_programs",		uc_bpf_module_get_programs },
	{ "get_program",		uc_bpf_module_get_program },
};

static void module_free(void *ptr)
{
	struct bpf_object *obj = ptr;

	bpf_object__close(obj);
}

static const uc_function_list_t map_fns[] = {
	{ "pin",			uc_bpf_map_pin },
	{ "info",			uc_bpf_map_info },
	{ "get",			uc_bpf_map_get },
	{ "set",			uc_bpf_map_set },
	{ "delete",			uc_bpf_map_delete },
	{ "delete_all",			uc_bpf_map_delete_all },
	{ "foreach",			uc_bpf_map_foreach },
	{ "iterator",			uc_bpf_map_iterator },
	{ "ringbuf",			uc_bpf_map_ringbuf },
	{ "perf_buffer",		uc_bpf_map_perf_buffer },
};

static void uc_bpf_buffer_free(void *ptr)
{
	struct uc_bpf_buffer *buf = ptr;

	if (buf->perf)
		perf_buffer__free(buf->pb);
	else
		ring_buffer__free(buf->rb);
}

static const uc_function_list_t buffer_fns[] = {
	{ "fileno",			uc_bpf_buffer_fileno },
	{ "poll",			uc_bpf_buffer_poll },
	{ "consume",			uc_bpf_buffer_consume },
};

static void uc_bpf_fd_free(void *ptr)
{
	struct uc_bpf_fd *f = ptr;

	if (f->close)
		close(f->fd);
}

static const uc_function_list_t map_iter_fns[] = {
	{ "next",			uc_bpf_map_iter_next },
	{ "next_int",			uc_bpf_map_iter_next_int },
};

static const uc_function_list_t prog_fns[] = {
	{ "pin",			uc_bpf_program_pin },
	{ "tc_attach",			uc_bpf_program_tc_attach },
};

static const uc_function_list_t global_fns[] = {
	{ "error",			uc_bpf_error },
	{ "set_debug_handler",		uc_bpf_set_debug_handler },
	{ "open_module",		uc_bpf_open_module },
	{ "open_map",			uc_bpf_open_map },
	{ "open_map_id",		uc_bpf_open_map_id },
	{ "open_program",		uc_bpf_open_program },
	{ "open_program_id",		uc_bpf_open_program_id },
	{ "tc_detach",			uc_bpf_tc_detach },
};

void uc_module_init(uc_vm_t *vm, uc_value_t *scope)
{
	uc_function_list_register(scope, global_fns);
	register_constants(vm, scope);

	registry = ucv_array_new(vm);
	uc_vm_registry_set(vm, "bpf.registry", registry);

	uc_type_declare(vm, "bpf.module", module_fns, module_free);
	uc_type_declare(vm, "bpf.map", map_fns, uc_bpf_fd_free);
	uc_type_declare(vm, "bpf.map_iter", map_iter_fns, NULL);
	uc_type_declare(vm, "bpf.buffer", buffer_fns, uc_bpf_buffer_free);
	uc_type_declare(vm, "bpf.program", prog_fns, uc_bpf_fd_free);
}
