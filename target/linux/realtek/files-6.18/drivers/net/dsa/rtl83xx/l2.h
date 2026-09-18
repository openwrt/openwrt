/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_L2_H
#define _OTTO_L2_H

#include "rtl-otto.h"

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

#endif /* _OTTO_L2_H */
