/*
 * uhttpd - Tiny single-threaded httpd - Lua handler
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

#include "uhttpd.h"
#include "uhttpd-utils.h"
#include "uhttpd-lua.h"


static int uh_lua_recv(lua_State *L)
{
	size_t length;
	char buffer[UH_LIMIT_MSGHEAD];
	ssize_t rlen = 0;
	fd_set reader;
	struct timeval timeout;

	length = luaL_checknumber(L, 1);

	if( (length > 0) && (length <= sizeof(buffer)) )
	{
		FD_ZERO(&reader);
		FD_SET(fileno(stdin), &reader);

		/* fail after 0.1s */
		timeout.tv_sec  = 0;
		timeout.tv_usec = 100000;

		/* check whether fd is readable */
		if( select(fileno(stdin) + 1, &reader, NULL, NULL, &timeout) > 0 )
		{
			/* receive data */
			rlen = read(fileno(stdin), buffer, length);
			lua_pushnumber(L, rlen);

			if( rlen > 0 )
			{
				lua_pushlstring(L, buffer, rlen);
				return 2;
			}

			return 1;
		}

		/* no, timeout and actually no data */
		lua_pushnumber(L, -2);
		return 1;
	}

	/* parameter error */
	lua_pushnumber(L, -3);
	return 1;
}

static int uh_lua_send_common(lua_State *L, int chunked)
{
	size_t length;
	const char *buffer;
	char chunk[16];
	ssize_t slen = 0;

	buffer = luaL_checklstring(L, 1, &length);

	if( chunked )
	{
		if( length > 0 )
		{
			snprintf(chunk, sizeof(chunk), "%X\r\n", length);
			slen =  write(fileno(stdout), chunk, strlen(chunk));
			slen += write(fileno(stdout), buffer, length);
			slen += write(fileno(stdout), "\r\n", 2);
		}
		else
		{
			slen = write(fileno(stdout), "0\r\n\r\n", 5);
		}
	}
	else
	{
		slen = write(fileno(stdout), buffer, length);
	}

	lua_pushnumber(L, slen);
	return 1;
}

static int uh_lua_send(lua_State *L)
{
	return uh_lua_send_common(L, 0);
}

static int uh_lua_sendc(lua_State *L)
{
	return uh_lua_send_common(L, 1);
}

static int uh_lua_urldecode(lua_State *L)
{
	size_t inlen, outlen;
	const char *inbuf;
	char outbuf[UH_LIMIT_MSGHEAD];

	inbuf = luaL_checklstring(L, 1, &inlen);
	outlen = uh_urldecode(outbuf, sizeof(outbuf), inbuf, inlen);

	lua_pushlstring(L, outbuf, outlen);
	return 1;
}


lua_State * uh_lua_init(const char *handler)
{
	lua_State *L = lua_open();
	const char *err_str = NULL;

	/* Load standard libaries */
	luaL_openlibs(L);

	/* build uhttpd api table */
	lua_newtable(L);

	/* register global send and receive functions */
	lua_pushcfunction(L, uh_lua_recv);
	lua_setfield(L, -2, "recv");

	lua_pushcfunction(L, uh_lua_send);
	lua_setfield(L, -2, "send");

	lua_pushcfunction(L, uh_lua_sendc);
	lua_setfield(L, -2, "sendc");

	lua_pushcfunction(L, uh_lua_urldecode);
	lua_setfield(L, -2, "urldecode");

	/* _G.uhttpd = { ... } */
	lua_setfield(L, LUA_GLOBALSINDEX, "uhttpd");


	/* load Lua handler */
	switch( luaL_loadfile(L, handler) )
	{
		case LUA_ERRSYNTAX:
			fprintf(stderr,
				"Lua handler contains syntax errors, unable to continue\n");
			exit(1);

		case LUA_ERRMEM:
			fprintf(stderr,
				"Lua handler ran out of memory, unable to continue\n");
			exit(1);

		case LUA_ERRFILE:
			fprintf(stderr,
				"Lua cannot open the handler script, unable to continue\n");
			exit(1);

		default:
			/* compile Lua handler */
			switch( lua_pcall(L, 0, 0, 0) )
			{
				case LUA_ERRRUN:
					err_str = luaL_checkstring(L, -1);
					fprintf(stderr,
						"Lua handler had runtime error, unable to continue\n"
						"Error: %s\n", err_str
					);
					exit(1);

				case LUA_ERRMEM:
					err_str = luaL_checkstring(L, -1);
					fprintf(stderr,
						"Lua handler ran out of memory, unable to continue\n"
						"Error: %s\n", err_str
					);
					exit(1);

				default:
					/* test handler function */
					lua_getglobal(L, UH_LUA_CALLBACK);

					if( ! lua_isfunction(L, -1) )
					{
						fprintf(stderr,
							"Lua handler provides no " UH_LUA_CALLBACK "(), unable to continue\n");
						exit(1);
					}

					lua_pop(L, 1);
					break;
			}

			break;
	}

	return L;
}

void uh_lua_request(struct client *cl, struct http_request *req, lua_State *L)
{
	int i, data_sent;
	int content_length = 0;
	int buflen = 0;
	int fd_max = 0;
	char *query_string;
	const char *prefix = cl->server->conf->lua_prefix;
	const char *err_str = NULL;

	int rfd[2] = { 0, 0 };
	int wfd[2] = { 0, 0 };

	char buf[UH_LIMIT_MSGHEAD];

	pid_t child;

	fd_set reader;
	fd_set writer;

	struct sigaction sa;
	struct timeval timeout;


	/* spawn pipes for me->child, child->me */
	if( (pipe(rfd) < 0) || (pipe(wfd) < 0) )
	{
		uh_http_sendhf(cl, 500, "Internal Server Error",
			"Failed to create pipe: %s", strerror(errno));

		if( rfd[0] > 0 ) close(rfd[0]);
		if( rfd[1] > 0 ) close(rfd[1]);
		if( wfd[0] > 0 ) close(wfd[0]);
		if( wfd[1] > 0 ) close(wfd[1]);

		return;
	}


	switch( (child = fork()) )
	{
		case -1:
			uh_http_sendhf(cl, 500, "Internal Server Error",
				"Failed to fork child: %s", strerror(errno));
			break;

		case 0:
			/* restore SIGTERM */
			sa.sa_flags = 0;
			sa.sa_handler = SIG_DFL;
			sigemptyset(&sa.sa_mask);
			sigaction(SIGTERM, &sa, NULL);

			/* close loose pipe ends */
			close(rfd[0]);
			close(wfd[1]);

			/* patch stdout and stdin to pipes */
			dup2(rfd[1], 1);
			dup2(wfd[0], 0);

			/* put handler callback on stack */
			lua_getglobal(L, UH_LUA_CALLBACK);

			/* build env table */
			lua_newtable(L);

			/* request method */
			switch(req->method)
			{
				case UH_HTTP_MSG_GET:
					lua_pushstring(L, "GET");
					break;

				case UH_HTTP_MSG_HEAD:
					lua_pushstring(L, "HEAD");
					break;

				case UH_HTTP_MSG_POST:
					lua_pushstring(L, "POST");
					break;
			}

			lua_setfield(L, -2, "REQUEST_METHOD");

			/* request url */
			lua_pushstring(L, req->url);
			lua_setfield(L, -2, "REQUEST_URI");

			/* script name */
			lua_pushstring(L, cl->server->conf->lua_prefix);
			lua_setfield(L, -2, "SCRIPT_NAME");

			/* query string, path info */
			if( (query_string = strchr(req->url, '?')) != NULL )
			{
				lua_pushstring(L, query_string + 1);
				lua_setfield(L, -2, "QUERY_STRING");

				if( (int)(query_string - req->url) > strlen(prefix) )
				{
					lua_pushlstring(L,
						&req->url[strlen(prefix)],
						(int)(query_string - req->url) - strlen(prefix)
					);

					lua_setfield(L, -2, "PATH_INFO");
				}
			}
			else if( strlen(req->url) > strlen(prefix) )
			{
				lua_pushstring(L, &req->url[strlen(prefix)]);
				lua_setfield(L, -2, "PATH_INFO");
			}

			/* http protcol version */
			lua_pushnumber(L, floor(req->version * 10) / 10);
			lua_setfield(L, -2, "HTTP_VERSION");

			if( req->version > 1.0 )
				lua_pushstring(L, "HTTP/1.1");
			else
				lua_pushstring(L, "HTTP/1.0");

			lua_setfield(L, -2, "SERVER_PROTOCOL");


			/* address information */
			lua_pushstring(L, sa_straddr(&cl->peeraddr));
			lua_setfield(L, -2, "REMOTE_ADDR");

			lua_pushinteger(L, sa_port(&cl->peeraddr));
			lua_setfield(L, -2, "REMOTE_PORT");

			lua_pushstring(L, sa_straddr(&cl->servaddr));
			lua_setfield(L, -2, "SERVER_ADDR");

			lua_pushinteger(L, sa_port(&cl->servaddr));
			lua_setfield(L, -2, "SERVER_PORT");

			/* essential env vars */
			foreach_header(i, req->headers)
			{
				if( !strcasecmp(req->headers[i], "Content-Length") )
				{
					lua_pushnumber(L, atoi(req->headers[i+1]));
					lua_setfield(L, -2, "CONTENT_LENGTH");
				}
				else if( !strcasecmp(req->headers[i], "Content-Type") )
				{
					lua_pushstring(L, req->headers[i+1]);
					lua_setfield(L, -2, "CONTENT_TYPE");
				}
			}

			/* misc. headers */
			lua_newtable(L);

			foreach_header(i, req->headers)
			{
				if( strcasecmp(req->headers[i], "Content-Length") &&
					strcasecmp(req->headers[i], "Content-Type")
				) {
					lua_pushstring(L, req->headers[i+1]);
					lua_setfield(L, -2, req->headers[i]);
				}
			}

			lua_setfield(L, -2, "headers");


			/* call */
			switch( lua_pcall(L, 1, 0, 0) )
			{
				case LUA_ERRMEM:
				case LUA_ERRRUN:
					err_str = luaL_checkstring(L, -1);

					if( ! err_str )
						err_str = "Unknown error";

					printf(
						"HTTP/%.1f 500 Internal Server Error\r\n"
						"Connection: close\r\n"
						"Content-Type: text/plain\r\n"
						"Content-Length: %i\r\n\r\n"
						"Lua raised a runtime error:\n  %s\n",
							req->version, 31 + strlen(err_str), err_str
					);

					break;

				default:
					break;
			}

			close(wfd[0]);
			close(rfd[1]);
			exit(0);

			break;

		/* parent; handle I/O relaying */
		default:
			/* close unneeded pipe ends */
			close(rfd[1]);
			close(wfd[0]);

			/* max watch fd */
			fd_max = max(rfd[0], wfd[1]) + 1;

			/* find content length */
			if( req->method == UH_HTTP_MSG_POST )
			{
				foreach_header(i, req->headers)
				{
					if( ! strcasecmp(req->headers[i], "Content-Length") )
					{
						content_length = atoi(req->headers[i+1]);
						break;
					}
				}
			}


#define ensure(x) \
	do { if( x < 0 ) goto out; } while(0)

			data_sent = 0;

			timeout.tv_sec = cl->server->conf->script_timeout;
			timeout.tv_usec = 0;

			/* I/O loop, watch our pipe ends and dispatch child reads/writes from/to socket */
			while( 1 )
			{
				FD_ZERO(&reader);
				FD_ZERO(&writer);

				FD_SET(rfd[0], &reader);
				FD_SET(wfd[1], &writer);

				/* wait until we can read or write or both */
				if( select_intr(fd_max, &reader,
				    (content_length > -1) ? &writer : NULL, NULL,
					(data_sent < 1) ? &timeout : NULL) > 0
				) {
					/* ready to write to Lua child */
					if( FD_ISSET(wfd[1], &writer) )
					{
						/* there is unread post data waiting */
						if( content_length > 0 )
						{
							/* read it from socket ... */
							if( (buflen = uh_tcp_recv(cl, buf, min(content_length, sizeof(buf)))) > 0 )
							{
								/* ... and write it to child's stdin */
								if( write(wfd[1], buf, buflen) < 0 )
									perror("write()");

								content_length -= buflen;
							}

							/* unexpected eof! */
							else
							{
								if( write(wfd[1], "", 0) < 0 )
									perror("write()");

								content_length = 0;
							}
						}

						/* there is no more post data, close pipe to child's stdin */
						else if( content_length > -1 )
						{
							close(wfd[1]);
							content_length = -1;
						}
					}

					/* ready to read from Lua child */
					if( FD_ISSET(rfd[0], &reader) )
					{
						/* read data from child ... */
						if( (buflen = read(rfd[0], buf, sizeof(buf))) > 0 )
						{
							/* pass through buffer to socket */
							ensure(uh_tcp_send(cl, buf, buflen));
							data_sent = 1;
						}

						/* looks like eof from child */
						else
						{
							/* error? */
							if( ! data_sent )
								uh_http_sendhf(cl, 500, "Internal Server Error",
									"The Lua child did not produce any response");

							break;
						}
					}
				}

				/* timeout exceeded or interrupted by SIGCHLD */
				else
				{
					if( (errno != EINTR) && ! data_sent )
					{
						ensure(uh_http_sendhf(cl, 504, "Gateway Timeout",
							"The Lua script took too long to produce "
							"a response"));
					}

					break;
				}
			}

		out:
			close(rfd[0]);
			close(wfd[1]);

			if( !kill(child, 0) )
			{
				kill(child, SIGTERM);
				waitpid(child, NULL, 0);
			}

			break;
	}
}

void uh_lua_close(lua_State *L)
{
	lua_close(L);
}
