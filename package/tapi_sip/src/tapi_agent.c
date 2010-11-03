
#include <stdbool.h>
#include <stdio.h>
#include <tapi-port.h>

#include "session.h"
#include "agent.h"
#include "tapi_agent.h"

static int tapi_agent_invite(struct agent *agent, struct session *session)
{
	struct tapi_agent *tagent = agent_to_tapi_agent(agent);

	if (tagent->session)
		return -1;

	tagent->state = TAPI_AGENT_STATE_RINGING;
	tapi_port_set_ring(&tagent->port, true);

	tagent->session = session;

	return 0;
}

static int tapi_agent_accept(struct agent *agent, struct session *session)
{
	return 0;
}

static int tapi_agent_hangup(struct agent *agent, struct session *session)
{
	struct tapi_agent *tagent = agent_to_tapi_agent(agent);

	switch (tagent->state) {
	case TAPI_AGENT_STATE_RINGING:
		tapi_port_set_ring(&tagent->port, false);
		break;
	default:
		break;
	}

	tagent->state = TAPI_AGENT_STATE_IDLE;
	tagent->session = NULL;

	return 0;
}

static int tapi_agent_get_endpoint(struct agent *agent, struct session *session)
{
	struct tapi_agent *tagent = agent_to_tapi_agent(agent);
	return tapi_port_get_endpoint(&tagent->port);
}

static const struct agent_ops tapi_agent_ops = {
	.invite = tapi_agent_invite,
	.accept = tapi_agent_accept,
	.hangup = tapi_agent_hangup,
	.get_endpoint = tapi_agent_get_endpoint,
};

static void tapi_agent_event(struct tapi_port *port, struct tapi_event *event,
	void *data)
{
	struct tapi_agent *tagent = data;

	if (event->type != TAPI_EVENT_TYPE_HOOK)
		return;

	if (!tagent->session)
		return;

	if (event->hook.on) {
		session_hangup(tagent->session, &tagent->agent);
		tagent->state = TAPI_AGENT_STATE_IDLE;
		tagent->session = NULL;
	} else {
		session_accept(tagent->session, &tagent->agent);
		tagent->state = TAPI_AGENT_STATE_ACTIVE;
	}
}

void tapi_agent_init(struct tapi_device *tdev, int port, struct tapi_agent *tagent)
{
	int ret;

	tagent->agent.ops = &tapi_agent_ops;
	tagent->state = TAPI_AGENT_STATE_IDLE;
	tagent->session = NULL;

	ret = tapi_port_open(tdev, port, &tagent->port);
	if (ret) {
		printf("Failed to open tapi port %d: %d\n", port, ret);
		return;
	}

	tagent->event_listener.callback = tapi_agent_event;
	tagent->event_listener.data = tagent;

	tapi_port_register_event(&tagent->port, &tagent->event_listener);
}

void tapi_agent_free(struct tapi_agent *tagent)
{
	tapi_port_unregister_event(&tagent->port, &tagent->event_listener);
}
