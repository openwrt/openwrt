#ifndef __COMPAT_H
#define __COMPAT_H

#include <linux/kernel.h>
#include <linux/types.h>

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define DECLARE_MAC_BUF(var) char var[18] __maybe_unused

static inline char *print_mac(char *buf, const u8 *addr)
{
	sprintf(buf, MAC_FMT,
		addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	return buf;
}

#endif
