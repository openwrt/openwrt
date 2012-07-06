/*
 * uhttpd - Tiny single-threaded httpd - Main header
 *
 *   Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
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

#ifndef _UHTTPD_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <dlfcn.h>

#include <libubox/list.h>
#include <libubox/uloop.h>


#ifdef HAVE_LUA
#include <lua.h>
#endif

#ifdef HAVE_TLS
#include <openssl/ssl.h>
#endif

/* uClibc... */
#ifndef SOL_TCP
#define SOL_TCP	6
#endif

#ifdef DEBUG
#define D(...) fprintf(stderr, __VA_ARGS__)
#else
#define D(...)
#endif


#define UH_LIMIT_MSGHEAD	4096
#define UH_LIMIT_HEADERS	64

#define UH_LIMIT_CLIENTS	64

#define UH_HTTP_MSG_GET		0
#define UH_HTTP_MSG_HEAD	1
#define UH_HTTP_MSG_POST	2

#define UH_SOCK_CLIENT		0
#define UH_SOCK_SERVER		1

struct listener;
struct client;
struct interpreter;
struct http_request;
struct uh_ubus_state;

struct config {
	char docroot[PATH_MAX];
	char *realm;
	char *file;
	char *index_file;
	char *error_handler;
	int no_symlinks;
	int no_dirlists;
	int network_timeout;
	int rfc1918_filter;
	int tcp_keepalive;
	int max_requests;
#ifdef HAVE_CGI
	char *cgi_prefix;
#endif
#ifdef HAVE_LUA
	char *lua_prefix;
	char *lua_handler;
	lua_State *lua_state;
	lua_State * (*lua_init) (const struct config *conf);
	void (*lua_close) (lua_State *L);
	bool (*lua_request) (struct client *cl, lua_State *L);
#endif
#ifdef HAVE_UBUS
	char *ubus_prefix;
	char *ubus_socket;
	void *ubus_state;
	struct uh_ubus_state * (*ubus_init) (const struct config *conf);
	void (*ubus_close) (struct uh_ubus_state *state);
	bool (*ubus_request) (struct client *cl, struct uh_ubus_state *state);
#endif
#if defined(HAVE_CGI) || defined(HAVE_LUA) || defined(HAVE_UBUS)
	int script_timeout;
#endif
#ifdef HAVE_TLS
	char *cert;
	char *key;
	SSL_CTX *tls;
	SSL_CTX * (*tls_init) (void);
	int (*tls_cert) (SSL_CTX *c, const char *file);
	int (*tls_key) (SSL_CTX *c, const char *file);
	void (*tls_free) (struct listener *l);
	int (*tls_accept) (struct client *c);
	void (*tls_close) (struct client *c);
	int (*tls_recv) (struct client *c, char *buf, int len);
	int (*tls_send) (struct client *c, const char *buf, int len);
#endif
};

struct http_request {
	int	method;
	float version;
	int redirect_status;
	char *url;
	char *headers[UH_LIMIT_HEADERS];
	struct auth_realm *realm;
};

struct http_response {
	int statuscode;
	char *statusmsg;
	char *headers[UH_LIMIT_HEADERS];
};

struct listener {
	struct uloop_fd fd;
	int socket;
	int n_clients;
	struct sockaddr_in6 addr;
	struct config *conf;
#ifdef HAVE_TLS
	SSL_CTX *tls;
#endif
	struct listener *next;
};

struct client {
#ifdef HAVE_TLS
	SSL *tls;
#endif
	struct uloop_fd fd;
	struct uloop_fd pipe;
	struct uloop_process proc;
	struct uloop_timeout timeout;
	bool (*cb)(struct client *);
	void *priv;
	bool dispatched;
	bool dead;
	struct {
		char buf[UH_LIMIT_MSGHEAD];
		char *ptr;
		int len;
	} httpbuf;
	struct listener *server;
	struct http_request request;
	struct http_response response;
	struct sockaddr_in6 servaddr;
	struct sockaddr_in6 peeraddr;
	struct client *next;
};

struct client_light {
#ifdef HAVE_TLS
	SSL *tls;
#endif
	struct uloop_fd fd;
};

struct auth_realm {
	char path[PATH_MAX];
	char user[32];
	char pass[128];
	struct auth_realm *next;
};

#ifdef HAVE_CGI
struct interpreter {
	char path[PATH_MAX];
	char extn[32];
	struct interpreter *next;
};
#endif

#endif
