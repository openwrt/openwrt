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


SSL_CTX * uh_tls_ctx_init()
{
	SSL_CTX *c;

	SSL_load_error_strings();
	SSL_library_init();

	if( (c = SSL_CTX_new(TLSv1_server_method())) != NULL )
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
	int rv;

	if( c->server && c->server->tls )
	{
		c->tls = SSL_new(c->server->tls);
		if( c->tls )
		{
			if( (rv = SSL_set_fd(c->tls, c->socket)) < 1 )
				goto cleanup;
			if( (rv = SSL_accept(c->tls)) < 1 )
				goto cleanup;
		}
		else
			rv = 0;
	}
	else
	{
		c->tls = NULL;
		rv = 1;
	}

done:
	return rv;

cleanup:
	SSL_free(c->tls);
	c->tls = NULL;
	goto done;
}

int uh_tls_client_recv(struct client *c, void *buf, int len)
{
	int rv = SSL_read(c->tls, buf, len);
	return (rv > 0) ? rv : -1;
}

int uh_tls_client_send(struct client *c, void *buf, int len)
{
	int rv = SSL_write(c->tls, buf, len);
	return (rv > 0) ? rv : -1;
}

void uh_tls_client_close(struct client *c)
{
	if( c->tls )
	{
		SSL_shutdown(c->tls);
		SSL_free(c->tls);

		c->tls = NULL;
	}
}
