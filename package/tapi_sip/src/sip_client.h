#ifndef __SIP_CLIENT_H__
#define __SIP_CLIENT_H__

#include <tapi-stream.h>
#include <tapi-device.h>

#include <pjsip.h>
#include <pjsip_ua.h>
#include <pjsip_simple.h>
#include <pjlib-util.h>
#include <pjlib.h>

#include <stdlib.h>
#include <sys/socket.h>
#include <stdint.h>

struct stun_client;
struct sip_agent;

struct sip_client_config {
	const char *iface;

	const char *host;
	uint16_t port;
	const char *username;
	const char *password;

	const char *stun_host;
	uint16_t stun_port;
};

struct sip_client {
	const struct sip_client_config *config;

	struct tapi_device *tdev;
	struct stun_client *stun;

	struct sockaddr_storage public_addr;
	struct sockaddr_storage local_addr;

	int sockfd;

	pj_thread_t *sip_thread;
    pj_caching_pool	 cp;
    pj_pool_t		*pool;
    pjsip_endpoint	*sip_endpt;
	pjsip_cred_info cred;
    pj_str_t		 local_contact;
	pj_str_t		server_uri;

	pjsip_regc *regc;

	int (*incoming_call_cb)(struct sip_client *client, struct sip_agent *agent);
};

void sip_client_init(struct sip_client *client, struct tapi_device *dev,
	const struct sip_client_config *config);

struct sip_agent *sip_client_alloc_agent(struct sip_client *client, const char *dst_uri);

#endif
