#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <linux/input.h>

#include "tapi-ioctl.h"
#include "tapi-device.h"
#include "tapi-port.h"

#include "events.h"
#include "list.h"

static void tapi_port_event_dispatch(struct tapi_port *port,
	struct tapi_event *event)
{
	struct tapi_port_event_listener *l;

	list_for_each_entry(l, &port->event_listeners, head) {
		l->callback(port, event, l->data);
	}
}

static bool tapi_port_input_event(int events, void *data)
{
	struct tapi_port *port = data;
	struct input_event event;
	struct tapi_event tapi_event;
	int ret;

	ret = read(port->input_fd, &event, sizeof(event));
	if (ret < 0) {
		fprintf(stderr, "Port %d failed to read from input device: %d\n",
			port->id, errno);
		return true;
	}

	if (!event.value)
		return true;

	switch (event.code) {
	case KEY_NUMERIC_0 ... KEY_NUMERIC_9:
		tapi_event.type = TAPI_EVENT_TYPE_DTMF;
		tapi_event.dtmf.code = event.code - KEY_NUMERIC_0;
		break;
	case KEY_NUMERIC_POUND:
		tapi_event.type = TAPI_EVENT_TYPE_DTMF;
		tapi_event.dtmf.code = 10;
		break;
	case KEY_NUMERIC_STAR:
		tapi_event.type = TAPI_EVENT_TYPE_DTMF;
		tapi_event.dtmf.code = 11;
		break;
	case KEY_ESC:
		tapi_event.type = TAPI_EVENT_TYPE_HOOK;
		tapi_event.hook.on = true;
		break;
	case KEY_ENTER:
		tapi_event.type = TAPI_EVENT_TYPE_HOOK;
		tapi_event.hook.on = false;
		break;
	default:
		return true;
	}

	if (tapi_event.type == TAPI_EVENT_TYPE_DTMF)
		tapi_event.dtmf.time = event.time;

	tapi_port_event_dispatch(port, &tapi_event);

	return true;
}

int tapi_port_open(struct tapi_device *dev, unsigned int id, struct tapi_port *port)
{
	int ret;
	char path[100];

	port->id = id;

	snprintf(path, 100, "/dev/tapi%uP%u", dev->id, id);
	port->fd = open(path, 0);
	if (port->fd < 0) {
		printf("Failed to open %s: %d\n", path, errno);
		return errno;
	}

	snprintf(path, 100, "/dev/event%u", id);
	port->input_fd = open(path, O_RDONLY);
	if (port->input_fd < 0) {
		printf("Failed to open %s: %d\n", path, errno);
		return errno;
	}

	port->ep = ioctl(port->fd, TAPI_PORT_IOCTL_GET_ENDPOINT, 0);

	INIT_LIST_HEAD(&port->event_listeners);

	port->input_cb.callback = tapi_port_input_event;
	port->input_cb.data = port;

	return event_register(port->input_fd, EPOLLIN,
		&port->input_cb);
}

int tapi_port_set_ring(struct tapi_port *port, bool ring)
{
	return ioctl(port->fd, TAPI_PORT_IOCTL_SET_RING, ring);
}

int tapi_port_register_event(struct tapi_port *port,
	struct tapi_port_event_listener *cb)
{
	list_add_tail(&cb->head, &port->event_listeners);
	return 0;
}

void tapi_port_unregister_event(struct tapi_port *port,
	struct tapi_port_event_listener *cb)
{
	list_del(&cb->head);
}
