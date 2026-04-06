#include "flow_table.h"
#include <stdlib.h>
#include <string.h>

static uint32_t flow_hash(const struct flow_key *key)
{
	const uint8_t *data = (const uint8_t *)key;
	uint32_t hash = 5381;
	size_t len = sizeof(*key);

	for (size_t i = 0; i < len; i++)
		hash = ((hash << 5) + hash) ^ data[i];

	return hash % FLOW_TABLE_SIZE;
}

struct flow_table *flow_table_create(uint32_t max_entries)
{
	struct flow_table *ft = calloc(1, sizeof(*ft));
	if (!ft)
		return NULL;

	ft->max_entries = max_entries;
	return ft;
}

void flow_table_destroy(struct flow_table *ft)
{
	if (!ft)
		return;

	for (int i = 0; i < FLOW_TABLE_SIZE; i++) {
		struct flow_entry *e = ft->buckets[i];
		while (e) {
			struct flow_entry *next = e->hash_next;
			free(e);
			e = next;
		}
	}
	free(ft);
}

struct flow_entry *flow_table_lookup(struct flow_table *ft,
				     const struct flow_key *key)
{
	uint32_t idx = flow_hash(key);
	struct flow_entry *e = ft->buckets[idx];

	while (e) {
		if (memcmp(&e->key, key, sizeof(*key)) == 0)
			return e;
		e = e->hash_next;
	}
	return NULL;
}

struct flow_entry *flow_table_insert(struct flow_table *ft,
				     const struct flow_key *key,
				     const uint8_t *src_mac)
{
	if (ft->count >= ft->max_entries)
		return NULL;

	struct flow_entry *e = calloc(1, sizeof(*e));
	if (!e)
		return NULL;

	memcpy(&e->key, key, sizeof(*key));
	if (src_mac)
		memcpy(e->src_mac, src_mac, 6);

	e->active = true;
	e->classification = CLASS_UNKNOWN;
	e->confidence = 0.0f;
	e->created = time(NULL);
	e->last_seen = e->created;

	uint32_t idx = flow_hash(key);
	e->hash_next = ft->buckets[idx];
	ft->buckets[idx] = e;
	ft->count++;

	return e;
}

void flow_table_expire(struct flow_table *ft, time_t now, int timeout_sec)
{
	for (int i = 0; i < FLOW_TABLE_SIZE; i++) {
		struct flow_entry **pp = &ft->buckets[i];
		while (*pp) {
			struct flow_entry *e = *pp;
			if ((now - e->last_seen) > timeout_sec) {
				*pp = e->hash_next;
				ft->count--;
				free(e);
			} else {
				pp = &e->hash_next;
			}
		}
	}
}

int flow_table_for_each(struct flow_table *ft,
			int (*cb)(struct flow_entry *entry, void *ctx),
			void *ctx)
{
	int count = 0;

	for (int i = 0; i < FLOW_TABLE_SIZE; i++) {
		struct flow_entry *e = ft->buckets[i];
		while (e) {
			struct flow_entry *next = e->hash_next;
			if (cb(e, ctx) < 0)
				return count;
			count++;
			e = next;
		}
	}
	return count;
}
