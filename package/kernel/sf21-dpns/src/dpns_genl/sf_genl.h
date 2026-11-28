#ifndef __SF_GENL_H__
#define __SF_GENL_H__

#include <linux/netlink.h>

#ifndef __KERNEL__
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#endif

#define SF_GENL_FAMILY_NAME             "sf_genl"

#define SF_GENL_ATTR_HELLO_LEN          16
#define SF_GENL_ATTR_MSG_LEN            2048
#define SF_GENL_ATTR_EVENT_LEN          2048

enum sf_genl_cmd {
        SF_GENL_CMD_UNSPEC = 0,         // DO NOT USE
        SF_GENL_CMD_HELLO,
        SF_GENL_CMD_MSG,
        SF_GENL_CMD_EVENT,
        NUM_SF_GENL_CMDS,
};

enum sf_genl_attr {
        SF_GENL_ATTR_UNSPEC = 0,        // DO NOT USE
        SF_GENL_ATTR_HELLO,
        SF_GENL_ATTR_MSG,
        SF_GENL_ATTR_EVENT,
        NUM_SF_GENL_ATTRS,
};

#define SF_GENL_ATTR_MAX_VALID          (NUM_SF_GENL_ATTRS - 1)

static struct nla_policy sf_genl_policy[NUM_SF_GENL_ATTRS] = {
        [SF_GENL_ATTR_HELLO] = {
                .type = NLA_BINARY,
#ifdef __KERNEL__
                .len = SF_GENL_ATTR_HELLO_LEN,
#else
                .maxlen = SF_GENL_ATTR_HELLO_LEN,
#endif
        },
        [SF_GENL_ATTR_MSG] = {
                .type = NLA_BINARY,
#ifdef __KERNEL__
                .len = SF_GENL_ATTR_MSG_LEN,
#else
                .maxlen = SF_GENL_ATTR_MSG_LEN,
#endif
        },
        [SF_GENL_ATTR_EVENT] = {
                .type = NLA_BINARY,
#ifdef __KERNEL__
                .len = SF_GENL_ATTR_EVENT_LEN,
#else
                .maxlen = SF_GENL_ATTR_EVENT_LEN,
#endif
        },
};

#endif // __SF_GENL_H__
