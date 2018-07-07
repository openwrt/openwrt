/*
 *  Realtek RLX based SoC specific CPU feature overrides
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */
#ifndef __ASM_MACH_REALTEK_CPU_FEATURE_OVERRIDES_H
#define __ASM_MACH_REALTEK_CPU_FEATURE_OVERRIDES_H

#define cpu_has_tlb			1
#define cpu_has_4kex			0
#define cpu_has_3k_cache		1
#define cpu_has_4k_cache		0
#define cpu_has_6k_cache		0
#define cpu_has_8k_cache		0
#define cpu_has_tx39_cache		0
#define cpu_has_octeon_cache		0
#define cpu_has_sb1_cache		0
#define cpu_has_fpu			0
#define cpu_has_32fpr			0
#define cpu_has_counter			0
#define cpu_has_watch			0
#define cpu_has_divec			0
#define cpu_has_vce 			0
#define cpu_has_mcheck			0
#define cpu_has_nofpuex			0

#define cpu_has_prefetch		0
#define cpu_has_ejtag			1
#define cpu_has_llsc			0

#define cpu_has_mips16			1
#define cpu_has_mdmx			0
#define cpu_has_mips3d			0
#define cpu_has_smartmips		0

#define cpu_has_dc_aliases		0
#define cpu_has_vtag_icache		0
#define cpu_has_cache_cdex_p		0
#define cpu_has_cache_cdex_s		0
#define cpu_has_ic_fills_f_dc		0
#define cpu_has_pindexed_dcache		0
#define cpu_has_inclusive_pcaches	0
#define cpu_icache_snoops_remote_store	1

#define cpu_has_mips32r1		0
#define cpu_has_mips32r2		0
#define cpu_has_mips64r1		0
#define cpu_has_mips64r2		0

#define cpu_has_clo_clz			0
#define cpu_has_dsp			0
#define cpu_has_mipsmt			0
#define cpu_has_userlocal		0

#define cpu_has_64bits			0
#define cpu_has_64bit_zero_reg		0
#define cpu_has_64bit_gp_regs		0
#define cpu_has_64bit_addresses		0

#endif /* __ASM_MACH_REALTEK_CPU_FEATURE_OVERRIDES_H */
