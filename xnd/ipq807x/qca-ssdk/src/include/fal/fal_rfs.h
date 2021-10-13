/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
 */


/**
 * @defgroup fal_rfs FAL_RFS
 * @{
 */
#ifndef _FAL_RFS_H_
#define _FAL_RFS_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */
#if 0
typedef struct
{
    u8 addr[6];
    u16    fid;
    u8 load_balance;
} ssdk_fdb_rfs_t;

typedef struct
{
    u8 mac_addr[6];
    u32 ip4_addr;
	u32     vid;
	u8      load_balance;
} ssdk_ip4_rfs_t;

typedef struct
{
    u8 mac_addr[6];
    u32 ip6_addr[4];
	u32     vid;
	u8      load_balance;
} ssdk_ip6_rfs_t;



int ssdk_rfs_ipct_rule_set(
	__be32 ip_src, __be32 ip_dst,
	__be16 sport, __be16 dport, uint8_t proto,
	u16 loadbalance, bool action);

int ssdk_rfs_mac_rule_set(ssdk_fdb_rfs_t *rfs);
int ssdk_rfs_mac_rule_del(ssdk_fdb_rfs_t *rfs);

int
ssdk_ip_rfs_ip4_rule_set(ssdk_ip4_rfs_t * rfs);

int
ssdk_ip_rfs_ip6_rule_set(ssdk_ip6_rfs_t * rfs);

int
ssdk_ip_rfs_ip4_rule_del(ssdk_ip4_rfs_t * rfs);

int
ssdk_ip_rfs_ip6_rule_del(ssdk_ip6_rfs_t * rfs);
#endif
int ssdk_rfs_mac_rule_set(u16 vid, u8* mac, u8 ldb, int is_set);
int ssdk_rfs_ip4_rule_set(u16 vid, u32 ip, u8* mac, u8 ldb, int is_set);
int ssdk_rfs_ip6_rule_set(u16 vid, u8* ip, u8* mac, u8 ldb, int is_set);
int ssdk_rfs_ipct_rule_set(
	__be32 ip_src, __be32 ip_dst,
	__be16 sport, __be16 dport, uint8_t proto,
	u16 loadbalance, bool action);



#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_RFS_H_ */

/**
 * @}
 */

