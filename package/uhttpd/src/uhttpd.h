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


#define UH_LIMIT_MSGHEAD	4096
#define UH_LIMIT_HEADERS	64

#define UH_LIMIT_CLIENTS	64

#define UH_HTTP_MSG_GET		0
#define UH_HTTP_MSG_HEAD	1
#define UH_HTTP_MSG_POST	2

struct listener;
struct client;
struct interpreter;
struct http_request;

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
#ifdef HAVE_CGI
	char *cgi_prefix;
#endif
#ifdef HAVE_LUA
	char *lua_prefix;
	char *lua_handler;
	lua_State *lua_state;
	lua_State * (*lua_init) (const char *handler);
	void (*lua_close) (lua_State *L);
	void (*lua_request) (struct client *cl, struct http_request *req, lua_State *L);
#endif
#if defined(HAVE_CGI) || defined(HAVE_LUA)
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
	int (*tls_recv) (struct client *c, void *buf, int len);
	int (*tls_send) (struct client *c, void *buf, int len);
#endif
};

struct listener {
	int socket;
	struct sockaddr_in6 addr;
	struct config *conf;
#ifdef HAVE_TLS
	SSL_CTX *tls;
#endif
	struct listener *next;
};

struct client {
	int socket;
	int peeklen;
	char peekbuf[UH_LIMIT_MSGHEAD];
	struct listener *server;
	struct sockaddr_in6 servaddr;
	struct sockaddr_in6 peeraddr;
#ifdef HAVE_TLS
	SSL *tls;
#endif
	struct client *next;
};

struct auth_realm {
	char path[PATH_MAX];
	char user[32];
	char pass[128];
	struct auth_realm *next;
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

#ifdef HAVE_CGI
struct interpreter {
	char path[PATH_MAX];
	char extn[32];
	struct interpreter *next;
};
#endif

#endif
