/* conf.h.  Generated automatically by configure.  */
#define HAVE_NET_IF_H			1
#define HAVE_NETINET_IN_H		1 /* Socket data structure */
#define HAVE_NETINET_IN_SYSTM_H		1
#define HAVE_NETINET_IP_H		1
#define HAVE_NETINET_UDP_H		1
#define HAVE_NETINET_TCP_H		1
#define HAVE_NETDB_H			1
#define HAVE_ARPA_INET_H		1
#define HAVE_SYS_SOCKET_H		1
#define HAVE_FCNTL_H			1 /* O_RDONLY              */
#define HAVE_LINUX_IF_ETHER_H		1
#define HAVE_SIGNAL_H			1
#define HAVE_STRING_H			1
#define HAVE_STDLIB_H			1
#define HAVE_STDIO_H			1
#define HAVE_SYS_TIME_H			1
#define HAVE_UNISTD_H			1
#define HAVE_SYS_STAT_H			1
#define HAVE_SYS_TYPES_H		1
#define HAVE_ERRNO_H			1
#define HAVE_LINUX_SOCKIOS_H		1 /* SIOCGIFHWADDR */
#define HAVE_CTYPE_H			1
#define HAVE_PCAP_H			0
#define TIME_WITH_SYS_TIME              1
#define HAVE_SYS_SOCKET_H               1
#define HAVE_SYS_IOCTL_H                1
#define HAVE_TERMIOS_H                  1

#if HAVE_TERMIOS_H
#include <termios.h>
#endif

#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_CTYPE_H
#include <ctype.h>
#endif

#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#if HAVE_NET_IF_H
#include <net/if.h>
#endif

#if HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>
#endif

#if HAVE_NETINET_IP_H
#include <netinet/ip.h>
#endif

#if HAVE_NETINET_UDP_H
#include <netinet/udp.h>
#endif

#if HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif

#if HAVE_NETDB_H			
#include <netdb.h>			
#endif

#if HAVE_ARPA_INET_H
#include <arpa/inet.h>			
#endif

#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_LINUX_IF_ETHER_H
#include <linux/if_ether.h>
#endif

#if HAVE_SIGNAL_H
#include <signal.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_LINUX_SOCKIOS_H
#include <linux/sockios.h>
#endif

#if HAVE_PCAP_H
#include <pcap.h>
#endif

#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
