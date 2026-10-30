/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_HAL_NB_REGS_H__
#define __AL_HAL_NB_REGS_H__

/*
* Unit Registers
*/

struct al_nb_global {
	/* [0x0]  */
	uint32_t cpus_config;
	/* [0x4]  */
	uint32_t cpus_secure;
	/* [0x8] Force init reset. */
	uint32_t cpus_init_control;
	/* [0xc] Force init reset per decei mode. */
	uint32_t cpus_init_status;
	/* [0x10]  */
	uint32_t nb_int_cause;
	/* [0x14]  */
	uint32_t sev_int_cause;
	/* [0x18]  */
	uint32_t pmus_int_cause;
	/* [0x1c]  */
	uint32_t sev_mask;
	/* [0x20]  */
	uint32_t cpus_hold_reset;
	/* [0x24]  */
	uint32_t cpus_software_reset;
	/* [0x28]  */
	uint32_t wd_timer0_reset;
	/* [0x2c]  */
	uint32_t wd_timer1_reset;
	/* [0x30]  */
	uint32_t wd_timer2_reset;
	/* [0x34]  */
	uint32_t wd_timer3_reset;
	/* [0x38]  */
	uint32_t ddrc_hold_reset;
	/* [0x3c]  */
	uint32_t fabric_software_reset;
	/* [0x40]  */
	uint32_t cpus_power_ctrl;
	uint32_t rsrvd_0[7];
	/* [0x60]  */
	uint32_t acf_base_high;
	/* [0x64]  */
	uint32_t acf_base_low;
	/* [0x68]  */
	uint32_t acf_control_override;
	/* [0x6c] Read-only that reflects CPU Cluster Local GIC base  ... */
	uint32_t lgic_base_high;
	/* [0x70] Read-only that reflects CPU Cluster Local GIC base  ... */
	uint32_t lgic_base_low;
	/* [0x74] Read-only that reflects the device's IOGIC base hig ... */
	uint32_t iogic_base_high;
	/* [0x78] Read-only that reflects IOGIC base low address  */
	uint32_t iogic_base_low;
	/* [0x7c]  */
	uint32_t io_wr_split_control;
	/* [0x80]  */
	uint32_t io_rd_rob_control;
	/* [0x84]  */
	uint32_t sb_pos_error_log_1;
	/* [0x88]  */
	uint32_t sb_pos_error_log_0;
	/* [0x8c]  */
	uint32_t c2swb_config;
	/* [0x90]  */
	uint32_t msix_error_log;
	/* [0x94]  */
	uint32_t error_cause;
	/* [0x98]  */
	uint32_t error_mask;
	uint32_t rsrvd_1;
	/* [0xa0]  */
	uint32_t qos_peak_control;
	/* [0xa4]  */
	uint32_t qos_set_control;
	/* [0xa8]  */
	uint32_t ddr_qos;
	uint32_t rsrvd_2[9];
	/* [0xd0]  */
	uint32_t acf_misc;
	/* [0xd4]  */
	uint32_t config_bus_control;
	uint32_t rsrvd_3[10];
	/* [0x100]  */
	uint32_t cpu_max_pd_timer;
	/* [0x104]  */
	uint32_t cpu_max_pu_timer;
	uint32_t rsrvd_4[2];
	/* [0x110]  */
	uint32_t auto_ddr_self_refresh_counter;
	uint32_t rsrvd_5[3];
	/* [0x120]  */
	uint32_t coresight_pd;
	/* [0x124]  */
	uint32_t coresight_internal_0;
	/* [0x128]  */
	uint32_t coresight_dbgromaddr;
	/* [0x12c]  */
	uint32_t coresight_dbgselfaddr;
	/* [0x130]  */
	uint32_t coresght_targetid;
	/* [0x134]  */
	uint32_t coresght_targetid0;
	uint32_t rsrvd[946];
};
struct al_nb_system_counter {
	/* [0x0]  */
	uint32_t cnt_control;
	/* [0x4]  */
	uint32_t cnt_base_freq;
	/* [0x8]  */
	uint32_t cnt_low;
	/* [0xc]  */
	uint32_t cnt_high;
	/* [0x10]  */
	uint32_t cnt_init_low;
	/* [0x14]  */
	uint32_t cnt_init_high;
	uint32_t rsrvd[58];
};
struct al_nb_rams_control_misc {
	/* [0x0]  */
	uint32_t ca15_rf_misc;
	uint32_t rsrvd_0;
	/* [0x8]  */
	uint32_t nb_rf_misc;
	uint32_t rsrvd[61];
};
struct al_nb_ca15_rams_control {
	/* [0x0]  */
	uint32_t rf_0;
	/* [0x4]  */
	uint32_t rf_1;
	/* [0x8]  */
	uint32_t rf_2;
	uint32_t rsrvd;
};
struct al_nb_semaphores {
	/* [0x0] This configration is only sampled during reset of t ... */
	uint32_t lockn;
};
struct al_nb_debug {
	/* [0x0]  */
	uint32_t ca15_outputs_1;
	/* [0x4]  */
	uint32_t ca15_outputs_2;
	uint32_t rsrvd_0[2];
	/* [0x10]  */
	uint32_t cpu_msg[4];
	/* [0x20]  */
	uint32_t rsv0_config;
	/* [0x24]  */
	uint32_t rsv1_config;
	uint32_t rsrvd_1[2];
	/* [0x30]  */
	uint32_t rsv0_status;
	/* [0x34]  */
	uint32_t rsv1_status;
	uint32_t rsrvd_2[2];
	/* [0x40]  */
	uint32_t ddrc;
	/* [0x44]  */
	uint32_t ddrc_phy_smode_control;
	/* [0x48]  */
	uint32_t ddrc_phy_smode_status;
	uint32_t rsrvd_3[5];
	/* [0x60]  */
	uint32_t pmc;
	uint32_t rsrvd_4[3];
	/* [0x70]  */
	uint32_t cpus_general;
	uint32_t rsrvd_5[3];
	/* [0x80]  */
	uint32_t cpus_int_out;
	uint32_t rsrvd_6[31];
	/* [0x100]  */
	uint32_t track_dump_ctrl;
	/* [0x104]  */
	uint32_t track_dump_rdata_0;
	/* [0x108]  */
	uint32_t track_dump_rdata_1;
	uint32_t rsrvd_7[5];
	/* [0x120]  */
	uint32_t track_events;
	uint32_t rsrvd_8[3];
	/* [0x130]  */
	uint32_t pos_track_dump_ctrl;
	/* [0x134]  */
	uint32_t pos_track_dump_rdata_0;
	/* [0x138]  */
	uint32_t pos_track_dump_rdata_1;
	uint32_t rsrvd_9;
	/* [0x140]  */
	uint32_t c2swb_track_dump_ctrl;
	/* [0x144]  */
	uint32_t c2swb_track_dump_rdata_0;
	/* [0x148]  */
	uint32_t c2swb_track_dump_rdata_1;
	uint32_t rsrvd_10[5];
	/* [0x160]  */
	uint32_t c2swb_bar_ovrd_high;
	/* [0x164]  */
	uint32_t c2swb_bar_ovrd_low;
	uint32_t rsrvd[38];
};
struct al_nb_cpun_config_status {
	/* [0x0] This configration is only sampled during reset of t ... */
	uint32_t config;
	uint32_t rsrvd_0;
	/* [0x8]  */
	uint32_t local_cause_mask;
	uint32_t rsrvd_1;
	/* [0x10]  */
	uint32_t pmus_cause_mask;
	uint32_t rsrvd_2[3];
	/* [0x20] Specifies the state of the CPU with reference to po ... */
	uint32_t power_ctrl;
	/* [0x24]  */
	uint32_t power_status;
	/* [0x28]  */
	uint32_t resume_addr_l;
	/* [0x2c]  */
	uint32_t resume_addr_h;
	uint32_t rsrvd[52];
};
struct al_nb_mc_pmu {
	/* [0x0] PMU Global Control Register */
	uint32_t pmu_control;
	/* [0x4] PMU Global Control Register */
	uint32_t overflow;
	uint32_t rsrvd[62];
};
struct al_nb_mc_pmu_counters {
	/* [0x0] Counter Configuration Register */
	uint32_t cfg;
	/* [0x4] Counter Control Register */
	uint32_t cntl;
	/* [0x8] Counter Control Register */
	uint32_t low;
	/* [0xc] Counter Control Register */
	uint32_t high;
	uint32_t rsrvd[4];
};
struct al_nb_nb_version {
	/* [0x0] Northbridge Revision */
	uint32_t version;
	uint32_t rsrvd;
};
struct al_nb_sriov {
	/* [0x0]  */
	uint32_t cpu_vmid[4];
	uint32_t rsrvd[4];
};
union al_nb_pcie_logging {
	struct {
		/* [0x0]  */
		uint32_t control;
		uint32_t rsrvd_0[3];
		/* [0x10]  */
		uint32_t wr_window_low;
		/* [0x14]  */
		uint32_t wr_window_high;
		/* [0x18]  */
		uint32_t wr_window_size;
		uint32_t rsrvd_1;
		/* [0x20]  */
		uint32_t fifo_base;
		/* [0x24]  */
		uint32_t fifo_size;
		/* [0x28]   */
		uint32_t fifo_head;
		/* [0x2c]  */
		uint32_t fifo_tail;
		/* [0x30]  */
		uint32_t wr_window_low_1;
		/* [0x34]  */
		uint32_t wr_window_high_1;
		/* [0x38]  */
		uint32_t wr_window_size_1;
		uint32_t rsrvd_2;
		/* [0x40]  */
		uint32_t fifo_base_1;
		/* [0x44]  */
		uint32_t fifo_size_1;
		/* [0x48]   */
		uint32_t fifo_head_1;
		/* [0x4c]  */
		uint32_t fifo_tail_1;
		/* [0x50]  */
		uint32_t rd_window_low;
		/* [0x54]  */
		uint32_t rd_window_high;
		/* [0x58]  */
		uint32_t rd_window_size;
		/* [0x5c]  */
		uint32_t read_latch;
		/* [0x60]  */
		uint32_t rd_window_low_1;
		/* [0x64]  */
		uint32_t rd_window_high_1;
		/* [0x68]  */
		uint32_t rd_window_size_1;
		/* [0x6c]  */
		uint32_t read_latch_1;
		/* [0x70]  */
		uint32_t read_latch_timeout;
		uint32_t rsrvd[35];
	} a0;
	struct {
		uint32_t control;
		uint32_t read_latch;
		uint32_t window_low;
		uint32_t rsrvd_0;
		uint32_t window_high;
		uint32_t fifo_base;
		uint32_t fifo_size;
		uint32_t fifo_head;             /*   */
		uint32_t fifo_tail;
		uint32_t rsrvd[55];
	} m0;
};

struct al_nb_regs {
	struct al_nb_global global;                             /* [0x0] */
	struct al_nb_system_counter system_counter;             /* [0x1000] */
	struct al_nb_rams_control_misc rams_control_misc;       /* [0x1100] */
	struct al_nb_ca15_rams_control ca15_rams_control[5];    /* [0x1200] */
	uint32_t rsrvd_0[108];
	struct al_nb_semaphores semaphores[64];                 /* [0x1400] */
	uint32_t rsrvd_1[320];
	struct al_nb_debug debug;                               /* [0x1a00] */
	uint32_t rsrvd_2[256];
	struct al_nb_cpun_config_status cpun_config_status[4];  /* [0x2000] */
	uint32_t rsrvd_3[1792];
	struct al_nb_mc_pmu mc_pmu;                             /* [0x4000] */
	struct al_nb_mc_pmu_counters mc_pmu_counters[4];        /* [0x4100] */
	uint32_t rsrvd_4[160];
	struct al_nb_nb_version nb_version;                     /* [0x4400] */
	uint32_t rsrvd_5[126];
	struct al_nb_sriov sriov;                               /* [0x4600] */
	uint32_t rsrvd_6[632];
	union al_nb_pcie_logging pcie_logging;                  /* [0x5000] */
};

/*
* Registers Fields
*/

/**** Coresight_INTERNAL_0 register ****/

/**** CA15_RF_Misc register ****/

/**** CA15_outputs_1 register ****/

/**** CA15_outputs_2 register ****/

#endif /* __AL_HAL_NB_REG_H */

/** @} end of ... group */
