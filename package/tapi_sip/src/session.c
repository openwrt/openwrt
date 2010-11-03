#include <stdlib.h>
#include <stdio.h>

#include "agent.h"
#include "session.h"

#include <tapi-device.h>


struct session
{
	struct agent *agents[2];

	struct tapi_device *tdev;

	void (*release)(struct session *);

	int link;
};

struct session *session_alloc(struct tapi_device *dev, struct agent *caller,
	struct agent *callee, void (*release)(struct session *))
{
	struct session *session;
	int ret;

	session = malloc(sizeof(*session));

	session->tdev = dev;

	session->agents[0] = caller;
	session->agents[1] = callee;

	ret = agent_invite(callee, session);
	if (ret < 0) {
		session_hangup(session, callee);
		free(session);
		return NULL;
	}

	session->release = release;

	return session;
}

void session_accept(struct session *session, struct agent *agent)
{
	int ep[2];

	printf("session_accept: %p %p\n", session, agent);
	printf("session agents: %p %p\n", session->agents[0], session->agents[1]);
	printf("session tdev: %p\n", session->tdev);

	agent_accept(session->agents[0], session);

	ep[0] = agent_get_endpoint(session->agents[0], session);
	ep[1] = agent_get_endpoint(session->agents[1], session);
	session->link = tapi_link_alloc(session->tdev, ep[0], ep[1]);

	printf("eps: %d %d\n", ep[0], ep[1]);

	tapi_link_enable(session->tdev, session->link);

	tapi_sync(session->tdev);
}

void session_hangup(struct session *session, struct agent *agent)
{
	struct agent *other_agent;

	if (session->agents[0] == agent)
		other_agent = session->agents[1];
	else
		other_agent = session->agents[0];

	agent_hangup(other_agent, session);

	tapi_link_disable(session->tdev, session->link);
	tapi_link_free(session->tdev, session->link);
	tapi_sync(session->tdev);

	if (session->release)
		session->release(session);
}
