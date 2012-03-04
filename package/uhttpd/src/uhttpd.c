/*
 * uhttpd - Tiny single-threaded httpd - Main component
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

#define _XOPEN_SOURCE 500	/* crypt() */

#include "uhttpd.h"
#include "uhttpd-utils.h"
#include "uhttpd-file.h"

#ifdef HAVE_CGI
#include "uhttpd-cgi.h"
#endif

#ifdef HAVE_LUA
#include "uhttpd-lua.h"
#endif

#ifdef HAVE_TLS
#include "uhttpd-tls.h"
#endif


static int run = 1;

static void uh_sigterm(int sig)
{
	run = 0;
}

static void uh_sigchld(int sig)
{
	while( waitpid(-1, NULL, WNOHANG) > 0 ) { }
}

static void uh_config_parse(struct config *conf)
{
	FILE *c;
	char line[512];
	char *col1 = NULL;
	char *col2 = NULL;
	char *eol  = NULL;

	const char *path = conf->file ? conf->file : "/etc/httpd.conf";


	if( (c = fopen(path, "r")) != NULL )
	{
		memset(line, 0, sizeof(line));

		while( fgets(line, sizeof(line) - 1, c) )
		{
			if( (line[0] == '/') && (strchr(line, ':') != NULL) )
			{
				if( !(col1 = strchr(line, ':')) || (*col1++ = 0) ||
				    !(col2 = strchr(col1, ':')) || (*col2++ = 0) ||
					!(eol = strchr(col2, '\n')) || (*eol++  = 0) )
						continue;

				if( !uh_auth_add(line, col1, col2) )
				{
					fprintf(stderr,
						"Notice: No password set for user %s, ignoring "
						"authentication on %s\n", col1, line
					);
				}
			}
			else if( !strncmp(line, "I:", 2) )
			{
				if( !(col1 = strchr(line, ':')) || (*col1++ = 0) ||
				    !(eol = strchr(col1, '\n')) || (*eol++  = 0) )
				    	continue;

				conf->index_file = strdup(col1);
			}
			else if( !strncmp(line, "E404:", 5) )
			{
				if( !(col1 = strchr(line, ':')) || (*col1++ = 0) ||
				    !(eol = strchr(col1, '\n')) || (*eol++  = 0) )
						continue;

				conf->error_handler = strdup(col1);
			}
#ifdef HAVE_CGI
			else if( (line[0] == '*') && (strchr(line, ':') != NULL) )
			{
				if( !(col1 = strchr(line, '*')) || (*col1++ = 0) ||
				    !(col2 = strchr(col1, ':')) || (*col2++ = 0) ||
				    !(eol = strchr(col2, '\n')) || (*eol++  = 0) )
						continue;

				if( !uh_interpreter_add(col1, col2) )
				{
					fprintf(stderr,
						"Unable to add interpreter %s for extension %s: "
						"Out of memory\n", col2, col1
					);
				}
			}
#endif
		}

		fclose(c);
	}
}

static int uh_socket_bind(
	fd_set *serv_fds, int *max_fd, const char *host, const char *port,
	struct addrinfo *hints, int do_tls, struct config *conf
) {
	int sock = -1;
	int yes = 1;
	int status;
	int bound = 0;

	int tcp_ka_idl, tcp_ka_int, tcp_ka_cnt;

	struct listener *l = NULL;
	struct addrinfo *addrs = NULL, *p = NULL;

	if( (status = getaddrinfo(host, port, hints, &addrs)) != 0 )
	{
		fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
	}

	/* try to bind a new socket to each found address */
	for( p = addrs; p; p = p->ai_next )
	{
		/* get the socket */
		if( (sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 )
		{
			perror("socket()");
			goto error;
		}

		/* "address already in use" */
		if( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) )
		{
			perror("setsockopt()");
			goto error;
		}

		/* TCP keep-alive */
		if( conf->tcp_keepalive > 0 )
		{
			tcp_ka_idl = 1;
			tcp_ka_cnt = 3;
			tcp_ka_int = conf->tcp_keepalive;

			if( setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes)) ||
			    setsockopt(sock, SOL_TCP, TCP_KEEPIDLE,  &tcp_ka_idl, sizeof(tcp_ka_idl)) ||
			    setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, &tcp_ka_int, sizeof(tcp_ka_int)) ||
			    setsockopt(sock, SOL_TCP, TCP_KEEPCNT,   &tcp_ka_cnt, sizeof(tcp_ka_cnt)) )
			{
			    fprintf(stderr, "Notice: Unable to enable TCP keep-alive: %s\n",
			    	strerror(errno));
			}
		}

		/* required to get parallel v4 + v6 working */
		if( p->ai_family == AF_INET6 )
		{
			if( setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes)) == -1 )
			{
				perror("setsockopt()");
				goto error;
			}
		}

		/* bind */
		if( bind(sock, p->ai_addr, p->ai_addrlen) == -1 )
		{
			perror("bind()");
			goto error;
		}

		/* listen */
		if( listen(sock, UH_LIMIT_CLIENTS) == -1 )
		{
			perror("listen()");
			goto error;
		}

		/* add listener to global list */
		if( ! (l = uh_listener_add(sock, conf)) )
		{
			fprintf(stderr, "uh_listener_add(): Failed to allocate memory\n");
			goto error;
		}

#ifdef HAVE_TLS
		/* init TLS */
		l->tls = do_tls ? conf->tls : NULL;
#endif

		/* add socket to server fd set */
		FD_SET(sock, serv_fds);
		fd_cloexec(sock);
		*max_fd = max(*max_fd, sock);

		bound++;
		continue;

		error:
		if( sock > 0 )
			close(sock);
	}

	freeaddrinfo(addrs);

	return bound;
}

static struct http_request * uh_http_header_parse(struct client *cl, char *buffer, int buflen)
{
	char *method  = &buffer[0];
	char *path    = NULL;
	char *version = NULL;

	char *headers = NULL;
	char *hdrname = NULL;
	char *hdrdata = NULL;

	int i;
	int hdrcount = 0;

	static struct http_request req;

	memset(&req, 0, sizeof(req));


	/* terminate initial header line */
	if( (headers = strfind(buffer, buflen, "\r\n", 2)) != NULL )
	{
		buffer[buflen-1] = 0;

		*headers++ = 0;
		*headers++ = 0;

		/* find request path */
		if( (path = strchr(buffer, ' ')) != NULL )
			*path++ = 0;

		/* find http version */
		if( (path != NULL) && ((version = strchr(path, ' ')) != NULL) )
			*version++ = 0;


		/* check method */
		if( strcmp(method, "GET") && strcmp(method, "HEAD") && strcmp(method, "POST") )
		{
			/* invalid method */
			uh_http_response(cl, 405, "Method Not Allowed");
			return NULL;
		}
		else
		{
			switch(method[0])
			{
				case 'G':
					req.method = UH_HTTP_MSG_GET;
					break;

				case 'H':
					req.method = UH_HTTP_MSG_HEAD;
					break;

				case 'P':
					req.method = UH_HTTP_MSG_POST;
					break;
			}
		}

		/* check path */
		if( !path || !strlen(path) )
		{
			/* malformed request */
			uh_http_response(cl, 400, "Bad Request");
			return NULL;
		}
		else
		{
			req.url = path;
		}

		/* check version */
		if( (version == NULL) || (strcmp(version, "HTTP/0.9") &&
		    strcmp(version, "HTTP/1.0") && strcmp(version, "HTTP/1.1")) )
		{
			/* unsupported version */
			uh_http_response(cl, 400, "Bad Request");
			return NULL;
		}
		else
		{
			req.version = strtof(&version[5], NULL);
		}


		/* process header fields */
		for( i = (int)(headers - buffer); i < buflen; i++ )
		{
			/* found eol and have name + value, push out header tuple */
			if( hdrname && hdrdata && (buffer[i] == '\r' || buffer[i] == '\n') )
			{
				buffer[i] = 0;

				/* store */
				if( (hdrcount + 1) < array_size(req.headers) )
				{
					req.headers[hdrcount++] = hdrname;
					req.headers[hdrcount++] = hdrdata;

					hdrname = hdrdata = NULL;
				}

				/* too large */
				else
				{
					uh_http_response(cl, 413, "Request Entity Too Large");
					return NULL;
				}
			}

			/* have name but no value and found a colon, start of value */
			else if( hdrname && !hdrdata &&
			    ((i+1) < buflen) && (buffer[i] == ':')
			) {
				buffer[i] = 0;
				hdrdata = &buffer[i+1];

				while ((hdrdata + 1) < (buffer + buflen) && *hdrdata == ' ')
					hdrdata++;
			}

			/* have no name and found [A-Za-z], start of name */
			else if( !hdrname && isalpha(buffer[i]) )
			{
				hdrname = &buffer[i];
			}
		}

		/* valid enough */
		req.redirect_status = 200;
		return &req;
	}

	/* Malformed request */
	uh_http_response(cl, 400, "Bad Request");
	return NULL;
}


static struct http_request * uh_http_header_recv(struct client *cl)
{
	static char buffer[UH_LIMIT_MSGHEAD];
	char *bufptr = &buffer[0];
	char *idxptr = NULL;

	struct timeval timeout;

	fd_set reader;

	ssize_t blen = sizeof(buffer)-1;
	ssize_t rlen = 0;

	memset(buffer, 0, sizeof(buffer));

	while( blen > 0 )
	{
		FD_ZERO(&reader);
		FD_SET(cl->socket, &reader);

		/* fail after 0.1s */
		timeout.tv_sec  = 0;
		timeout.tv_usec = 100000;

		/* check whether fd is readable */
		if( select(cl->socket + 1, &reader, NULL, NULL, &timeout) > 0 )
		{
			/* receive data */
			ensure_out(rlen = uh_tcp_peek(cl, bufptr, blen));

			if( (idxptr = strfind(buffer, sizeof(buffer), "\r\n\r\n", 4)) )
			{
				ensure_out(rlen = uh_tcp_recv(cl, bufptr,
					(int)(idxptr - bufptr) + 4));

				/* header read complete ... */
				blen -= rlen;
				return uh_http_header_parse(cl, buffer,
					sizeof(buffer) - blen - 1);
			}
			else
			{
				ensure_out(rlen = uh_tcp_recv(cl, bufptr, rlen));

				/* unexpected eof - #7904 */
				if( rlen == 0 )
					return NULL;

				blen -= rlen;
				bufptr += rlen;
			}
		}
		else
		{
			/* invalid request (unexpected eof/timeout) */
			return NULL;
		}
	}

	/* request entity too large */
	uh_http_response(cl, 413, "Request Entity Too Large");

out:
	return NULL;
}

#if defined(HAVE_LUA) || defined(HAVE_CGI)
static int uh_path_match(const char *prefix, const char *url)
{
	if( (strstr(url, prefix) == url) &&
	    ((prefix[strlen(prefix)-1] == '/') ||
		 (strlen(url) == strlen(prefix))   ||
		 (url[strlen(prefix)] == '/'))
	) {
		return 1;
	}

	return 0;
}
#endif

static void uh_dispatch_request(
	struct client *cl, struct http_request *req, struct path_info *pin
) {
#ifdef HAVE_CGI
	struct interpreter *ipr = NULL;

	if( uh_path_match(cl->server->conf->cgi_prefix, pin->name) ||
		(ipr = uh_interpreter_lookup(pin->phys)) )
	{
		uh_cgi_request(cl, req, pin, ipr);
	}
	else
#endif
	{
		uh_file_request(cl, req, pin);
	}
}

static void uh_mainloop(struct config *conf, fd_set serv_fds, int max_fd)
{
	/* master file descriptor list */
	fd_set used_fds, read_fds;

	/* working structs */
	struct http_request *req;
	struct path_info *pin;
	struct client *cl;

	/* maximum file descriptor number */
	int new_fd, cur_fd = 0;

	/* clear the master and temp sets */
	FD_ZERO(&used_fds);
	FD_ZERO(&read_fds);

	/* backup server descriptor set */
	used_fds = serv_fds;

	/* loop */
	while(run)
	{
		/* create a working copy of the used fd set */
		read_fds = used_fds;

		/* sleep until socket activity */
		if( select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1 )
		{
			perror("select()");
			exit(1);
		}

		/* run through the existing connections looking for data to be read */
		for( cur_fd = 0; cur_fd <= max_fd; cur_fd++ )
		{
			/* is a socket managed by us */
			if( FD_ISSET(cur_fd, &read_fds) )
			{
				/* is one of our listen sockets */
				if( FD_ISSET(cur_fd, &serv_fds) )
				{
					/* handle new connections */
					if( (new_fd = accept(cur_fd, NULL, 0)) != -1 )
					{
						/* add to global client list */
						if( (cl = uh_client_add(new_fd, uh_listener_lookup(cur_fd))) != NULL )
						{
#ifdef HAVE_TLS
							/* setup client tls context */
							if( conf->tls )
							{
								if( conf->tls_accept(cl) < 1 )
								{
									fprintf(stderr,
										"tls_accept failed, "
										"connection dropped\n");

									/* close client socket */
									close(new_fd);

									/* remove from global client list */
									uh_client_remove(new_fd);

									continue;
								}
							}
#endif

							/* add client socket to global fdset */
							FD_SET(new_fd, &used_fds);
							fd_cloexec(new_fd);
							max_fd = max(max_fd, new_fd);
						}

						/* insufficient resources */
						else
						{
							fprintf(stderr,
								"uh_client_add(): Cannot allocate memory\n");

							close(new_fd);
						}
					}
				}

				/* is a client socket */
				else
				{
					if( ! (cl = uh_client_lookup(cur_fd)) )
					{
						/* this should not happen! */
						fprintf(stderr,
							"uh_client_lookup(): No entry for fd %i!\n",
							cur_fd);

						goto cleanup;
					}

					/* parse message header */
					if( (req = uh_http_header_recv(cl)) != NULL )
					{
						/* RFC1918 filtering required? */
						if( conf->rfc1918_filter &&
						    sa_rfc1918(&cl->peeraddr) &&
						    !sa_rfc1918(&cl->servaddr) )
						{
							uh_http_sendhf(cl, 403, "Forbidden",
								"Rejected request from RFC1918 IP "
								"to public server address");
						}
						else
#ifdef HAVE_LUA
						/* Lua request? */
						if( conf->lua_state &&
						    uh_path_match(conf->lua_prefix, req->url) )
						{
							conf->lua_request(cl, req, conf->lua_state);
						}
						else
#endif
						/* dispatch request */
						if( (pin = uh_path_lookup(cl, req->url)) != NULL )
						{
							/* auth ok? */
							if( !pin->redirected && uh_auth_check(cl, req, pin) )
								uh_dispatch_request(cl, req, pin);
						}

						/* 404 */
						else
						{
							/* Try to invoke an error handler */
							pin = uh_path_lookup(cl, conf->error_handler);

							if( pin && uh_auth_check(cl, req, pin) )
							{
								req->redirect_status = 404;
								uh_dispatch_request(cl, req, pin);
							}
							else
							{
								uh_http_sendhf(cl, 404, "Not Found",
									"No such file or directory");
							}
						}
					}

#ifdef HAVE_TLS
					/* free client tls context */
					if( conf->tls )
						conf->tls_close(cl);
#endif

					cleanup:

					/* close client socket */
					close(cur_fd);
					FD_CLR(cur_fd, &used_fds);

					/* remove from global client list */
					uh_client_remove(cur_fd);
				}
			}
		}
	}

#ifdef HAVE_LUA
	/* destroy the Lua state */
	if( conf->lua_state != NULL )
		conf->lua_close(conf->lua_state);
#endif
}

#ifdef HAVE_TLS
static inline int uh_inittls(struct config *conf)
{
	/* library handle */
	void *lib;

	/* already loaded */
	if( conf->tls != NULL )
		return 0;

	/* load TLS plugin */
	if( ! (lib = dlopen("uhttpd_tls.so", RTLD_LAZY | RTLD_GLOBAL)) )
	{
		fprintf(stderr,
			"Notice: Unable to load TLS plugin - disabling SSL support! "
			"(Reason: %s)\n", dlerror()
		);

		return 1;
	}
	else
	{
		/* resolve functions */
		if( !(conf->tls_init   = dlsym(lib, "uh_tls_ctx_init"))      ||
		    !(conf->tls_cert   = dlsym(lib, "uh_tls_ctx_cert"))      ||
		    !(conf->tls_key    = dlsym(lib, "uh_tls_ctx_key"))       ||
		    !(conf->tls_free   = dlsym(lib, "uh_tls_ctx_free"))      ||
		    !(conf->tls_accept = dlsym(lib, "uh_tls_client_accept")) ||
		    !(conf->tls_close  = dlsym(lib, "uh_tls_client_close"))  ||
		    !(conf->tls_recv   = dlsym(lib, "uh_tls_client_recv"))   ||
		    !(conf->tls_send   = dlsym(lib, "uh_tls_client_send"))
		) {
			fprintf(stderr,
				"Error: Failed to lookup required symbols "
				"in TLS plugin: %s\n", dlerror()
			);
			exit(1);
		}

		/* init SSL context */
		if( ! (conf->tls = conf->tls_init()) )
		{
			fprintf(stderr, "Error: Failed to initalize SSL context\n");
			exit(1);
		}
	}

	return 0;
}
#endif

int main (int argc, char **argv)
{
	/* master file descriptor list */
	fd_set serv_fds;

	/* working structs */
	struct addrinfo hints;
	struct sigaction sa;
	struct config conf;

	/* signal mask */
	sigset_t ss;

	/* maximum file descriptor number */
	int cur_fd, max_fd = 0;

#ifdef HAVE_TLS
	int tls = 0;
	int keys = 0;
#endif

	int bound = 0;
	int nofork = 0;

	/* args */
	int opt;
	char bind[128];
	char *port = NULL;

#ifdef HAVE_LUA
	/* library handle */
	void *lib;
#endif

	FD_ZERO(&serv_fds);

	/* handle SIGPIPE, SIGINT, SIGTERM, SIGCHLD */
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	sa.sa_handler = uh_sigchld;
	sigaction(SIGCHLD, &sa, NULL);

	sa.sa_handler = uh_sigterm;
	sigaction(SIGINT,  &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	/* defer SIGCHLD */
	sigemptyset(&ss);
	sigaddset(&ss, SIGCHLD);
	sigprocmask(SIG_BLOCK, &ss, NULL);

	/* prepare addrinfo hints */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	/* parse args */
	memset(&conf, 0, sizeof(conf));
	memset(bind, 0, sizeof(bind));


	while( (opt = getopt(argc, argv,
		"fSDRC:K:E:I:p:s:h:c:l:L:d:r:m:x:i:t:T:A:")) > 0
	) {
		switch(opt)
		{
			/* [addr:]port */
			case 'p':
			case 's':
				if( (port = strrchr(optarg, ':')) != NULL )
				{
					if( (optarg[0] == '[') && (port > optarg) && (port[-1] == ']') )
						memcpy(bind, optarg + 1,
							min(sizeof(bind), (int)(port - optarg) - 2));
					else
						memcpy(bind, optarg,
							min(sizeof(bind), (int)(port - optarg)));

					port++;
				}
				else
				{
					port = optarg;
				}

#ifdef HAVE_TLS
				if( opt == 's' )
				{
					if( uh_inittls(&conf) )
					{
						fprintf(stderr,
							"Notice: TLS support is disabled, "
							"ignoring '-s %s'\n", optarg
						);
						continue;
					}

					tls = 1;
				}
#endif

				/* bind sockets */
				bound += uh_socket_bind(
					&serv_fds, &max_fd, bind[0] ? bind : NULL, port,
					&hints,	(opt == 's'), &conf
				);

				memset(bind, 0, sizeof(bind));
				break;

#ifdef HAVE_TLS
			/* certificate */
			case 'C':
				if( !uh_inittls(&conf) )
				{
					if( conf.tls_cert(conf.tls, optarg) < 1 )
					{
						fprintf(stderr,
							"Error: Invalid certificate file given\n");
						exit(1);
					}

					keys++;
				}

				break;

			/* key */
			case 'K':
				if( !uh_inittls(&conf) )
				{
					if( conf.tls_key(conf.tls, optarg) < 1 )
					{
						fprintf(stderr,
							"Error: Invalid private key file given\n");
						exit(1);
					}

					keys++;
				}

				break;
#endif

			/* docroot */
			case 'h':
				if( ! realpath(optarg, conf.docroot) )
				{
					fprintf(stderr, "Error: Invalid directory %s: %s\n",
						optarg, strerror(errno));
					exit(1);
				}
				break;

			/* error handler */
			case 'E':
				if( (strlen(optarg) == 0) || (optarg[0] != '/') )
				{
					fprintf(stderr, "Error: Invalid error handler: %s\n",
						optarg);
					exit(1);
				}
				conf.error_handler = optarg;
				break;

			/* index file */
			case 'I':
				if( (strlen(optarg) == 0) || (optarg[0] == '/') )
				{
					fprintf(stderr, "Error: Invalid index page: %s\n",
						optarg);
					exit(1);
				}
				conf.index_file = optarg;
				break;

			/* don't follow symlinks */
			case 'S':
				conf.no_symlinks = 1;
				break;

			/* don't list directories */
			case 'D':
				conf.no_dirlists = 1;
				break;

			case 'R':
				conf.rfc1918_filter = 1;
				break;

#ifdef HAVE_CGI
			/* cgi prefix */
			case 'x':
				conf.cgi_prefix = optarg;
				break;

			/* interpreter */
			case 'i':
				if( (optarg[0] == '.') && (port = strchr(optarg, '=')) )
				{
					*port++ = 0;
					uh_interpreter_add(optarg, port);
				}
				else
				{
					fprintf(stderr, "Error: Invalid interpreter: %s\n",
						optarg);
					exit(1);
				}
				break;
#endif

#ifdef HAVE_LUA
			/* lua prefix */
			case 'l':
				conf.lua_prefix = optarg;
				break;

			/* lua handler */
			case 'L':
				conf.lua_handler = optarg;
				break;
#endif

#if defined(HAVE_CGI) || defined(HAVE_LUA)
			/* script timeout */
			case 't':
				conf.script_timeout = atoi(optarg);
				break;
#endif

			/* network timeout */
			case 'T':
				conf.network_timeout = atoi(optarg);
				break;

			/* tcp keep-alive */
			case 'A':
				conf.tcp_keepalive = atoi(optarg);
				break;

			/* no fork */
			case 'f':
				nofork = 1;
				break;

			/* urldecode */
			case 'd':
				if( (port = malloc(strlen(optarg)+1)) != NULL )
				{
					/* "decode" plus to space to retain compat */
					for (opt = 0; optarg[opt]; opt++)
						if (optarg[opt] == '+')
							optarg[opt] = ' ';

					memset(port, 0, strlen(optarg)+1);
					uh_urldecode(port, strlen(optarg), optarg, strlen(optarg));

					printf("%s", port);
					free(port);
					exit(0);
				}
				break;

			/* basic auth realm */
			case 'r':
				conf.realm = optarg;
				break;

			/* md5 crypt */
			case 'm':
				printf("%s\n", crypt(optarg, "$1$"));
				exit(0);
				break;

			/* config file */
			case 'c':
				conf.file = optarg;
				break;

			default:
				fprintf(stderr,
					"Usage: %s -p [addr:]port [-h docroot]\n"
					"	-f              Do not fork to background\n"
					"	-c file         Configuration file, default is '/etc/httpd.conf'\n"
					"	-p [addr:]port  Bind to specified address and port, multiple allowed\n"
#ifdef HAVE_TLS
					"	-s [addr:]port  Like -p but provide HTTPS on this port\n"
					"	-C file         ASN.1 server certificate file\n"
					"	-K file         ASN.1 server private key file\n"
#endif
					"	-h directory    Specify the document root, default is '.'\n"
					"	-E string       Use given virtual URL as 404 error handler\n"
					"	-I string       Use given filename as index page for directories\n"
					"	-S              Do not follow symbolic links outside of the docroot\n"
					"	-D              Do not allow directory listings, send 403 instead\n"
					"	-R              Enable RFC1918 filter\n"
#ifdef HAVE_LUA
					"	-l string       URL prefix for Lua handler, default is '/lua'\n"
					"	-L file         Lua handler script, omit to disable Lua\n"
#endif
#ifdef HAVE_CGI
					"	-x string       URL prefix for CGI handler, default is '/cgi-bin'\n"
					"	-i .ext=path    Use interpreter at path for files with the given extension\n"
#endif
#if defined(HAVE_CGI) || defined(HAVE_LUA)
					"	-t seconds      CGI and Lua script timeout in seconds, default is 60\n"
#endif
					"	-T seconds      Network timeout in seconds, default is 30\n"
					"	-d string       URL decode given string\n"
					"	-r string       Specify basic auth realm\n"
					"	-m string       MD5 crypt given string\n"
					"\n", argv[0]
				);

				exit(1);
		}
	}

#ifdef HAVE_TLS
	if( (tls == 1) && (keys < 2) )
	{
		fprintf(stderr, "Error: Missing private key or certificate file\n");
		exit(1);
	}
#endif

	if( bound < 1 )
	{
		fprintf(stderr, "Error: No sockets bound, unable to continue\n");
		exit(1);
	}

	/* default docroot */
	if( !conf.docroot[0] && !realpath(".", conf.docroot) )
	{
		fprintf(stderr, "Error: Can not determine default document root: %s\n",
			strerror(errno));
		exit(1);
	}

	/* default realm */
	if( ! conf.realm )
		conf.realm = "Protected Area";

	/* config file */
	uh_config_parse(&conf);

	/* default network timeout */
	if( conf.network_timeout <= 0 )
		conf.network_timeout = 30;

#if defined(HAVE_CGI) || defined(HAVE_LUA)
	/* default script timeout */
	if( conf.script_timeout <= 0 )
		conf.script_timeout = 60;
#endif

#ifdef HAVE_CGI
	/* default cgi prefix */
	if( ! conf.cgi_prefix )
		conf.cgi_prefix = "/cgi-bin";
#endif

#ifdef HAVE_LUA
	/* load Lua plugin */
	if( ! (lib = dlopen("uhttpd_lua.so", RTLD_LAZY | RTLD_GLOBAL)) )
	{
		fprintf(stderr,
			"Notice: Unable to load Lua plugin - disabling Lua support! "
			"(Reason: %s)\n", dlerror()
		);
	}
	else
	{
		/* resolve functions */
		if( !(conf.lua_init    = dlsym(lib, "uh_lua_init"))    ||
		    !(conf.lua_close   = dlsym(lib, "uh_lua_close"))   ||
		    !(conf.lua_request = dlsym(lib, "uh_lua_request"))
		) {
			fprintf(stderr,
				"Error: Failed to lookup required symbols "
				"in Lua plugin: %s\n", dlerror()
			);
			exit(1);
		}

		/* init Lua runtime if handler is specified */
		if( conf.lua_handler )
		{
			/* default lua prefix */
			if( ! conf.lua_prefix )
				conf.lua_prefix = "/lua";

			conf.lua_state = conf.lua_init(conf.lua_handler);
		}
	}
#endif

	/* fork (if not disabled) */
	if( ! nofork )
	{
		switch( fork() )
		{
			case -1:
				perror("fork()");
				exit(1);

			case 0:
				/* daemon setup */
				if( chdir("/") )
					perror("chdir()");

				if( (cur_fd = open("/dev/null", O_WRONLY)) > -1 )
					dup2(cur_fd, 0);

				if( (cur_fd = open("/dev/null", O_RDONLY)) > -1 )
					dup2(cur_fd, 1);

				if( (cur_fd = open("/dev/null", O_RDONLY)) > -1 )
					dup2(cur_fd, 2);

				break;

			default:
				exit(0);
		}
	}

	/* server main loop */
	uh_mainloop(&conf, serv_fds, max_fd);

#ifdef HAVE_LUA
	/* destroy the Lua state */
	if( conf.lua_state != NULL )
		conf.lua_close(conf.lua_state);
#endif

	return 0;
}
