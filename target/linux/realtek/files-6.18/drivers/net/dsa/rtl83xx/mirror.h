/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_MIRROR_H
#define _OTTO_MIRROR_H

#include <linux/types.h>

/* Port Mirroring */
#define RTL838X_MIR_CTRL			(0x5D00)
#define RTL838X_MIR_DPM_CTRL			(0x5D20)
#define RTL838X_MIR_SPM_CTRL			(0x5D10)

#define RTL839X_MIR_CTRL			(0x2500)
#define RTL839X_MIR_DPM_CTRL			(0x2530)
#define RTL839X_MIR_SPM_CTRL			(0x2510)

struct dsa_switch;
struct dsa_mall_mirror_tc_entry;
struct netlink_ext_ack;

/**
 * struct rtldsa_mirror_config - Mirror configuration for specific group and port
 */
struct rtldsa_mirror_config {
	/** @ctrl: control register for mirroring group */
	int ctrl;

	/** @spm: register for the destination port members */
	int spm;

	/** @dpm: register for the source port members */
	int dpm;

	/** @val: @ctrl register settings to enable mirroring */
	u32 val;
};

int rtldsa_838x_get_mirror_config(struct rtldsa_mirror_config *config, int group, int port);
int rtldsa_839x_get_mirror_config(struct rtldsa_mirror_config *config, int group, int port);
int rtldsa_930x_get_mirror_config(struct rtldsa_mirror_config *config, int group, int port);
int rtldsa_931x_get_mirror_config(struct rtldsa_mirror_config *config, int group, int port);
int rtldsa_port_mirror_add(struct dsa_switch *ds, int port,
			   struct dsa_mall_mirror_tc_entry *mirror,
			   bool ingress, struct netlink_ext_ack *extack);
void rtldsa_port_mirror_del(struct dsa_switch *ds, int port,
			    struct dsa_mall_mirror_tc_entry *mirror);

#endif /* _OTTO_MIRROR_H */
