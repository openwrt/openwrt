/*
 * uhttpd - Tiny single-threaded httpd - Lua handler
 *
 *   Copyright (C) 2010-2012 Jo-Philipp Wich <xm@subsignal.org>
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

	int to = 1;
	int fd = fileno(stdin);
	int rlen = 0;

	length = luaL_checknumber(L, 1);

	if ((length > 0) && (length <= sizeof(buffer)))
	{
		/* receive data */
		rlen = uh_raw_recv(fd, buffer, length, to);

		/* data read */
		if (rlen > 0)
		{
			lua_pushnumber(L, rlen);
			lua_pushlstring(L, buffer, rlen);
			return 2;
		}

		/* eof */
		else if (rlen == 0)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		/* no, timeout and actually no data */
		else
		{
			lua_pushnumber(L, -1);
			return 1;
		}
	}

	/* parameter error */
	lua_pushnumber(L, -2);
	return 1;
}

static int uh_lua_send_common(lua_State *L, int chunked)
{
	size_t length;

	char chunk[16];
	const char *buffer;

	int rv;
	int to = 1;
	int fd = fileno(stdout);
	int slen = 0;

	buffer = luaL_checklstring(L, 1, &length);

	if (chunked)
	{
		if (length > 0)
		{
			snprintf(chunk, sizeof(chunk), "%X\r\n", length);

			ensure_out(rv = uh_raw_send(fd, chunk, strlen(chunk), to));
			slen += rv;

			ensure_out(rv = uh_raw_send(fd, buffer, length, to));
			slen += rv;

			ensure_out(rv = uh_raw_send(fd, "\r\n", 2, to));
			slen += rv;
		}
		else
		{
			slen = uh_raw_send(fd, "0\r\n\r\n", 5, to);
		}
	}
	else
	{
		slen = uh_raw_send(fd, buffer, length, to);
	}

out:
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

static int uh_lua_str2str(lua_State *L, int (*xlate_func) (char *, int, const char *, int))
{
	size_t inlen;
	int outlen;
	const char *inbuf;
	char outbuf[UH_LIMIT_MSGHEAD];

	inbuf = luaL_checklstring(L, 1, &inlen);
	outlen = (* xlate_func)(outbuf, sizeof(outbuf), inbuf, inlen);
	if (outlen < 0)
		luaL_error(L, "%s on URL-encode codec",
				   (outlen==-1) ? "buffer overflow" : "malformed string");

	lua_pushlstring(L, outbuf, outlen);
	return 1;
}

static int uh_lua_urldecode(lua_State *L)
{
	return uh_lua_str2str( L, uh_urldecode );
}


static int uh_lua_urlencode(lua_State *L)
{
	return uh_lua_str2str( L, uh_urlencode );
}


lua_State * uh_lua_init(const struct config *conf)
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

	lua_pushcfunction(L, uh_lua_urlencode);
	lua_setfield(L, -2, "urlencode");

	/* Pass the document-root to the Lua handler by placing it in
	** uhttpd.docroot.  It could alternatively be placed in env.DOCUMENT_ROOT
	** which would more closely resemble the CGI protocol; but would mean that
	** it is not available at the time when the handler-chunk is loaded but
	** rather not until the handler is called, without any code savings. */
	lua_pushstring(L, conf->docroot);
	lua_setfield(L, -2, "docroot");

	/* _G.uhttpd = { ... } */
	lua_setfield(L, LUA_GLOBALSINDEX, "uhttpd");


	/* load Lua handler */
	switch (luaL_loadfile(L, conf->lua_handler))
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
			switch (lua_pcall(L, 0, 0, 0))
			{
				case LUA_ERRRUN:
					err_str = luaL_checkstring(L, -1);
					fprintf(stderr,
							"Lua handler had runtime error, "
							"unable to continue\n"
							"Error: %s\n", err_str);
					exit(1);

				case LUA_ERRMEM:
					err_str = luaL_checkstring(L, -1);
					fprintf(stderr,
							"Lua handler ran out of memory, "
							"unable to continue\n"
							"Error: %s\n", err_str);
					exit(1);

				default:
					/* test handler function */
					lua_getglobal(L, UH_LUA_CALLBACK);

					if (! lua_isfunction(L, -1))
					{
						fprintf(stderr,
								"Lua handler provides no "UH_LUA_CALLBACK"(), "
								"unable to continue\n");
						exit(1);
					}

					lua_pop(L, 1);
					break;
			}

			break;
	}

	return L;
}

static void uh_lua_shutdown(struct uh_lua_state *state)
{
	close(state->rfd);
	close(state->wfd);
	free(state);
}

static bool uh_lua_socket_cb(struct client *cl)
{
	int len;
	char buf[UH_LIMIT_MSGHEAD];

	struct uh_lua_state *state = (struct uh_lua_state *)cl->priv;

	/* there is unread post data waiting */
	while (state->content_length > 0)
	{
		/* remaining data in http head buffer ... */
		if (state->cl->httpbuf.len > 0)
		{
			len = min(state->content_length, state->cl->httpbuf.len);

			D("Lua: Child(%d) feed %d HTTP buffer bytes\n",
			  state->cl->proc.pid, len);

			memcpy(buf, state->cl->httpbuf.ptr, len);

			state->cl->httpbuf.len -= len;
			state->cl->httpbuf.ptr += len;
		}

		/* read it from socket ... */
		else
		{
			len = uh_tcp_recv(state->cl, buf,
							  min(state->content_length, sizeof(buf)));

			if ((len < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
				break;

			D("Lua: Child(%d) feed %d/%d TCP socket bytes\n",
			  state->cl->proc.pid, len,
			  min(state->content_length, sizeof(buf)));
		}

		if (len)
			state->content_length -= len;
		else
			state->content_length = 0;

		/* ... write to Lua process */
		len = uh_raw_send(state->wfd, buf, len,
						  cl->server->conf->script_timeout);

		/* explicit EOF notification for the child */
		if (state->content_length <= 0)
			close(state->wfd);
	}

	/* try to read data from child */
	while ((len = uh_raw_recv(state->rfd, buf, sizeof(buf), -1)) > 0)
	{
		/* pass through buffer to socket */
		D("Lua: Child(%d) relaying %d normal bytes\n", state->cl->proc.pid, len);
		ensure_out(uh_tcp_send(state->cl, buf, len));
		state->data_sent = true;
	}

	/* child has been marked dead by timeout or child handler, bail out */
	if (false && cl->dead)
	{
		D("Lua: Child(%d) is marked dead, returning\n", state->cl->proc.pid);
		goto out;
	}

	if ((len == 0) ||
		((errno != EAGAIN) && (errno != EWOULDBLOCK) && (len == -1)))
	{
		D("Lua: Child(%d) presumed dead [%s]\n",
		  state->cl->proc.pid, strerror(errno));

		goto out;
	}

	return true;

out:
	if (!state->data_sent)
	{
		if (state->cl->timeout.pending)
			uh_http_sendhf(state->cl, 502, "Bad Gateway",
						   "The Lua process did not produce any response\n");
		else
			uh_http_sendhf(state->cl, 504, "Gateway Timeout",
						   "The Lua process took too long to produce a "
						   "response\n");
	}

	uh_lua_shutdown(state);
	return false;
}

bool uh_lua_request(struct client *cl, lua_State *L)
{
	int i;
	char *query_string;
	const char *prefix = cl->server->conf->lua_prefix;
	const char *err_str = NULL;

	int rfd[2] = { 0, 0 };
	int wfd[2] = { 0, 0 };

	pid_t child;

	struct uh_lua_state *state;
	struct http_request *req = &cl->request;

	int content_length = cl->httpbuf.len;


	/* allocate state */
	if (!(state = malloc(sizeof(*state))))
	{
		uh_client_error(cl, 500, "Internal Server Error", "Out of memory");
		return false;
	}

	/* spawn pipes for me->child, child->me */
	if ((pipe(rfd) < 0) || (pipe(wfd) < 0))
	{
		if (rfd[0] > 0) close(rfd[0]);
		if (rfd[1] > 0) close(rfd[1]);
		if (wfd[0] > 0) close(wfd[0]);
		if (wfd[1] > 0) close(wfd[1]);

		uh_client_error(cl, 500, "Internal Server Error",
						"Failed to create pipe: %s", strerror(errno));

		return false;
	}


	switch ((child = fork()))
	{
	case -1:
		uh_client_error(cl, 500, "Internal Server Error",
						"Failed to fork child: %s", strerror(errno));

		return false;

	case 0:
#ifdef DEBUG
		sleep(atoi(getenv("UHTTPD_SLEEP_ON_FORK") ?: "0"));
#endif

		/* close loose pipe ends */
		close(rfd[0]);
		close(wfd[1]);

		/* patch stdout and stdin to pipes */
		dup2(rfd[1], 1);
		dup2(wfd[0], 0);

		/* avoid leaking our pipe into child-child processes */
		fd_cloexec(rfd[1]);
		fd_cloexec(wfd[0]);

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
		if ((query_string = strchr(req->url, '?')) != NULL)
		{
			lua_pushstring(L, query_string + 1);
			lua_setfield(L, -2, "QUERY_STRING");

			if ((int)(query_string - req->url) > strlen(prefix))
			{
				lua_pushlstring(L,
					&req->url[strlen(prefix)],
					(int)(query_string - req->url) - strlen(prefix)
				);

				lua_setfield(L, -2, "PATH_INFO");
			}
		}
		else if (strlen(req->url) > strlen(prefix))
		{
			lua_pushstring(L, &req->url[strlen(prefix)]);
			lua_setfield(L, -2, "PATH_INFO");
		}

		/* http protcol version */
		lua_pushnumber(L, floor(req->version * 10) / 10);
		lua_setfield(L, -2, "HTTP_VERSION");

		if (req->version > 1.0)
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
			if (!strcasecmp(req->headers[i], "Content-Length"))
			{
				content_length = atoi(req->headers[i+1]);
			}
			else if (!strcasecmp(req->headers[i], "Content-Type"))
			{
				lua_pushstring(L, req->headers[i+1]);
				lua_setfield(L, -2, "CONTENT_TYPE");
			}
		}

		lua_pushnumber(L, content_length);
		lua_setfield(L, -2, "CONTENT_LENGTH");

		/* misc. headers */
		lua_newtable(L);

		foreach_header(i, req->headers)
		{
			if( strcasecmp(req->headers[i], "Content-Length") &&
				strcasecmp(req->headers[i], "Content-Type"))
			{
				lua_pushstring(L, req->headers[i+1]);
				lua_setfield(L, -2, req->headers[i]);
			}
		}

		lua_setfield(L, -2, "headers");


		/* call */
		switch (lua_pcall(L, 1, 0, 0))
		{
			case LUA_ERRMEM:
			case LUA_ERRRUN:
				err_str = luaL_checkstring(L, -1);

				if (! err_str)
					err_str = "Unknown error";

				printf("HTTP/%.1f 500 Internal Server Error\r\n"
					   "Connection: close\r\n"
					   "Content-Type: text/plain\r\n"
					   "Content-Length: %i\r\n\r\n"
					   "Lua raised a runtime error:\n  %s\n",
					   req->version, 31 + strlen(err_str), err_str);

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
		memset(state, 0, sizeof(*state));

		state->cl = cl;
		state->cl->proc.pid = child;

		/* close unneeded pipe ends */
		close(rfd[1]);
		close(wfd[0]);

		D("Lua: Child(%d) created: rfd(%d) wfd(%d)\n", child, rfd[0], wfd[1]);

		state->content_length = cl->httpbuf.len;

		/* find content length */
		if (req->method == UH_HTTP_MSG_POST)
		{
			foreach_header(i, req->headers)
			{
				if (!strcasecmp(req->headers[i], "Content-Length"))
				{
					state->content_length = atoi(req->headers[i+1]);
					break;
				}
			}
		}

		state->rfd = rfd[0];
		fd_nonblock(state->rfd);

		state->wfd = wfd[1];
		fd_nonblock(state->wfd);

		cl->cb = uh_lua_socket_cb;
		cl->priv = state;

		break;
	}

	return true;
}

void uh_lua_close(lua_State *L)
{
	lua_close(L);
}
