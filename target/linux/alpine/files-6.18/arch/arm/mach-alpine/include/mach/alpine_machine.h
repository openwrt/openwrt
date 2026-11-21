/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ALPINE_MACHINE_H__
#define __ALPINE_MACHINE_H__

/* Get SerDes group regs base, to be used in relevant Alpine drivers.
 * Valid group is 0..3.
 * Returns virtual base address of the group regs base. */
void __iomem *alpine_serdes_resource_get(u32 group);

/* SerDes ETH mode */
enum alpine_serdes_eth_mode {
	ALPINE_SERDES_ETH_MODE_SGMII,
	ALPINE_SERDES_ETH_MODE_KR,
};

/*
 * Set SerDes ETH mode for an entire group, unless already set
 * Valid group is 0..3.
 * Returns 0 upon success.
 */
int alpine_serdes_eth_mode_set(
	u32				group,
	enum alpine_serdes_eth_mode	mode);

/* Lock the all serdes group for using common registers */
void alpine_serdes_eth_group_lock(u32 group);

/* Unlock the all serdes group for using common registers */
void alpine_serdes_eth_group_unlock(u32 group);

/* Alpine CPU Power Management Services Initialization */
void __init alpine_cpu_pm_init(void);

/* Determine whether Alpine CPU PM services are available */
int alpine_cpu_suspend_wakeup_supported(void);

/* Wake-up a CPU */
void alpine_cpu_wakeup(unsigned int cpu, uintptr_t resume_addr);

/* Power-off a CPU permanently */
void alpine_cpu_die(unsigned int cpu);

/* Kill a CPU */
int alpine_cpu_kill(unsigned int cpu);

/* Suspend a CPU temporarily */
void alpine_cpu_suspend(void);

#endif /* __ALPINE_MACHINE_H__ */
