#ifndef __TAPI_SESSION_H__
#define __TAPI_SESSION_H__

struct tapi_device;
struct tapi_port;
struct tapi_session;

struct tapi_session *tapi_session_alloc(struct tapi_device *dev,
	struct tapi_port *caller, struct tapi_port *callee,
	void (*release)(struct tapi_session *session, void *data), void	*release_data);

void tapi_session_free(struct tapi_session *session);

#endif
