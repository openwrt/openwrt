#include <ucode/module.h>
#include "uht.h"

static uc_resource_type_t *reader_type, *hashtbl_type;
static char *hash_key;

static uint32_t
writer_store_data(struct uht_writer *wr, uc_value_t *val)
{
	uint32_t *data, ret;
	size_t i, len;

	switch (ucv_type(val)) {
	case UC_STRING:
		return uht_writer_add_string(wr, ucv_string_get(val));
	case UC_INTEGER:
		return uht_writer_add_int(wr, ucv_int64_get(val));
	case UC_DOUBLE:
		return uht_writer_add_double(wr, ucv_double_get(val));
	case UC_BOOLEAN:
		return uht_writer_add_bool(wr, ucv_boolean_get(val));
	case UC_ARRAY:
		len = ucv_array_length(val);
		data = calloc(len, sizeof(*data));
		for (i = 0; i < len; i++)
			data[i] = writer_store_data(wr, ucv_array_get(val, i));
		ret = uht_writer_add_array(wr, data, len);
		free(data);
		return ret;
	case UC_OBJECT:
		len = ucv_object_length(val);
		if (ucv_is_truish(ucv_object_get(val, hash_key, NULL))) {
			ret = uht_writer_hashtbl_alloc(wr, len - 1);
			ucv_object_foreach(val, key, value) {
				if (!strcmp(key, hash_key))
					continue;
				uht_writer_hashtbl_add_element(wr, ret, key,
							       writer_store_data(wr, value));
			}
			uht_writer_hashtbl_done(wr, ret);
			return ret;
		}
		data = calloc(2 * len, sizeof(*data));
		i = 0;
		ucv_object_foreach(val, key, value) {
			data[i] = uht_writer_add_string(wr, key);
			data[len + i] = writer_store_data(wr, value);
			i++;
		}
		ret = uht_writer_add_object(wr, data, data + len, len);
		free(data);
		return ret;
	default:
		return 0;
	}
}

static uc_value_t *
writer_save(uc_vm_t *vm, size_t nargs)
{
	struct uht_writer wr = {};
	uc_value_t *file = uc_fn_arg(0);
	uc_value_t *data = uc_fn_arg(1);
	uint32_t val;
	int ret = -1;
	FILE *f;

	if (ucv_type(file) != UC_STRING || !data)
		return NULL;

	f = fopen(ucv_string_get(file), "w");
	if (!f)
		return NULL;

	uht_writer_init(&wr);
	val = writer_store_data(&wr, data);
	if (val)
		ret = uht_writer_save(&wr, f, val);
	fflush(f);
	fclose(f);
	uht_writer_free(&wr);

	return ucv_boolean_new(ret == 0);
}

static uc_value_t *
__reader_get_value(uc_vm_t *vm, struct uht_reader *r, uint32_t attr, bool dump)
{
	enum uht_type type = uht_entry_type(attr);
	uc_value_t *val;
	size_t i;

	switch (type) {
	case UHT_NULL:
		return NULL;
	case UHT_STRING:
		return ucv_string_new(uht_reader_get_string(r, attr));
	case UHT_INT:
		return ucv_int64_new(uht_reader_get_int(r, attr));
	case UHT_DOUBLE:
		return ucv_double_new(uht_reader_get_double(r, attr));
	case UHT_BOOL:
		return ucv_boolean_new(uht_reader_get_bool(r, attr));
	case UHT_HASHTBL:
		if (!dump)
			return ucv_resource_new(hashtbl_type, (void *)(uintptr_t)attr);
		/* fallthrough */
	case UHT_OBJECT:
		val = ucv_object_new(vm);
		if (type == UHT_HASHTBL)
			ucv_object_add(val, hash_key, ucv_boolean_new(true));
		uht_for_each(r, iter, attr)
			ucv_object_add(val, iter.key, ucv_get(__reader_get_value(vm, r, iter.val, dump)));
		return val;
	case UHT_ARRAY:
		val = ucv_array_new(vm);
		i = 0;
		uht_for_each(r, iter, attr)
			ucv_array_set(val, i++, ucv_get(__reader_get_value(vm, r, iter.val, dump)));
		return val;
	}

	return NULL;
}

static uc_value_t *
reader_open(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *file = uc_fn_arg(0);
	struct uht_reader *r;

	if (ucv_type(file) != UC_STRING)
		return NULL;

	r = calloc(1, sizeof(*r));
	if (uht_reader_open(r, ucv_string_get(file))) {
		free(r);
		return NULL;
	}

	return ucv_resource_new(reader_type, r);
}

static void
reader_free(void *ptr)
{
	struct uht_reader *r = ptr;

	uht_reader_close(r);
	free(r);
}

static uc_value_t *
set_hashtbl_key(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *key = uc_fn_arg(0);

	if (ucv_type(key) != UC_STRING)
		return NULL;

	free(hash_key);
	hash_key = strdup(ucv_string_get(key));

	return ucv_boolean_new(true);
}

static uc_value_t *
mark_hashtbl(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *obj = uc_fn_arg(0);

	if (ucv_type(obj) != UC_OBJECT)
		return NULL;

	ucv_object_add(obj, hash_key, ucv_boolean_new(true));
	return ucv_boolean_new(true);
}

static uint32_t
reader_get_htable(uc_value_t *tbl)
{
	void *htbl;

	if (ucv_type(tbl) != UC_RESOURCE)
		return 0;

	htbl = ucv_resource_data(tbl, "uht.hashtbl");
	if (!htbl)
		return 0;

	return (uint32_t)(uintptr_t)htbl;
}

static uc_value_t *
reader_get(uc_vm_t *vm, size_t nargs)
{
	struct uht_reader *r = uc_fn_thisval("uht.reader");
	uc_value_t *tbl = uc_fn_arg(0);
	uc_value_t *key = uc_fn_arg(1);
	uc_value_t *dump = uc_fn_arg(2);
	uint32_t val;

	if (!r)
		return NULL;

	if (tbl)
		val = reader_get_htable(tbl);
	else
		val = r->val;

	if (key) {
		if (uht_entry_type(val) != UHT_HASHTBL)
			return 0;

		val = uht_reader_hashtbl_lookup(r, val, ucv_string_get(key));
	}

	return __reader_get_value(vm, r, val, ucv_is_truish(dump));
}

static const uc_function_list_t no_fns[] = {};

static const uc_function_list_t reader_fns[] = {
	{ "get", reader_get },
};

static const uc_function_list_t hashtbl_fns[] = {
	{ "save", writer_save },
	{ "open", reader_open },
	{ "mark_hashtable", mark_hashtbl },
	{ "set_hashtable_key", set_hashtbl_key },
};

void uc_module_init(uc_vm_t *vm, uc_value_t *scope)
{
	hash_key = strdup("##hash_table");
	reader_type = uc_type_declare(vm, "uht.reader", reader_fns, reader_free);
	hashtbl_type = uc_type_declare(vm, "uht.hashtbl", no_fns, NULL);
	uc_function_list_register(scope, hashtbl_fns);
}
