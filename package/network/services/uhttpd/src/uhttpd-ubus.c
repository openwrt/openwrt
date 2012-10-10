/*
 * uhttpd - Tiny single-threaded httpd - ubus handler
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

#include "uhttpd.h"
#include "uhttpd-utils.h"
#include "uhttpd-ubus.h"


enum {
	UH_UBUS_SN_TIMEOUT,
	__UH_UBUS_SN_MAX,
};

static const struct blobmsg_policy new_policy[__UH_UBUS_SN_MAX] = {
	[UH_UBUS_SN_TIMEOUT] = { .name = "timeout", .type = BLOBMSG_TYPE_INT32 },
};


enum {
	UH_UBUS_SI_SID,
	__UH_UBUS_SI_MAX,
};

static const struct blobmsg_policy sid_policy[__UH_UBUS_SI_MAX] = {
	[UH_UBUS_SI_SID] = { .name = "sid", .type = BLOBMSG_TYPE_STRING },
};


enum {
	UH_UBUS_SS_SID,
	UH_UBUS_SS_VALUES,
	__UH_UBUS_SS_MAX,
};

static const struct blobmsg_policy set_policy[__UH_UBUS_SS_MAX] = {
	[UH_UBUS_SS_SID] = { .name = "sid", .type = BLOBMSG_TYPE_STRING },
	[UH_UBUS_SS_VALUES] = { .name = "values", .type = BLOBMSG_TYPE_TABLE },
};


enum {
	UH_UBUS_SG_SID,
	UH_UBUS_SG_KEYS,
	__UH_UBUS_SG_MAX,
};

static const struct blobmsg_policy get_policy[__UH_UBUS_SG_MAX] = {
	[UH_UBUS_SG_SID] = { .name = "sid", .type = BLOBMSG_TYPE_STRING },
	[UH_UBUS_SG_KEYS] = { .name = "keys", .type = BLOBMSG_TYPE_ARRAY },
};


enum {
	UH_UBUS_SA_SID,
	UH_UBUS_SA_OBJECTS,
	__UH_UBUS_SA_MAX,
};

static const struct blobmsg_policy acl_policy[__UH_UBUS_SA_MAX] = {
	[UH_UBUS_SA_SID] = { .name = "sid", .type = BLOBMSG_TYPE_STRING },
	[UH_UBUS_SA_OBJECTS] = { .name = "objects", .type = BLOBMSG_TYPE_ARRAY },
};


static bool
uh_ubus_strmatch(const char *str, const char *pat)
{
	while (*pat)
	{
		if (*pat == '?')
		{
			if (!*str)
				return false;

			str++;
			pat++;
		}
		else if (*pat == '*')
		{
			if (uh_ubus_strmatch(str, pat+1))
				return true;

			if (*str && uh_ubus_strmatch(str+1, pat))
				return true;

			return false;
		}
		else if (*str++ != *pat++)
		{
			return false;
		}
	}

	return (!*str && !*pat);
}

static int
uh_ubus_avlcmp(const void *k1, const void *k2, void *ptr)
{
	return strcmp((char *)k1, (char *)k2);
}

static void
uh_ubus_random(char *dest)
{
	int i;
	unsigned char buf[16] = { 0 };
	FILE *f;

	if ((f = fopen("/dev/urandom", "r")) != NULL)
	{
		fread(buf, 1, sizeof(buf), f);
		fclose(f);
	}

	for (i = 0; i < sizeof(buf); i++)
		sprintf(dest + (i<<1), "%02x", buf[i]);
}

static void
uh_ubus_session_dump_data(struct uh_ubus_session *ses, struct blob_buf *b)
{
	struct uh_ubus_session_data *d;

	avl_for_each_element(&ses->data, d, avl)
	{
		blobmsg_add_field(b, blobmsg_type(d->attr), blobmsg_name(d->attr),
						  blobmsg_data(d->attr), blobmsg_data_len(d->attr));
	}
}

static void
uh_ubus_session_dump_acls(struct uh_ubus_session *ses, struct blob_buf *b)
{
	struct uh_ubus_session_acl *acl;
	const char *lastobj = NULL;
	void *c = NULL;

	avl_for_each_element(&ses->acls, acl, avl)
	{
		if (!lastobj || strcmp(acl->object, lastobj))
		{
			if (c) blobmsg_close_array(b, c);
			c = blobmsg_open_array(b, acl->object);
		}

		blobmsg_add_string(b, NULL, acl->function);
		lastobj = acl->object;
	}

	if (c) blobmsg_close_array(b, c);
}

static void
uh_ubus_session_dump(struct uh_ubus_session *ses,
					 struct ubus_context *ctx,
					 struct ubus_request_data *req)
{
	void *c;
	struct blob_buf b;

	memset(&b, 0, sizeof(b));
	blob_buf_init(&b, 0);

	blobmsg_add_string(&b, "sid", ses->id);
	blobmsg_add_u32(&b, "timeout", ses->timeout);
	blobmsg_add_u32(&b, "touched", ses->touched.tv_sec);

	c = blobmsg_open_table(&b, "acls");
	uh_ubus_session_dump_acls(ses, &b);
	blobmsg_close_table(&b, c);

	c = blobmsg_open_table(&b, "data");
	uh_ubus_session_dump_data(ses, &b);
	blobmsg_close_table(&b, c);

	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);
}

static struct uh_ubus_session *
uh_ubus_session_create(struct uh_ubus_state *state, int timeout)
{
	struct uh_ubus_session *ses;

	ses = malloc(sizeof(*ses));

	/* failed to allocate memory... */
	if (!ses)
		return NULL;

	memset(ses, 0, sizeof(*ses));

	uh_ubus_random(ses->id);

	ses->timeout  = timeout;
	ses->avl.key  = ses->id;

	avl_insert(&state->sessions, &ses->avl);
	avl_init(&ses->acls, uh_ubus_avlcmp, true, NULL);
	avl_init(&ses->data, uh_ubus_avlcmp, false, NULL);
	clock_gettime(CLOCK_MONOTONIC, &ses->touched);

	return ses;
}


static struct uh_ubus_session *
uh_ubus_session_get(struct uh_ubus_state *state, const char *id)
{
	struct uh_ubus_session *ses;

	ses = avl_find_element(&state->sessions, id, ses, avl);

	if (ses)
		clock_gettime(CLOCK_MONOTONIC, &ses->touched);

	return ses;
}

static void
uh_ubus_session_destroy(struct uh_ubus_state *state,
						struct uh_ubus_session *ses)
{
	struct uh_ubus_session_acl *acl, *nacl;
	struct uh_ubus_session_data *data, *ndata;

	avl_remove_all_elements(&ses->acls, acl, avl, nacl)
		free(acl);

	avl_remove_all_elements(&ses->data, data, avl, ndata)
		free(data);

	avl_delete(&state->sessions, &ses->avl);
	free(ses);
}

static void
uh_ubus_session_cleanup(struct uh_ubus_state *state)
{
	struct timespec now;
	struct uh_ubus_session *ses, *nses;

	clock_gettime(CLOCK_MONOTONIC, &now);

	avl_for_each_element_safe(&state->sessions, ses, avl, nses)
	{
		if ((now.tv_sec - ses->touched.tv_sec) >= ses->timeout)
			uh_ubus_session_destroy(state, ses);
	}
}


static int
uh_ubus_handle_create(struct ubus_context *ctx, struct ubus_object *obj,
					  struct ubus_request_data *req, const char *method,
					  struct blob_attr *msg)
{
	struct uh_ubus_state *state = container_of(obj, struct uh_ubus_state, ubus);
	struct uh_ubus_session *ses;
	struct blob_attr *tb[__UH_UBUS_SN_MAX];

	int timeout = state->timeout;

	blobmsg_parse(new_policy, __UH_UBUS_SN_MAX, tb, blob_data(msg), blob_len(msg));

	/* TODO: make this a uloop timeout */
	uh_ubus_session_cleanup(state);

	if (tb[UH_UBUS_SN_TIMEOUT])
		timeout = *(uint32_t *)blobmsg_data(tb[UH_UBUS_SN_TIMEOUT]);

	ses = uh_ubus_session_create(state, timeout);

	if (ses)
		uh_ubus_session_dump(ses, ctx, req);

	return 0;
}

static int
uh_ubus_handle_list(struct ubus_context *ctx, struct ubus_object *obj,
					struct ubus_request_data *req, const char *method,
					struct blob_attr *msg)
{
	struct uh_ubus_state *state = container_of(obj, struct uh_ubus_state, ubus);
	struct uh_ubus_session *ses;
	struct blob_attr *tb[__UH_UBUS_SI_MAX];

	blobmsg_parse(sid_policy, __UH_UBUS_SI_MAX, tb, blob_data(msg), blob_len(msg));

	/* TODO: make this a uloop timeout */
	uh_ubus_session_cleanup(state);

	if (!tb[UH_UBUS_SI_SID])
	{
		avl_for_each_element(&state->sessions, ses, avl)
			uh_ubus_session_dump(ses, ctx, req);
	}
	else
	{
		ses = uh_ubus_session_get(state, blobmsg_data(tb[UH_UBUS_SI_SID]));

		if (!ses)
			return UBUS_STATUS_NOT_FOUND;

		uh_ubus_session_dump(ses, ctx, req);
	}

	return 0;
}


static int
uh_ubus_session_grant(struct uh_ubus_session *ses, struct ubus_context *ctx,
					  const char *object, const char *function)
{
	struct uh_ubus_session_acl *acl, *nacl;

	acl = avl_find_element(&ses->acls, object, acl, avl);

	if (acl)
	{
		avl_for_element_to_last(&ses->acls, acl, acl, avl)
		{
			if (!strcmp(acl->function, function))
				return 1;
		}
	}

	nacl = malloc(sizeof(*nacl) + strlen(object) + strlen(function) + 2);

	if (nacl)
	{
		memset(nacl, 0, sizeof(*nacl));
		nacl->function = nacl->object + 1;
		nacl->function += sprintf(nacl->object, "%s", object);
		sprintf(nacl->function, "%s", function);

		nacl->avl.key = nacl->object;
		avl_insert(&ses->acls, &nacl->avl);
	}

	return 0;
}

static int
uh_ubus_session_revoke(struct uh_ubus_session *ses, struct ubus_context *ctx,
					   const char *object, const char *function)
{
	struct uh_ubus_session_acl *acl, *nacl;

	if (!object && !function)
	{
		avl_remove_all_elements(&ses->acls, acl, avl, nacl)
			free(acl);
	}
	else
	{
		avl_for_each_element_safe(&ses->acls, acl, avl, nacl)
		{
			if (uh_ubus_strmatch(acl->object, object) &&
				uh_ubus_strmatch(acl->function, function))
			{
				avl_delete(&ses->acls, &acl->avl);
				free(acl);
			}
		}
	}

	return 0;
}


static int
uh_ubus_handle_grant(struct ubus_context *ctx, struct ubus_object *obj,
					 struct ubus_request_data *req, const char *method,
					 struct blob_attr *msg)
{
	struct uh_ubus_state *state = container_of(obj, struct uh_ubus_state, ubus);
	struct uh_ubus_session *ses;
	struct blob_attr *tb[__UH_UBUS_SA_MAX];
	struct blob_attr *attr, *sattr;
	const char *object, *function;
	int rem1, rem2;

	blobmsg_parse(acl_policy, __UH_UBUS_SA_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[UH_UBUS_SA_SID] || !tb[UH_UBUS_SA_OBJECTS])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ses = uh_ubus_session_get(state, blobmsg_data(tb[UH_UBUS_SA_SID]));

	if (!ses)
		return UBUS_STATUS_NOT_FOUND;

	blobmsg_for_each_attr(attr, tb[UH_UBUS_SA_OBJECTS], rem1)
	{
		if (blob_id(attr) != BLOBMSG_TYPE_ARRAY)
			continue;

		object = NULL;
		function = NULL;

		blobmsg_for_each_attr(sattr, attr, rem2)
		{
			if (blob_id(sattr) != BLOBMSG_TYPE_STRING)
				continue;

			if (!object)
				object = blobmsg_data(sattr);
			else if (!function)
				function = blobmsg_data(sattr);
			else
				break;
		}

		if (object && function)
			uh_ubus_session_grant(ses, ctx, object, function);
	}

	return 0;
}

static int
uh_ubus_handle_revoke(struct ubus_context *ctx, struct ubus_object *obj,
					  struct ubus_request_data *req, const char *method,
					  struct blob_attr *msg)
{
	struct uh_ubus_state *state = container_of(obj, struct uh_ubus_state, ubus);
	struct uh_ubus_session *ses;
	struct blob_attr *tb[__UH_UBUS_SA_MAX];
	struct blob_attr *attr, *sattr;
	const char *object, *function;
	int rem1, rem2;

	blobmsg_parse(acl_policy, __UH_UBUS_SA_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[UH_UBUS_SA_SID])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ses = uh_ubus_session_get(state, blobmsg_data(tb[UH_UBUS_SA_SID]));

	if (!ses)
		return UBUS_STATUS_NOT_FOUND;

	if (!tb[UH_UBUS_SA_OBJECTS])
	{
		uh_ubus_session_revoke(ses, ctx, NULL, NULL);
	}
	else
	{
		blobmsg_for_each_attr(attr, tb[UH_UBUS_SA_OBJECTS], rem1)
		{
			if (blob_id(attr) != BLOBMSG_TYPE_ARRAY)
				continue;

			object = NULL;
			function = NULL;

			blobmsg_for_each_attr(sattr, attr, rem2)
			{
				if (blob_id(sattr) != BLOBMSG_TYPE_STRING)
					continue;

				if (!object)
					object = blobmsg_data(sattr);
				else if (!function)
					function = blobmsg_data(sattr);
				else
					break;
			}

			if (object && function)
				uh_ubus_session_revoke(ses, ctx, object, function);
		}
	}

	return 0;
}

static int
uh_ubus_handle_set(struct ubus_context *ctx, struct ubus_object *obj,
				   struct ubus_request_data *req, const char *method,
				   struct blob_attr *msg)
{
	struct uh_ubus_state *state = container_of(obj, struct uh_ubus_state, ubus);
	struct uh_ubus_session *ses;
	struct uh_ubus_session_data *data;
	struct blob_attr *tb[__UH_UBUS_SA_MAX];
	struct blob_attr *attr;
	int rem;

	blobmsg_parse(set_policy, __UH_UBUS_SS_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[UH_UBUS_SS_SID] || !tb[UH_UBUS_SS_VALUES])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ses = uh_ubus_session_get(state, blobmsg_data(tb[UH_UBUS_SS_SID]));

	if (!ses)
		return UBUS_STATUS_NOT_FOUND;

	blobmsg_for_each_attr(attr, tb[UH_UBUS_SS_VALUES], rem)
	{
		if (!blobmsg_name(attr)[0])
			continue;

		data = avl_find_element(&ses->data, blobmsg_name(attr), data, avl);

		if (data)
		{
			avl_delete(&ses->data, &data->avl);
			free(data);
		}

		data = malloc(sizeof(*data) + blob_pad_len(attr));

		if (!data)
			break;

		memset(data, 0, sizeof(*data) + blob_pad_len(attr));
		memcpy(data->attr, attr, blob_pad_len(attr));

		data->avl.key = blobmsg_name(data->attr);
		avl_insert(&ses->data, &data->avl);
	}

	return 0;
}

static int
uh_ubus_handle_get(struct ubus_context *ctx, struct ubus_object *obj,
				   struct ubus_request_data *req, const char *method,
				   struct blob_attr *msg)
{
	struct uh_ubus_state *state = container_of(obj, struct uh_ubus_state, ubus);
	struct uh_ubus_session *ses;
	struct uh_ubus_session_data *data;
	struct blob_attr *tb[__UH_UBUS_SA_MAX];
	struct blob_attr *attr;
	struct blob_buf b;
	void *c;
	int rem;

	blobmsg_parse(get_policy, __UH_UBUS_SG_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[UH_UBUS_SG_SID])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ses = uh_ubus_session_get(state, blobmsg_data(tb[UH_UBUS_SG_SID]));

	if (!ses)
		return UBUS_STATUS_NOT_FOUND;

	memset(&b, 0, sizeof(b));
	blob_buf_init(&b, 0);
	c = blobmsg_open_table(&b, "values");

	if (!tb[UH_UBUS_SG_KEYS])
	{
		uh_ubus_session_dump_data(ses, &b);
	}
	else
	{
		blobmsg_for_each_attr(attr, tb[UH_UBUS_SG_KEYS], rem)
		{
			if (blob_id(attr) != BLOBMSG_TYPE_STRING)
				continue;

			data = avl_find_element(&ses->data, blobmsg_data(attr), data, avl);

			if (!data)
				continue;

			blobmsg_add_field(&b, blobmsg_type(data->attr),
							  blobmsg_name(data->attr),
							  blobmsg_data(data->attr),
							  blobmsg_data_len(data->attr));
		}
	}

	blobmsg_close_table(&b, c);
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return 0;
}

static int
uh_ubus_handle_unset(struct ubus_context *ctx, struct ubus_object *obj,
				     struct ubus_request_data *req, const char *method,
				     struct blob_attr *msg)
{
	struct uh_ubus_state *state = container_of(obj, struct uh_ubus_state, ubus);
	struct uh_ubus_session *ses;
	struct uh_ubus_session_data *data, *ndata;
	struct blob_attr *tb[__UH_UBUS_SA_MAX];
	struct blob_attr *attr;
	int rem;

	blobmsg_parse(get_policy, __UH_UBUS_SG_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[UH_UBUS_SG_SID])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ses = uh_ubus_session_get(state, blobmsg_data(tb[UH_UBUS_SG_SID]));

	if (!ses)
		return UBUS_STATUS_NOT_FOUND;

	if (!tb[UH_UBUS_SG_KEYS])
	{
		avl_remove_all_elements(&ses->data, data, avl, ndata)
			free(data);
	}
	else
	{
		blobmsg_for_each_attr(attr, tb[UH_UBUS_SG_KEYS], rem)
		{
			if (blob_id(attr) != BLOBMSG_TYPE_STRING)
				continue;

			data = avl_find_element(&ses->data, blobmsg_data(attr), data, avl);

			if (!data)
				continue;

			avl_delete(&ses->data, &data->avl);
			free(data);
		}
	}

	return 0;
}

static int
uh_ubus_handle_destroy(struct ubus_context *ctx, struct ubus_object *obj,
					   struct ubus_request_data *req, const char *method,
					   struct blob_attr *msg)
{
	struct uh_ubus_state *state = container_of(obj, struct uh_ubus_state, ubus);
	struct uh_ubus_session *ses;
	struct blob_attr *tb[__UH_UBUS_SA_MAX];

	blobmsg_parse(sid_policy, __UH_UBUS_SI_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[UH_UBUS_SI_SID])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ses = uh_ubus_session_get(state, blobmsg_data(tb[UH_UBUS_SI_SID]));

	if (!ses)
		return UBUS_STATUS_NOT_FOUND;

	uh_ubus_session_destroy(state, ses);

	return 0;
}


struct uh_ubus_state *
uh_ubus_init(const struct config *conf)
{
	int rv;
	struct uh_ubus_state *state;
	struct ubus_object *session_object;

	static struct ubus_method session_methods[] = {
		UBUS_METHOD("create",  uh_ubus_handle_create,  new_policy),
		UBUS_METHOD("list",    uh_ubus_handle_list,    sid_policy),
		UBUS_METHOD("grant",   uh_ubus_handle_grant,   acl_policy),
		UBUS_METHOD("revoke",  uh_ubus_handle_revoke,  acl_policy),
		UBUS_METHOD("set",     uh_ubus_handle_set,     set_policy),
		UBUS_METHOD("get",     uh_ubus_handle_get,     get_policy),
		UBUS_METHOD("unset",   uh_ubus_handle_unset,   get_policy),
		UBUS_METHOD("destroy", uh_ubus_handle_destroy, sid_policy),
	};

	static struct ubus_object_type session_type =
		UBUS_OBJECT_TYPE("uhttpd", session_methods);

	state = malloc(sizeof(*state));

	if (!state)
	{
		fprintf(stderr, "Unable to allocate memory for ubus state\n");
		exit(1);
	}

	memset(state, 0, sizeof(*state));
	state->ctx = ubus_connect(conf->ubus_socket);
	state->timeout = conf->script_timeout;

	if (!state->ctx)
	{
		fprintf(stderr, "Unable to connect to ubus socket\n");
		exit(1);
	}

	ubus_add_uloop(state->ctx);

	session_object = &state->ubus;
	session_object->name = "session";
	session_object->type = &session_type;
	session_object->methods = session_methods;
	session_object->n_methods = ARRAY_SIZE(session_methods);

	rv = ubus_add_object(state->ctx, &state->ubus);

	if (rv)
	{
		fprintf(stderr, "Unable to publish ubus object: %s\n",
				ubus_strerror(rv));
		exit(1);
	}

	blob_buf_init(&state->buf, 0);
	avl_init(&state->sessions, uh_ubus_avlcmp, false, NULL);

	return state;
}


static bool
uh_ubus_request_parse_url(struct client *cl, char **sid, char **obj, char **fun)
{
	char *url = cl->request.url + strlen(cl->server->conf->ubus_prefix);

	for (; url && *url == '/'; *url++ = 0);
	*sid = url;

	for (url = url ? strchr(url, '/') : NULL; url && *url == '/'; *url++ = 0);
	*obj = url;

	for (url = url ? strchr(url, '/') : NULL; url && *url == '/'; *url++ = 0);
	*fun = url;

	for (url = url ? strchr(url, '/') : NULL; url && *url == '/'; *url++ = 0);
	return (*sid && *obj && *fun);
}

static bool
uh_ubus_request_parse_post(struct client *cl, int len, struct blob_buf *b)
{
	int rlen;
	bool rv = false;
	char buf[UH_LIMIT_MSGHEAD];

	struct json_object *obj = NULL;
	struct json_tokener *tok = NULL;

	if (!len)
		return NULL;

	memset(b, 0, sizeof(*b));
	blob_buf_init(b, 0);

	tok = json_tokener_new();

	while (len > 0)
	{
		/* remaining data in http head buffer ... */
		if (cl->httpbuf.len > 0)
		{
			rlen = min(len, cl->httpbuf.len);

			D("ubus: feed %d HTTP buffer bytes\n", rlen);

			memcpy(buf, cl->httpbuf.ptr, rlen);

			cl->httpbuf.len -= rlen;
			cl->httpbuf.ptr += rlen;
		}

		/* read it from socket ... */
		else
		{
			ensure_out(rlen = uh_tcp_recv(cl, buf, min(len, sizeof(buf))));

			if ((rlen < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
				break;

			D("ubus: feed %d/%d TCP socket bytes\n",
			  rlen, min(len, sizeof(buf)));
		}

		obj = json_tokener_parse_ex(tok, buf, rlen);
		len -= rlen;

		if (tok->err != json_tokener_continue && !is_error(obj))
			break;
	}

out:
	if (!is_error(obj))
	{
		if (json_object_get_type(obj) == json_type_object)
		{
			rv = true;
			json_object_object_foreach(obj, key, val)
			{
				if (!blobmsg_add_json_element(b, key, val))
				{
					rv = false;
					break;
				}
			}
		}

		json_object_put(obj);
	}

	json_tokener_free(tok);

	if (!rv)
		blob_buf_free(b);

	return rv;
}

static void
uh_ubus_request_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int len;
	char *str;
	struct client *cl = (struct client *)req->priv;

	if (!msg)
	{
		uh_http_sendhf(cl, 204, "No content", "Function did not return data\n");
		return;
	}

	str = blobmsg_format_json_indent(msg, true, 0);
	len = strlen(str);

	ensure_out(uh_http_sendf(cl, NULL, "HTTP/1.0 200 OK\r\n"));
	ensure_out(uh_http_sendf(cl, NULL, "Content-Type: application/json\r\n"));
	ensure_out(uh_http_sendf(cl, NULL, "Content-Length: %i\r\n\r\n", len));
	ensure_out(uh_http_send(cl, NULL, str, len));

out:
	free(str);
}

bool
uh_ubus_request(struct client *cl, struct uh_ubus_state *state)
{
	int i, len = 0;
	bool access = false;
	char *sid, *obj, *fun;

	struct blob_buf buf;
	struct uh_ubus_session *ses;
	struct uh_ubus_session_acl *acl;

	uint32_t obj_id;


	memset(&buf, 0, sizeof(buf));
	blob_buf_init(&buf, 0);

	if (!uh_ubus_request_parse_url(cl, &sid, &obj, &fun))
	{
		uh_http_sendhf(cl, 400, "Bad Request", "Invalid Request\n");
		goto out;
	}

	if (!(ses = uh_ubus_session_get(state, sid)))
	{
		uh_http_sendhf(cl, 404, "Not Found", "No such session\n");
		goto out;
	}

	avl_for_each_element(&ses->acls, acl, avl)
	{
		if (uh_ubus_strmatch(obj, acl->object) &&
			uh_ubus_strmatch(fun, acl->function))
		{
			access = true;
			break;
		}
	}

	if (!access)
	{
		uh_http_sendhf(cl, 403, "Denied", "Access to object denied\n");
		goto out;
	}

	/* find content length */
	if (cl->request.method == UH_HTTP_MSG_POST)
	{
		foreach_header(i, cl->request.headers)
		{
			if (!strcasecmp(cl->request.headers[i], "Content-Length"))
			{
				len = atoi(cl->request.headers[i+1]);
				break;
			}
		}
	}

	if (len > UH_UBUS_MAX_POST_SIZE)
	{
		uh_http_sendhf(cl, 413, "Too Large", "Message too big\n");
		goto out;
	}

	if (len && !uh_ubus_request_parse_post(cl, len, &buf))
	{
		uh_http_sendhf(cl, 400, "Bad Request", "Invalid JSON data\n");
		goto out;
	}

	if (ubus_lookup_id(state->ctx, obj, &obj_id))
	{
		uh_http_sendhf(cl, 500, "Internal Error", "Unable to lookup object\n");
		goto out;
	}

	if (ubus_invoke(state->ctx, obj_id, fun, buf.head,
					uh_ubus_request_cb, cl, state->timeout * 1000))
	{
		uh_http_sendhf(cl, 500, "Internal Error", "Unable to invoke function\n");
		goto out;
	}

out:
	blob_buf_free(&buf);
	return false;
}

void
uh_ubus_close(struct uh_ubus_state *state)
{
	if (state->ctx)
		ubus_free(state->ctx);

	free(state);
}
