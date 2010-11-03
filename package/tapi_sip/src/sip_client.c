#include "sip_client.h"

#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "stun.h"
#include "sip_agent.h"
#include "session.h"
#include "list.h"

static inline struct sip_agent *agent_to_sip_agent(struct agent *agent)
{
	return container_of(agent, struct sip_agent, agent);
}

static int iface_get_addr(const char *iface, struct sockaddr_storage *addr)
{
	int fd;
	int ret;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);
	ret = ioctl(fd, SIOCGIFADDR, &ifr);
	if (ret < 0)
		perror("Failed to get interface address");

	close(fd);

	if (ret == 0)
		memcpy(addr, &ifr.ifr_addr, sizeof(ifr.ifr_addr));

	return ret;
}

#if 0
static bool sockaddr_is_local(struct sockaddr_storage *addr)
{
	unsigned long s_addr;
	bool is_local = false;

	switch (addr->ss_family) {
	case AF_INET:
		s_addr = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
		if ((s_addr & 0xff000000) == 0x10000000)
			is_local = true;
		else if ((s_addr & 0xfff00000) == 0xac100000)
			is_local = true;
		else if ((s_addr & 0xffff0000) == 0xc0a80000)
			is_local = true;
		break;
	default:
		break;
	}

	return is_local;
}
#endif

static uint16_t sockaddr_get_port(struct sockaddr_storage *addr)
{
	uint16_t port;

	switch (addr->ss_family) {
	case AF_INET:
		port = ((struct sockaddr_in *)addr)->sin_port;
		break;
	case AF_INET6:
		port = ((struct sockaddr_in6 *)addr)->sin6_port;
		break;
	default:
		port = 0;
		break;
	}

	return port;
}

static void sockaddr_set_port(struct sockaddr_storage *addr, uint16_t port)
{
	switch (addr->ss_family) {
	case AF_INET:
		((struct sockaddr_in *)addr)->sin_port = port;
		break;
	case AF_INET6:
		((struct sockaddr_in6 *)addr)->sin6_port = port;
		break;
	default:
		break;
	}
}

static void *sockaddr_get_addr(struct sockaddr_storage *addr)
{
	void *a;
	switch (addr->ss_family) {
	case AF_INET:
		a = &((struct sockaddr_in *)addr)->sin_addr.s_addr;
		break;
	case AF_INET6:
		a = ((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr;
		break;
	default:
		a = NULL;
		break;
	}

	return a;
}

static int sockaddr_to_string(struct sockaddr_storage *addr, char *s, size_t n)
{
	return inet_ntop(addr->ss_family, sockaddr_get_addr(addr), s, n) == NULL ? -1 : 0;
}

static pjsip_module mod_siprtp;
static struct sip_client *global_client;

/* Creates a datagram socket and binds it to a port in the range of
 * start_port-end_port */
static int sip_client_create_socket(struct sip_client *client,
	struct sockaddr_storage *sockaddr, uint16_t start_port,
	uint16_t end_port)
{
	int sockfd;
	int ret;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		return sockfd;

	memcpy(sockaddr, &client->local_addr, sizeof(client->local_addr));
	do {
		sockaddr_set_port(sockaddr, start_port);
		ret = bind(sockfd, (struct sockaddr *)sockaddr, sizeof(*sockaddr));
		++start_port;
	} while (ret == -1 && start_port < end_port);

	if (ret == -1)
		return -1;

	return sockfd;
}

static int sip_worker_thread(void *arg)
{
	struct sip_client *client = arg;

    while (1) {
		pj_time_val timeout = {0, 10};
		pjsip_endpt_handle_events(client->sip_endpt, &timeout);
    }

    return 0;
}

static bool sip_agent_stream_recv_callback(int events, void *data)
{
	struct sip_agent *agent = data;
	char buf[512];
	int len;
	int ret;

	len = read(agent->stream->fd, buf, 512);
	if (len < 0)
		return true;

	ret = sendto(agent->rtp_sockfd, buf, len, 0,
		(struct sockaddr *)&agent->remote_addr, sizeof(agent->remote_addr));

	if (ret < 0)
		printf("failed to send rtp data: %d\n", errno);

	return true;
}


static bool sip_agent_rtp_recv_callback(int events, void *data)
{
	struct sip_agent *agent = data;
	char buf[512];
	int len;

	len = recvfrom(agent->rtp_sockfd, buf, 512, 0, NULL, NULL);
	if (agent->stream)
		write(agent->stream->fd, buf, len);

	return true;
}

static pj_status_t sip_client_create_sdp(struct sip_client *client, pj_pool_t *pool,
			       struct sip_agent *agent,
			       pjmedia_sdp_session **p_sdp)
{
    pj_time_val tv;
    pjmedia_sdp_session *sdp;
    pjmedia_sdp_media *m;
    pjmedia_sdp_attr *attr;
	struct sockaddr_storage rtp_addr;
	char addr[INET6_ADDRSTRLEN];

    PJ_ASSERT_RETURN(pool && p_sdp, PJ_EINVAL);

	agent->rtp_sockfd = sip_client_create_socket(client, &rtp_addr, 4000, 5000);
	if (client->stun)
		stun_client_resolve(client->stun, agent->rtp_sockfd,
			(struct sockaddr *)&rtp_addr);

	agent->rtp_recv_callback.callback = sip_agent_rtp_recv_callback;
	agent->rtp_recv_callback.data = agent;
	event_register(agent->rtp_sockfd, EPOLLIN, &agent->rtp_recv_callback);

    /* Create and initialize basic SDP session */
    sdp = pj_pool_zalloc (pool, sizeof(pjmedia_sdp_session));

    pj_gettimeofday(&tv);
    sdp->origin.user = pj_str("pjsip-siprtp");
    sdp->origin.version = sdp->origin.id = tv.sec + 2208988800UL;
    sdp->origin.net_type = pj_str("IN");
    sdp->origin.addr_type = pj_str("IP4");
    sdp->origin.addr = *pj_gethostname();
    sdp->name = pj_str("pjsip");

    /* Since we only support one media stream at present, put the
     * SDP connection line in the session level.
     */
    sdp->conn = pj_pool_zalloc(pool, sizeof(pjmedia_sdp_conn));
    sdp->conn->net_type = pj_str("IN");
    sdp->conn->addr_type = pj_str("IP4");
	sockaddr_to_string(&rtp_addr, addr, sizeof(addr));
	pj_strdup2_with_null(pool, &sdp->conn->addr, addr);

    /* SDP time and attributes. */
    sdp->time.start = sdp->time.stop = 0;
    sdp->attr_count = 0;

    /* Create media stream 0: */

    sdp->media_count = 1;
    m = pj_pool_zalloc(pool, sizeof(pjmedia_sdp_media));
    sdp->media[0] = m;

    /* Standard media info: */
    m->desc.media = pj_str("audio");
    m->desc.port = sockaddr_get_port(&rtp_addr);
    m->desc.port_count = 1;
    m->desc.transport = pj_str("RTP/AVP");

    /* Add format and rtpmap for each codec. */
    m->desc.fmt_count = 1;
    m->attr_count = 0;

    {
	pjmedia_sdp_rtpmap rtpmap;
	char ptstr[10];

	sprintf(ptstr, "%d", 0);
	pj_strdup2_with_null(pool, &m->desc.fmt[0], ptstr);
	rtpmap.pt = m->desc.fmt[0];
	rtpmap.clock_rate = 64000;
	rtpmap.enc_name = pj_str("PCMU");
	rtpmap.param.slen = 0;

    }

    /* Add sendrecv attribute. */
    attr = pj_pool_zalloc(pool, sizeof(pjmedia_sdp_attr));
    attr->name = pj_str("sendrecv");
    m->attr[m->attr_count++] = attr;

    /* Done */
    *p_sdp = sdp;

    return PJ_SUCCESS;
}

static int sip_agent_invite(struct agent *agent, struct session *session)
{
	struct sip_agent *sip_agent = agent_to_sip_agent(agent);
	struct sip_client *client = sip_agent->client;
    pjsip_dialog *dlg;
    pjmedia_sdp_session *sdp;
    pjsip_tx_data *tdata;
    pj_status_t status;
	pj_str_t dst_uri;

	sip_agent->session = session;

	dst_uri = pj_str((char *)sip_agent->identifier);

    /* Create UAC dialog */
    status = pjsip_dlg_create_uac(pjsip_ua_instance(),
				   &client->local_contact,	/* local URI	    */
				   &client->local_contact,	/* local Contact    */
				   &dst_uri,		/* remote URI    */
				   &dst_uri,		/* remote target	    */
				   &dlg);		/* dialog	    */
    if (status != PJ_SUCCESS) {
		fprintf(stderr, "Failed to create uac dialog\n");
		return -1;
	}

	pjsip_auth_clt_set_credentials(&dlg->auth_sess, 1, &client->cred);

    /* Create SDP */
    sip_client_create_sdp(client, dlg->pool, sip_agent, &sdp);

    /* Create the INVITE session. */
    status = pjsip_inv_create_uac(dlg, sdp, 0, &sip_agent->inv);
    if (status != PJ_SUCCESS) {
		fprintf(stderr, "Failed to create invite session\n");
		pjsip_dlg_terminate(dlg);
		return -1;
    }


    /* Attach call data to invite session */
    sip_agent->inv->mod_data[mod_siprtp.id] = sip_agent;

    /* Create initial INVITE request.
     * This INVITE request will contain a perfectly good request and
     * an SDP body as well.
     */
    status = pjsip_inv_invite(sip_agent->inv, &tdata);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, -1); /*TODO*/

    /* Send initial INVITE request.
     * From now on, the invite session's state will be reported to us
     * via the invite session callbacks.
     */
    status = pjsip_inv_send_msg(sip_agent->inv, tdata);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, -1); /*TODO*/

    return 0;
}

static int sip_agent_hangup(struct agent *agent, struct session *session)
{
	struct sip_agent *sip_agent = agent_to_sip_agent(agent);
    pjsip_tx_data *tdata;
    pj_status_t status;

	printf("hangup %p\n", sip_agent->inv);

    status = pjsip_inv_end_session(sip_agent->inv, 603, NULL, &tdata);
    if (status == PJ_SUCCESS && tdata != NULL)
		pjsip_inv_send_msg(sip_agent->inv, tdata);

	if (sip_agent->rtp_sockfd) {
		event_unregister(sip_agent->rtp_sockfd);
		close(sip_agent->rtp_sockfd);
	}
	if (sip_agent->stream) {
		event_unregister(sip_agent->stream->fd);
		tapi_stream_free(sip_agent->stream);
	}

	sip_agent->inv->mod_data[mod_siprtp.id] = NULL;
	free(sip_agent);

	return 0;
}

static int sip_agent_alloc_stream(struct sip_agent *agent)
{
	int flags;

	if (agent->stream)
		printf("BUG!!!! %s:%s[%d]\n", __FILE__, __func__, __LINE__);

	agent->stream = tapi_stream_alloc(agent->client->tdev);
	agent->stream_recv_callback.callback = sip_agent_stream_recv_callback;
	agent->stream_recv_callback.data = agent;

	flags = fcntl(agent->stream->fd, F_GETFL, 0);
	fcntl(agent->stream->fd, F_SETFL, flags | O_NONBLOCK);

	event_register(agent->stream->fd, EPOLLIN, &agent->stream_recv_callback);

	return 0;
}

static void sip_agent_free_stream(struct sip_agent *agent)
{
	if (!agent->stream)
		return;

	event_unregister(agent->stream->fd);
	tapi_stream_free(agent->stream);
	agent->stream = NULL;
}

static int sip_agent_accept(struct agent *agent, struct session *session)
{
	struct sip_agent *sip_agent = agent_to_sip_agent(agent);
    pj_status_t status;
	pjsip_tx_data *tdata;

    /* Create 200 response .*/
    status = pjsip_inv_answer(sip_agent->inv, 200,
				      NULL, NULL, &tdata);
    if (status != PJ_SUCCESS) {
		status = pjsip_inv_answer(sip_agent->inv,
					  PJSIP_SC_NOT_ACCEPTABLE,
					  NULL, NULL, &tdata);
		if (status == PJ_SUCCESS)
			pjsip_inv_send_msg(sip_agent->inv, tdata);
		else
			pjsip_inv_terminate(sip_agent->inv, 500, PJ_FALSE);
		return -1;
    }

    /* Send the 200 response. */
    status = pjsip_inv_send_msg(sip_agent->inv, tdata);
    PJ_ASSERT_ON_FAIL(status == PJ_SUCCESS, return -1);

	sip_agent_alloc_stream(sip_agent);

	return 0;
}

static int sip_agent_get_endpoint(struct agent *agent, struct session *session)
{
	struct sip_agent *sip_agent = agent_to_sip_agent(agent);
	return tapi_stream_get_endpoint(sip_agent->stream);
}

static const struct agent_ops sip_agent_ops = {
	.invite = sip_agent_invite,
	.accept = sip_agent_accept,
	.hangup = sip_agent_hangup,
	.get_endpoint = sip_agent_get_endpoint,
};

struct sip_agent *sip_client_alloc_agent(struct sip_client *client,
	const char *identifier)
{
	struct sip_agent *agent;

	agent = malloc(sizeof(*agent));
	memset(agent, 0, sizeof(*agent));

	agent->agent.ops = &sip_agent_ops;
	agent->identifier = identifier;
	agent->client = client;

	return agent;
}

/*
 * Receive incoming call
 */
static void process_incoming_call(struct sip_client *client, pjsip_rx_data *rdata)
{
    unsigned options;
	struct sip_agent *agent;
    pjsip_tx_data *tdata;
    pj_status_t status;
    pjsip_dialog *dlg;
    pjmedia_sdp_session *sdp;

	agent = sip_client_alloc_agent(client, "extern");

    /* Verify that we can handle the request. */
    options = 0;
    status = pjsip_inv_verify_request(rdata, &options, NULL, NULL,
  				   client->sip_endpt, &tdata);
    if (status != PJ_SUCCESS) {
		/*
		 * No we can't handle the incoming INVITE request.
		 */
		if (tdata) {
			pjsip_response_addr res_addr;
			pjsip_get_response_addr(tdata->pool, rdata, &res_addr);
			pjsip_endpt_send_response(client->sip_endpt, &res_addr, tdata,
			NULL, NULL);
		} else {
			/* Respond with 500 (Internal Server Error) */
			pjsip_endpt_respond_stateless(client->sip_endpt, rdata, 500, NULL,
			NULL, NULL);
		}

		return;
    }

    /* Create UAS dialog */
    status = pjsip_dlg_create_uas(pjsip_ua_instance(), rdata,
				   &client->local_contact, &dlg);
    if (status != PJ_SUCCESS) {
		const pj_str_t reason = pj_str("Unable to create dialog");
		pjsip_endpt_respond_stateless(client->sip_endpt, rdata, 
				       500, &reason,
				       NULL, NULL);
		return;
    }

    /* Create SDP */
    sip_client_create_sdp(client, dlg->pool, agent, &sdp);

    /* Create UAS invite session */
    status = pjsip_inv_create_uas(dlg, rdata, sdp, 0, &agent->inv);
    if (status != PJ_SUCCESS) {
		pjsip_dlg_create_response(dlg, rdata, 500, NULL, &tdata);
		pjsip_dlg_send_response(dlg, pjsip_rdata_get_tsx(rdata), tdata);
		return;
    }

    /* Attach call data to invite session */
    agent->inv->mod_data[mod_siprtp.id] = agent;

    /* Create 180 response .*/
    status = pjsip_inv_initial_answer(agent->inv, rdata, 180,
				      NULL, NULL, &tdata);
    if (status != PJ_SUCCESS) {
		status = pjsip_inv_initial_answer(agent->inv, rdata,
					  PJSIP_SC_NOT_ACCEPTABLE,
					  NULL, NULL, &tdata);
		if (status == PJ_SUCCESS)
			pjsip_inv_send_msg(agent->inv, tdata);
		else
			pjsip_inv_terminate(agent->inv, 500, PJ_FALSE);
		return;
    }

    /* Send the 180 response. */
    status = pjsip_inv_send_msg(agent->inv, tdata);
    PJ_ASSERT_ON_FAIL(status == PJ_SUCCESS, return);

	if (client->incoming_call_cb)
		client->incoming_call_cb(client, agent);
}

/* Callback to be called to handle incoming requests outside dialogs: */
static pj_bool_t on_rx_request(pjsip_rx_data *rdata)
{
	struct sip_client *client = global_client;

    /* Ignore strandled ACKs (must not send respone */
    if (rdata->msg_info.msg->line.req.method.id == PJSIP_ACK_METHOD)
		return PJ_FALSE;

    /* Respond (statelessly) any non-INVITE requests with 500  */
    if (rdata->msg_info.msg->line.req.method.id != PJSIP_INVITE_METHOD) {
		pj_str_t reason = pj_str("Unsupported Operation");
		pjsip_endpt_respond_stateless(client->sip_endpt, rdata,
				       500, &reason,
				       NULL, NULL);
		return PJ_TRUE;
    }

    /* Handle incoming INVITE */
    process_incoming_call(client, rdata);

    /* Done */
    return PJ_TRUE;
}

static pjsip_module sip_client_mod = {
    NULL, NULL,			    /* prev, next.		*/
    { "mod-tapisip", 13 },	    /* Name.			*/
    -1,				    /* Id			*/
    PJSIP_MOD_PRIORITY_APPLICATION, /* Priority			*/
    NULL,			    /* load()			*/
    NULL,			    /* start()			*/
    NULL,			    /* stop()			*/
    NULL,			    /* unload()			*/
    &on_rx_request,		    /* on_rx_request()		*/
    NULL,			    /* on_rx_response()		*/
    NULL,			    /* on_tx_request.		*/
    NULL,			    /* on_tx_response()		*/
    NULL,			    /* on_tsx_state()		*/
};

/* Callback to be called when dialog has forked: */
static void call_on_forked(pjsip_inv_session *inv, pjsip_event *e)
{
    PJ_UNUSED_ARG(inv);
    PJ_UNUSED_ARG(e);
}

/* Callback to be called when invite session's state has changed: */
static void call_on_state_changed(pjsip_inv_session *inv, pjsip_event *e)
{
    struct sip_agent *agent = inv->mod_data[mod_siprtp.id];

	printf("state changed: %d\n", inv->state);

    if (!agent)
		return;

	switch (inv->state) {
    case PJSIP_INV_STATE_DISCONNECTED:
		printf("Disconnected\n");
		if (agent->session)
			session_hangup(agent->session, &agent->agent);
		if (agent->rtp_sockfd) {
			event_unregister(agent->rtp_sockfd);
			close(agent->rtp_sockfd);
		}
		sip_agent_free_stream(agent);
		free(agent);
		inv->mod_data[mod_siprtp.id] = NULL;
		break;
    case PJSIP_INV_STATE_CONFIRMED:
		printf("Connected: %p\n", agent->stream);
		if (agent->stream)
			break;
		sip_agent_alloc_stream(agent);
		session_accept(agent->session, &agent->agent);
		break;
	default:
		break;
    }
}

static void call_on_media_update(pjsip_inv_session *inv, pj_status_t status)
{
    struct sip_agent *agent;
    pj_pool_t *pool;
    const pjmedia_sdp_session *local_sdp, *remote_sdp;
	char local[100];
	char remote[100];
	int i;

	printf("media updte\n");

    agent = inv->mod_data[mod_siprtp.id];
    pool = inv->dlg->pool;

    /* Do nothing if media negotiation has failed */
    if (status != PJ_SUCCESS)
		return;
    /* Capture stream definition from the SDP */
    pjmedia_sdp_neg_get_active_local(inv->neg, &local_sdp);
    pjmedia_sdp_neg_get_active_remote(inv->neg, &remote_sdp);

	strlcpy(local, local_sdp->conn->addr.ptr, local_sdp->conn->addr.slen + 1);
	printf("local media count: %d\n", local_sdp->media_count);
	printf("local: %s %d\n", local,
	ntohs(local_sdp->media[0]->desc.port));
	strlcpy(remote, remote_sdp->conn->addr.ptr, remote_sdp->conn->addr.slen + 1);
	printf("remote media count: %d\n", remote_sdp->media_count);
	printf("remote: %s %d\n", remote,
	ntohs(remote_sdp->media[0]->desc.port));

	agent->remote_addr.ss_family = AF_INET;
	inet_pton(AF_INET, remote,
		sockaddr_get_addr(&agent->remote_addr));
	sockaddr_set_port(&agent->remote_addr, remote_sdp->media[0]->desc.port);

	printf("attributes: %d\n", remote_sdp->attr_count);
	for (i = 0; i < remote_sdp->attr_count; ++i)
		printf("%s: %s\n", remote_sdp->attr[i]->name.ptr,
		remote_sdp->attr[i]->value.ptr);

}

static int sip_client_init_sip_endpoint(struct sip_client *client)
{
    pj_status_t status;
	pjsip_host_port addrname;
	pjsip_inv_callback inv_cb;
	pjsip_transport *tp;
	char public_addr[INET6_ADDRSTRLEN];

	global_client = client;

    pj_caching_pool_init(&client->cp, &pj_pool_factory_default_policy, 0);
    client->pool = pj_pool_create(&client->cp.factory, "tapi sip", 1000, 1000, NULL);

    status = pjsip_endpt_create(&client->cp.factory, NULL,
				&client->sip_endpt);

	client->sockfd = sip_client_create_socket(client, &client->public_addr, 5060, 5100);
	if (client->stun)
		stun_client_resolve(client->stun, client->sockfd, (struct sockaddr *)&client->public_addr);

	sockaddr_to_string(&client->public_addr, public_addr, sizeof(public_addr));

	addrname.host = pj_str(public_addr);
	addrname.port = sockaddr_get_port(&client->public_addr);

	pjsip_udp_transport_attach(client->sip_endpt, client->sockfd, &addrname, 1, &tp);

    status = pjsip_tsx_layer_init_module(client->sip_endpt);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

    status = pjsip_ua_init_module(client->sip_endpt, NULL);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

    status = pjsip_100rel_init_module(client->sip_endpt);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

	pj_bzero(&inv_cb, sizeof(inv_cb));
	inv_cb.on_state_changed = &call_on_state_changed;
	inv_cb.on_new_session = &call_on_forked;
	inv_cb.on_media_update = &call_on_media_update;

	status = pjsip_inv_usage_init(client->sip_endpt, &inv_cb);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    status = pjsip_endpt_register_module(client->sip_endpt, &sip_client_mod);

	pj_thread_create(client->pool, "sip client", &sip_worker_thread, client,
			  0, 0, &client->sip_thread);

	return status;
}

static void sip_client_register_callback(struct pjsip_regc_cbparam *param)
{
	if (param->status != PJ_SUCCESS || param->code / 100 != 2)
		printf("Failed to register: %d %d", param->status, param->code);
}

void sip_client_free(struct sip_client *client)
{
	pjsip_regc_destroy(client->regc);
	pjsip_endpt_destroy(client->sip_endpt);
}

void sip_client_register(struct sip_client *client)
{
    pjsip_tx_data *tdata;

	pjsip_regc_register(client->regc, true, &tdata);
	pjsip_regc_send(client->regc, tdata);
}

void sip_client_set_cred(struct sip_client *client)
{
	char local_contact[100];
	char server_uri[100];
	char s[INET6_ADDRSTRLEN];
	char contact_addr[INET6_ADDRSTRLEN + 10];
	pj_str_t pj_contact_addr;

	sockaddr_to_string(&client->public_addr, s, sizeof(s));

	snprintf(contact_addr, sizeof(contact_addr), "sip:%s:%d", s,
		sockaddr_get_port(&client->public_addr));
	pj_contact_addr = pj_str(contact_addr);

	client->cred.realm = pj_str((char *)client->config->host);
	client->cred.scheme = pj_str("digest");
	client->cred.username = pj_str((char *)client->config->username);
	client->cred.data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	client->cred.data = pj_str((char *)client->config->password);

	snprintf(local_contact, sizeof(local_contact), "sip:%s@%s",
		client->config->username, client->config->host);
    pj_strdup2_with_null(client->pool, &client->local_contact, local_contact);

	snprintf(server_uri, sizeof(server_uri), "sip:%s\n", client->config->host);
    pj_strdup2_with_null(client->pool, &client->server_uri, server_uri);

	pjsip_regc_init(client->regc, &client->server_uri, &client->local_contact,
		&client->local_contact, 1, &pj_contact_addr, 3600);
	pjsip_regc_set_credentials(client->regc, 1, &client->cred);

	sip_client_register(client);
}

void sip_client_init(struct sip_client *client, struct tapi_device *tdev,
	const struct sip_client_config *config)
{
	global_client = client;
	client->config = config;

	client->tdev = tdev;

	iface_get_addr(config->iface, &client->local_addr);

	if (config->stun_host && config->stun_port)
		client->stun = stun_client_alloc(config->stun_host, config->stun_port);
	else
		client->stun = NULL;

	sip_client_init_sip_endpoint(client);

	pjsip_regc_create(client->sip_endpt, client, sip_client_register_callback,
		&client->regc);

	sip_client_set_cred(client);
}


