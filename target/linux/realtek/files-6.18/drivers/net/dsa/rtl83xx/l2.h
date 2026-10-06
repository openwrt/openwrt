/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_L2_H
#define _OTTO_L2_H

#include <linux/bits.h>
#include <linux/types.h>

struct rtl838x_switch_priv;
struct rtl838x_l2_entry;

enum rtldsa_flood_type {
	RTLDSA_FLOOD_TYPE_FORWARD = 0,
	RTLDSA_FLOOD_TYPE_DROP,
	RTLDSA_FLOOD_TYPE_TRAP2CPU,
	RTLDSA_FLOOD_TYPE_COPY2CPU,
	RTLDSA_FLOOD_TYPE_TRAP2MASTER,
	RTLDSA_FLOOD_TYPE_COPY2MASTER,
};

#define RTL838X_PORT_ISO_CTRL(port)		(0x4100 + ((port) << 2))

#define RTL930X_L2_CTRL				(0x8FD8)
#define RTL931X_L2_CTRL				(0xC800)
#define RTL838X_L2_CTRL_1			(0x3204)
#define RTL839X_L2_CTRL_1			(0x3804)
#define RTL930X_L2_AGE_CTRL			(0x8FDC)
#define RTL931X_L2_AGE_CTRL			(0xC804)
#define RTL838X_L2_PORT_AGING_OUT		(0x3358)
#define RTL839X_L2_PORT_AGING_OUT		(0x3b74)
#define	RTL930X_L2_PORT_AGE_CTRL		(0x8FE0)
#define	RTL931X_L2_PORT_AGE_CTRL		(0xc808)

#define RTL930X_L2_TBL_FLUSH_CTRL		(0x9404)
#define RTL931X_L2_TBL_FLUSH_CTRL		(0xCD9C)

#define SALRN_PORT_SHIFT(p)			((p % 16) * 2)
#define SALRN_MODE_MASK				0x3
#define SALRN_MODE_HARDWARE			0
#define SALRN_MODE_DISABLED			2

#define RTL930X_L2_PORT_SABLK_CTRL		(0x905c)
#define RTL930X_L2_PORT_DABLK_CTRL		(0x9060)

/* ToDo: MAX_MC_GROUPS could be increased
 * 838x/839x/930x/931x -> 8192/16384/16384/32768 entries (priv->fib_entries)
 * They are shared with unicast entries
 */
#define MAX_MC_GROUPS 512
/* ToDo: MAX_MC_PMASKS could be increased
 * 838x/839x/930x/931x -> 512/4096/1024/4096 entries
 */
#define MAX_MC_PMASKS 512
#define RTL838X_MC_PMASK_ALL_PORTS (GENMASK(RTL838X_CPU_PORT, 0))
#define RTL930X_MC_PMASK_ALL_PORTS (GENMASK(RTL930X_CPU_PORT, 0))
#define RTL931X_MC_PMASK_ALL_PORTS (GENMASK_ULL(RTL931X_CPU_PORT, 0))
#define MC_PMASK_ALL_PORTS_IDX	((MAX_MC_PMASKS - 1))

#define RTLDSA_L2_L3_REFCOUNT_MAX	0x7f

u64 rtl838x_l2_hash_seed(u64 mac, u32 vid);
u32 rtl838x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed);
int rtl838x_l2_port_new_salrn(int p);
int rtl838x_l2_port_new_sa_fwd(int p);
u64 rtl838x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
void rtl838x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
u64 rtl838x_read_cam(int idx, struct rtl838x_l2_entry *e);
void rtl838x_write_cam(int idx, struct rtl838x_l2_entry *e);
u64 rtl838x_read_mcast_pmask(int idx);
void rtl838x_write_mcast_pmask(int idx, u64 portmask);
void rtl838x_l2_learning_setup(void);
void rtl838x_enable_learning(int port, bool enable);
void rtl838x_enable_flood(int port, enum rtldsa_flood_type mode);
void rtl838x_enable_mcast_flood(int port, bool enable);
void rtl838x_enable_bcast_flood(int port, bool enable);
void rtl838x_set_static_move_action(int port, bool forward);
int rtldsa_838x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid);
int rtl838x_set_ageing_time(unsigned long msec);
u64 rtl839x_l2_hash_seed(u64 mac, u32 vid);
u32 rtl839x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed);
int rtl839x_l2_port_new_salrn(int p);
int rtl839x_l2_port_new_sa_fwd(int p);
u64 rtl839x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
void rtl839x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
u64 rtl839x_read_cam(int idx, struct rtl838x_l2_entry *e);
void rtl839x_write_cam(int idx, struct rtl838x_l2_entry *e);
u64 rtl839x_read_mcast_pmask(int idx);
void rtl839x_write_mcast_pmask(int idx, u64 portmask);
void rtl839x_l2_learning_setup(void);
void rtl839x_enable_learning(int port, bool enable);
void rtl839x_enable_flood(int port, enum rtldsa_flood_type mode);
void rtl839x_enable_mcast_flood(int port, bool enable);
void rtl839x_enable_bcast_flood(int port, bool enable);
void rtl839x_set_static_move_action(int port, bool forward);
int rtldsa_839x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid);
int rtl839x_set_ageing_time(unsigned long msec);
int rtl930x_l2_port_new_salrn(int p);
int rtl930x_l2_port_new_sa_fwd(int p);
void rtl930x_l2_learning_setup(void);
void rtldsa_930x_enable_learning(int port, bool enable);
void rtldsa_930x_l2_port_new_sa_fwd(int port, enum rtldsa_flood_type mode);
void rtldsa_930x_enable_flood(int port, enum rtldsa_flood_type mode);
void rtldsa_930x_enable_bcast_flood(int port, bool enable);
u64 rtl930x_l2_hash_seed(u64 mac, u32 vid);
u32 rtl930x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed);
u64 rtl930x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
void rtl930x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
u64 rtl930x_read_cam(int idx, struct rtl838x_l2_entry *e);
void rtl930x_write_cam(int idx, struct rtl838x_l2_entry *e);
u64 rtl930x_read_mcast_pmask(int idx);
void rtl930x_write_mcast_pmask(int idx, u64 portmask);
int rtldsa_930x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid);
int rtl930x_set_ageing_time(unsigned long msec);
int rtl931x_l2_port_new_salrn(int p);
int rtl931x_l2_port_new_sa_fwd(int p);
u64 rtldsa_931x_l2_hash_seed(u64 mac, u32 vid);
u32 rtl931x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed);
u64 rtl931x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
u64 rtl931x_read_cam(int idx, struct rtl838x_l2_entry *e);
void rtl931x_write_cam(int idx, struct rtl838x_l2_entry *e);
void rtl931x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
void rtl931x_l2_learning_setup(void);
void rtldsa_931x_enable_learning(int port, bool enable);
void rtldsa_931x_l2_port_new_sa_fwd(int port, enum rtldsa_flood_type mode);
void rtldsa_931x_enable_flood(int port, enum rtldsa_flood_type mode);
void rtldsa_931x_enable_bcast_flood(int port, bool enable);
u64 rtl931x_read_mcast_pmask(int idx);
void rtl931x_write_mcast_pmask(int idx, u64 portmask);
int rtl931x_set_ageing_time(unsigned long msec);
int rtldsa_931x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid);

void rtldsa_l2_dbgfs_init(struct rtl838x_switch_priv *priv);

#endif /* _OTTO_L2_H */
