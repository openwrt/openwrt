/*
 * uhttpd - Tiny single-threaded httpd - TLS helper
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
#include "uhttpd-tls.h"
#include "uhttpd-utils.h"

#include <syslog.h>
#define dbg(...) syslog(LOG_INFO, __VA_ARGS__)

SSL_CTX * uh_tls_ctx_init(void)
{
	SSL_CTX *c;

	SSL_load_error_strings();
	SSL_library_init();

#if TLS_IS_OPENSSL
	if ((c = SSL_CTX_new(SSLv23_server_method())) != NULL)
#else
	if ((c = SSL_CTX_new(TLSv1_server_method())) != NULL)
#endif
		SSL_CTX_set_verify(c, SSL_VERIFY_NONE, NULL);

	return c;
}

int uh_tls_ctx_cert(SSL_CTX *c, const char *file)
{
	int rv;

	if( (rv = SSL_CTX_use_certificate_file(c, file, SSL_FILETYPE_PEM)) < 1 )
		rv = SSL_CTX_use_certificate_file(c, file, SSL_FILETYPE_ASN1);

	return rv;
}

int uh_tls_ctx_key(SSL_CTX *c, const char *file)
{
	int rv;

	if( (rv = SSL_CTX_use_PrivateKey_file(c, file, SSL_FILETYPE_PEM)) < 1 )
		rv = SSL_CTX_use_PrivateKey_file(c, file, SSL_FILETYPE_ASN1);

	return rv;
}

void uh_tls_ctx_free(struct listener *l)
{
	SSL_CTX_free(l->tls);
}


int uh_tls_client_accept(struct client *c)
{
	int rv, err;
	int fd = c->fd.fd;

	if (!c->server || !c->server->tls)
	{
		c->tls = NULL;
		return 1;
	}

	if ((c->tls = SSL_new(c->server->tls)))
	{
		if ((rv = SSL_set_fd(c->tls, fd)) < 1)
		{
			SSL_free(c->tls);
			c->tls = NULL;
		}
		else
		{
			while (true)
			{
				rv = SSL_accept(c->tls);
				err = SSL_get_error(c->tls, rv);

				if ((rv != 1) &&
					(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE))
				{
					if (uh_socket_wait(fd, c->server->conf->network_timeout,
									   (err == SSL_ERROR_WANT_WRITE)))
					{
						D("TLS: accept(%d) = retry\n", fd);
						continue;
					}

					D("TLS: accept(%d) = timeout\n", fd);
				}
				else if (rv == 1)
				{
					D("TLS: accept(%d) = %p\n", fd, c->tls);
					return 1;
				}

#ifdef TLS_IS_OPENSSL
				D("TLS: accept(%d) = failed: %s\n",
				  fd, ERR_error_string(ERR_get_error(), NULL));
#endif

				SSL_free(c->tls);
				c->tls = NULL;
				break;
			}
		}
	}

	return 0;
}

int uh_tls_client_recv(struct client *c, char *buf, int len)
{
	int rv = SSL_read(c->tls, buf, len);
	int err = SSL_get_error(c->tls, 0);

	if ((rv == -1) && (err == SSL_ERROR_WANT_READ))
	{
		D("TLS: recv(%d, %d) = retry\n", c->fd.fd, len);
		errno = EAGAIN;
		return -1;
	}

	D("TLS: recv(%d, %d) = %d\n", c->fd.fd, len, rv);
	return rv;
}

int uh_tls_client_send(struct client *c, const char *buf, int len)
{
	int rv = SSL_write(c->tls, buf, len);
	int err = SSL_get_error(c->tls, 0);

	if ((rv == -1) && (err == SSL_ERROR_WANT_WRITE))
	{
		D("TLS: send(%d, %d) = retry\n", c->fd.fd, len);
		errno = EAGAIN;
		return -1;
	}

	D("TLS: send(%d, %d) = %d\n", c->fd.fd, len, rv);
	return rv;
}

void uh_tls_client_close(struct client *c)
{
	if (c->tls)
	{
		D("TLS: close(%d)\n", c->fd.fd);

		SSL_shutdown(c->tls);
		SSL_free(c->tls);

		c->tls = NULL;
	}
}
