/*========================================================================================

 thermal_server.c

 GENERAL DESCRIPTION
 Thermal socket server related code to communicate with thermal DLL.

 Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

==========================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <sys/ioctl.h>
#include <private/android_filesystem_config.h>

#include "thermal.h"
#include "thermal_lib_common.h"
#include "thermal_server.h"

static int sockfd_server_send = -1;
static int sockfd_server_recv = -1;
static pthread_t listen_client_fd_thread;
static int server_socket_exit = 0;
static pthread_mutex_t soc_client_mutex;
static int thermal_send_fds[NUM_LISTEN_QUEUE];
struct thermal_cdata *list_head = NULL;

static char *req_client_names[] =
{
	"spkr",
	"override",
};
static char *notify_client_names[] =
{
	"camera",
	"camcorder",
	"spkr",
};

/*========================================================================================
FUNCTION thermal_recv_data_from_client

Recieve requested data from client through socket and based on data
invoke appropriate thermal callback from list which already registered.

ARGUMENTS
	client_fd -  client fd for thermal recv socket

RETURN VALUE
	0 on success, negative on failure.
========================================================================================*/
static int thermal_recv_data_from_client(int client_fd)
{
	int rc;
	int i;
	int ret = -EINVAL;
	int count = 0;
	int (*callback)(int, void *, void *);
	struct thermal_cdata *callback_node;
	struct thermal_msg_data client_msg;

	if (client_fd == -1)
		return ret;

	memset(&client_msg, 0, sizeof(struct thermal_msg_data));
	rc = recv(client_fd, &client_msg, sizeof(struct thermal_msg_data), 0);
	if (rc <= 0) {
		msg("Thermal-Server: %s: recv failed", __func__);
		return ret;
	}

	if (rc != sizeof(struct thermal_msg_data))
		return ret;

	for (i = 0; i < CLIENT_NAME_MAX; i++) {
		if (client_msg.client_name[i] == '\0')
			break;
	}
	if (i >= CLIENT_NAME_MAX)
		client_msg.client_name[CLIENT_NAME_MAX - 1] = '\0';

	info("Thermal-Server: Thermal received "
	     "msg %s %d", client_msg.client_name, client_msg.req_data);

	/* Check for client is supported  or not*/
	for (i = 0; i < ARRAY_SIZE(req_client_names); i++) {
		if (0 == strncmp(req_client_names[i], client_msg.client_name, CLIENT_NAME_MAX))
			break;
	}

	if (i >= ARRAY_SIZE(req_client_names)) {
		msg("Thermal-Server:%s is not in supported "
		    "thermal client list", client_msg.client_name);
		return ret;
	}

	for (callback_node = list_head; callback_node != NULL; callback_node = callback_node->next) {

		callback_node = get_callback_node_from_list(callback_node, client_msg.client_name);
		if (callback_node) {
			count++;
			callback = callback_node->callback;
			if (callback)
				callback(client_msg.req_data, callback_node->user_data, callback_node->data_reserved);
		} else {
			if (count == 0)
				msg("Thermal-Server: No clients are "
				    "connected for %s", client_msg.client_name);
			break;
		}
	}
	ret = 0;
	return ret;
}

/*========================================================================================
FUNCTION do_listen_client_fd

Seperate thread function for monitoring thermal usersapce clients and recieving
client data from thermal recv socket once it is notified.

ARGUMENTS
	data - data, not used

RETURN VALUE
	void
========================================================================================*/
static void *do_listen_client_fd(void *data)
{
	int fd;
	int i;
	int nread;
	int result;
	int client_len;
	int client_fd = -1;
	struct sockaddr_un client_addr;
	fd_set t_readfds,testfds;

	FD_ZERO(&t_readfds);
	FD_SET(sockfd_server_send, &t_readfds);
	FD_SET(sockfd_server_recv, &t_readfds);

	for (i = 0; i < NUM_LISTEN_QUEUE; i++) {
		thermal_send_fds[i] = -1;
	}

	while(server_socket_exit != 1) {
		testfds = t_readfds;
		result = select(FD_SETSIZE, &testfds, (fd_set *)0,
			       (fd_set *)0, (struct timeval *) 0);

		if (result < 1) {
			msg("Thermal-Server: %s select error", __func__);
			break;
		}

		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd,&testfds)) {
				if (fd == sockfd_server_send) {
					client_len = sizeof(struct sockaddr_un);
					client_fd = accept(sockfd_server_send,
						          (struct sockaddr *)&client_addr,
						           &client_len);
					if (client_fd < 0)
						continue;
					FD_SET(client_fd, &t_readfds);
					info("Thermal-Server: Adding thermal event listener on fd %d\n", client_fd);
					for (i = 0; i < NUM_LISTEN_QUEUE && thermal_send_fds[i] != -1; i++)
							continue;
						if (i < NUM_LISTEN_QUEUE)
							thermal_send_fds[i] = client_fd;
				} else if (fd == sockfd_server_recv) {
					client_len = sizeof(struct sockaddr_un);
					client_fd = accept(sockfd_server_recv,
						          (struct sockaddr *)&client_addr,
						           &client_len);
					if (client_fd < 0)
						continue;
					thermal_recv_data_from_client(client_fd);
					close(client_fd);
				} else {
					ioctl(fd, FIONREAD, &nread);
					if (nread == 0) {
						close(fd);
						FD_CLR(fd, &t_readfds);
						info("Thermal-Server: removing client on fd %d\n", fd);
						for (i = 0; i < NUM_LISTEN_QUEUE && thermal_send_fds[i] != fd; i++)
							continue;
						if (i < NUM_LISTEN_QUEUE) {
							thermal_send_fds[i] = -1;
						}
					}
				}
			}
		}
	}
	for (i = 0; i < NUM_LISTEN_QUEUE; i++) {
		if (thermal_send_fds[i] > -1)
			close(thermal_send_fds[i]);
	}
	return NULL;
}

/*========================================================================================
FUNCTION thermal_server_init

Thermal socket initialization function for different clients
This function setup a server for both thermal sending socket and
thermal recieving socket.

ARGUMENTS
	none

RETURN VALUE
	0 on success, negative on failure.
=========================================================================================*/
int thermal_server_init(void)
{
	int rc;
	int ret = -EINVAL;
	static struct sockaddr_un server_addr_send;
	static struct sockaddr_un server_addr_recv;

	if (sockfd_server_send != -1 || sockfd_server_recv != -1) {
		sockfd_server_send = -1;
		sockfd_server_recv = -1;
	}

	sockfd_server_send = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sockfd_server_send < 0) {
		goto error;
	}

	sockfd_server_recv = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sockfd_server_recv < 0) {
		goto error;
	}

	memset(&server_addr_send, 0, sizeof(struct sockaddr_un));
	snprintf(server_addr_send.sun_path, UNIX_PATH_MAX, THERMAL_SEND_CLIENT_SOCKET);
	server_addr_send.sun_family = AF_LOCAL;

	memset(&server_addr_recv, 0, sizeof(struct sockaddr_un));
	snprintf(server_addr_recv.sun_path, UNIX_PATH_MAX, THERMAL_RECV_CLIENT_SOCKET);
	server_addr_recv.sun_family = AF_LOCAL;

	/* Delete existing socket file if necessary */
	unlink(server_addr_send.sun_path);
	unlink(server_addr_recv.sun_path);

	rc = bind(sockfd_server_send,(struct sockaddr  const *)&server_addr_send, sizeof(struct sockaddr_un));
	if (rc != 0) {
		msg("Thermal-Server: Send socket: bind error - %s", strerror(errno));
		goto error;
	}
	rc = bind(sockfd_server_recv,(struct sockaddr  const *)&server_addr_recv, sizeof(struct sockaddr_un));
	if (rc != 0) {
		msg("Thermal-Server: Recv socket: bind error - %s", strerror(errno));
		goto error;
	}

	/* Add Camera and media server group permission  for sending
	   socket to socket and add media server group for recieving
	   socket untill having better security solution */
	chown(server_addr_send.sun_path, AID_ROOT, AID_CAMERA);
	chmod(server_addr_send.sun_path, 0660);
	chmod(server_addr_recv.sun_path, 0660);

	rc = listen(sockfd_server_send, NUM_LISTEN_QUEUE);
	if (rc != 0) {
		goto error;
	}

	rc = listen(sockfd_server_recv, NUM_LISTEN_QUEUE);
	if (rc != 0) {
		goto error;
	}

	rc = pthread_create(&listen_client_fd_thread, NULL, do_listen_client_fd, NULL);
	if (rc != 0) {
		msg("Thermal-Server: Unable to create pthread to "
		    "listen thermal clients fd");
		goto error;
	}

	ret = 0;
	return ret;
error:
	msg("Thermal-Server: Unable to create socket server for thermal clients");
	if (sockfd_server_send != -1) {
		close(sockfd_server_send);
		sockfd_server_send = -1;
	}
	if (sockfd_server_recv != -1) {
		close(sockfd_server_recv);
		sockfd_server_recv = -1;
	}
	return ret;
}

/*=======================================================================================
FUNCTION thermal_server_notify_clients

Function to send thermal event through socket to thermal clients.

ARGUMENTS
	client_name - client_name
	level       - mitigation level

RETURN VALUE
	return 0 on success and -EINVAL on failure.
========================================================================================*/
int thermal_server_notify_clients(char *client_name, int level)
{
	int rc;
	int i = 0;
	int ret = -EINVAL;
	int send_count = 0;
	struct thermal_msg_data thermal_msg;

	/* Check for client is supported  or not*/
	for (i = 0; i < ARRAY_SIZE(notify_client_names); i++) {
		if (0 == strncmp(notify_client_names[i], client_name, CLIENT_NAME_MAX))
			break;
	}

	if (i >= ARRAY_SIZE(notify_client_names)) {
		msg("Thermal-Server: %s is not in supported "
		    "thermal client list", client_name);
		return ret;
	}

	memset(&thermal_msg, 0, sizeof(struct thermal_msg_data));
	strlcpy(thermal_msg.client_name, client_name, CLIENT_NAME_MAX);
	thermal_msg.req_data = level;

	pthread_mutex_lock(&soc_client_mutex);

	if (sockfd_server_send == -1) {
		msg("Thermal-Server: socket is not created, Trying to start server..");
		thermal_server_release();
		if ( thermal_server_init() || sockfd_server_send == -1) {
			goto handle_error;
		}
	}

	/* Send event to all connected clients */
	for (i = 0; i < NUM_LISTEN_QUEUE; i++) {
		if(thermal_send_fds[i] != -1) {
			send_count++;
			rc = send(thermal_send_fds[i], &thermal_msg,
				      sizeof(struct thermal_msg_data), 0);
			if (rc <= 0) {
				msg("Thermal-Server: Unable to send "
				    "event to fd %d", thermal_send_fds[i]);
			}
		}
	}

	if (send_count == 0) {
		msg("Thermal-Server: No client connected to socket");
		goto handle_error;
	}
	ret = 0;

handle_error:
	pthread_mutex_unlock(&soc_client_mutex);
	return ret;
}

/*================================================================================================
FUNCTION thermal_server_register_client_req_handler

Thermal register function to register callback
Whenever thermal wants to register a new callback for clients,
invoke this API.

ARGUMENTS
	client_name - client name
	callback    - callback function pointer with req_data, user_data pointer and
	              reserved data as arguments
	data        - data

RETURN VALUE
	valid non zero client_cb_handle on success, zero on failure.
=================================================================================================*/
int thermal_server_register_client_req_handler(char *client_name, int (*callback)(int, void *, void *), void *data)
{
	int rc = 0;
	int i;
	int client_cb_handle;

	if (NULL == client_name ||
	    NULL == callback) {
		msg("Thermal-Server: %s: unexpected NULL client registraion failed ", __func__);
		return 0;
	}

	/* Check for client is supported  or not*/
	for (i = 0; i < ARRAY_SIZE(req_client_names); i++) {
		if (0 == strncmp(req_client_names[i], client_name, CLIENT_NAME_MAX))
			break;
	}

	if (i >= ARRAY_SIZE(req_client_names)) {
		msg("Thermal-Server: %s is not in supported thermal client list", client_name);
		return 0;
	}

	client_cb_handle = add_to_list(client_name, callback, data);
	if (client_cb_handle == 0) {
		msg("Thermal-Server: %s: Client Registration failed", __func__);
		return 0;
	}

	return client_cb_handle;
}

/*========================================================================================
FUNCTION  thermal_server_unregister_client_req_handler

Function to unregister client req_handler.

ARGUMENTS
	client_cb_handle - client handle which retured on succesful registeration

RETURN VALUE
	void
========================================================================================*/
void thermal_server_unregister_client_req_handler(int client_cb_handle)
{
	if (remove_from_list(client_cb_handle) < 0)
		msg("Thermal-Server: thermal client unregister callback error");
}
/*========================================================================================
FUNCTION  thermal_server_release

Function to release socket thread and fd

ARGUMENTS
	none

RETURN VALUE
	void
========================================================================================*/
void thermal_server_release(void)
{
	int i;

	server_socket_exit = 1;
	pthread_join(listen_client_fd_thread, NULL);
	close(sockfd_server_send);
	close(sockfd_server_recv);
	sockfd_server_send = -1;
	sockfd_server_recv = -1;

	for (i = 1; i < CLIENT_HANDLE_MAX && list_head != NULL; i++) {
		if (remove_from_list(i) < 0)
			msg("Thermal-Server: thermal client unregister callback error");
	}
}
