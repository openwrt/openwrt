/*
 **************************************************************************
 * Copyright (c) 2017-2018, 2021, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/*
 * nss_ppe_stats.h
 *	NSS PPE statistics header file.
 */

#ifndef __NSS_PPE_STATS_H
#define __NSS_PPE_STATS_H

#include <nss_cmn.h>

/*
 * NSS PPE connection statistics
 */
enum nss_ppe_stats_conn {
	NSS_PPE_STATS_V4_L3_FLOWS,		/* No of v4 routed flows */
	NSS_PPE_STATS_V4_L2_FLOWS,		/* No of v4 bridge flows */
	NSS_PPE_STATS_V4_CREATE_REQ,		/* No of v4 create requests */
	NSS_PPE_STATS_V4_CREATE_FAIL,		/* No of v4 create failure */
	NSS_PPE_STATS_V4_DESTROY_REQ,		/* No of v4 delete requests */
	NSS_PPE_STATS_V4_DESTROY_FAIL,		/* No of v4 delete failure */
	NSS_PPE_STATS_V4_MC_CREATE_REQ,		/* No of v4 MC create requests */
	NSS_PPE_STATS_V4_MC_CREATE_FAIL,	/* No of v4 MC create failure */
	NSS_PPE_STATS_V4_MC_UPDATE_REQ,		/* No of v4 MC update requests */
	NSS_PPE_STATS_V4_MC_UPDATE_FAIL,	/* No of v4 MC update failure */
	NSS_PPE_STATS_V4_MC_DESTROY_REQ,	/* No of v4 MC delete requests */
	NSS_PPE_STATS_V4_MC_DESTROY_FAIL,	/* No of v4 MC delete failure */
	NSS_PPE_STATS_V4_UNKNOWN_INTERFACE,	/* No of v4 create failure due to invalid if */

	NSS_PPE_STATS_V6_L3_FLOWS,		/* No of v6 routed flows */
	NSS_PPE_STATS_V6_L2_FLOWS,		/* No of v6 bridge flows */
	NSS_PPE_STATS_V6_CREATE_REQ,		/* No of v6 create requests */
	NSS_PPE_STATS_V6_CREATE_FAIL,		/* No of v6 create failure */
	NSS_PPE_STATS_V6_DESTROY_REQ,		/* No of v6 delete requests */
	NSS_PPE_STATS_V6_DESTROY_FAIL,		/* No of v6 delete failure */
	NSS_PPE_STATS_V6_MC_CREATE_REQ,		/* No of v6 MC create requests */
	NSS_PPE_STATS_V6_MC_CREATE_FAIL,	/* No of v6 MC create failure */
	NSS_PPE_STATS_V6_MC_UPDATE_REQ,		/* No of v6 MC update requests */
	NSS_PPE_STATS_V6_MC_UPDATE_FAIL,	/* No of v6 MC update failure */
	NSS_PPE_STATS_V6_MC_DESTROY_REQ,	/* No of v6 MC delete requests */
	NSS_PPE_STATS_V6_MC_DESTROY_FAIL,	/* No of v6 MC delete failure */
	NSS_PPE_STATS_V6_UNKNOWN_INTERFACE,	/* No of v6 create failure due to invalid if */

	NSS_PPE_STATS_FAIL_VP_FULL,		/* Create req fail due to VP table full */
	NSS_PPE_STATS_FAIL_NH_FULL,		/* Create req fail due to nexthop table full */
	NSS_PPE_STATS_FAIL_FLOW_FULL,		/* Create req fail due to flow table full */
	NSS_PPE_STATS_FAIL_HOST_FULL,		/* Create req fail due to host table full */
	NSS_PPE_STATS_FAIL_PUBIP_FULL,		/* Create req fail due to pub-ip table full */
	NSS_PPE_STATS_FAIL_PORT_SETUP,		/* Create req fail due to PPE port not setup */
	NSS_PPE_STATS_FAIL_RW_FIFO_FULL,	/* Create req fail due to rw fifo full */
	NSS_PPE_STATS_FAIL_FLOW_COMMAND,	/* Create req fail due to PPE flow command failure */
	NSS_PPE_STATS_FAIL_UNKNOWN_PROTO,	/* Create req fail due to unknown protocol */
	NSS_PPE_STATS_FAIL_PPE_UNRESPONSIVE,	/* Create req fail due to PPE not responding */
	NSS_PPE_STATS_CE_OPAQUE_INVALID,	/* Create req fail due to invalid opaque in CE */
	NSS_PPE_STATS_FAIL_FQG_FULL,		/* Create req fail due to flow qos group full */
	NSS_PPE_STATS_CONN_MAX
};

/*
 * NSS PPE SC statistics
 */
enum nss_ppe_stats_service_code {
	NSS_PPE_STATS_SERVICE_CODE_CB_UNREGISTER,
	NSS_PPE_STATS_SERVICE_CODE_PROCESS_OK,
	NSS_PPE_STATS_SERVICE_CODE_PROCESS_FAIL,
	NSS_PPE_STATS_SERVICE_CODE_MAX
};

/*
 * NSS PPE L3 statistics
 */
enum nss_ppe_stats_l3 {
	NSS_PPE_STATS_L3_DBG_0,		/* PPE L3 debug register 0 */
	NSS_PPE_STATS_L3_DBG_1,		/* PPE L3 debug register 1 */
	NSS_PPE_STATS_L3_DBG_2,		/* PPE L3 debug register 2 */
	NSS_PPE_STATS_L3_DBG_3,		/* PPE L3 debug register 3 */
	NSS_PPE_STATS_L3_DBG_4,		/* PPE L3 debug register 4 */
	NSS_PPE_STATS_L3_DBG_PORT,	/* PPE L3 debug register Port */
	NSS_PPE_STATS_L3_MAX
};

/*
 * NSS PPE_code statistics
 */
enum nss_ppe_stats_code {
	NSS_PPE_STATS_CODE_CPU,		/* PPE CPU code for last packet processed */
	NSS_PPE_STATS_CODE_DROP,	/* PPE DROP code for last packet processed */
	NSS_PPE_STATS_CODE_MAX
};

/*
 * PPE drop codes
 */
enum nss_ppe_stats_dc {
	NSS_PPE_STATS_DROP_CODE_UNKNOWN,                                /* PPE drop code unknown */
	NSS_PPE_STATS_DROP_CODE_EXP_UNKNOWN_L2_PROT,                    /* PPE drop code exp unknown l2 prot */
	NSS_PPE_STATS_DROP_CODE_EXP_PPPOE_WRONG_VER_TYPE,               /* PPE drop code exp pppoe wrong ver type */
	NSS_PPE_STATS_DROP_CODE_EXP_PPPOE_WRONG_CODE,                   /* PPE drop code exp pppoe wrong code */
	NSS_PPE_STATS_DROP_CODE_EXP_PPPOE_UNSUPPORTED_PPP_PROT,         /* PPE drop code exp pppoe unsupported ppp prot */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_WRONG_VER,                     /* PPE drop code exp ipv4 wrong ver */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_SMALL_IHL,                     /* PPE drop code exp ipv4 small ihl */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_WITH_OPTION,                   /* PPE drop code exp ipv4 with option */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_HDR_INCOMPLETE,                /* PPE drop code exp ipv4 hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_BAD_TOTAL_LEN,                 /* PPE drop code exp ipv4 bad total len */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_DATA_INCOMPLETE,               /* PPE drop code exp ipv4 data incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_FRAG,                          /* PPE drop code exp ipv4 frag */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_PING_OF_DEATH,                 /* PPE drop code exp ipv4 ping of death */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_SNALL_TTL,                     /* PPE drop code exp ipv4 snall ttl */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_UNK_IP_PROT,                   /* PPE drop code exp ipv4 unk ip prot */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_CHECKSUM_ERR,                  /* PPE drop code exp ipv4 checksum err */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_INV_SIP,                       /* PPE drop code exp ipv4 inv sip */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_INV_DIP,                       /* PPE drop code exp ipv4 inv dip */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_LAND_ATTACK,                   /* PPE drop code exp ipv4 land attack */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_AH_HDR_INCOMPLETE,             /* PPE drop code exp ipv4 ah hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_AH_HDR_CROSS_BORDER,           /* PPE drop code exp ipv4 ah hdr cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV4_ESP_HDR_INCOMPLETE,            /* PPE drop code exp ipv4 esp hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_WRONG_VER,                     /* PPE drop code exp ipv6 wrong ver */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_HDR_INCOMPLETE,                /* PPE drop code exp ipv6 hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_BAD_PAYLOAD_LEN,               /* PPE drop code exp ipv6 bad payload len */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_DATA_INCOMPLETE,               /* PPE drop code exp ipv6 data incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_WITH_EXT_HDR,                  /* PPE drop code exp ipv6 with ext hdr */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_SMALL_HOP_LIMIT,               /* PPE drop code exp ipv6 small hop limit */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_INV_SIP,                       /* PPE drop code exp ipv6 inv sip */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_INV_DIP,                       /* PPE drop code exp ipv6 inv dip */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_LAND_ATTACK,                   /* PPE drop code exp ipv6 land attack */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_FRAG,                          /* PPE drop code exp ipv6 frag */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_PING_OF_DEATH,                 /* PPE drop code exp ipv6 ping of death */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_WITH_MORE_EXT_HDR,             /* PPE drop code exp ipv6 with more ext hdr */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_UNK_LAST_NEXT_HDR,             /* PPE drop code exp ipv6 unk last next hdr */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_MOBILITY_HDR_INCOMPLETE,       /* PPE drop code exp ipv6 mobility hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_MOBILITY_HDR_CROSS_BORDER,     /* PPE drop code exp ipv6 mobility hdr cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_AH_HDR_INCOMPLETE,             /* PPE drop code exp ipv6 ah hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_AH_HDR_CROSS_BORDER,           /* PPE drop code exp ipv6 ah hdr cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_ESP_HDR_INCOMPLETE,            /* PPE drop code exp ipv6 esp hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_ESP_HDR_CROSS_BORDER,          /* PPE drop code exp ipv6 esp hdr cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_OTHER_EXT_HDR_INCOMPLETE,      /* PPE drop code exp ipv6 other ext hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_IPV6_OTHER_EXT_HDR_CROSS_BORDER,    /* PPE drop code exp ipv6 other ext hdr cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_HDR_INCOMPLETE,                 /* PPE drop code exp tcp hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_HDR_CROSS_BORDER,               /* PPE drop code exp tcp hdr cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_SMAE_SP_DP,                     /* PPE drop code exp tcp smae sp dp */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_SMALL_DATA_OFFSET,              /* PPE drop code exp tcp small data offset */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_FLAGS_0,                        /* PPE drop code exp tcp flags 0 */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_FLAGS_1,                        /* PPE drop code exp tcp flags 1 */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_FLAGS_2,                        /* PPE drop code exp tcp flags 2 */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_FLAGS_3,                        /* PPE drop code exp tcp flags 3 */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_FLAGS_4,                        /* PPE drop code exp tcp flags 4 */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_FLAGS_5,                        /* PPE drop code exp tcp flags 5 */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_FLAGS_6,                        /* PPE drop code exp tcp flags 6 */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_FLAGS_7,                        /* PPE drop code exp tcp flags 7 */
	NSS_PPE_STATS_DROP_CODE_EXP_TCP_CHECKSUM_ERR,                   /* PPE drop code exp tcp checksum err */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_HDR_INCOMPLETE,                 /* PPE drop code exp udp hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_HDR_CROSS_BORDER,               /* PPE drop code exp udp hdr cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_SMAE_SP_DP,                     /* PPE drop code exp udp smae sp dp */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_BAD_LEN,                        /* PPE drop code exp udp bad len */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_DATA_INCOMPLETE,                /* PPE drop code exp udp data incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_CHECKSUM_ERR,                   /* PPE drop code exp udp checksum err */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_LITE_HDR_INCOMPLETE,            /* PPE drop code exp udp lite hdr incomplete */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_LITE_HDR_CROSS_BORDER,          /* PPE drop code exp udp lite hdr cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_LITE_SMAE_SP_DP,                /* PPE drop code exp udp lite smae sp dp */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_LITE_CSM_COV_1_TO_7,            /* PPE drop code exp udp lite csm cov 1 to 7 */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_LITE_CSM_COV_TOO_LONG,          /* PPE drop code exp udp lite csm cov too long */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_LITE_CSM_COV_CROSS_BORDER,      /* PPE drop code exp udp lite csm cov cross border */
	NSS_PPE_STATS_DROP_CODE_EXP_UDP_LITE_CHECKSUM_ERR,              /* PPE drop code exp udp lite checksum err */
	NSS_PPE_STATS_DROP_CODE_L3_MC_BRIDGE_ACTION,                    /* PPE drop code l3 mc bridge action */
	NSS_PPE_STATS_DROP_CODE_L3_NO_ROUTE_PREHEAD_NAT_ACTION,         /* PPE drop code l3 no route prehead nat action */
	NSS_PPE_STATS_DROP_CODE_L3_NO_ROUTE_PREHEAD_NAT_ERROR,          /* PPE drop code l3 no route prehead nat error */
	NSS_PPE_STATS_DROP_CODE_L3_ROUTE_ACTION,                        /* PPE drop code l3 route action */
	NSS_PPE_STATS_DROP_CODE_L3_NO_ROUTE_ACTION,                     /* PPE drop code l3 no route action */
	NSS_PPE_STATS_DROP_CODE_L3_NO_ROUTE_NH_INVALID_ACTION,          /* PPE drop code l3 no route nh invalid action */
	NSS_PPE_STATS_DROP_CODE_L3_NO_ROUTE_PREHEAD_ACTION,             /* PPE drop code l3 no route prehead action */
	NSS_PPE_STATS_DROP_CODE_L3_BRIDGE_ACTION,                       /* PPE drop code l3 bridge action */
	NSS_PPE_STATS_DROP_CODE_L3_FLOW_ACTION,                         /* PPE drop code l3 flow action */
	NSS_PPE_STATS_DROP_CODE_L3_FLOW_MISS_ACTION,                    /* PPE drop code l3 flow miss action */
	NSS_PPE_STATS_DROP_CODE_L2_EXP_MRU_FAIL,                        /* PPE drop code l2 exp mru fail */
	NSS_PPE_STATS_DROP_CODE_L2_EXP_MTU_FAIL,                        /* PPE drop code l2 exp mtu fail */
	NSS_PPE_STATS_DROP_CODE_L3_EXP_IP_PREFIX_BC,                    /* PPE drop code l3 exp ip prefix bc */
	NSS_PPE_STATS_DROP_CODE_L3_EXP_MTU_FAIL,                        /* PPE drop code l3 exp mtu fail */
	NSS_PPE_STATS_DROP_CODE_L3_EXP_MRU_FAIL,                        /* PPE drop code l3 exp mru fail */
	NSS_PPE_STATS_DROP_CODE_L3_EXP_ICMP_RDT,                        /* PPE drop code l3 exp icmp rdt */
	NSS_PPE_STATS_DROP_CODE_FAKE_MAC_HEADER_ERR,                    /* PPE drop code fake mac header err */
	NSS_PPE_STATS_DROP_CODE_L3_EXP_IP_RT_TTL_ZERO,                  /* PPE drop code l3 exp ip rt ttl zero */
	NSS_PPE_STATS_DROP_CODE_L3_FLOW_SERVICE_CODE_LOOP,              /* PPE drop code l3 flow service code loop */
	NSS_PPE_STATS_DROP_CODE_L3_FLOW_DE_ACCELEARTE,                  /* PPE drop code l3 flow de accelearte */
	NSS_PPE_STATS_DROP_CODE_L3_EXP_FLOW_SRC_IF_CHK_FAIL,            /* PPE drop code l3 exp flow src if chk fail */
	NSS_PPE_STATS_DROP_CODE_L3_FLOW_SYNC_TOGGLE_MISMATCH,           /* PPE drop code l3 flow sync toggle mismatch */
	NSS_PPE_STATS_DROP_CODE_L3_EXP_MTU_DF_FAIL,                     /* PPE drop code l3 exp mtu df fail */
	NSS_PPE_STATS_DROP_CODE_L3_EXP_PPPOE_MULTICAST,                 /* PPE drop code l3 exp pppoe multicast */
	NSS_PPE_STATS_DROP_CODE_IPV4_SG_UNKNOWN,                        /* PPE drop code ipv4 sg unknown */
	NSS_PPE_STATS_DROP_CODE_IPV6_SG_UNKNOWN,                        /* PPE drop code ipv6 sg unknown */
	NSS_PPE_STATS_DROP_CODE_ARP_SG_UNKNOWN,                         /* PPE drop code arp sg unknown */
	NSS_PPE_STATS_DROP_CODE_ND_SG_UNKNOWN,                          /* PPE drop code nd sg unknown */
	NSS_PPE_STATS_DROP_CODE_IPV4_SG_VIO,                            /* PPE drop code ipv4 sg vio */
	NSS_PPE_STATS_DROP_CODE_IPV6_SG_VIO,                            /* PPE drop code ipv6 sg vio */
	NSS_PPE_STATS_DROP_CODE_ARP_SG_VIO,                             /* PPE drop code arp sg vio */
	NSS_PPE_STATS_DROP_CODE_ND_SG_VIO,                              /* PPE drop code nd sg vio */
	NSS_PPE_STATS_DROP_CODE_L2_NEW_MAC_ADDRESS,                     /* PPE drop code l2 new mac address */
	NSS_PPE_STATS_DROP_CODE_L2_HASH_COLLISION,                      /* PPE drop code l2 hash collision */
	NSS_PPE_STATS_DROP_CODE_L2_STATION_MOVE,                        /* PPE drop code l2 station move */
	NSS_PPE_STATS_DROP_CODE_L2_LEARN_LIMIT,                         /* PPE drop code l2 learn limit */
	NSS_PPE_STATS_DROP_CODE_L2_SA_LOOKUP_ACTION,                    /* PPE drop code l2 sa lookup action */
	NSS_PPE_STATS_DROP_CODE_L2_DA_LOOKUP_ACTION,                    /* PPE drop code l2 da lookup action */
	NSS_PPE_STATS_DROP_CODE_APP_CTRL_ACTION,                        /* PPE drop code app ctrl action */
	NSS_PPE_STATS_DROP_CODE_IN_VLAN_FILTER_ACTION,                  /* PPE drop code in vlan filter action */
	NSS_PPE_STATS_DROP_CODE_IN_VLAN_XLT_MISS,                       /* PPE drop code in vlan xlt miss */
	NSS_PPE_STATS_DROP_CODE_EG_VLAN_FILTER_DROP,                    /* PPE drop code eg vlan filter drop */
	NSS_PPE_STATS_DROP_CODE_ACL_PRE_ACTION,                         /* PPE drop code acl pre action */
	NSS_PPE_STATS_DROP_CODE_ACL_POST_ACTION,                        /* PPE drop code acl post action */
	NSS_PPE_STATS_DROP_CODE_MC_BC_SA,                               /* PPE drop code mc bc sa */
	NSS_PPE_STATS_DROP_CODE_NO_DESTINATION,                         /* PPE drop code no destination */
	NSS_PPE_STATS_DROP_CODE_STG_IN_FILTER,                          /* PPE drop code stg in filter */
	NSS_PPE_STATS_DROP_CODE_STG_EG_FILTER,                          /* PPE drop code stg eg filter */
	NSS_PPE_STATS_DROP_CODE_SOURCE_FILTER_FAIL,                     /* PPE drop code source filter fail */
	NSS_PPE_STATS_DROP_CODE_TRUNK_SEL_FAIL,                         /* PPE drop code trunk sel fail */
	NSS_PPE_STATS_DROP_CODE_TX_EN_FAIL,                             /* PPE drop code tx en fail */
	NSS_PPE_STATS_DROP_CODE_VLAN_TAG_FMT,                           /* PPE drop code vlan tag fmt */
	NSS_PPE_STATS_DROP_CODE_CRC_ERR,                                /* PPE drop code crc err */
	NSS_PPE_STATS_DROP_CODE_PAUSE_FRAME,                            /* PPE drop code pause frame */
	NSS_PPE_STATS_DROP_CODE_PROMISC,                                /* PPE drop code promisc */
	NSS_PPE_STATS_DROP_CODE_ISOLATION,                              /* PPE drop code isolation */
	NSS_PPE_STATS_DROP_CODE_MGMT_APP,                               /* PPE drop code mgmt app */
	NSS_PPE_STATS_DROP_CODE_FAKE_L2_PROT_ERR,                       /* PPE drop code fake l2 prot err */
	NSS_PPE_STATS_DROP_CODE_POLICER,                                /* PPE drop code policer */
	NSS_PPE_STATS_DROP_CODE_MAX                                     /* PPE drop code max */
};

/*
 * PPE CPU codes
 */
#define NSS_PPE_STATS_CPU_CODE_MAX 150
#define NSS_PPE_STATS_CPU_CODE_EXCEPTION_MAX 69
#define NSS_PPE_STATS_CPU_CODE_NONEXCEPTION_START 69
#define NSS_PPE_STATS_CPU_CODE_NONEXCEPTION_MAX (NSS_PPE_STATS_CPU_CODE_MAX - NSS_PPE_STATS_CPU_CODE_NONEXCEPTION_START)

enum nss_ppe_stats_cc {
	NSS_PPE_STATS_CPU_CODE_FORWARDING                         = 0, /* PPE cpu code forwarding */
	NSS_PPE_STATS_CPU_CODE_EXP_UNKNOWN_L2_PROT                = 1, /* PPE cpu code exp unknown l2 prot */
	NSS_PPE_STATS_CPU_CODE_EXP_PPPOE_WRONG_VER_TYPE           = 2, /* PPE cpu code exp pppoe wrong ver type */
	NSS_PPE_STATS_CPU_CODE_EXP_PPPOE_WRONG_CODE               = 3, /* PPE cpu code exp pppoe wrong code */
	NSS_PPE_STATS_CPU_CODE_EXP_PPPOE_UNSUPPORTED_PPP_PROT     = 4, /* PPE cpu code exp pppoe unsupported ppp prot */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_WRONG_VER                 = 5, /* PPE cpu code exp ipv4 wrong ver */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_SMALL_IHL                 = 6, /* PPE cpu code exp ipv4 small ihl */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_WITH_OPTION               = 7, /* PPE cpu code exp ipv4 with option */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_HDR_INCOMPLETE            = 8, /* PPE cpu code exp ipv4 hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_BAD_TOTAL_LEN             = 9, /* PPE cpu code exp ipv4 bad total len */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_DATA_INCOMPLETE           = 10, /* PPE cpu code exp ipv4 data incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_FRAG                      = 11, /* PPE cpu code exp ipv4 frag */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_PING_OF_DEATH             = 12, /* PPE cpu code exp ipv4 ping of death */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_SNALL_TTL                 = 13, /* PPE cpu code exp ipv4 snall ttl */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_UNK_IP_PROT               = 14, /* PPE cpu code exp ipv4 unk ip prot */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_CHECKSUM_ERR              = 15, /* PPE cpu code exp ipv4 checksum err */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_INV_SIP                   = 16, /* PPE cpu code exp ipv4 inv sip */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_INV_DIP                   = 17, /* PPE cpu code exp ipv4 inv dip */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_LAND_ATTACK               = 18, /* PPE cpu code exp ipv4 land attack */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_AH_HDR_INCOMPLETE         = 19, /* PPE cpu code exp ipv4 ah hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_AH_HDR_CROSS_BORDER       = 20, /* PPE cpu code exp ipv4 ah hdr cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV4_ESP_HDR_INCOMPLETE        = 21, /* PPE cpu code exp ipv4 esp hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_WRONG_VER                 = 22, /* PPE cpu code exp ipv6 wrong ver */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_HDR_INCOMPLETE            = 23, /* PPE cpu code exp ipv6 hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_BAD_PAYLOAD_LEN           = 24, /* PPE cpu code exp ipv6 bad payload len */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_DATA_INCOMPLETE           = 25, /* PPE cpu code exp ipv6 data incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_WITH_EXT_HDR              = 26, /* PPE cpu code exp ipv6 with ext hdr */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_SMALL_HOP_LIMIT           = 27, /* PPE cpu code exp ipv6 small hop limit */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_INV_SIP                   = 28, /* PPE cpu code exp ipv6 inv sip */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_INV_DIP                   = 29, /* PPE cpu code exp ipv6 inv dip */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_LAND_ATTACK               = 30, /* PPE cpu code exp ipv6 land attack */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_FRAG                      = 31, /* PPE cpu code exp ipv6 frag */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_PING_OF_DEATH             = 32, /* PPE cpu code exp ipv6 ping of death */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_WITH_MORE_EXT_HDR         = 33, /* PPE cpu code exp ipv6 with more ext hdr */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_UNK_LAST_NEXT_HDR         = 34, /* PPE cpu code exp ipv6 unk last next hdr */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_MOBILITY_HDR_INCOMPLETE   = 35, /* PPE cpu code exp ipv6 mobility hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_MOBILITY_HDR_CROSS_BORDER = 36, /* PPE cpu code exp ipv6 mobility hdr cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_AH_HDR_INCOMPLETE         = 37, /* PPE cpu code exp ipv6 ah hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_AH_HDR_CROSS_BORDER       = 38, /* PPE cpu code exp ipv6 ah hdr cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_ESP_HDR_INCOMPLETE        = 39, /* PPE cpu code exp ipv6 esp hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_ESP_HDR_CROSS_BORDER      = 40, /* PPE cpu code exp ipv6 esp hdr cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_OTHER_EXT_HDR_INCOMPLETE  = 41, /* PPE cpu code exp ipv6 other ext hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_IPV6_OTHER_EXT_HDR_CROSS_BORDER = 42, /* PPE cpu code exp ipv6 other ext hdr cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_HDR_INCOMPLETE             = 43, /* PPE cpu code exp tcp hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_HDR_CROSS_BORDER           = 44, /* PPE cpu code exp tcp hdr cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_SMAE_SP_DP                 = 45, /* PPE cpu code exp tcp smae sp dp */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_SMALL_DATA_OFFSET          = 46, /* PPE cpu code exp tcp small data offset */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_FLAGS_0                    = 47, /* PPE cpu code exp tcp flags 0 */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_FLAGS_1                    = 48, /* PPE cpu code exp tcp flags 1 */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_FLAGS_2                    = 49, /* PPE cpu code exp tcp flags 2 */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_FLAGS_3                    = 50, /* PPE cpu code exp tcp flags 3 */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_FLAGS_4                    = 51, /* PPE cpu code exp tcp flags 4 */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_FLAGS_5                    = 52, /* PPE cpu code exp tcp flags 5 */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_FLAGS_6                    = 53, /* PPE cpu code exp tcp flags 6 */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_FLAGS_7                    = 54, /* PPE cpu code exp tcp flags 7 */
	NSS_PPE_STATS_CPU_CODE_EXP_TCP_CHECKSUM_ERR               = 55, /* PPE cpu code exp tcp checksum err */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_HDR_INCOMPLETE             = 56, /* PPE cpu code exp udp hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_HDR_CROSS_BORDER           = 57, /* PPE cpu code exp udp hdr cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_SMAE_SP_DP                 = 58, /* PPE cpu code exp udp smae sp dp */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_BAD_LEN                    = 59, /* PPE cpu code exp udp bad len */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_DATA_INCOMPLETE            = 60, /* PPE cpu code exp udp data incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_CHECKSUM_ERR               = 61, /* PPE cpu code exp udp checksum err */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_LITE_HDR_INCOMPLETE        = 62, /* PPE cpu code exp udp lite hdr incomplete */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_LITE_HDR_CROSS_BORDER      = 63, /* PPE cpu code exp udp lite hdr cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_LITE_SMAE_SP_DP            = 64, /* PPE cpu code exp udp lite smae sp dp */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_LITE_CSM_COV_1_TO_7        = 65, /* PPE cpu code exp udp lite csm cov 1 to 7 */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_LITE_CSM_COV_TOO_LONG      = 66, /* PPE cpu code exp udp lite csm cov too long */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_LITE_CSM_COV_CROSS_BORDER  = 67, /* PPE cpu code exp udp lite csm cov cross border */
	NSS_PPE_STATS_CPU_CODE_EXP_UDP_LITE_CHECKSUM_ERR          = 68, /* PPE cpu code exp udp lite checksum err */
	NSS_PPE_STATS_CPU_CODE_EXP_FAKE_L2_PROT_ERR               = 69, /* PPE cpu code exp fake l2 prot err */
	NSS_PPE_STATS_CPU_CODE_EXP_FAKE_MAC_HEADER_ERR            = 70, /* PPE cpu code exp fake mac header err */
	NSS_PPE_STATS_CPU_CODE_EXP_BITMAP_MAX                     = 78, /* PPE cpu code exp bitmap max */
	NSS_PPE_STATS_CPU_CODE_L2_EXP_MRU_FAIL                    = 79, /* PPE cpu code l2 exp mru fail */
	NSS_PPE_STATS_CPU_CODE_L2_EXP_MTU_FAIL                    = 80, /* PPE cpu code l2 exp mtu fail */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_IP_PREFIX_BC                = 81, /* PPE cpu code l3 exp ip prefix bc */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_MTU_FAIL                    = 82, /* PPE cpu code l3 exp mtu fail */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_MRU_FAIL                    = 83, /* PPE cpu code l3 exp mru fail */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_ICMP_RDT                    = 84, /* PPE cpu code l3 exp icmp rdt */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_IP_RT_TTL1_TO_ME            = 85, /* PPE cpu code l3 exp ip rt ttl1 to me */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_IP_RT_TTL_ZERO              = 86, /* PPE cpu code l3 exp ip rt ttl zero */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_SERVICE_CODE_LOOP          = 87, /* PPE cpu code l3 flow service code loop */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_DE_ACCELERATE              = 88, /* PPE cpu code l3 flow de accelerate */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_FLOW_SRC_IF_CHK_FAIL        = 89, /* PPE cpu code l3 exp flow src if chk fail */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_SYNC_TOGGLE_MISMATCH       = 90, /* PPE cpu code l3 flow sync toggle mismatch */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_MTU_DF_FAIL                 = 91, /* PPE cpu code l3 exp mtu df fail */
	NSS_PPE_STATS_CPU_CODE_L3_EXP_PPPOE_MULTICAST             = 92, /* PPE cpu code l3 exp pppoe multicast */
	NSS_PPE_STATS_CPU_CODE_MGMT_OFFSET                        = 96, /* PPE cpu code mgmt offset */
	NSS_PPE_STATS_CPU_CODE_MGMT_EAPOL                         = 97, /* PPE cpu code mgmt eapol */
	NSS_PPE_STATS_CPU_CODE_MGMT_PPPOE_DIS                     = 98, /* PPE cpu code mgmt pppoe dis */
	NSS_PPE_STATS_CPU_CODE_MGMT_IGMP                          = 99, /* PPE cpu code mgmt igmp */
	NSS_PPE_STATS_CPU_CODE_MGMT_ARP_REQ                       = 100, /* PPE cpu code mgmt arp req */
	NSS_PPE_STATS_CPU_CODE_MGMT_ARP_REP                       = 101, /* PPE cpu code mgmt arp rep */
	NSS_PPE_STATS_CPU_CODE_MGMT_DHCPv4                        = 102, /* PPE cpu code mgmt dhcpv4 */
	NSS_PPE_STATS_CPU_CODE_MGMT_MLD                           = 107, /* PPE cpu code mgmt mld */
	NSS_PPE_STATS_CPU_CODE_MGMT_NS                            = 108, /* PPE cpu code mgmt ns */
	NSS_PPE_STATS_CPU_CODE_MGMT_NA                            = 109, /* PPE cpu code mgmt na */
	NSS_PPE_STATS_CPU_CODE_MGMT_DHCPv6                        = 110, /* PPE cpu code mgmt dhcpv6 */
	NSS_PPE_STATS_CPU_CODE_PTP_OFFSET                         = 112, /* PPE cpu code ptp offset */
	NSS_PPE_STATS_CPU_CODE_PTP_SYNC                           = 113, /* PPE cpu code ptp sync */
	NSS_PPE_STATS_CPU_CODE_PTP_FOLLOW_UP                      = 114, /* PPE cpu code ptp follow up */
	NSS_PPE_STATS_CPU_CODE_PTP_DELAY_REQ                      = 115, /* PPE cpu code ptp delay req */
	NSS_PPE_STATS_CPU_CODE_PTP_DELAY_RESP                     = 116, /* PPE cpu code ptp delay resp */
	NSS_PPE_STATS_CPU_CODE_PTP_PDELAY_REQ                     = 117, /* PPE cpu code ptp pdelay req */
	NSS_PPE_STATS_CPU_CODE_PTP_PDELAY_RESP                    = 118, /* PPE cpu code ptp pdelay resp */
	NSS_PPE_STATS_CPU_CODE_PTP_PDELAY_RESP_FOLLOW_UP          = 119, /* PPE cpu code ptp pdelay resp follow up */
	NSS_PPE_STATS_CPU_CODE_PTP_ANNOUNCE                       = 120, /* PPE cpu code ptp announce */
	NSS_PPE_STATS_CPU_CODE_PTP_MANAGEMENT                     = 121, /* PPE cpu code ptp management */
	NSS_PPE_STATS_CPU_CODE_PTP_SIGNALING                      = 122, /* PPE cpu code ptp signaling */
	NSS_PPE_STATS_CPU_CODE_PTP_PKT_RSV_MSG                    = 127, /* PPE cpu code ptp pkt rsv msg */
	NSS_PPE_STATS_CPU_CODE_IPV4_SG_UNKNOWN                    = 136, /* PPE cpu code ipv4 sg unknown */
	NSS_PPE_STATS_CPU_CODE_IPV6_SG_UNKNOWN                    = 137, /* PPE cpu code ipv6 sg unknown */
	NSS_PPE_STATS_CPU_CODE_ARP_SG_UNKNOWN                     = 138, /* PPE cpu code arp sg unknown */
	NSS_PPE_STATS_CPU_CODE_ND_SG_UNKNOWN                      = 139, /* PPE cpu code nd sg unknown */
	NSS_PPE_STATS_CPU_CODE_IPV4_SG_VIO                        = 140, /* PPE cpu code ipv4 sg vio */
	NSS_PPE_STATS_CPU_CODE_IPV6_SG_VIO                        = 141, /* PPE cpu code ipv6 sg vio */
	NSS_PPE_STATS_CPU_CODE_ARP_SG_VIO                         = 142, /* PPE cpu code arp sg vio */
	NSS_PPE_STATS_CPU_CODE_ND_SG_VIO                          = 143, /* PPE cpu code nd sg vio */
	NSS_PPE_STATS_CPU_CODE_L3_ROUTING_IP_TO_ME                = 148, /* PPE cpu code l3 routing ip to me */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_SNAT_ACTION                = 149, /* PPE cpu code l3 flow snat action */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_DNAT_ACTION                = 150, /* PPE cpu code l3 flow dnat action */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_RT_ACTION                  = 151, /* PPE cpu code l3 flow rt action */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_BR_ACTION                  = 152, /* PPE cpu code l3 flow br action */
	NSS_PPE_STATS_CPU_CODE_L3_MC_BRIDGE_ACTION                = 153, /* PPE cpu code l3 mc bridge action */
	NSS_PPE_STATS_CPU_CODE_L3_ROUTE_PREHEAD_RT_ACTION         = 154, /* PPE cpu code l3 route prehead rt action */
	NSS_PPE_STATS_CPU_CODE_L3_ROUTE_PREHEAD_SNAPT_ACTION      = 155, /* PPE cpu code l3 route prehead snapt action */
	NSS_PPE_STATS_CPU_CODE_L3_ROUTE_PREHEAD_DNAPT_ACTION      = 156, /* PPE cpu code l3 route prehead dnapt action */
	NSS_PPE_STATS_CPU_CODE_L3_ROUTE_PREHEAD_SNAT_ACTION       = 157, /* PPE cpu code l3 route prehead snat action */
	NSS_PPE_STATS_CPU_CODE_L3_ROUTE_PREHEAD_DNAT_ACTION       = 158, /* PPE cpu code l3 route prehead dnat action */
	NSS_PPE_STATS_CPU_CODE_L3_NO_ROUTE_PREHEAD_NAT_ACTION     = 159, /* PPE cpu code l3 no route prehead nat action */
	NSS_PPE_STATS_CPU_CODE_L3_NO_ROUTE_PREHEAD_NAT_ERROR      = 160, /* PPE cpu code l3 no route prehead nat error */
	NSS_PPE_STATS_CPU_CODE_L3_ROUTE_ACTION                    = 161, /* PPE cpu code l3 route action */
	NSS_PPE_STATS_CPU_CODE_L3_NO_ROUTE_ACTION                 = 162, /* PPE cpu code l3 no route action */
	NSS_PPE_STATS_CPU_CODE_L3_NO_ROUTE_NH_INVALID_ACTION      = 163, /* PPE cpu code l3 no route nh invalid action */
	NSS_PPE_STATS_CPU_CODE_L3_NO_ROUTE_PREHEAD_ACTION         = 164, /* PPE cpu code l3 no route prehead action */
	NSS_PPE_STATS_CPU_CODE_L3_BRIDGE_ACTION                   = 165, /* PPE cpu code l3 bridge action */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_ACTION                     = 166, /* PPE cpu code l3 flow action */
	NSS_PPE_STATS_CPU_CODE_L3_FLOW_MISS_ACTION                = 167, /* PPE cpu code l3 flow miss action */
	NSS_PPE_STATS_CPU_CODE_L2_NEW_MAC_ADDRESS                 = 168, /* PPE cpu code l2 new mac address */
	NSS_PPE_STATS_CPU_CODE_L2_HASH_COLLISION                  = 169, /* PPE cpu code l2 hash collision */
	NSS_PPE_STATS_CPU_CODE_L2_STATION_MOVE                    = 170, /* PPE cpu code l2 station move */
	NSS_PPE_STATS_CPU_CODE_L2_LEARN_LIMIT                     = 171, /* PPE cpu code l2 learn limit */
	NSS_PPE_STATS_CPU_CODE_L2_SA_LOOKUP_ACTION                = 172, /* PPE cpu code l2 sa lookup action */
	NSS_PPE_STATS_CPU_CODE_L2_DA_LOOKUP_ACTION                = 173, /* PPE cpu code l2 da lookup action */
	NSS_PPE_STATS_CPU_CODE_APP_CTRL_ACTION                    = 174, /* PPE cpu code app ctrl action */
	NSS_PPE_STATS_CPU_CODE_IN_VLAN_FILTER_ACTION              = 175, /* PPE cpu code in vlan filter action */
	NSS_PPE_STATS_CPU_CODE_IN_VLAN_XLT_MISS                   = 176, /* PPE cpu code in vlan xlt miss */
	NSS_PPE_STATS_CPU_CODE_EG_VLAN_FILTER_DROP                = 177, /* PPE cpu code eg vlan filter drop */
	NSS_PPE_STATS_CPU_CODE_ACL_PRE_ACTION                     = 178, /* PPE cpu code acl pre action */
	NSS_PPE_STATS_CPU_CODE_ACL_POST_ACTION                    = 179, /* PPE cpu code acl post action */
	NSS_PPE_STATS_CPU_CODE_SERVICE_CODE_ACTION                = 180, /* PPE cpu code service code action */
};

/*
 * nss_ppe_sc_stats_debug
 */
struct nss_ppe_sc_stats_debug {
	uint64_t nss_ppe_sc_cb_unregister;      /* Per service-code counter for callback not registered */
	uint64_t nss_ppe_sc_cb_success;         /* Per service-code coutner for successful callback */
	uint64_t nss_ppe_sc_cb_failure;         /* Per service-code counter for failure callback */
};

/*
 * NSS PPE statistics
 */
struct nss_ppe_stats_debug {
	uint64_t conn_stats[NSS_PPE_STATS_CONN_MAX];
	uint32_t l3_stats[NSS_PPE_STATS_L3_MAX];
	uint32_t code_stats[NSS_PPE_STATS_CODE_MAX];
	struct nss_ppe_sc_stats_debug sc_stats[NSS_PPE_SC_MAX];
	int32_t if_index;
	uint32_t if_num;	/* nss interface number */
	bool valid;
};

/*
 * Data structures to store NSS PPE debug statistics
 */
extern struct nss_ppe_stats_debug nss_ppe_debug_stats;

/**
 * nss_ppe_stats_notification
 *	PPE transmission statistics structure.
 */
struct nss_ppe_stats_notification {
	struct nss_ppe_sc_stats_debug ppe_stats_sc[NSS_PPE_SC_MAX];	/* PPE service code stats. */
	uint64_t ppe_stats_conn[NSS_PPE_STATS_CONN_MAX];		/* PPE connection statistics. */
	uint32_t core_id;						/* Core ID. */
	uint32_t if_num;						/* Interface number. */
};

/*
 * NSS PPE statistics APIs
 */
extern void nss_ppe_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
extern void nss_ppe_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_ppe_sync_stats_msg *stats_msg, uint16_t if_num);
extern void nss_ppe_stats_dentry_create(void);

#endif /* __NSS_PPE_STATS_H */
