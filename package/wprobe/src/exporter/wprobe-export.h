#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * struct wprobe_value: data structure for attribute values
 * see kernel api netlink attributes for more information
 */
struct wprobe_value {
	/* attribute value */
	union data {
		const char *STRING;
		uint8_t U8;
		uint16_t U16;
		uint32_t U32;
		uint64_t U64;
		int8_t S8;
		int16_t S16;
		int32_t S32;
		int64_t S64;
	} data;
	/* statistics */
	int64_t s, ss;
	double avg, stdev;
	unsigned int n;
	void *usedata;  /* Pointer to used data.something */
};

struct exporter_data {
	int id; /* ipfix id */
        int userid; /* focus or global */
	int size; /* size in byte*/
	struct wprobe_value val;
};
