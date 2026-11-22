#include <errno.h>

#include "utils.h"

int is_valid_ipaddr(char *ipstr, int ipver)
{
        unsigned char buf[sizeof(struct in6_addr)];

        return (inet_pton(ipver, ipstr, buf) == 1);
}

int ipstr_aton(char *ipstr, struct sockaddr_storage *sock)
{
        int ok;

        if (is_valid_ip4(ipstr)) {
                sock->ss_family = AF_INET;
                ok = inet_pton(AF_INET, ipstr, &((struct sockaddr_in *)sock)->sin_addr);
                if (ok)
                        return 0;
        } else if (is_valid_ip6(ipstr)) {
                sock->ss_family = AF_INET6;
                ok = inet_pton(AF_INET6, ipstr, &((struct sockaddr_in6 *)sock)->sin6_addr);
                if (ok)
                        return 0;
        }

        return -EINVAL;
}

static __thread char inet6_ntop_buf[INET_ADDRSTRLEN];

char *inet6_ntoa(struct in6_addr in)
{
        inet_ntop(AF_INET6, &in, inet6_ntop_buf, sizeof(inet6_ntop_buf));

        return inet6_ntop_buf;
}
