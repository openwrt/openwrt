/*========================================================================================

 thermal_client.c

 GENERAL DESCRIPTION
 Thermal client library for external userspace apps interested in thermal mitigation.

 Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

==========================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/un.h>

#include "thermal_lib_common.h"
#include "thermal_client.h"

#ifdef ANDROID
#  include "cutils/properties.h"
#  ifdef USE_ANDROID_LOG
#    define LOG_TAG         "Thermal-Lib"
#    include "cutils/log.h"
#  endif
#endif
#include "common_log.h" /* define after cutils/log.h */

#ifdef USE_ANDROID_LOG
#define msg(format, ...)   LOGE(format, ## __VA_ARGS__)
#define info(format, ...)   LOGI(format, ## __VA_ARGS__)
#else
#define msg(format, ...)   printf(format, ## __VA_ARGS__)
#define info(format, ...)   printf(format, ## __VA_ARGS__)
#endif

/* Utility macros */
#define ARRAY_SIZE(x) (int)(sizeof(x)/sizeof(x[0]))

static int sockfd_client_send = -1;
static int sockfd_client_recv = -1;
static pthread_t thermal_client_recv_thread;
static int thermal_client_shutdown = 0;
static int first_client = 1;
static struct thermal_msg_data client_msg;
struct thermal_cdata *list_head = NULL;

/* Supported Thermal clients */
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

/*================================================================================================
FUNCTION do_request_to_thermal

Thermal client request sending thread.
This function will run in a separate thread. This thread is intented to
unblock parent client if connect() fails.

ARGUMENTS
	data - data pointer which points to message to be sent

RETURN VALUE
	void * - not used.
=================================================================================================*/
static void *do_request_to_thermal(void *data)
{
	int rc = 0;
	struct sockaddr_un client_addr_send;

	while(1) {
		sockfd_client_send = socket(AF_LOCAL, SOCK_STREAM, 0);
		if (sockfd_client_send < 0) {
			msg("Thermal-Lib-Client: %s: failed setup "
			    "client send sockfd", __func__);
			sleep(5);
			continue;
		}

		memset(&client_addr_send, 0, sizeof(struct sockaddr_un));
		snprintf(client_addr_send.sun_path, UNIX_PATH_MAX, THERMAL_RECV_CLIENT_SOCKET);
		client_addr_send.sun_family = AF_LOCAL;
		rc = connect(sockfd_client_send, (struct sockaddr *)&client_addr_send,
				     sizeof(sa_family_t) + strlen(THERMAL_RECV_CLIENT_SOCKET));
		if (rc != 0) {
			close(sockfd_client_send);
			sleep(5);
			continue;
		}

		rc = send(sockfd_client_send, &client_msg, sizeof(struct thermal_msg_data), 0);
		if (rc <= 0) {
			msg("Thermal-Lib-Client: "
			    "Unable to send request data to fd %d", sockfd_client_send);
			break;
		}
		info("Thermal-Lib-Client: Client request sent");
		break;
	}
	close(sockfd_client_send);
	return NULL;
}
/*================================================================================================
FUNCTION thermal_client_request

Thermal client request to thermal function.
The client which will send/notify request to thermal.

ARGUMENTS
	client_name - client name
	req_data    - requested data to be sent

RETURN VALUE
	0 on success, negative on failure.
=================================================================================================*/
int thermal_client_request(char *client_name, int req_data)
{
	int rc = 0;
	int ret = -EINVAL;
	int i;
	pthread_t thermal_client_request_thread;

	if (NULL == client_name) {
		msg("Thermal-Lib-Client:%s: unexpected NULL", __func__);
		return ret;
	}
	/* Check for client is supported  or not*/
	for (i = 0; i < ARRAY_SIZE(req_client_names); i++) {
		if (0 == strncmp(req_client_names[i], client_name, CLIENT_NAME_MAX))
			break;
	}

	if (i >= ARRAY_SIZE(req_client_names)) {
		msg("Thermal-Lib-Client:%s is not in supported "
		    "thermal client list", client_name);
		return ret;
	}

	memset(&client_msg, 0, sizeof(struct thermal_msg_data));
	strlcpy(client_msg.client_name, client_name, CLIENT_NAME_MAX);
	client_msg.req_data = req_data;

	rc = pthread_create(&thermal_client_request_thread, NULL, do_request_to_thermal, NULL);
	if (rc != 0) {
		msg("Thermal-Lib-Client: Unable to create pthread to "
		    "send client request from %s", client_name);
		return ret;
	}

	ret = 0;
	return ret;
}

/*============================================================================================
FUNCTION do_listen

Function to listen thermal socket.
This function will run in a separate thread.

ARGUMENTS
	data - data pointer.

RETURN VALUE
	void * - not used.
=============================================================================================*/
static void *do_listen(void *data)
{
	int rc;
	int i;
	int count;
	int (*callback)(int, void *, void *);
	struct thermal_cdata *callback_node;
	static struct sockaddr_un client_addr;
	struct thermal_msg_data thermal_msg;

	while (thermal_client_shutdown != 1) {

		sockfd_client_recv = socket(AF_LOCAL, SOCK_STREAM, 0);
		if (sockfd_client_recv < 0) {
			sleep(5);
			continue;
		}

		memset(&client_addr, 0, sizeof(struct sockaddr_un));
		snprintf(client_addr.sun_path, UNIX_PATH_MAX, THERMAL_SEND_CLIENT_SOCKET);
		client_addr.sun_family = AF_LOCAL;

		rc = connect(sockfd_client_recv, (struct sockaddr *)&client_addr,
				     sizeof(sa_family_t) + strlen(THERMAL_SEND_CLIENT_SOCKET));
		if (rc != 0) {
			close(sockfd_client_recv);
			sleep(5);
			continue;
		}
		while (thermal_client_shutdown != 1) {
			memset(&thermal_msg, 0, sizeof(struct thermal_msg_data));
			rc = recv(sockfd_client_recv, &thermal_msg, sizeof(struct thermal_msg_data), 0);
			if (rc <= 0) {
				msg("Thermal-Lib-Client:%s: recv failed", __func__);
				break;
			}

			if (rc != sizeof(struct thermal_msg_data))
				continue;

			for (i = 0; i < CLIENT_NAME_MAX; i++) {
				if (thermal_msg.client_name[i] == '\0')
					break;
			}
			if (i >= CLIENT_NAME_MAX)
				thermal_msg.client_name[CLIENT_NAME_MAX - 1] = '\0';

			info("Thermal-Lib-Client: Client received msg %s %d",
					thermal_msg.client_name, thermal_msg.req_data);

			/* Check for client is supported  or not*/
			for (i = 0; i < ARRAY_SIZE(notify_client_names); i++) {
				if (0 == strncmp(notify_client_names[i], thermal_msg.client_name, CLIENT_NAME_MAX))
					break;
			}

			if (i >= ARRAY_SIZE(notify_client_names)) {
				msg("Thermal-Lib-Client:%s is not in supported "
				    "thermal client list", thermal_msg.client_name);
				continue;
			} else if (thermal_msg.req_data < 0 || thermal_msg.req_data > LEVEL_MAX) {
				msg("Thermal-Lib-Client:%s: invalid level %d "
				    "unexpected", __func__, thermal_msg.req_data);
				continue;
			}

			callback_node = list_head;
			count = 0;
			for (; callback_node != NULL; callback_node = callback_node->next) {

				callback_node = get_callback_node_from_list(callback_node, thermal_msg.client_name);
				if (callback_node) {
					count++;
					callback = callback_node->callback;
					if (callback)
						callback(thermal_msg.req_data, callback_node->user_data,
									    callback_node->data_reserved);
				} else {
					if (count == 0)
						msg("Thermal-Lib-Client: No clients are "
						    "connected for %s", thermal_msg.client_name);
					break;
				}
			}
		}
		close(sockfd_client_recv);
	}
	return NULL;
}

/*================================================================================================
FUNCTION thermal_client_register_callback

Thermal client registration function.
The client is registered with name and a callback funcion and keep on connecting
thermal local socket.Based on client name in the message from socket,corresponding
callback will be called.

ARGUMENTS
	client_name - client name
	callback    - callback function pointer with level, user_data pointer and
	              reserved data as arguments
	data        - user data

RETURN VALUE
	valid non zero client_cb_handle on success, zero on failure.
=================================================================================================*/
int thermal_client_register_callback(char *client_name, int (*callback)(int, void *, void *), void *data)
{
	int rc = 0;
	int ret = 0;
	int i;
	int client_cb_handle;

	if (NULL == client_name ||
	    NULL == callback) {
		msg("Thermal-Lib-Client:%s: unexpected NULL client registraion "
		    "failed ", __func__);
		return ret;
	}

	/* Check for client is supported  or not*/
	for (i = 0; i < ARRAY_SIZE(notify_client_names); i++) {
		if (0 == strncmp(notify_client_names[i], client_name, CLIENT_NAME_MAX))
			break;
	}

	if (i >= ARRAY_SIZE(notify_client_names)) {
		msg("Thermal-Lib-Client:%s is not in supported thermal client list", client_name);
		return ret;
	}

	client_cb_handle = add_to_list(client_name, callback, data);
	if (client_cb_handle == 0) {
		msg("Thermal-Lib-Client: %s: Client Registration failed", __func__);
		return ret;
	}

	if (first_client == 1) {
		first_client = 0;
		rc = pthread_create(&thermal_client_recv_thread, NULL, do_listen, NULL);
		if (rc != 0) {
			msg("Thermal-Lib-Client: Unable to create pthread to "
			    "listen thermal events for %s", client_name);
			remove_from_list(client_cb_handle);
			return ret;
		}
	}

	info("Thermal-Lib-Client: Registraion successfully "
	      "finished for %s", client_name);
	ret = client_cb_handle;
	return ret;
}

/*===========================================================================
FUNCTION thermal_client_unregister_callback

Function to unregister client req_handler.

ARGUMENTS
	client_cb_handle - client handle which retured on
	                   succesful registeration

RETURN VALUE
	void  - return nothing.
===========================================================================*/
void thermal_client_unregister_callback(int client_cb_handle)
{
	if (remove_from_list(client_cb_handle) < 0)
		msg("Thermal-Lib-Client: thermal client unregister callback error");

	if (list_head == NULL) {
		if (thermal_client_shutdown != 1) {
			thermal_client_shutdown = 1;
			pthread_join(thermal_client_recv_thread, NULL);
			close(sockfd_client_recv);
			sockfd_client_recv = -1;
			first_client = 1;
		}
	}
}
