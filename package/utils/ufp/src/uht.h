#ifndef __UHT_H
#define __UHT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <libubox/avl.h>
#include <libubox/utils.h>

#define UHT_TYPE_MASK	0xf
#define UHT_TYPE_BITS	4
#define UHT_ALIGN_BITS	2

#define UHT_ALIGN_MASK ((1 << UHT_ALIGN_BITS) - 1)

enum uht_type {
	UHT_NULL, /* can also be used as value */
	UHT_STRING,
	UHT_INT,
	UHT_DOUBLE,
	UHT_BOOL,
	UHT_HASHTBL,
	UHT_OBJECT,
	UHT_ARRAY,
};

struct uht_writer {
	struct avl_tree data;
	void *buf;
	size_t buf_ofs;
	size_t buf_len;
};

uint32_t uht_writer_hashtbl_alloc(struct uht_writer *wr, size_t n_members);
void uht_writer_hashtbl_add_element(struct uht_writer *wr, uint32_t hashtbl,
				    const char *key, uint32_t val);
void uht_writer_hashtbl_done(struct uht_writer *wr, uint32_t hashtbl);

uint32_t uht_writer_add_array(struct uht_writer *wr, uint32_t *members, size_t n);
uint32_t uht_writer_add_object(struct uht_writer *wr, uint32_t *keys,
			       uint32_t *values, size_t n);
uint32_t uht_writer_add_string(struct uht_writer *wr, const char *val);
uint32_t uht_writer_add_double(struct uht_writer *wr, double val);

static inline uint32_t
uht_writer_add_bool(struct uht_writer *wr, bool val)
{
	return (((uint32_t)val) << UHT_TYPE_BITS) | UHT_BOOL;
}

uint32_t uht_writer_add_int(struct uht_writer *wr, int64_t val);

void uht_writer_init(struct uht_writer *wr);
int uht_writer_save(struct uht_writer *wr, FILE *out, uint32_t val);
void uht_writer_free(struct uht_writer *wr);

static inline uint32_t
uht_entry_offset(uint32_t entry)
{
	return ((entry & ~UHT_TYPE_MASK) >> (UHT_TYPE_BITS - UHT_ALIGN_BITS));
}

static inline void *
uht_entry_ptr(void *buf, uint32_t entry)
{
	return buf + uht_entry_offset(entry);
}

static inline enum uht_type
uht_entry_type(uint32_t attr)
{
	return attr & UHT_TYPE_MASK;
}

static inline bool
uht_entry_valid(size_t len, uint32_t attr)
{
	return uht_entry_offset(attr) + 4 <= len;
}

struct uht_reader {
	void *data;
	size_t len;
	uint32_t val;
	int fd;
};

struct uht_reader_iter {
	uint32_t *__data;
	uint8_t type;

	uint32_t index, size;
	const char *key;
	uint32_t val;
};

int uht_reader_open(struct uht_reader *r, const char *file);
void uht_reader_close(struct uht_reader *r);

static inline const char *
uht_reader_get_string(struct uht_reader *r, uint32_t attr)
{
	return uht_entry_ptr(r->data, attr);
}

static inline int64_t
uht_reader_get_int(struct uht_reader *r, uint32_t attr)
{
	return le64_to_cpu(*(int64_t *)uht_entry_ptr(r->data, attr));
}

static inline bool
uht_reader_get_bool(struct uht_reader *r, uint32_t attr)
{
	return !!(attr >> UHT_TYPE_BITS);
}

static inline double
uht_reader_get_double(struct uht_reader *r, uint32_t attr)
{
	union {
		double d;
		uint64_t u64;
	} v = {
		.u64 = le64_to_cpu(*(uint64_t *)uht_entry_ptr(r->data, attr))
	};
	return v.d;
}

void __uht_object_iter_next(struct uht_reader *r, struct uht_reader_iter *iter);
struct uht_reader_iter __uht_object_iter_init(struct uht_reader *r, uint32_t attr);
uint32_t uht_reader_hashtbl_lookup(struct uht_reader *r, uint32_t hashtbl,
				   const char *key);

#define uht_for_each(r, iter, attr)							\
	for (struct uht_reader_iter iter = __uht_object_iter_init(r, attr);		\
	     iter.index < iter.size; __uht_object_iter_next(r, &iter), iter.index++)

#endif
