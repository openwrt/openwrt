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
	SSL_CTX *c = NULL;
	SSL_load_error_strings();
	SSL_library_init();

	if( (c = SSL_CTX_new(TLSv1_server_method())) != NULL )
		SSL_CTX_set_verify(c, SSL_VERIFY_NONE, NULL);

	return c;
}

int uh_tls_ctx_cert(SSL_CTX *c, const char *file)
{
	return SSL_CTX_use_certificate_file(c, file, SSL_FILETYPE_ASN1);
}

int uh_tls_ctx_key(SSL_CTX *c, const char *file)
{
	return SSL_CTX_use_PrivateKey_file(c, file, SSL_FILETYPE_ASN1);
}

void uh_tls_ctx_free(struct listener *l)
{
	SSL_CTX_free(l->tls);
}


void uh_tls_client_accept(struct client *c)
{
	if( c->server && c->server->tls )
	{
		c->tls = SSL_new(c->server->tls);
		SSL_set_fd(c->tls, c->socket);
	}
}

int uh_tls_client_recv(struct client *c, void *buf, int len)
{
	return SSL_read(c->tls, buf, len);
}

int uh_tls_client_send(struct client *c, void *buf, int len)
{
	return SSL_write(c->tls, buf, len);
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


