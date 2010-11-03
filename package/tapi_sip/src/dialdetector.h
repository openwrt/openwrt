#include <linux/input.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <stdbool.h>


#include <stdlib.h>
#include <stdio.h>

#include "events.h"
#include "timerfd.h"

#include "tapi-port.h"

enum dialdetector_dial_state {
	DIALDETECTOR_DIAL_WAIT = 1,
	DIALDETECTOR_DIAL_WAIT_TIMEOUT = 2,
};

enum dialdetector_port_state {
	DIALDETECTOR_PORT_INACTIVE = 0,
	DIALDETECTOR_PORT_ACTIVE = 1,
	DIALDETECTOR_PORT_ACTIVE_DOWN = 2,
};

struct dialdetector {
	enum dialdetector_dial_state dial_state;
	enum dialdetector_port_state port_state;

	struct tapi_port *port;
	int timer_fd;
	int impulse_timer_fd;

	struct event_callback timeout_cb;
	struct event_callback impulse_cb;
	struct tapi_port_event_listener port_listener;

	size_t num_digits;
	unsigned char digits[20];

	unsigned int impulses;

	void (*dial_callback)(struct tapi_port *port, size_t num_digits, const unsigned char *digits);
};


struct tapi_port;

struct dialdetector *dialdetector_alloc(struct tapi_port *port);
