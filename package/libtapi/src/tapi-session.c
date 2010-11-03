#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "tapi-device.h"
#include "tapi-port.h"
#include "tapi-session.h"

enum tapi_session_port_state {
	TAPI_SESSION_PORT_STATE_IDLE,
	TAPI_SESSION_PORT_STATE_RINGING,
	TAPI_SESSION_PORT_STATE_ACTIVE,
};

struct tapi_session_port {
	struct tapi_port *port;
	struct tapi_port_event_listener event_listener;

	enum tapi_session_port_state state;
};

struct tapi_session {
	struct tapi_device *dev;
	struct tapi_session_port caller;
	struct tapi_session_port callee;

	bool active;
	unsigned int link;

	void (*release)(struct tapi_session *session, void *data);
};

static void tapi_session_terminate(struct tapi_session *session)
{
	if (session->active) {
		tapi_link_enable(session->dev, session->link);
		tapi_sync(session->dev);
		tapi_link_free(session->dev, session->link);
	}

	switch (session->callee.state) {
	case TAPI_SESSION_PORT_STATE_RINGING:
		tapi_port_set_ring(session->callee.port, false);
		break;
	default:
		break;
	}

	session->active = false;
}

static void tapi_session_caller_event(struct tapi_port *port,
	struct tapi_event *event, void *data)
{
	struct tapi_session *session = data;

	if (event->type != TAPI_EVENT_TYPE_HOOK)
		return;

	if (event->hook.on) {
		tapi_session_terminate(session);
	}
}

static void tapi_session_callee_event(struct tapi_port *port,
	struct tapi_event *event, void *data)
{
	struct tapi_session *session = data;

	if (event->type != TAPI_EVENT_TYPE_HOOK)
		return;

	if (event->hook.on) {
		if (session->callee.state == TAPI_SESSION_PORT_STATE_ACTIVE) {
			tapi_session_terminate(session);
		}
	} else  {
		if (session->callee.state == TAPI_SESSION_PORT_STATE_RINGING) {
			tapi_port_set_ring(session->callee.port, false);
			session->link = tapi_link_alloc(session->dev,
				session->caller.port->ep, session->callee.port->ep);
			session->callee.state = TAPI_SESSION_PORT_STATE_ACTIVE;
			tapi_link_enable(session->dev, session->link);
			tapi_sync(session->dev);
			session->active = true;
		}
	}
}

struct tapi_session *tapi_session_alloc(struct tapi_device *dev,
	struct tapi_port *caller, struct tapi_port *callee,
	void (*release)(struct tapi_session *session, void *data), void *release_data)
{
	struct tapi_session *session;
	struct tapi_session_port *session_port;

	session = malloc(sizeof(*session));

	session->dev = dev;

	session->callee.port = callee;
	session->callee.state = TAPI_SESSION_PORT_STATE_RINGING;
	session->callee.event_listener.callback = tapi_session_callee_event;
	session->callee.event_listener.data = session;
	tapi_port_register_event(callee, &session->callee.event_listener);

	session->caller.port = caller;
	session->caller.state = TAPI_SESSION_PORT_STATE_ACTIVE;
	session->caller.event_listener.callback = tapi_session_caller_event;
	session->caller.event_listener.data = session;
	tapi_port_register_event(caller, &session->caller.event_listener);

	tapi_port_set_ring(callee, true);
}

void tapi_session_free(struct tapi_session *session)
{
	tapi_session_terminate(session);
	tapi_port_register_event(session->callee.port, &session->callee.event_listener);
	tapi_port_register_event(session->caller.port, &session->caller.event_listener);
	free(session);
}
