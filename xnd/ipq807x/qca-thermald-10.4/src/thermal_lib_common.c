/*========================================================================================

 thermal_lib_common.c

 GENERAL DESCRIPTION
 Common utility functions for thermald and thermal client lib.

 Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

==========================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "thermal_lib_common.h"

extern struct thermal_cdata *list_head;
static uint32_t client_cb_handle = 0;

#define CLIENT_HANDLE_COND(x)   (client_cb_handle & (1 << (x)))

/*===========================================================================
FUNCTION get_callback_node_from_list

Function to get callback node from thermal client list.

ARGUMENTS
	list - Node of list from which loop start to iterate
	name - client name

RETURN VALUE
	return valid thermal_cdata node if find a callback,
	otherwise return NULL.
===========================================================================*/
struct thermal_cdata *get_callback_node_from_list(struct thermal_cdata *list, char *name)
{
	for (; list != NULL; list = list->next) {
		if (0 != strncmp(list->client_name, name, CLIENT_NAME_MAX))
			continue;
		else
			return list;
	}
	return NULL;
}

/*===========================================================================
FUNCTION remove_from_list

Function to remove unregistered client and its data from thermal
client list based on client_cb_handle.

ARGUMENTS
	handle  - client_cb_handle,node of this to be removed

RETURN VALUE
	return 0 on success, -negative on failure.
===========================================================================*/
int remove_from_list(int handle)
{
	int ret = -EINVAL;
	struct thermal_cdata *client = NULL,*prev_client = NULL;

	if ( handle == 0 ||
	     handle >= CLIENT_HANDLE_MAX ||
	     0 == CLIENT_HANDLE_COND(handle))
		return ret;

	for (client = list_head; client != NULL; client = client->next) {
		if (client->client_cb_handle != handle) {
			prev_client = client;
			continue;
		}
		if (client == list_head) {
			list_head = list_head->next;
			break;
		} else {
			prev_client->next = client->next;
			break;
		}
	}

	client_cb_handle &= ~(1 << handle);

	if (client == NULL)
		return ret;
	free(client);
	ret = 0;
	return ret;
}

/*===========================================================================================
FUNCTION add_to_list

Function to add new client and its data to list while registering
with thermal.

ARGUMENTS
	name - client name.
	callback - callback function
	data - client specific data

RETURN VALUE
	return a unsigned non zero client handle on success, zero on failure.
=============================================================================================*/
int add_to_list(char *name,  void *callback, void *data)
{
	int ret = 0;
	int i;
	struct thermal_cdata *newclient = NULL;

	if (NULL == name ||
	     NULL == callback)
		return ret;

	newclient = (struct thermal_cdata *) malloc(sizeof(struct thermal_cdata));
	if (NULL == newclient) {
		return ret;
	}
	memset(newclient, 0, sizeof(struct thermal_cdata));

	for (i = 1; i < CLIENT_HANDLE_MAX; i++) {
		if (!CLIENT_HANDLE_COND(i))
			break;
	}
	if (i >= CLIENT_HANDLE_MAX)
		return ret;

	client_cb_handle |= (1<<i);

	newclient->client_cb_handle = i;
	newclient->client_name = name;
	newclient->callback = callback;
	newclient->user_data = data;
	newclient->data_reserved = NULL;
	newclient->next = NULL;

	if (list_head == NULL) {
		list_head = newclient;
	} else {
		newclient->next = list_head;
		list_head = newclient;
	}
	ret = newclient->client_cb_handle;
	return ret;
}
