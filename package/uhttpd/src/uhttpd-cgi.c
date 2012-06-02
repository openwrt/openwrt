/*
 * uhttpd - Tiny single-threaded httpd - CGI handler
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
#include "uhttpd-cgi.h"


static bool
uh_cgi_header_parse(struct http_response *res, char *buf, int len, int *off)
{
	char *bufptr = NULL;
	char *hdrname = NULL;
	int hdrcount = 0;
	int pos = 0;

	if (((bufptr = strfind(buf, len, "\r\n\r\n", 4)) != NULL) ||
	    ((bufptr = strfind(buf, len, "\n\n", 2)) != NULL))
	{
		*off = (int)(bufptr - buf) + ((bufptr[0] == '\r') ? 4 : 2);

		memset(res, 0, sizeof(*res));

		res->statuscode = 200;
		res->statusmsg  = "OK";

		bufptr = &buf[0];

		for (pos = 0; pos < *off; pos++)
		{
			if (!hdrname && (buf[pos] == ':'))
			{
				buf[pos++] = 0;

				if ((pos < len) && (buf[pos] == ' '))
					pos++;

				if (pos < len)
				{
					hdrname = bufptr;
					bufptr = &buf[pos];
				}
			}

			else if ((buf[pos] == '\r') || (buf[pos] == '\n'))
			{
				if (! hdrname)
					break;

				buf[pos++] = 0;

				if ((pos < len) && (buf[pos] == '\n'))
					pos++;

				if (pos <= len)
				{
					if ((hdrcount+1) < array_size(res->headers))
					{
						if (!strcasecmp(hdrname, "Status"))
						{
							res->statuscode = atoi(bufptr);

							if (res->statuscode < 100)
								res->statuscode = 200;

							if (((bufptr = strchr(bufptr, ' ')) != NULL) &&
								(&bufptr[1] != 0))
							{
								res->statusmsg = &bufptr[1];
							}

							D("CGI: HTTP/1.x %03d %s\n",
							  res->statuscode, res->statusmsg);
						}
						else
						{
							D("CGI: HTTP: %s: %s\n", hdrname, bufptr);

							res->headers[hdrcount++] = hdrname;
							res->headers[hdrcount++] = bufptr;
						}

						bufptr = &buf[pos];
						hdrname = NULL;
					}
					else
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	return false;
}

static char * uh_cgi_header_lookup(struct http_response *res,
								   const char *hdrname)
{
	int i;

	foreach_header(i, res->headers)
	{
		if (!strcasecmp(res->headers[i], hdrname))
			return res->headers[i+1];
	}

	return NULL;
}

static void uh_cgi_shutdown(struct uh_cgi_state *state)
{
	close(state->rfd);
	close(state->wfd);
	free(state);
}

static bool uh_cgi_socket_cb(struct client *cl)
{
	int i, len, hdroff;
	char buf[UH_LIMIT_MSGHEAD];

	struct uh_cgi_state *state = (struct uh_cgi_state *)cl->priv;
	struct http_response *res = &state->cl->response;
	struct http_request *req = &state->cl->request;

	/* there is unread post data waiting */
	while (state->content_length > 0)
	{
		/* remaining data in http head buffer ... */
		if (state->cl->httpbuf.len > 0)
		{
			len = min(state->content_length, state->cl->httpbuf.len);

			D("CGI: Child(%d) feed %d HTTP buffer bytes\n",
			  state->cl->proc.pid, len);

			memcpy(buf, state->cl->httpbuf.ptr, len);

			state->cl->httpbuf.len -= len;
			state->cl->httpbuf.ptr +=len;
		}

		/* read it from socket ... */
		else
		{
			len = uh_tcp_recv(state->cl, buf,
							  min(state->content_length, sizeof(buf)));

			if ((len < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
				break;

			D("CGI: Child(%d) feed %d/%d TCP socket bytes\n",
			  state->cl->proc.pid, len,
			  min(state->content_length, sizeof(buf)));
		}

		if (len)
			state->content_length -= len;
		else
			state->content_length = 0;

		/* ... write to CGI process */
		len = uh_raw_send(state->wfd, buf, len,
						  cl->server->conf->script_timeout);

		/* explicit EOF notification for the child */
		if (state->content_length <= 0)
			close(state->wfd);
	}

	/* try to read data from child */
	while ((len = uh_raw_recv(state->rfd, buf, sizeof(buf), -1)) > 0)
	{
		/* we have not pushed out headers yet, parse input */
		if (!state->header_sent)
		{
			/* try to parse header ... */
			memcpy(state->httpbuf, buf, len);

			if (uh_cgi_header_parse(res, state->httpbuf, len, &hdroff))
			{
				/* write status */
				ensure_out(uh_http_sendf(state->cl, NULL,
					"HTTP/%.1f %03d %s\r\n"
					"Connection: close\r\n",
					req->version, res->statuscode, res->statusmsg));

				/* add Content-Type if no Location or Content-Type */
				if (!uh_cgi_header_lookup(res, "Location") &&
					!uh_cgi_header_lookup(res, "Content-Type"))
				{
					ensure_out(uh_http_send(state->cl, NULL,
						"Content-Type: text/plain\r\n", -1));
				}

				/* if request was HTTP 1.1 we'll respond chunked */
				if ((req->version > 1.0) &&
					!uh_cgi_header_lookup(res, "Transfer-Encoding"))
				{
					ensure_out(uh_http_send(state->cl, NULL,
						"Transfer-Encoding: chunked\r\n", -1));
				}

				/* write headers from CGI program */
				foreach_header(i, res->headers)
				{
					ensure_out(uh_http_sendf(state->cl, NULL, "%s: %s\r\n",
						res->headers[i], res->headers[i+1]));
				}

				/* terminate header */
				ensure_out(uh_http_send(state->cl, NULL, "\r\n", -1));

				state->header_sent = true;

				/* push out remaining head buffer */
				if (hdroff < len)
				{
					D("CGI: Child(%d) relaying %d rest bytes\n",
					  state->cl->proc.pid, len - hdroff);

					ensure_out(uh_http_send(state->cl, req,
											&buf[hdroff], len - hdroff));
				}
			}

			/* ... failed and head buffer exceeded */
			else
			{
				/* I would do this ...
				 *
				 *    uh_cgi_error_500(cl, req,
				 *        "The CGI program generated an "
				 *        "invalid response:\n\n");
				 *
				 * ... but in order to stay as compatible as possible,
				 * treat whatever we got as text/plain response and
				 * build the required headers here.
				 */

				ensure_out(uh_http_sendf(state->cl, NULL,
										 "HTTP/%.1f 200 OK\r\n"
										 "Content-Type: text/plain\r\n"
										 "%s\r\n",
										 req->version, (req->version > 1.0)
										 ? "Transfer-Encoding: chunked\r\n" : ""
				));

				state->header_sent = true;

				D("CGI: Child(%d) relaying %d invalid bytes\n",
				  state->cl->proc.pid, len);

				ensure_out(uh_http_send(state->cl, req, buf, len));
			}
		}
		else
		{
			/* headers complete, pass through buffer to socket */
			D("CGI: Child(%d) relaying %d normal bytes\n",
			  state->cl->proc.pid, len);

			ensure_out(uh_http_send(state->cl, req, buf, len));
		}
	}

	/* got EOF or read error from child */
	if ((len == 0) ||
		((errno != EAGAIN) && (errno != EWOULDBLOCK) && (len == -1)))
	{
		D("CGI: Child(%d) presumed dead [%s]\n",
		  state->cl->proc.pid, strerror(errno));

		goto out;
	}

	return true;

out:
	if (!state->header_sent)
	{
		if (state->cl->timeout.pending)
			uh_http_sendhf(state->cl, 502, "Bad Gateway",
						   "The CGI process did not produce any response\n");
		else
			uh_http_sendhf(state->cl, 504, "Gateway Timeout",
						   "The CGI process took too long to produce a "
						   "response\n");
	}
	else
	{
		uh_http_send(state->cl, req, "", 0);
	}

	uh_cgi_shutdown(state);
	return false;
}

bool uh_cgi_request(struct client *cl, struct path_info *pi,
					struct interpreter *ip)
{
	int i;

	int rfd[2] = { 0, 0 };
	int wfd[2] = { 0, 0 };

	pid_t child;

	struct uh_cgi_state *state;
	struct http_request *req = &cl->request;

	/* allocate state */
	if (!(state = malloc(sizeof(*state))))
	{
		uh_http_sendhf(cl, 500, "Internal Server Error", "Out of memory");
		return false;
	}

	/* spawn pipes for me->child, child->me */
	if ((pipe(rfd) < 0) || (pipe(wfd) < 0))
	{
		if (rfd[0] > 0) close(rfd[0]);
		if (rfd[1] > 0) close(rfd[1]);
		if (wfd[0] > 0) close(wfd[0]);
		if (wfd[1] > 0) close(wfd[1]);

		uh_http_sendhf(cl, 500, "Internal Server Error",
						"Failed to create pipe: %s\n", strerror(errno));

		return false;
	}

	/* fork off child process */
	switch ((child = fork()))
	{
	/* oops */
	case -1:
		uh_http_sendhf(cl, 500, "Internal Server Error",
						"Failed to fork child: %s\n", strerror(errno));

		return false;

	/* exec child */
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

		/* check for regular, world-executable file _or_ interpreter */
		if (((pi->stat.st_mode & S_IFREG) &&
			 (pi->stat.st_mode & S_IXOTH)) || (ip != NULL))
		{
			/* build environment */
			clearenv();

			/* common information */
			setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
			setenv("SERVER_SOFTWARE", "uHTTPd", 1);
			setenv("PATH", "/sbin:/usr/sbin:/bin:/usr/bin", 1);

#ifdef HAVE_TLS
			/* https? */
			if (cl->tls)
				setenv("HTTPS", "on", 1);
#endif

			/* addresses */
			setenv("SERVER_NAME", sa_straddr(&cl->servaddr), 1);
			setenv("SERVER_ADDR", sa_straddr(&cl->servaddr), 1);
			setenv("SERVER_PORT", sa_strport(&cl->servaddr), 1);
			setenv("REMOTE_HOST", sa_straddr(&cl->peeraddr), 1);
			setenv("REMOTE_ADDR", sa_straddr(&cl->peeraddr), 1);
			setenv("REMOTE_PORT", sa_strport(&cl->peeraddr), 1);

			/* path information */
			setenv("SCRIPT_NAME", pi->name, 1);
			setenv("SCRIPT_FILENAME", pi->phys, 1);
			setenv("DOCUMENT_ROOT", pi->root, 1);
			setenv("QUERY_STRING", pi->query ? pi->query : "", 1);

			if (pi->info)
				setenv("PATH_INFO", pi->info, 1);

			/* REDIRECT_STATUS, php-cgi wants it */
			switch (req->redirect_status)
			{
				case 404:
					setenv("REDIRECT_STATUS", "404", 1);
					break;

				default:
					setenv("REDIRECT_STATUS", "200", 1);
					break;
			}

			/* http version */
			if (req->version > 1.0)
				setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
			else
				setenv("SERVER_PROTOCOL", "HTTP/1.0", 1);

			/* request method */
			switch (req->method)
			{
				case UH_HTTP_MSG_GET:
					setenv("REQUEST_METHOD", "GET", 1);
					break;

				case UH_HTTP_MSG_HEAD:
					setenv("REQUEST_METHOD", "HEAD", 1);
					break;

				case UH_HTTP_MSG_POST:
					setenv("REQUEST_METHOD", "POST", 1);
					break;
			}

			/* request url */
			setenv("REQUEST_URI", req->url, 1);

			/* remote user */
			if (req->realm)
				setenv("REMOTE_USER", req->realm->user, 1);

			/* request message headers */
			foreach_header(i, req->headers)
			{
				if (!strcasecmp(req->headers[i], "Accept"))
					setenv("HTTP_ACCEPT", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Accept-Charset"))
					setenv("HTTP_ACCEPT_CHARSET", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Accept-Encoding"))
					setenv("HTTP_ACCEPT_ENCODING", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Accept-Language"))
					setenv("HTTP_ACCEPT_LANGUAGE", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Authorization"))
					setenv("HTTP_AUTHORIZATION", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Connection"))
					setenv("HTTP_CONNECTION", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Cookie"))
					setenv("HTTP_COOKIE", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Host"))
					setenv("HTTP_HOST", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Referer"))
					setenv("HTTP_REFERER", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "User-Agent"))
					setenv("HTTP_USER_AGENT", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Content-Type"))
					setenv("CONTENT_TYPE", req->headers[i+1], 1);

				else if (!strcasecmp(req->headers[i], "Content-Length"))
					setenv("CONTENT_LENGTH", req->headers[i+1], 1);
			}


			/* execute child code ... */
			if (chdir(pi->root))
				perror("chdir()");

			if (ip != NULL)
				execl(ip->path, ip->path, pi->phys, NULL);
			else
				execl(pi->phys, pi->phys, NULL);

			/* in case it fails ... */
			printf("Status: 500 Internal Server Error\r\n\r\n"
				   "Unable to launch the requested CGI program:\n"
				   "  %s: %s\n", ip ? ip->path : pi->phys, strerror(errno));
		}

		/* 403 */
		else
		{
			printf("Status: 403 Forbidden\r\n\r\n"
				   "Access to this resource is forbidden\n");
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

		D("CGI: Child(%d) created: rfd(%d) wfd(%d)\n", child, rfd[0], wfd[1]);

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

		cl->cb = uh_cgi_socket_cb;
		cl->priv = state;

		break;
	}

	return true;
}
