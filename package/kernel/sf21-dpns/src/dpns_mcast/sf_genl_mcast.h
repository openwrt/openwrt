#ifndef __SF_GENL_L3MCAST_H__
#define __SF_GENL_L3MCAST_H__

#ifdef __KERNEL__
#include <linux/types.h>
#endif // KERNEL

#ifndef __KERNEL__ // USERSPACE
#include <stdint.h>
#endif // USERSPACE

enum mcast_msg_cmd {
        MC_MSG_CMD_ADD = 0,
        MC_MSG_CMD_DEL,
        MC_MSG_CMD_DEL_MARKED,
        MC_MSG_CMD_LIST,
        NUM_MC_MSG_CMDS,
};

enum mcast_type {
        MCAST_L2 = 0,
        MCAST_L3,
        NUM_MCAST_TYPES,
};

struct mcast_genl_msg {
        uint32_t cmd;
        uint32_t mc_type;
        uint32_t buflen;
        uint8_t buf[];
};

static inline size_t mcast_genl_newsz(size_t buflen)
{
        return sizeof(struct mcast_genl_msg) + buflen;
}

static inline size_t mcast_genl_msglen(struct mcast_genl_msg *msg)
{
        return sizeof(struct mcast_genl_msg) + msg->buflen;
}

static inline size_t is_invalid_mcast_msg(struct mcast_genl_msg *msg, size_t msglen)
{
        if (mcast_genl_msglen(msg) > msglen)
                return 1;

        if (msg->cmd >= NUM_MC_MSG_CMDS)
                return 1;

        if (msg->mc_type >= NUM_MCAST_TYPES)
                return 1;

        return 0;
}

#endif // __SF_GENL_L3MCAST_H__
