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

#ifdef TLS_IS_CYASSL
static int uh_cyassl_recv_cb(char *buf, int sz, void *ctx)
{
	int rv;
	int socket = *(int *)ctx;
	struct client *cl;

	if (!(cl = uh_client_lookup(socket)))
		return -1; /* unexpected error */

	rv = uh_tcp_recv_lowlevel(cl, buf, sz);

	if (rv < 0)
		return -4; /* interrupted */

	if (rv == 0)
		return -5; /* connection closed */

	return rv;
}

static int uh_cyassl_send_cb(char *buf, int sz, void *ctx)
{
	int rv;
	int socket = *(int *)ctx;
	struct client *cl;

	if (!(cl = uh_client_lookup(socket)))
		return -1; /* unexpected error */

	rv = uh_tcp_send_lowlevel(cl, buf, sz);

	if (rv <= 0)
		return -5; /* connection dead */

	return rv;
}

void SetCallbackIORecv_Ctx(SSL_CTX*, int (*)(char *, int, void *));
void SetCallbackIOSend_Ctx(SSL_CTX*, int (*)(char *, int, void *));

static void uh_tls_ctx_setup(SSL_CTX *ctx)
{
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
	SetCallbackIORecv_Ctx(ctx, uh_cyassl_recv_cb);
	SetCallbackIOSend_Ctx(ctx, uh_cyassl_send_cb);
	return;
}

static int uh_tls_client_ctx_setup(SSL *ssl, int socket)
{
	return SSL_set_fd(ssl, socket);
}
#endif /* TLS_IS_CYASSL */

#ifdef TLS_IS_OPENSSL
static long uh_openssl_bio_ctrl_cb(BIO *b, int cmd, long num, void *ptr)
{
	long rv = 1;

	switch (cmd)
	{
		case BIO_C_SET_FD:
			b->num      = *((int *)ptr);
			b->shutdown = (int)num;
			b->init     = 1;
			break;

		case BIO_C_GET_FD:
			if (!b->init)
				return -1;

			if (ptr)
				*((int *)ptr) = b->num;

			rv = b->num;
			break;
	}

	return rv;
}

static int uh_openssl_bio_read_cb(BIO *b, char *out, int outl)
{
	int rv = 0;
	struct client *cl;

	if (!(cl = uh_client_lookup(b->num)))
		return -1;

	if (out != NULL)
		rv = uh_tcp_recv_lowlevel(cl, out, outl);

	return rv;
}

static int uh_openssl_bio_write_cb(BIO *b, const char *in, int inl)
{
	struct client *cl;

	if (!(cl = uh_client_lookup(b->num)))
		return -1;

	return uh_tcp_send_lowlevel(cl, in, inl);
}

static BIO_METHOD uh_openssl_bio_methods = {
	.type   = BIO_TYPE_SOCKET,
	.name   = "uhsocket",
	.ctrl   = uh_openssl_bio_ctrl_cb,
	.bwrite = uh_openssl_bio_write_cb,
	.bread  = uh_openssl_bio_read_cb
};

static void uh_tls_ctx_setup(SSL_CTX *ctx)
{
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
	return;
}

static int uh_tls_client_ctx_setup(SSL *ssl, int socket)
{
	BIO *b;

	if (!(b = BIO_new(&uh_openssl_bio_methods)))
		return 0;

	BIO_set_fd(b, socket, BIO_NOCLOSE);
	SSL_set_bio(ssl, b, b);

	return 1;
}
#endif /* TLS_IS_OPENSSL */


SSL_CTX * uh_tls_ctx_init()
{
	SSL_CTX *c;

	SSL_load_error_strings();
	SSL_library_init();

	if ((c = SSL_CTX_new(TLSv1_server_method())) != NULL)
		uh_tls_ctx_setup(c);

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
			if( (rv = uh_tls_client_ctx_setup(c->tls, c->socket)) < 1 )
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
