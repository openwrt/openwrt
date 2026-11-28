#ifndef __SF_GENL_MSG_H__
#define __SF_GENL_MSG_H__

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/compiler.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif


#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

enum sfgenl_hello {
        SF_GENL_HELLO = 0,
        SF_GENL_BYE = 233,
        NUM_SFGENL_HELLO_CMDS,
};

enum sfgenl_comp {
        SF_GENL_COMP_NL = 0,
        SF_GENL_COMP_TMU,
        SF_GENL_COMP_MCAST,
        SF_GENL_COMP_L2_MAC,
        SF_GENL_COMP_NAT,
        SF_GENL_COMP_ACL,
        SF_GENL_COMP_VLAN,
        SF_GENL_COMP_ROUTER,
        SF_GENL_COMP_COMMON,
        NUM_SF_GENL_COMPS,
};

// for common logging
enum sfgenl_event {
        SF_GENL_EVT_NL = 0,
        SF_GENL_EVT_TEST,
        SF_GENL_EVT_UPDOWN,
        NUM_SF_GENL_EVENTS,
};

struct sfgenl_evthdr {
        uint32_t comp_id;
        uint32_t event_id;
        uint32_t buflen; // length of @buf
        uint8_t buf[];
} __packed;

struct sfgenl_msghdr {
        uint32_t comp_id;
        uint32_t buflen;  // length of @buf
        uint8_t buf[];
} __packed;

struct sf_mac_updown {
	uint8_t dsmac[6];
	uint8_t port;
	char ifname[16];
	uint16_t vlan_id;
	bool updown;
	bool is_wifi;
	bool notify_easymesh_flag;
};

static inline size_t sfgenl_evtlen(struct sfgenl_evthdr *hdr)
{
        return sizeof(struct sfgenl_evthdr) + hdr->buflen;
}

static inline size_t sfgenl_msglen(struct sfgenl_msghdr *hdr)
{
        return sizeof(struct sfgenl_msghdr) + hdr->buflen;
}

static inline void *sfgenl_msgbuf(struct sfgenl_msghdr *hdr)
{
        return hdr->buf;
}
#ifdef __KERNEL__ // KERNELSPACE
struct sfgenl_msg_ops {
        int (*msg_recv)(struct genl_info *info, void *buf, size_t buflen);
};

int sfgenl_ops_register(uint32_t comp_id, struct sfgenl_msg_ops *ops);
int sfgenl_msg_ops_unregister(uint32_t comp_id);

int sfgenl_msg_reply(struct genl_info *info, void *buf, size_t buflen);

int sfgenl_event_send(u32 comp_id, u32 event, void *buf, u32 buflen);
#endif // KERNELSPACE
#endif // __SF_GENL_MSG_H__
