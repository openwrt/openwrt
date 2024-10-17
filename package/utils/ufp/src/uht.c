#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <libubox/utils.h>

#include "xxhash32.h"
#include "uht.h"

#define ALIGN_OFS(ofs) ((ofs + UHT_ALIGN_MASK) & ~UHT_ALIGN_MASK)

#define UHT_HASHTBL_SIZE_SHIFT	5
#define UHT_HASHTBL_ORDER_MASK	((1 << UHT_HASHTBL_SIZE_SHIFT) - 1)

#define UHT_HASHTBL_KEY_FLAG_FIRST	1

struct uht_file_hdr {
	uint8_t version;
	uint8_t _pad[3];
	uint32_t val;
};

struct uht_key {
	uint32_t entry;
	uint32_t len;
};

struct uht_entry {
	struct avl_node node;
	struct uht_key key;
};

struct uht_hashtbl_meta {
	uint32_t *ht;
	uint32_t *ht_slot;
	uint32_t *ht_entry;
	uint32_t elements;
	uint8_t order;
};

static int
uht_key_comp(const void *k1, const void *k2, void *ptr)
{
	const struct uht_key *key1 = k1, *key2 = k2;
	struct uht_writer *wr = ptr;

	if (key1->len != key2->len)
		return key1->len - key2->len;

	return memcmp(uht_entry_ptr(wr->buf, key1->entry),
		      uht_entry_ptr(wr->buf, key2->entry), key1->len);
}

static uint32_t
uht_writer_check_insert(struct uht_writer *wr, struct uht_key *key)
{
	struct uht_entry *entry;

	entry = avl_find_element(&wr->data, key, entry, node);
	if (entry)
		return entry->key.entry;

	entry = calloc(1, sizeof(*entry));
	entry->node.key = &entry->key;
	entry->key = *key;
	avl_insert(&wr->data, &entry->node);
	wr->buf_ofs += key->len;

	return key->entry;
}

void uht_writer_init(struct uht_writer *wr)
{
	avl_init(&wr->data, uht_key_comp, false, wr);
	wr->buf_len = 256;
	wr->buf = calloc(1, wr->buf_len);
	wr->buf_ofs = sizeof(struct uht_file_hdr);
}

static void *
__uht_writer_alloc(struct uht_writer *wr, struct uht_key *key, size_t size)
{
	void *ret;

	if (size >= (1 << 24) || wr->buf_ofs + size >= (1 << 30))
		return NULL;

	size = ALIGN_OFS(size);
	while (wr->buf_ofs + size > wr->buf_len) {
		wr->buf_len <<= 1;
		wr->buf = realloc(wr->buf, wr->buf_len);
	}

	key->len = size;
	key->entry = wr->buf_ofs << (UHT_TYPE_BITS - UHT_ALIGN_BITS);
	ret = wr->buf + wr->buf_ofs;

	return ret;
}

static void *
uht_writer_alloc(struct uht_writer *wr, struct uht_key *key, size_t size)
{
	void *ret = __uht_writer_alloc(wr, key, size);
	wr->buf_ofs += size;
	return ret;
}

static uint32_t
uht_writer_add_generic(struct uht_writer *wr, const void *data, size_t len)
{
	struct uht_key key;

	memcpy(__uht_writer_alloc(wr, &key, len), data, len);
	return uht_writer_check_insert(wr, &key);
}

static int uht_hashtbl_get_meta(struct uht_hashtbl_meta *meta, void *buf, size_t len,
				uint32_t attr)
{
	uint32_t val;

	meta->ht = buf + uht_entry_offset(attr);
	val = cpu_to_le32(*meta->ht);
	meta->order = val & UHT_HASHTBL_ORDER_MASK;
	meta->elements = val >> UHT_HASHTBL_SIZE_SHIFT;
	meta->ht_slot = meta->ht + 1;
	meta->ht_entry = meta->ht_slot + (1 << meta->order);
	if ((void *)&meta->ht_entry[2 * meta->elements] > buf + len)
		return -1;

	return 0;
}

uint32_t uht_writer_hashtbl_alloc(struct uht_writer *wr, size_t n_members)
{
	struct uht_key key;
	uint32_t *ht, ht_size;
	uint8_t order = 2;

	if (n_members >= 1 << 24)
		return 0;

	while (n_members > 1U << order)
		order++;

	ht_size = 4 + (4 << order) + 8 * n_members;
	ht = uht_writer_alloc(wr, &key, ht_size);
	if (!ht)
		return 0;

	memset(ht, 0, ht_size);
	*ht = order;

	return key.entry | UHT_HASHTBL;
}


void uht_writer_hashtbl_add_element(struct uht_writer *wr, uint32_t hashtbl,
				    const char *key, uint32_t val)
{
	struct uht_hashtbl_meta meta = {};
	uint32_t key_attr = uht_writer_add_string(wr, key);
	uint32_t *ht_next;

	if (uht_hashtbl_get_meta(&meta, wr->buf, wr->buf_ofs, hashtbl))
		return;

	ht_next = &meta.ht_entry[2 * meta.elements];
	*meta.ht = cpu_to_le32(le32_to_cpu(*meta.ht) + (1 << UHT_HASHTBL_SIZE_SHIFT));
	ht_next[0] = cpu_to_le32(key_attr);
	ht_next[1] = cpu_to_le32(val);
}

static uint32_t
uht_hashtbl_key_slot(const char *key, uint8_t order)
{
	uint32_t mask = (1 << order) - 1;

	return XXH32(key, strlen(key), 0) & mask;
}


static uint32_t
uht_hashtbl_entry_key_slot(struct uht_writer *wr, uint32_t k, uint8_t order)
{
	return uht_hashtbl_key_slot(uht_entry_ptr(wr->buf, k), order);
}

struct uht_writer *__sort_wr;
static uint8_t __sort_order;
static int __entry_sort_fn(const void *a1, const void *a2)
{
	struct uht_writer *wr = __sort_wr;
	const uint32_t *k1 = a1, *k2 = a2;
	uint32_t slot1 = uht_hashtbl_entry_key_slot(wr, le32_to_cpu(*k1), __sort_order);
	uint32_t slot2 = uht_hashtbl_entry_key_slot(wr, le32_to_cpu(*k2), __sort_order);

	return slot1 - slot2;
}

void uht_writer_hashtbl_done(struct uht_writer *wr, uint32_t hashtbl)
{
	struct uht_hashtbl_meta meta = {};
	uint32_t last_slot = ~0;

	if (uht_hashtbl_get_meta(&meta, wr->buf, wr->buf_ofs, hashtbl))
		return;

	__sort_wr = wr;
	__sort_order = meta.order;
	qsort(meta.ht_entry, meta.elements, 8, __entry_sort_fn);
	for (size_t i = 0; i < meta.elements; i++) {
		uint32_t key_attr = cpu_to_le32(meta.ht_entry[2 * i]);
		uint32_t slot = uht_hashtbl_entry_key_slot(wr, key_attr, __sort_order);
		meta.ht_entry[2 * i] &= ~cpu_to_le32(UHT_TYPE_MASK);
		if (slot != last_slot)
			meta.ht_entry[2 * i] |= cpu_to_le32(UHT_HASHTBL_KEY_FLAG_FIRST);
		meta.ht_slot[slot] = cpu_to_le32(i);
		last_slot = slot;
	}
}

uint32_t uht_writer_add_array(struct uht_writer *wr, uint32_t *values, size_t n)
{
	struct uht_key key;
	uint32_t *data;

	data = __uht_writer_alloc(wr, &key, 4 + n * 4);
	*(data++) = cpu_to_le32(n);
	for (size_t i = 0; i < n; i++)
		*(data++) = cpu_to_le32(values[i]);

	return uht_writer_check_insert(wr, &key) | UHT_ARRAY;
}

uint32_t uht_writer_add_object(struct uht_writer *wr, uint32_t *keys,
			       uint32_t *values, size_t n)
{
	struct uht_key key;
	uint32_t *data;

	data = __uht_writer_alloc(wr, &key, 4 + n * 8);
	*(data++) = cpu_to_le32(n);
	for (size_t i = 0; i < n; i++) {
		*(data++) = cpu_to_le32(keys[i]);
		*(data++) = cpu_to_le32(values[i]);
	}
	return uht_writer_check_insert(wr, &key) | UHT_OBJECT;
}

uint32_t uht_writer_add_string(struct uht_writer *wr, const char *val)
{
	return uht_writer_add_generic(wr, val, strlen(val) + 1) | UHT_STRING;
}

uint32_t uht_writer_add_double(struct uht_writer *wr, double val)
{
	union {
		double d;
		uint64_t u64;
	} v = {
		.d = val
	};
	v.u64 = cpu_to_le64(v.u64);
	return uht_writer_add_generic(wr, &v.u64, 8) | UHT_DOUBLE;
}

uint32_t uht_writer_add_int(struct uht_writer *wr, int64_t val)
{
	val = cpu_to_le64(val);
	return uht_writer_add_generic(wr, &val, 8) | UHT_INT;
}

int uht_writer_save(struct uht_writer *wr, FILE *out, uint32_t val)
{
	struct uht_file_hdr *hdr = wr->buf;

	hdr->val = val;

	if (fwrite(wr->buf, 1, wr->buf_ofs, out) != wr->buf_ofs)
		return -1;

	return 0;
}

void uht_writer_free(struct uht_writer *wr)
{
	struct uht_entry *e, *tmp;

	avl_remove_all_elements(&wr->data, e, node, tmp)
		free(e);
	free(wr->buf);
	memset(wr, 0, sizeof(*wr));
}

static inline uint32_t
__uht_iter_fetch(struct uht_reader_iter *iter)
{
	return le32_to_cpu(*(iter->__data++));
}

struct uht_reader_iter
__uht_object_iter_init(struct uht_reader *r, uint32_t attr)
{
	struct uht_reader_iter iter = {
		.type = uht_entry_type(attr),
	};

	switch (iter.type) {
	case UHT_HASHTBL:
		iter.__data = uht_entry_ptr(r->data, attr);
		iter.size = __uht_iter_fetch(&iter);
		iter.__data += 1 << (iter.size & UHT_HASHTBL_ORDER_MASK);
		iter.size >>= UHT_HASHTBL_SIZE_SHIFT;
		break;
	case UHT_ARRAY:
	case UHT_OBJECT:
		iter.__data = uht_entry_ptr(r->data, attr);
		iter.size = __uht_iter_fetch(&iter);
		break;
	default:
		break;
	}

	if (iter.index < iter.size)
		__uht_object_iter_next(r, &iter);

	return iter;
}

void __uht_object_iter_next(struct uht_reader *r, struct uht_reader_iter *iter)
{
	if (iter->type != UHT_ARRAY) {
		uint32_t key = __uht_iter_fetch(iter);
		key &= ~UHT_TYPE_MASK;
		key |= UHT_STRING;
		iter->key = uht_reader_get_string(r, key);
	}
	iter->val = __uht_iter_fetch(iter);
}

uint32_t uht_reader_hashtbl_lookup(struct uht_reader *r, uint32_t hashtbl,
				   const char *key)
{
	uint32_t *ht, *ht_end, val, slot, size, offset;
	size_t key_len = strlen(key);
	int32_t entry;
	uint8_t order;

	if (!uht_entry_valid(r->len, hashtbl))
		return 0;

	ht = uht_entry_ptr(r->data, hashtbl);
	val = le32_to_cpu(*ht);
	order = val & UHT_HASHTBL_ORDER_MASK;
	size = val >> UHT_HASHTBL_SIZE_SHIFT;
	offset = (1 << order) + 2 * size;
	ht_end = ht + offset;
	offset <<= 2 + UHT_TYPE_BITS - UHT_ALIGN_BITS;

	if (!uht_entry_valid(r->len, hashtbl + offset))
		return 0;

	ht++;
	slot = uht_hashtbl_key_slot(key, order);
	if (ht + slot >= ht_end)
		return 0;

	entry = le32_to_cpu(ht[slot]) * 2;
	if ((uint32_t)entry > 2 * size)
		return 0;

	ht += 1 << order;
	while (entry >= 0) {
		uint32_t cur_entry = le32_to_cpu(ht[entry]);
		const char *cur_key;
		size_t off;

		cur_entry &= ~UHT_TYPE_MASK;
		cur_entry |= UHT_STRING;
		if (!uht_entry_valid(r->len, cur_entry))
			return 0;

		cur_key = uht_reader_get_string(r, cur_entry);
		off = cur_key - (const char *)r->data;
		if (off + key_len >= r->len)
			return 0;

		if (!strncmp(key, cur_key, key_len + 1)) {
			cur_entry = le32_to_cpu(ht[entry + 1]);
			if (!uht_entry_valid(r->len, cur_entry))
				return 0;

			return cur_entry;
		}

		if (ht[entry] & cpu_to_le32(UHT_HASHTBL_KEY_FLAG_FIRST))
			return 0;

		entry -= 2;
	}

	return 0;
}

int uht_reader_open(struct uht_reader *r, const char *file)
{
	const struct uht_file_hdr *hdr;
	struct stat st;
	int fd;

	fd = open(file, O_RDONLY);
	if (fd < 0)
		return -1;

	if (fstat(fd, &st) < 0)
		goto close_fd;

	if (st.st_size < (off_t)sizeof(struct uht_file_hdr))
		goto close_fd;

	r->fd = fd;
	r->data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	r->len = st.st_size;
	hdr = r->data;
	r->val = hdr->val;

	return 0;

close_fd:
	close(fd);
	return -1;
}

void uht_reader_close(struct uht_reader *r)
{
	munmap(r->data, r->len);
	close(r->fd);
}
