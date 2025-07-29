/*
 * Copyright (C) 2017 Rockchip Electronics Co. Ltd.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

#ifndef __LINUX_ROCKCHIP_CPU_H
#define __LINUX_ROCKCHIP_CPU_H

#include <linux/of.h>

#define ROCKCHIP_CPU_MASK		0xffff0000
#define ROCKCHIP_CPU_SHIFT		16
#define ROCKCHIP_CPU_PX30		0x33260000
#define ROCKCHIP_CPU_RV1103		0x11030000
#define ROCKCHIP_CPU_RV1106		0x11060000
#define ROCKCHIP_CPU_RV1109		0x11090000
#define ROCKCHIP_CPU_RV1126		0x11260000
#define ROCKCHIP_CPU_RK312X		0x31260000
#define ROCKCHIP_CPU_RK3288		0x32880000
#define ROCKCHIP_CPU_RK3308		0x33080000
#define ROCKCHIP_CPU_RK3528		0x35280000
#define ROCKCHIP_CPU_RK3566		0x35660000
#define ROCKCHIP_CPU_RK3567		0x35670000
#define ROCKCHIP_CPU_RK3568		0x35680000

#if IS_REACHABLE(CONFIG_ROCKCHIP_CPUINFO)

extern unsigned long rockchip_soc_id;

#define ROCKCHIP_CPU_VERION_MASK	0x0000f000
#define ROCKCHIP_CPU_VERION_SHIFT	12

static inline unsigned long rockchip_get_cpu_version(void)
{
	return (rockchip_soc_id & ROCKCHIP_CPU_VERION_MASK)
		>> ROCKCHIP_CPU_VERION_SHIFT;
}

static inline void rockchip_set_cpu_version(unsigned long ver)
{
	rockchip_soc_id &= ~ROCKCHIP_CPU_VERION_MASK;
	rockchip_soc_id |=
		(ver << ROCKCHIP_CPU_VERION_SHIFT) & ROCKCHIP_CPU_VERION_MASK;
}

static inline void rockchip_set_cpu(unsigned long code)
{
	if (!code)
		return;

	rockchip_soc_id &= ~ROCKCHIP_CPU_MASK;
	rockchip_soc_id |= (code << ROCKCHIP_CPU_SHIFT) & ROCKCHIP_CPU_MASK;
}

int rockchip_soc_id_init(void);

#else

#define rockchip_soc_id 0

static inline unsigned long rockchip_get_cpu_version(void)
{
	return 0;
}

static inline void rockchip_set_cpu_version(unsigned long ver)
{
}

static inline void rockchip_set_cpu(unsigned long code)
{
}

static inline int rockchip_soc_id_init(void)
{
	return 0;
}

#endif

static inline bool cpu_is_px30(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_PX30;
	return of_machine_is_compatible("rockchip,px30") ||
	       of_machine_is_compatible("rockchip,px30s") ||
	       of_machine_is_compatible("rockchip,rk3326") ||
	       of_machine_is_compatible("rockchip,rk3326s");
}

static inline bool cpu_is_rv1103(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RV1103;
	return of_machine_is_compatible("rockchip,rv1103");
}

static inline bool cpu_is_rv1106(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RV1106;
	return of_machine_is_compatible("rockchip,rv1106");
}

static inline bool cpu_is_rv1109(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RV1109;
	return of_machine_is_compatible("rockchip,rv1109");
}

static inline bool cpu_is_rv1126(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RV1126;
	return of_machine_is_compatible("rockchip,rv1126");
}

static inline bool cpu_is_rk312x(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RK312X;
	return of_machine_is_compatible("rockchip,rk3126") ||
	       of_machine_is_compatible("rockchip,rk3126b") ||
	       of_machine_is_compatible("rockchip,rk3126c") ||
	       of_machine_is_compatible("rockchip,rk3128");
}

static inline bool cpu_is_rk3288(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RK3288;
	return of_machine_is_compatible("rockchip,rk3288") ||
	       of_machine_is_compatible("rockchip,rk3288w");
}

static inline bool cpu_is_rk3308(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RK3308;

	return of_machine_is_compatible("rockchip,rk3308") ||
	       of_machine_is_compatible("rockchip,rk3308bs");
}

static inline bool cpu_is_rk3528(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RK3528;
	return of_machine_is_compatible("rockchip,rk3528") ||
	       of_machine_is_compatible("rockchip,rk3528a");
}

static inline bool cpu_is_rk3566(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RK3566;
	return of_machine_is_compatible("rockchip,rk3566");
}

static inline bool cpu_is_rk3567(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RK3567;
	return of_machine_is_compatible("rockchip,rk3567");
}

static inline bool cpu_is_rk3568(void)
{
	if (rockchip_soc_id)
		return (rockchip_soc_id & ROCKCHIP_CPU_MASK) == ROCKCHIP_CPU_RK3568;
	return of_machine_is_compatible("rockchip,rk3568");
}

#define ROCKCHIP_SOC_MASK	(ROCKCHIP_CPU_MASK | 0xff)
#define ROCKCHIP_SOC_PX30	(ROCKCHIP_CPU_PX30 | 0x00)
#define ROCKCHIP_SOC_PX30S	(ROCKCHIP_CPU_PX30 | 0x01)
#define ROCKCHIP_SOC_RV1103     (ROCKCHIP_CPU_RV1103 | 0x00)
#define ROCKCHIP_SOC_RV1106     (ROCKCHIP_CPU_RV1106 | 0x00)
#define ROCKCHIP_SOC_RV1109     (ROCKCHIP_CPU_RV1109 | 0x00)
#define ROCKCHIP_SOC_RV1126     (ROCKCHIP_CPU_RV1126 | 0x00)
#define ROCKCHIP_SOC_RK3126     (ROCKCHIP_CPU_RK312X | 0x00)
#define ROCKCHIP_SOC_RK3126B    (ROCKCHIP_CPU_RK312X | 0x10)
#define ROCKCHIP_SOC_RK3126C    (ROCKCHIP_CPU_RK312X | 0x20)
#define ROCKCHIP_SOC_RK3128     (ROCKCHIP_CPU_RK312X | 0x01)
#define ROCKCHIP_SOC_RK3288     (ROCKCHIP_CPU_RK3288 | 0x00)
#define ROCKCHIP_SOC_RK3288W    (ROCKCHIP_CPU_RK3288 | 0x01)
#define ROCKCHIP_SOC_RK3308	(ROCKCHIP_CPU_RK3308 | 0x00)
#define ROCKCHIP_SOC_RK3308B	(ROCKCHIP_CPU_RK3308 | 0x01)
#define ROCKCHIP_SOC_RK3308BS	(ROCKCHIP_CPU_RK3308 | 0x02)
#define ROCKCHIP_SOC_RK3528	(ROCKCHIP_CPU_RK3528 | 0x00)
#define ROCKCHIP_SOC_RK3528A	(ROCKCHIP_CPU_RK3528 | 0x01)
#define ROCKCHIP_SOC_RK3566	(ROCKCHIP_CPU_RK3566 | 0x00)
#define ROCKCHIP_SOC_RK3567	(ROCKCHIP_CPU_RK3567 | 0x00)
#define ROCKCHIP_SOC_RK3568	(ROCKCHIP_CPU_RK3568 | 0x00)

#define ROCKCHIP_SOC(CPU, id, ID) \
static inline bool soc_is_##id(void) \
{ \
	if (rockchip_soc_id) \
		return ((rockchip_soc_id & ROCKCHIP_SOC_MASK) == ROCKCHIP_SOC_ ##ID); \
	return of_machine_is_compatible("rockchip,"#id); \
}

ROCKCHIP_SOC(PX30, px30, PX30)
ROCKCHIP_SOC(PX30, px30s, PX30S)
ROCKCHIP_SOC(RV1106, rv1103, RV1103)
ROCKCHIP_SOC(RV1106, rv1106, RV1106)
ROCKCHIP_SOC(RV1126, rv1109, RV1109)
ROCKCHIP_SOC(RV1126, rv1126, RV1126)
ROCKCHIP_SOC(RK312X, rk3126, RK3126)
ROCKCHIP_SOC(RK312X, rk3126b, RK3126B)
ROCKCHIP_SOC(RK312X, rk3126c, RK3126C)
ROCKCHIP_SOC(RK312X, rk3128, RK3128)
ROCKCHIP_SOC(RK3288, rk3288, RK3288)
ROCKCHIP_SOC(RK3288, rk3288w, RK3288W)
ROCKCHIP_SOC(RK3308, rk3308, RK3308)
ROCKCHIP_SOC(RK3308, rk3308b, RK3308B)
ROCKCHIP_SOC(RK3308, rk3308bs, RK3308BS)
ROCKCHIP_SOC(RK3528, rk3528, RK3528)
ROCKCHIP_SOC(RK3528, rk3528a, RK3528A)
ROCKCHIP_SOC(RK3568, rk3566, RK3566)
ROCKCHIP_SOC(RK3567, rk3567, RK3567)
ROCKCHIP_SOC(RK3568, rk3568, RK3568)

#endif
