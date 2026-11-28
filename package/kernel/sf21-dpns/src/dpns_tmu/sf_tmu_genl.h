#ifndef __SF_GENL_TMU_H__
#define __SF_GENL_TMU_H__

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

enum tmu_msg_cmd {
        TMU_MSG_CMD_INFO_GET = 0,
        TMU_MSG_CMD_QUEUE_GET,
        TMU_MSG_CMD_QUEUE_SET,
        TMU_MSG_CMD_SCHED_GET,
        TMU_MSG_CMD_SCHED_SET,
        TMU_MSG_CMD_SHAPER_GET,
        TMU_MSG_CMD_SHAPER_SET,
        TMU_MSG_CMD_SHAPER_RATE_SET,
        TMU_MSG_CMD_LIF_SHAPER_RATE_SET,
        TMU_MSG_CMD_RST,
        TMU_MSG_CMD_PORT_RST,
        NUM_TMU_MSG_CMDS,
};

enum tmu_comp {
        TMU_COMP_PORT = 0,
        TMU_COMP_QUEUE,
        TMU_COMP_SCHED,
        TMU_COMP_SHAPER,
        NUM_TMU_COMPS,
};

struct tmu_info {
        uint32_t port_cnt;
        uint32_t queue_per_port;
        uint32_t shaper_per_port;
        uint32_t scheduler_per_port;
};

enum {
        QUEUE_BUF_MAX = 0,
        QUEUE_DROP_TYPE,
        QUEUE_LEN_MAX,
        QUEUE_LEN_MIN,
        QUEUE_WRED_PROBS,
        NUM_QUEUE_SET,
};

struct tmu_queue_info {
        uint32_t buf_cnt;       // ro
        uint32_t buf_max;
        uint32_t pkt_cnt;       // ro
        uint32_t q_head;        // ro
        uint32_t q_tail;        // ro
        uint32_t drop_type;
        uint32_t qlen_max;
        uint32_t qlen_min;
        uint8_t  wred_drop_probs[8];
};

struct tmu_queue_set {
        uint32_t set;
        struct tmu_queue_info info;
};

enum {
        SCHED_DEQ_ALGO = 0,
        SCHED_Q_MAP,
        SCHED_Q_WGHT,
        SCHED_BITRATE_MODE,
        SCHED_LOCATION,
        NUM_SCHED_SET,
};

struct tmu_sched_info {
        uint32_t deq_algo;
        uint32_t q_map[8];
        uint32_t q_weight[8];
        uint32_t bitrate_mode;
        uint32_t location;
};

struct tmu_sched_set {
        uint32_t set;
        struct tmu_sched_info info;
};

enum {
        SHAPER_ENABLED = 0,
        SHAPER_CREDIT_RATE,
        SHAPER_CREDIT_WEIGHT_INT,
        SHAPER_CREDIT_WEIGHT_FRAC,
        SHAPER_CREDIT_MAX,
        SHAPER_CREDIT_MIN,
        SHAPER_BITRATE_MODE,
        SHAPER_ALLOW_BURST,
        SHAPER_LOCATION,
        NUM_SHAPER_SET,
};

struct tmu_shaper_info {
        uint32_t enabled;
        uint32_t credit_rate;   // CLK_DIV
        uint32_t credit_weight_int;
        uint32_t credit_weight_frac;
        uint32_t credit_max;
        uint32_t credit_min;
        uint32_t credit_avail;  // ro
        uint32_t credit_clear;
        uint32_t location;
        uint32_t bitrate_mode;
        uint64_t bitrate;
        uint32_t is_working;    // ro
};

struct tmu_shaper_rate_info {
        uint32_t bps;
        uint32_t mbps;
        int32_t clk_div;
        uint32_t allow_burst;
};

struct tmu_shaper_set {
        uint32_t set;   // bitmap
        struct tmu_shaper_info info;
        struct tmu_shaper_rate_info rate_info;
};

struct tmu_msg {
        uint32_t cmd;
        uint32_t port;          // # port
        uint32_t comp;          // enum tmu_comp
        union {
                uint32_t nr_comp;
                uint32_t queue;
                uint32_t sched;
                uint32_t shaper;
        } u;                    // # comp
        uint32_t buflen;
        uint8_t buf[];
};

static inline size_t tmu_msglen(struct tmu_msg *msg)
{
        return sizeof(struct tmu_msg) + msg->buflen;
}

#define tmu_msg_newlen(buflen) (sizeof(struct tmu_msg) + buflen)

static inline int is_invalid_tmu_msg(struct tmu_msg *msg, size_t msglen)
{
        if (tmu_msglen(msg) > msglen)
                return 1;

        if (msg->cmd >= NUM_TMU_MSG_CMDS)
                return 1;

        if (msg->comp >= NUM_TMU_COMPS)
                return 1;

        return 0;
}

#endif // __SF_GENL_TMU_H__
