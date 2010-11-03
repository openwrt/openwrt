#ifndef __LINUX_TAPI_TAPI_EVENT_H__
#define __LINUX_TAPI_TAPI_EVENT_H__


struct tapi_device;
struct tapi_port;

struct tapi_hook_event {
	bool on;
};

struct tapi_dtmf_event {
	unsigned char code;
};

enum tapi_event_type {
	TAPI_EVENT_TYPE_HOOK,
	TAPI_EVENT_TYPE_DTMF,
};

struct tapi_event {
	struct timeval time;
	enum tapi_event_type type;
	unsigned int port;
	union {
		struct tapi_hook_event hook;
		struct tapi_dtmf_event dtmf;
	};
};

void tapi_report_event(struct tapi_device *tdev, struct tapi_event *event);
void tapi_report_hook_event(struct tapi_device *tdev, struct tapi_port *port,
	bool on);
void tapi_report_dtmf_event(struct tapi_device *tdev, struct tapi_port *port,
	unsigned char code);

#endif
