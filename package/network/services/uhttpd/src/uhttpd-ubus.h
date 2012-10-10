/*
 * uhttpd - Tiny single-threaded httpd - ubus header
 *
 *   Copyright (C) 2012 Jo-Philipp Wich <xm@subsignal.org>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _UHTTPD_UBUS_

#include <time.h>

#include <libubus.h>
#include <libubox/avl.h>
#include <libubox/blobmsg_json.h>
#include <json/json.h>


#define UH_UBUS_MAX_POST_SIZE	4096


struct uh_ubus_state {
	struct ubus_context *ctx;
	struct ubus_object ubus;
	struct blob_buf buf;
	struct avl_tree sessions;
	int timeout;
};

struct uh_ubus_request_data {
	const char *sid;
	const char *object;
	const char *function;
};

struct uh_ubus_session {
	char id[33];
	int timeout;
	struct avl_node avl;
	struct avl_tree data;
	struct avl_tree acls;
	struct timespec touched;
};

struct uh_ubus_session_data {
	struct avl_node avl;
	struct blob_attr attr[];
};

struct uh_ubus_session_acl {
	struct avl_node avl;
	char *function;
	char object[];
};

struct uh_ubus_state * uh_ubus_init(const struct config *conf);
bool uh_ubus_request(struct client *cl, struct uh_ubus_state *state);
void uh_ubus_close(struct uh_ubus_state *state);

#endif
