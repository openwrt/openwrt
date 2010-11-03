#ifndef __EVENTS_H__
#define __EVENTS_H__

struct event_callback {
	bool (*callback)(int events, void *data);
	void *data;
	int fd;
};

int event_register(int fd, int events,
	struct event_callback *callback);

int event_unregister(int fd);


int tapi_mainloop(void);

#endif
