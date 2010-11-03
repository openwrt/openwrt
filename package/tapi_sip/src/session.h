#ifndef __SESSION_H__
#define __SESSION_H__

struct agent;
struct session;
struct tapi_device;

struct session *session_alloc(struct tapi_device *, struct agent *caller,
	struct agent *callee, void (*release)(struct session *));
void session_hangup(struct session *, struct agent *);
void session_accept(struct session *, struct agent *);
void session_free(struct session *);

#endif
