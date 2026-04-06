#ifndef STA_TRACKER_H
#define STA_TRACKER_H

#include <stdint.h>
#include <libubus.h>

#define MAX_STATIONS   128
#define MAX_SSID_LEN   33
#define MAX_IFNAME_LEN 16

struct sta_entry {
	uint8_t mac[6];
	char ssid[MAX_SSID_LEN];
	char ifname[MAX_IFNAME_LEN];
	int32_t signal;
	uint64_t rx_bytes;
	uint64_t tx_bytes;
	bool active;
};

struct sta_tracker {
	struct sta_entry stations[MAX_STATIONS];
	int count;
	struct ubus_context *ubus;
};

struct sta_tracker *sta_tracker_init(struct ubus_context *ubus);
void sta_tracker_destroy(struct sta_tracker *st);

int sta_tracker_refresh(struct sta_tracker *st);

const struct sta_entry *sta_tracker_find_mac(const struct sta_tracker *st,
					     const uint8_t *mac);

#endif
