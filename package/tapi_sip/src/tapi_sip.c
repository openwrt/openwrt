
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <string.h>

#include <linux/input.h>
#include "dialdetector.h"

#include <tapi-ioctl.h>

#include <tapi-device.h>
#include <tapi-port.h>

#include "contact.h"
#include "session.h"
#include "sip_client.h"
#include "sip_agent.h"
#include "tapi_agent.h"

static struct tapi_device dev;
static struct tapi_agent *ports;

static struct sip_client sip_client;

static void release_session(struct session *session)
{
	free(session);
}

static void dial(struct tapi_agent *caller, struct agent *callee)
{
	struct session *session;

	session = session_alloc(&dev, &caller->agent, callee, release_session);

	if (!session)
		return;

	caller->session = session;
}

static void tel_dial(struct tapi_agent *caller, const char *number)
{
	int callee;

	callee = atoi(number) - 1;

	if (callee < 0 || callee > 1)
		return;
	dial(caller, &ports[callee].agent);
}

static void sip_dial(struct tapi_agent *caller, const char *identifier)
{
	struct sip_agent *callee;

	callee = sip_client_alloc_agent(&sip_client, identifier);
	if (!callee)
		return;

	dial(caller, &callee->agent);
}

static void dial_callback(struct tapi_port *port, size_t num_digits, const unsigned char *digits)
{
	struct tapi_agent *tagent = port_to_tapi_agent(port);
	char number[100];
	struct contact *contact;
	size_t i;

	if (tagent->state != TAPI_AGENT_STATE_IDLE)
		return;

	for (i = 0; i < num_digits; ++i) {
		if (digits[0] > 9)
			break;
		number[i] = digits[i] + '0';
	}
	number[i] = '\0';

	printf("dial callback: %s\n", number);

	contact = contact_get(number);

	if (!contact)
		return;

	if (strncmp("tel:", contact->identifier, 4) == 0) {
		tel_dial(tagent, contact->identifier + 4);
	} else if (strncmp("sip:", contact->identifier, 4) == 0) {
		sip_dial(tagent, contact->identifier);
	}
	tagent->state = TAPI_AGENT_STATE_ACTIVE;
}

static int incoming_sip_call(struct sip_client *client,
	struct sip_agent *caller)
{
	struct tapi_agent *callee = NULL;;
	struct session *session;
	int i;

	for (i = 0; i < 2; ++i) {
		if (ports[i].state == TAPI_AGENT_STATE_IDLE) {
			callee = &ports[i];
			break;
		}
	}

	if (callee == NULL)
		return -1;

	session = session_alloc(&dev, &caller->agent, &callee->agent,
		release_session);
	caller->session = session;

	return 0;
}

int main(int argc, char *argv[])
{
	struct dialdetector *dd, *dd2;
	struct account *account;
	struct sip_client_config config;
	const char *interface = "eth0";
	int ret;
	int i;

	if (argc > 1)
		interface = argv[1];

	pj_init();
    pjlib_util_init();

	contacts_init();

	account = get_account();
	if (!account) {
		printf("No account\n");
		return 1;
	}

	ret = tapi_device_open(0, &dev);
	if (ret) {
		printf("Failed to open tapi device: %d\n", ret);
		return 1;
	}

	ports = calloc(dev.num_ports, sizeof(*ports));
	for (i = 0; i < dev.num_ports; ++i)
		tapi_agent_init(&dev, i, &ports[i]);

	dd = dialdetector_alloc(&ports[0].port);
	dd->dial_callback = dial_callback;
	dd2 = dialdetector_alloc(&ports[1].port);
	dd2->dial_callback = dial_callback;

	config.iface = interface;
	config.host = account->realm;
	config.port = account->sip_port;
	config.username = account->username;
	config.password = account->password;

	config.stun_host = account->stun_host;
	config.stun_port = account->stun_port;

	sip_client_init(&sip_client, &dev, &config);

	sip_client.incoming_call_cb = incoming_sip_call;

	tapi_mainloop();

	return 0;
}
