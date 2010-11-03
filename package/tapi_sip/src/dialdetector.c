#include <linux/input.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <stdbool.h>


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <events.h>
#include "timerfd.h"

#include "tapi-port.h"
#include "dialdetector.h"

static const struct itimerspec dialdetector_timeout = {
	.it_value.tv_sec = 3,
};

static const struct itimerspec dialdetector_impulse_timeout = {
	.it_value.tv_nsec = 200000000,
};

static void dialdetector_note_digit(struct dialdetector *d, unsigned char digit)
{
	printf("note digit: %d\n", d->num_digits);

	d->digits[d->num_digits] = digit;
	++d->num_digits;

	timerfd_settime(d->timer_fd, 0, &dialdetector_timeout, NULL);
	d->dial_state = DIALDETECTOR_DIAL_WAIT_TIMEOUT;
}

static void dialdetector_reset(struct dialdetector *d)
{
	d->num_digits = 0;
	d->impulses = 0;
	d->dial_state = DIALDETECTOR_DIAL_WAIT;
	d->port_state = DIALDETECTOR_PORT_INACTIVE;
}

static bool dialdetector_timeout_event(int events, void *data)
{
	char num[20];
	struct dialdetector *dialdetector = data;
	int i;
	uint64_t tmp;

	read(dialdetector->timer_fd, &tmp, sizeof(tmp));

	for (i = 0; i < dialdetector->num_digits; ++i) {
		num[i] = '0' + dialdetector->digits[i];
	}
	num[i] = '\0';

	dialdetector->dial_callback(dialdetector->port, dialdetector->num_digits,
	dialdetector->digits);

	dialdetector_reset(dialdetector);

	return true;
}

static bool dialdetector_impulse_timeout_cb(int events, void *data)
{
	struct dialdetector *d = data;
	uint64_t tmp;

	read(d->impulse_timer_fd, &tmp, sizeof(tmp));

	if (d->port_state == DIALDETECTOR_PORT_ACTIVE_DOWN) {
		d->port_state = DIALDETECTOR_PORT_INACTIVE;
	} else {
		printf("impulse: %d\n", d->impulses);
		if (d->impulses > 0)
			dialdetector_note_digit(d, d->impulses < 10 ? d->impulses : 0);
		d->impulses = 0;
	}

	return true;
}

static void dialdetector_port_event(struct tapi_port *port,
	struct tapi_event *event, void *data)
{
	struct dialdetector *d = data;

	printf("port event: %d %d\n", d->port_state, event->hook.on);

	switch (d->port_state) {
	case DIALDETECTOR_PORT_INACTIVE:
		if (event->type == TAPI_EVENT_TYPE_HOOK && event->hook.on == false)
			d->port_state = DIALDETECTOR_PORT_ACTIVE;
		break;
	case DIALDETECTOR_PORT_ACTIVE:
		switch (event->type) {
		case TAPI_EVENT_TYPE_HOOK:
			if (event->hook.on == true) {
				d->port_state = DIALDETECTOR_PORT_ACTIVE_DOWN;
				timerfd_settime(d->impulse_timer_fd, 0, &dialdetector_impulse_timeout, NULL);
			}
			break;
		case TAPI_EVENT_TYPE_DTMF:
			dialdetector_note_digit(d, event->dtmf.code);
			break;
		}
		break;
	case DIALDETECTOR_PORT_ACTIVE_DOWN:
		if (event->type == TAPI_EVENT_TYPE_HOOK && event->hook.on == false) {
			timerfd_settime(d->timer_fd, 0, &dialdetector_timeout, NULL);
			++d->impulses;
			d->port_state = DIALDETECTOR_PORT_ACTIVE;
		}
		break;
	}
}

struct dialdetector *dialdetector_alloc(struct tapi_port *port)
{
	struct dialdetector *dialdetector;
	dialdetector = malloc(sizeof(*dialdetector));

	dialdetector->timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	dialdetector->impulse_timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	dialdetector->port = port;
	dialdetector->num_digits = 0;
	dialdetector->impulses = 0;
	dialdetector->dial_state = DIALDETECTOR_DIAL_WAIT;
	dialdetector->port_state = DIALDETECTOR_PORT_INACTIVE;

	dialdetector->timeout_cb.callback = dialdetector_timeout_event;
	dialdetector->timeout_cb.data = dialdetector;
	dialdetector->impulse_cb.callback = dialdetector_impulse_timeout_cb;
	dialdetector->impulse_cb.data = dialdetector;

	dialdetector->port_listener.callback = dialdetector_port_event;
	dialdetector->port_listener.data = dialdetector;

	tapi_port_register_event(port, &dialdetector->port_listener);

	event_register(dialdetector->impulse_timer_fd, EPOLLIN,
		&dialdetector->impulse_cb);
	event_register(dialdetector->timer_fd, EPOLLIN, &dialdetector->timeout_cb);

	return dialdetector;
}
