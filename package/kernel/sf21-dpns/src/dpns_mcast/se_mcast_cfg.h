#ifndef __SF_SE_MCAST_CFG_H__
#define __SF_SE_MCAST_CFG_H__

#define SE_MCAST_MARK_SZ                        (16)
#define SE_MCAST_OIF_MAX                        (16)
#define SE_INVALID_IF_IDX                       (0)

#ifdef __KERNEL__
typedef __uint128_t uint128_t;
#endif // KERNEL

#ifndef __KERNEL__
#include <stdint.h>
#endif // USERSPACE

typedef struct se_l3_mcast_cfg {
        union {
                union {
                        uint8_t b[4];
                        uint32_t d;
                } ip4;
//                union {
//                        uint8_t b[16];
//                        uint128_t d;
//                } ip6;
        } sip; // little-endian

        union {
                union {
                        uint8_t b[4];
                        uint32_t d;
                } ip4;
//                union {
//                        uint8_t b[16];
//                        uint128_t d;
//                } ip6;
        } dip; // little-endian

        uint8_t is_mcsg;
        uint8_t is_ipv6;
        uint8_t iif;
        uint8_t oif[SE_MCAST_OIF_MAX];
        uint8_t oif_cnt;
        char mark[SE_MCAST_MARK_SZ];
} se_l3_mcast_cfg_t;

static inline size_t se_l3_mcast_cfg_sz(void)
{
        return sizeof(se_l3_mcast_cfg_t);
}

static inline size_t se_l3_mcast_cfg_newsz(void)
{
        return sizeof(se_l3_mcast_cfg_t);
}

#endif // __SF_SE_MCAST_CFG_H__
