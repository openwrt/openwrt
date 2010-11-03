#ifndef __STUN_H__
#define __STUN_H__

#include <stdint.h>

struct stun_client;

struct stun_client *stun_client_alloc(const char *hostname, uint16_t port);
void stun_client_free(struct stun_client *);
int stun_client_resolve(struct stun_client *stun, int sockfd, struct sockaddr *addr);

#endif
