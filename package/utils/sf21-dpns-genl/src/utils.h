#ifndef __SF_GENL_USER_UTILS_H__
#define __SF_GENL_USER_UTILS_H__

#include <stdint.h>

typedef __uint128_t uint128_t;

typedef uint32_t __le32;
typedef uint32_t __be32;

//
// bit magic
//

#ifdef __GNUC__
#ifndef BITS_PER_LONG_
#define BITS_PER_LONG_           (__LONG_WIDTH__)
#endif
#define BITS_PER_LONG_LONG      (__LONG_LONG_WIDTH__)
#endif /* __GNUC__ */

#ifdef __clang__
#ifndef BITS_PER_LONG_
#define BITS_PER_LONG_           (__SIZEOF_LONG__ * __CHAR_BIT__)
#endif
#ifndef BITS_PER_LONG_LONG_
#define BITS_PER_LONG_LONG      (__SIZEOF_LONG_LONG__ * __CHAR_BIT__)
#endif
#endif /* __clang__ */

#define BIT(nr)                 (1UL << (nr))
#define BIT_ULL(nr)             (1ULL << (nr))
#define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG_ ))
#define BIT_WORD(nr)            ((nr) / BITS_PER_LONG_ )
#define BIT_ULL_MASK(nr)        (1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)        ((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE           8
#define SIZE_TO_BITS(x)         (sizeof(x) * BITS_PER_BYTE)

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#define GENMASK(h, l) \
        (((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG_ - 1 - (h))))

#define GENMASK_ULL(h, l) \
        (((~0ULL) - (1ULL << (l)) + 1) & \
         (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))

#define BITS_MASK               GENMASK
#define BITS_MASK_ULL           GENMASK_ULL

//
// socket
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <arpa/inet.h>

#define IPV6_STRLEN_MAX                 INET6_ADDRSTRLEN
#define IPV4_STRLEN_MAX                 INET_ADDRSTRLEN

#define IPV6_STR_INADDR_ANY             "::"
#define IPV4_STR_INARRR_ANY             "0.0.0.0"

int is_valid_ipaddr(char *ipstr, int ipver);
int ipstr_aton(char *ipstr, struct sockaddr_storage *sock);
char *inet6_ntoa(struct in6_addr in);
int inetaddr_parse(char *addr_str, uint16_t port, struct sockaddr_storage *addr);

static inline int is_valid_ip4(char *ipstr)
{
        return is_valid_ipaddr(ipstr, AF_INET);
}

static inline int is_valid_ip6(char *ipstr)
{
        return is_valid_ipaddr(ipstr, AF_INET6);
}

#endif // __SF_GENL_USER_UTILS_H__
