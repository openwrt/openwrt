/*
 * HND SiliconBackplane PMU support.
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#ifndef _hndpmu_h_
#define _hndpmu_h_

#define SET_LDO_VOLTAGE_LDO1	1
#define SET_LDO_VOLTAGE_LDO2	2
#define SET_LDO_VOLTAGE_LDO3	3
#define SET_LDO_VOLTAGE_PAREF	4

extern void sb_pmu_init(sb_t *sbh, osl_t *osh);
extern void sb_pmu_pll_init(sb_t *sbh, osl_t *osh, uint32 xtalfreq);
extern void sb_pmu_res_init(sb_t *sbh, osl_t *osh);
extern uint32 sb_pmu_force_ilp(sb_t *sbh, osl_t *osh, bool force);
extern uint32 sb_pmu_cpu_clock(sb_t *sbh, osl_t *osh);
extern uint32 sb_pmu_alp_clock(sb_t *sbh, osl_t *osh);

extern void sb_pmu_set_switcher_voltage(sb_t *sbh, osl_t *osh, uint8 bb_voltage, uint8 rf_voltage);
extern void sb_pmu_set_ldo_voltage(sb_t *sbh, osl_t *osh, uint8 ldo, uint8 voltage);
extern void sb_pmu_paref_ldo_enable(sb_t *sbh, osl_t *osh, bool enable);
extern uint16 sb_pmu_fast_pwrup_delay(sb_t *sbh, osl_t *osh);
extern void sb_pmu_otp_power(sb_t *sbh, osl_t *osh, bool on);
extern void sb_pmu_rcal(sb_t *sbh, osl_t *osh);

#endif /* _hndpmu_h_ */
