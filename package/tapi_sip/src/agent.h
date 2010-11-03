#ifndef __AGENT_H__
#define __AGENT_H__

#include "agent.h"

struct session;
struct agent;

struct agent_ops {
	int (*invite)(struct agent *, struct session *);
	int (*accept)(struct agent *, struct session *);
	int (*hangup)(struct agent *, struct session *);

	int (*get_endpoint)(struct agent *, struct session *);
};

struct agent {
	const struct agent_ops *ops;
};

static inline int agent_invite(struct agent *agent, struct session *session)
{
	return agent->ops->invite(agent, session);
}

static inline int agent_accept(struct agent *agent, struct session *session)
{
	return agent->ops->accept(agent, session);
}

static inline int agent_hangup(struct agent *agent, struct session *session)
{
	return agent->ops->hangup(agent, session);
}

static inline int agent_get_endpoint(struct agent *agent, struct session *session)
{
	return agent->ops->get_endpoint(agent, session);
}

#endif
