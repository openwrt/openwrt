/*
 * uhttpd - Tiny single-threaded httpd - TLS header
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

#ifndef _UHTTPD_TLS_

#include <openssl/ssl.h>


SSL_CTX * uh_tls_ctx_init();
int uh_tls_ctx_cert(SSL_CTX *c, const char *file);
int uh_tls_ctx_key(SSL_CTX *c, const char *file);
void uh_tls_ctx_free(struct listener *l);

int uh_tls_client_accept(struct client *c);
int uh_tls_client_recv(struct client *c, void *buf, int len);
int uh_tls_client_send(struct client *c, void *buf, int len);
void uh_tls_client_close(struct client *c);

#endif
