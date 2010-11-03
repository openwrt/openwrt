#include <stdbool.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <errno.h>

#include <stdio.h>

#include "events.h"

struct event_callback *event_callbacks;

static int event_epoll_fd = -1;

int event_register(int fd, int events, struct event_callback *cb)
{
	struct epoll_event ev;

	if (event_epoll_fd == -1)
		event_epoll_fd = epoll_create(1);

	ev.events = events;
	ev.data.ptr = cb;

	cb->fd = fd;

	return epoll_ctl(event_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

int	event_unregister(int fd)
{
	return epoll_ctl(event_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

int tapi_mainloop(void)
{
	struct epoll_event ev[10];
	struct event_callback *cb;
	int ret;
	bool keep;
	int i;

	if (event_epoll_fd == -1)
		event_epoll_fd = epoll_create(1);

	while(true) {
		ret = epoll_wait(event_epoll_fd, ev, 10, -1);
		for(i = 0; i < ret; ++i) {
			cb = ev[i].data.ptr;
			keep = cb->callback(ev[i].events, cb->data);
			if (!keep)
				event_unregister(cb->fd);
		}
		if (ret < 0)
			printf("epoll: %d\n", errno);
	}

	return 0;
}
