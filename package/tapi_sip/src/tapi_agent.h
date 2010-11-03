#ifndef __TAPI_AGENT_H__
#define __TAPI_AGENT_H__

#include "agent.h"
#include <tapi-port.h>

struct session;

enum tapi_agent_state {
	TAPI_AGENT_STATE_IDLE,
	TAPI_AGENT_STATE_RINGING,
	TAPI_AGENT_STATE_ACTIVE,
};

struct tapi_agent {
	struct agent agent;
	struct tapi_port port;
	struct tapi_port_event_listener event_listener;

	enum tapi_agent_state state;

	struct session *session;
};

static inline struct tapi_agent *agent_to_tapi_agent(struct agent *agent)
{
	return container_of(agent, struct tapi_agent, agent);
}

static inline struct tapi_agent *port_to_tapi_agent(struct tapi_port *port)
{
	return container_of(port, struct tapi_agent, port);
}

void tapi_agent_init(struct tapi_device *tdev, int port, struct tapi_agent *tagent);

#endif
