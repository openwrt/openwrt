#ifndef __TAPI_PORT_H__
#define __TAPI_PORT_H__

#include <sys/time.h>

#include "list.h"
#include "events.h"

struct tapi_port;
struct tapi_device;

struct tapi_dtmf_event {
	struct timeval time;
	unsigned char code;
};

struct tapi_hook_event {
	bool on;
};

enum tapi_event_type {
	TAPI_EVENT_TYPE_DTMF,
	TAPI_EVENT_TYPE_HOOK,
};

struct tapi_event {
	enum tapi_event_type type;
	union {
		struct tapi_dtmf_event dtmf;
		struct tapi_hook_event hook;
	};
};

struct tapi_port_event_listener {
	void (*callback)(struct tapi_port *, struct tapi_event *event, void *data);
	void *data;

	struct list_head head;
};

struct tapi_port {
	int id;
	int fd;
	int input_fd;
	unsigned int ep;

	struct event_callback input_cb;

	struct list_head event_listeners;
};

int tapi_port_open(struct tapi_device *dev, unsigned int id, struct tapi_port
*port);
int tapi_port_set_ring(struct tapi_port *port, bool ring);
int tapi_port_register_event(struct tapi_port *port,
	struct tapi_port_event_listener *cb);

void tapi_port_unregister_event(struct tapi_port *port,
	struct tapi_port_event_listener *cb);

static inline int tapi_port_get_endpoint(struct tapi_port *port)
{
	return port->ep;
}

#endif
