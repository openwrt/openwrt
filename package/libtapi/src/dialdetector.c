#include <linux/input.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <stdbool.h>


#include <stdlib.h>
#include <stdio.h>

#include "events.h"
#include "timerfd.h"

#include "tapi-port.h"
#include "dialdetector.h"

static const struct itimerspec dialdetector_timeout = {
	.it_value.tv_sec = 3,
};

static void dialdetector_note_digit(struct dialdetector *d, unsigned char digit)
{
	printf("note digit: %d\n", d->num_digits);
	d->digits[d->num_digits] = digit;
	++d->num_digits;
}

static void dialdetector_reset(struct dialdetector *d)
{
	event_unregister(d->timer_fd);
	d->num_digits = 0;
	d->state = DIALDETECTOR_IDLE;
}

static bool dialdetector_timeout_event(int events, void *data)
{
	char num[20];
	struct dialdetector *dialdetector = data;
	int i;

	for (i = 0; i < dialdetector->num_digits; ++i) {
		num[i] = '0' + dialdetector->digits[i];
	}
	num[i] = '\0';

	printf("Dialing: %s\n", num);
	dialdetector->dial_callback(dialdetector->port, dialdetector->num_digits,
	dialdetector->digits);

	dialdetector_reset(dialdetector);

	return false;
}

static void dialdetector_port_event(struct tapi_port *port,
	struct tapi_event *event, void *data)
{
	struct dialdetector *d = data;

	printf("port event: %d %d\n", d->state, event->hook.on);

	switch (d->state) {
	case DIALDETECTOR_IDLE:
		if (event->type == TAPI_EVENT_TYPE_HOOK && event->hook.on == false) {
			d->state = DIALDETECTOR_WAIT_FOR_NUMBER;
			event_register(d->timer_fd, EPOLLIN, &d->timeout_cb);
			timerfd_settime(d->timer_fd, 0, &dialdetector_timeout, NULL);
		}
		break;
	case DIALDETECTOR_WAIT_FOR_NUMBER:
	case DIALDETECTOR_WAIT_FOR_NUMBER_TIMEOUT:
		switch (event->type) {
		case TAPI_EVENT_TYPE_HOOK:
			if (event->hook.on == true)
				dialdetector_reset(d);
			break;
		case TAPI_EVENT_TYPE_DTMF:
			if (d->state == DIALDETECTOR_WAIT_FOR_NUMBER)
				event_register(d->timer_fd, EPOLLIN, &d->timeout_cb);
			timerfd_settime(d->timer_fd, 0, &dialdetector_timeout, NULL);
			d->state = DIALDETECTOR_WAIT_FOR_NUMBER_TIMEOUT;
			dialdetector_note_digit(d, event->dtmf.code);
			break;
		default:
			break;
		}
	}
}

struct dialdetector *dialdetector_alloc(struct tapi_port *port)
{
	struct dialdetector *dialdetector;
	dialdetector = malloc(sizeof(*dialdetector));

	dialdetector->timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	dialdetector->port = port;
	dialdetector->num_digits = 0;
	dialdetector->state = DIALDETECTOR_IDLE;

	dialdetector->timeout_cb.callback = dialdetector_timeout_event;
	dialdetector->timeout_cb.data = dialdetector;

	dialdetector->port_listener.callback = dialdetector_port_event;
	dialdetector->port_listener.data = dialdetector;

	tapi_port_register_event(port, &dialdetector->port_listener);


	return dialdetector;
}
