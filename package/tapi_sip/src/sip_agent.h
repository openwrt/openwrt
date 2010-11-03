#ifndef __SIP_AGENT_H__
#define __SIP_AGENT_H__

#include "agent.h"
#include <events.h>

struct sip_agent {
	struct sip_client *client;
	const char *identifier;

	struct tapi_stream *stream;
	struct session *session;

	struct pjsip_inv_session *inv;

	int rtp_sockfd;

	struct sockaddr_storage remote_addr;
	struct sockaddr_storage local_addr;

	struct agent agent;

	struct event_callback rtp_recv_callback;
	struct event_callback stream_recv_callback;
};

#endif
