/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/


#ifndef __AL_HAL_NB_REGS_H__
#define __AL_HAL_NB_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif
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


/**** CPUs_Config register ****/
/* Disable broadcast of barrier onto system bus */
#define NB_GLOBAL_CPUS_CONFIG_SYSBARDISABLE (1 << 0)
/* Enable broadcast of inner shareable transactions from CPUs */
#define NB_GLOBAL_CPUS_CONFIG_BROADCASTINNER (1 << 1)
/* Disable broadcast of cache maintanance system bus */
#define NB_GLOBAL_CPUS_CONFIG_BROADCASTCACHEMAINT (1 << 2)
/* Enable broadcast of outer shareable transactions from CPUs */
#define NB_GLOBAL_CPUS_CONFIG_BROADCASTOUTER (1 << 3)
/* Defines the internal CPU GIC operating frequency ratio with t ... */
#define NB_GLOBAL_CPUS_CONFIG_PERIPHCLKEN_MASK 0x00000030
#define NB_GLOBAL_CPUS_CONFIG_PERIPHCLKEN_SHIFT 4

/**** CPUs_Secure register ****/
/* dbgen
Write once. */
#define NB_GLOBAL_CPUS_SECURE_DBGEN      (1 << 0)
/* niden
Write once. */
#define NB_GLOBAL_CPUS_SECURE_NIDEN      (1 << 1)
/* spiden
Write once. */
#define NB_GLOBAL_CPUS_SECURE_SPIDEN     (1 << 2)
/* spniden
Write once. */
#define NB_GLOBAL_CPUS_SECURE_SPNIDEN    (1 << 3)
/* Disable write access to some secure GIC registers */
#define NB_GLOBAL_CPUS_SECURE_CFGSDISABLE (1 << 4)

/**** CPUs_Init_Control register ****/
/* CPU Init DoneSpecifies which CPUs' inits are done and can exi ... */
#define NB_GLOBAL_CPUS_INIT_CONTROL_CPUS_INITDONE_MASK 0x0000000F
#define NB_GLOBAL_CPUS_INIT_CONTROL_CPUS_INITDONE_SHIFT 0
/* DBGPWRDNREQ MaskWhen CPU does not exist, its dbgpwrdnreq must ... */
#define NB_GLOBAL_CPUS_INIT_CONTROL_DBGPWRDNREQ_MASK_MASK 0x000000F0
#define NB_GLOBAL_CPUS_INIT_CONTROL_DBGPWRDNREQ_MASK_SHIFT 4
/* Force CPU init power-on-reset exit.
For debug purposes only. */
#define NB_GLOBAL_CPUS_INIT_CONTROL_FORCE_CPUPOR_MASK 0x00000F00
#define NB_GLOBAL_CPUS_INIT_CONTROL_FORCE_CPUPOR_SHIFT 8

/**** CPUs_Init_Status register ****/
/* Specifies which CPUs are enabled in the device configurations ... */
#define NB_GLOBAL_CPUS_INIT_STATUS_CPUS_EXIST_MASK 0x0000000F
#define NB_GLOBAL_CPUS_INIT_STATUS_CPUS_EXIST_SHIFT 0

/**** NB_Int_Cause register ****/
/*
 * Each bit corresponds to an IRQ.
 * value is 1 for level irq, 0 for trigger irq
 * Level IRQ indices: 12-13, 23, 24, 26-29
 */
#define NB_GLOBAL_NB_INT_CAUSE_LEVEL_IRQ_MASK	0x3D803000
/* Cross trigger interrupt  */
#define NB_GLOBAL_NB_INT_CAUSE_NCTIIRQ_MASK 0x0000000F
#define NB_GLOBAL_NB_INT_CAUSE_NCTIIRQ_SHIFT 0
/* Communications channel receive */
#define NB_GLOBAL_NB_INT_CAUSE_COMMRX_MASK 0x000000F0
#define NB_GLOBAL_NB_INT_CAUSE_COMMRX_SHIFT 4
/* Communication channel transmit */
#define NB_GLOBAL_NB_INT_CAUSE_COMMTX_MASK 0x00000F00
#define NB_GLOBAL_NB_INT_CAUSE_COMMTX_SHIFT 8
/* Emulation write fifo log has valid entry */
#define NB_GLOBAL_NB_INT_CAUSE_PCIE_LOG_FIFO_VALID_0 (1 << 12)
/* Write logging FIFO wrap occurred */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_WRAP_M0 (1 << 13)
/* Emulation write fifo log has valid entry */
#define NB_GLOBAL_NB_INT_CAUSE_PCIE_LOG_FIFO_VALID_1_A0 (1 << 13)
/* Write logging FIFO is full */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_FULL_M0 (1 << 14)
/* Reserved, read undefined must write as zeros. */
#define NB_GLOBAL_NB_INT_CAUSE_RESERVED_15_15 (1 << 15)
/* Error indicator for AXI write transactions with a BRESP error ... */
#define NB_GLOBAL_NB_INT_CAUSE_CPU_AXIERRIRQ (1 << 16)
/* Error indicator for: L2 RAM double-bit ECC error, illegal wri ... */
#define NB_GLOBAL_NB_INT_CAUSE_CPU_INTERRIRQ (1 << 17)
/* Coherent fabric error summary interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_ACF_ERRORIRQ (1 << 18)
/* DDR Controller ECC Correctable error summary interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_CORR_ERR (1 << 19)
/* DDR Controller ECC Uncorrectable error summary interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_UNCORR_ERR (1 << 20)
/* DRAM parity error interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_MCTL_PARITY_ERR (1 << 21)
/* Reserved, not functional */
#define NB_GLOBAL_NB_INT_CAUSE_MCTL_WDATARAM_PAR (1 << 22)
/* Error cause summary interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM_A0 (1 << 23)
/* SB PoS error */
#define NB_GLOBAL_NB_INT_CAUSE_SB_POS_ERR (1 << 24)
/* Received msix is not mapped to local GIC or IO-GIC spin */
#define NB_GLOBAL_NB_INT_CAUSE_MSIX_ERR_INT_M0 (1 << 25)
/* Coresight timestamp overflow */
#define NB_GLOBAL_NB_INT_CAUSE_CORESIGHT_TS_OVERFLOW_M0 (1 << 26)
/* Emulation write fifo log is wrapped */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_WRAP_A0 (1 << 26)
/* Write data parity error from SB channel 0. */
#define NB_GLOBAL_NB_INT_CAUSE_SB0_WRDATA_PERR_M0 (1 << 27)
/* Emulation write fifo log is full (new pushes might corrupt da ... */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_FULL_A0 (1 << 27)
/* Write data parity error from SB channel 1. */
#define NB_GLOBAL_NB_INT_CAUSE_SB1_WRDATA_PERR_M0 (1 << 28)
/* Emulation write fifo log is wrapped */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_WRAP_1_A0 (1 << 28)
/* Read data parity error from SB slaves. */
#define NB_GLOBAL_NB_INT_CAUSE_SB_SLV_RDATA_PERR_M0 (1 << 29)
/* Emulation write fifo log is full (new pushes might corrupt da ... */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_FULL_1_A0 (1 << 29)
/* PCIe read latched */
#define NB_GLOBAL_NB_INT_CAUSE_RD_LOG_SET_0 (1 << 30)
/* PCIe read latched */
#define NB_GLOBAL_NB_INT_CAUSE_RD_LOG_SET_1_A0 (1 << 31)

/**** SEV_Int_Cause register ****/
/* SMMU 0/1 global non-secure fault interrupt */
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_GBL_FLT_IRPT_NS_MASK 0x00000003
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_GBL_FLT_IRPT_NS_SHIFT 0
/* SMMU 0/1 non-secure context interrupt */
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_CXT_IRPT_NS_MASK 0x0000000C
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_CXT_IRPT_NS_SHIFT 2
/* SMMU0/1 Non-secure configurtion acess fault interrupt */
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_CFG_FLT_IRPT_S_MASK 0x00000030
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_CFG_FLT_IRPT_S_SHIFT 4
/* Reserved. Read undefined; must write as zeros. */
#define NB_GLOBAL_SEV_INT_CAUSE_RESERVED_11_6_MASK 0x00000FC0
#define NB_GLOBAL_SEV_INT_CAUSE_RESERVED_11_6_SHIFT 6
/* PCIe emulation: inbound writes fifo has valid entry */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_VALID_0 (1 << 12)
/* PCIe emulation: inbound writes fifo has being wrapped (tail p ... */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_WRAP_0 (1 << 13)
/* PCIe emulation: inbound writes fifo is full */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_FULL_0 (1 << 14)
/* PCIe emulation: inbound writes fifo has valid entry */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_VALID_1 (1 << 15)
/* PCIe emulation: inbound writes fifo has being wrapped (tail p ... */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_WRAP_1 (1 << 16)
/* PCIe emulation: inbound writes fifo is full */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_FULL_1 (1 << 17)
/* PCIe emulation: inbound pcie read is latched */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_RD_LOG_SET_0 (1 << 18)
/* PCIe emulation: inbound pcie read is latched */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_RD_LOG_SET_1 (1 << 19)
/* Reserved. Read undefined; must write as zeros. */
#define NB_GLOBAL_SEV_INT_CAUSE_RESERVED_31_20_MASK 0xFFF00000
#define NB_GLOBAL_SEV_INT_CAUSE_RESERVED_31_20_SHIFT 20

/**** PMUs_Int_Cause register ****/
/* CPUs PMU Overflow interrupt */
#define NB_GLOBAL_PMUS_INT_CAUSE_CPUS_OVFL_MASK 0x0000000F
#define NB_GLOBAL_PMUS_INT_CAUSE_CPUS_OVFL_SHIFT 0
/* Northbridge PMU overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_NB_OVFL (1 << 4)
/* Memory Controller PMU overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_MCTL_OVFL (1 << 5)
/* Coherency Interconnect PMU overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_CCI_OVFL_MASK 0x000007C0
#define NB_GLOBAL_PMUS_INT_CAUSE_CCI_OVFL_SHIFT 6
/* Coherency Interconnect PMU overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_SMMU_OVFL_MASK 0x00001800
#define NB_GLOBAL_PMUS_INT_CAUSE_SMMU_OVFL_SHIFT 11
/* Reserved. Read undefined; must write as zeros. */
#define NB_GLOBAL_PMUS_INT_CAUSE_RESERVED_23_13_MASK 0x00FFE000
#define NB_GLOBAL_PMUS_INT_CAUSE_RESERVED_23_13_SHIFT 13
/* Southbridge PMUs overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_SB_PMUS_OVFL_MASK 0xFF000000
#define NB_GLOBAL_PMUS_INT_CAUSE_SB_PMUS_OVFL_SHIFT 24

/**** CPUs_Hold_Reset register ****/
/* Shared L2 memory system, interrupt controller and timer logic ... */
#define NB_GLOBAL_CPUS_HOLD_RESET_L2RESET (1 << 0)
/* Shared debug domain reset */
#define NB_GLOBAL_CPUS_HOLD_RESET_PRESETDBG (1 << 1)
/* Individual CPU debug, PTM, watchpoint and breakpoint logic re ... */
#define NB_GLOBAL_CPUS_HOLD_RESET_CPU_DBGRESET_MASK 0x000000F0
#define NB_GLOBAL_CPUS_HOLD_RESET_CPU_DBGRESET_SHIFT 4
/* Individual CPU core and VFP/NEON logic reset */
#define NB_GLOBAL_CPUS_HOLD_RESET_CPU_CORERESET_MASK 0x00000F00
#define NB_GLOBAL_CPUS_HOLD_RESET_CPU_CORERESET_SHIFT 8
/* Individual CPU por-on-reset */
#define NB_GLOBAL_CPUS_HOLD_RESET_CPU_PORESET_MASK 0x0000F000
#define NB_GLOBAL_CPUS_HOLD_RESET_CPU_PORESET_SHIFT 12
/* Wait for interrupt mask */
#define NB_GLOBAL_CPUS_HOLD_RESET_WFI_MASK_MASK 0x000F0000
#define NB_GLOBAL_CPUS_HOLD_RESET_WFI_MASK_SHIFT 16

/**** CPUs_Software_Reset register ****/
/* Write 1. Apply the software reset. */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_SWRESET_REQ (1 << 0)
/* Defines the level of software reset. */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_MASK 0x0000000E
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT 1
/* Individual CPU core reset. */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CPU_CORE \
		(0x0 << NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT)
/* Individual CPU power-on-reset. */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CPU_PORESET \
		(0x1 << NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT)
/* Individual CPU debug reset. */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CPU_DBG \
		(0x2 << NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT)
/* A Cluster reset puts each core into core reset (no dbg) and a ... */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CLUSTER_NO_DBG \
		(0x3 << NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT)
/* A Cluster reset puts each core into power-on-reset and also r ... */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CLUSTER \
		(0x4 << NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT)
/* A Cluster power-on-reset puts each core into power-on-reset a ... */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CLUSTER_PORESET \
		(0x5 << NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT)
/* Defines which cores to reset when no cluster_poreset is reque ... */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_CORES_MASK 0x000000F0
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_CORES_SHIFT 4
/* CPUn wait for interrupt enable */
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_WFI_MASK_MASK 0x000F0000
#define NB_GLOBAL_CPUS_SOFTWARE_RESET_WFI_MASK_SHIFT 16

/**** WD_Timer0_Reset register ****/
/* Shared L2 memory system, interrupt controller and timer logic ... */
#define NB_GLOBAL_WD_TIMER0_RESET_L2RESET (1 << 0)
/* Shared debug domain reset */
#define NB_GLOBAL_WD_TIMER0_RESET_PRESETDBG (1 << 1)
/* Individual CPU debug PTM, watchpoint and breakpoint logic res ... */
#define NB_GLOBAL_WD_TIMER0_RESET_CPU_DBGRESET_MASK 0x000000F0
#define NB_GLOBAL_WD_TIMER0_RESET_CPU_DBGRESET_SHIFT 4
/* Individual CPU core and VFP/NEON logic reset */
#define NB_GLOBAL_WD_TIMER0_RESET_CPU_CORERESET_MASK 0x00000F00
#define NB_GLOBAL_WD_TIMER0_RESET_CPU_CORERESET_SHIFT 8
/* Individual CPU por-on-reset */
#define NB_GLOBAL_WD_TIMER0_RESET_CPU_PORESET_MASK 0x0000F000
#define NB_GLOBAL_WD_TIMER0_RESET_CPU_PORESET_SHIFT 12

/**** WD_Timer1_Reset register ****/
/* Shared L2 memory system, interrupt controller and timer logic ... */
#define NB_GLOBAL_WD_TIMER1_RESET_L2RESET (1 << 0)
/* Shared debug domain reset */
#define NB_GLOBAL_WD_TIMER1_RESET_PRESETDBG (1 << 1)
/* Individual CPU debug PTM, watchpoint and breakpoint logic res ... */
#define NB_GLOBAL_WD_TIMER1_RESET_CPU_DBGRESET_MASK 0x000000F0
#define NB_GLOBAL_WD_TIMER1_RESET_CPU_DBGRESET_SHIFT 4
/* Individual CPU core and VFP/NEON logic reset */
#define NB_GLOBAL_WD_TIMER1_RESET_CPU_CORERESET_MASK 0x00000F00
#define NB_GLOBAL_WD_TIMER1_RESET_CPU_CORERESET_SHIFT 8
/* Individual CPU por-on-reset */
#define NB_GLOBAL_WD_TIMER1_RESET_CPU_PORESET_MASK 0x0000F000
#define NB_GLOBAL_WD_TIMER1_RESET_CPU_PORESET_SHIFT 12

/**** WD_Timer2_Reset register ****/
/* Shared L2 memory system, interrupt controller and timer logic ... */
#define NB_GLOBAL_WD_TIMER2_RESET_L2RESET (1 << 0)
/* Shared debug domain reset */
#define NB_GLOBAL_WD_TIMER2_RESET_PRESETDBG (1 << 1)
/* Individual CPU debug, PTM, watchpoint and breakpoint logic re ... */
#define NB_GLOBAL_WD_TIMER2_RESET_CPU_DBGRESET_MASK 0x000000F0
#define NB_GLOBAL_WD_TIMER2_RESET_CPU_DBGRESET_SHIFT 4
/* Individual CPU core and VFP/NEON logic reset */
#define NB_GLOBAL_WD_TIMER2_RESET_CPU_CORERESET_MASK 0x00000F00
#define NB_GLOBAL_WD_TIMER2_RESET_CPU_CORERESET_SHIFT 8
/* Individual CPU por-on-reset */
#define NB_GLOBAL_WD_TIMER2_RESET_CPU_PORESET_MASK 0x0000F000
#define NB_GLOBAL_WD_TIMER2_RESET_CPU_PORESET_SHIFT 12

/**** WD_Timer3_Reset register ****/
/* Shared L2 memory system, interrupt controller and timer logic ... */
#define NB_GLOBAL_WD_TIMER3_RESET_L2RESET (1 << 0)
/* Shared debug domain reset */
#define NB_GLOBAL_WD_TIMER3_RESET_PRESETDBG (1 << 1)
/* Individual CPU debug, PTM, watchpoint and breakpoint logic re ... */
#define NB_GLOBAL_WD_TIMER3_RESET_CPU_DBGRESET_MASK 0x000000F0
#define NB_GLOBAL_WD_TIMER3_RESET_CPU_DBGRESET_SHIFT 4
/* Individual CPU core and VFP/NEON logic reset */
#define NB_GLOBAL_WD_TIMER3_RESET_CPU_CORERESET_MASK 0x00000F00
#define NB_GLOBAL_WD_TIMER3_RESET_CPU_CORERESET_SHIFT 8
/* Individual CPU por-on-reset */
#define NB_GLOBAL_WD_TIMER3_RESET_CPU_PORESET_MASK 0x0000F000
#define NB_GLOBAL_WD_TIMER3_RESET_CPU_PORESET_SHIFT 12

/**** DDRC_Hold_Reset register ****/
/* DDR Control and PHY memory mapped registers reset control0 -  ... */
#define NB_GLOBAL_DDRC_HOLD_RESET_APB_SYNC_RESET (1 << 0)
/* DDR Control Core reset control0 - Reset is deasserted */
#define NB_GLOBAL_DDRC_HOLD_RESET_CORE_SYNC_RESET (1 << 1)
/* DDR Control AXI Interface reset control0 - Reset is deasserte ... */
#define NB_GLOBAL_DDRC_HOLD_RESET_AXI_SYNC_RESET (1 << 2)
/* DDR PUB Controller reset control0 - Reset is deasserted */
#define NB_GLOBAL_DDRC_HOLD_RESET_PUB_CTL_SYNC_RESET (1 << 3)
/* DDR PUB SDR Controller reset control0 - Reset is deasserted */
#define NB_GLOBAL_DDRC_HOLD_RESET_PUB_SDR_SYNC_RESET (1 << 4)
/* DDR PHY reset control0 - Reset is deasserted */
#define NB_GLOBAL_DDRC_HOLD_RESET_PHY_SYNC_RESET (1 << 5)
/* Memory initialization input to DDR SRAM for parity check supp ... */
#define NB_GLOBAL_DDRC_HOLD_RESET_DDR_UNIT_MEM_INIT (1 << 6)

/**** Fabric_Software_Reset register ****/
/* Write 1 apply the software reset. */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_SWRESET_REQ (1 << 0)
/* Defines the level of software reset. */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_MASK 0x0000000E
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_SHIFT 1
/* Fabric reset */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_FABRIC \
		(0x0 << NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_SHIFT)
/* GIC reset */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_GIC \
		(0x1 << NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_SHIFT)
/* SMMU reset */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_SMMU \
		(0x2 << NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_SHIFT)
/* CPUn waiting for interrupt enable */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_WFI_MASK_MASK 0x000F0000
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_WFI_MASK_SHIFT 16

/**** CPUs_Power_Ctrl register ****/
/* L2 WFI enableWhen all the processors are in WFI mode or power ... */
#define NB_GLOBAL_CPUS_POWER_CTRL_L2WFI_EN (1 << 0)
/* L2 WFI status */
#define NB_GLOBAL_CPUS_POWER_CTRL_L2WFI_STATUS (1 << 1)
/* L2 RAMs Power DownPower down the L2 RAMs */
#define NB_GLOBAL_CPUS_POWER_CTRL_L2RAMS_PWRDN_EN (1 << 2)
/* L2 RAMs power down status */
#define NB_GLOBAL_CPUS_POWER_CTRL_L2RAMS_PWRDN_STATUS (1 << 3)
/* CPU state condition to enable L2 RAM power down0 - Power down ... */
#define NB_GLOBAL_CPUS_POWER_CTRL_L2RAMS_PWRDN_CPUS_STATE_MASK 0x000000F0
#define NB_GLOBAL_CPUS_POWER_CTRL_L2RAMS_PWRDN_CPUS_STATE_SHIFT 4
/* Enable external debugger over power-down */
#define NB_GLOBAL_CPUS_POWER_CTRL_EXT_DEBUGGER_OVER_PD_EN (1 << 8)
/* force wakeup the CPU in L2RAM powedwnINTERNAL DEBUG PURPOSE O ... */
#define NB_GLOBAL_CPUS_POWER_CTRL_FORCE_CPUS_OK_PWRUP (1 << 27)
/* L2 RAMs power down SM status */
#define NB_GLOBAL_CPUS_POWER_CTRL_L2RAMS_PWRDN_SM_STATUS_MASK 0xF0000000
#define NB_GLOBAL_CPUS_POWER_CTRL_L2RAMS_PWRDN_SM_STATUS_SHIFT 28

/**** ACF_Base_High register ****/
/* Coherency Fabric registers base [39:32]. */
#define NB_GLOBAL_ACF_BASE_HIGH_BASE_39_32_MASK 0x000000FF
#define NB_GLOBAL_ACF_BASE_HIGH_BASE_39_32_SHIFT 0
/* Coherency Fabric registers base [31:15] */
#define NB_GLOBAL_ACF_BASE_LOW_BASED_31_15_MASK 0xFFFF8000
#define NB_GLOBAL_ACF_BASE_LOW_BASED_31_15_SHIFT 15

/**** ACF_Control_Override register ****/
/* Override the AWCACHE[0] and ARCACHE[0] outputs to benon-buffe ... */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_BUFFOVRD_MASK 0x00000007
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_BUFFOVRD_SHIFT 0
/* Overrides the ARQOS and AWQOS input signals */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_QOSOVRD_MASK 0x000000F8
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_QOSOVRD_SHIFT 3
/* If LOW, then AC requests are never issued on the correspondin ... */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_ACE_CH_EN_MASK 0x00001F00
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_ACE_CH_EN_SHIFT 8
/* Internal register:Enables 4k hazard of post-barrier vs pre-ba ... */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_DMB_4K_HAZARD_EN (1 << 13)

/**** LGIC_Base_High register ****/
/* GIC registers base [39:32] */
#define NB_GLOBAL_LGIC_BASE_HIGH_BASE_39_32_MASK 0x000000FF
#define NB_GLOBAL_LGIC_BASE_HIGH_BASE_39_32_SHIFT 0
/* GIC registers base [31:15] */
#define NB_GLOBAL_LGIC_BASE_LOW_BASED_31_15_MASK 0xFFFF8000
#define NB_GLOBAL_LGIC_BASE_LOW_BASED_31_15_SHIFT 15

/**** IOGIC_Base_High register ****/
/* IOGIC registers base [39:32] */
#define NB_GLOBAL_IOGIC_BASE_HIGH_BASE_39_32_MASK 0x000000FF
#define NB_GLOBAL_IOGIC_BASE_HIGH_BASE_39_32_SHIFT 0
/* IOGIC registers base [31:15] */
#define NB_GLOBAL_IOGIC_BASE_LOW_BASED_31_15_MASK 0xFFFF8000
#define NB_GLOBAL_IOGIC_BASE_LOW_BASED_31_15_SHIFT 15

/**** IO_Wr_Split_Control register ****/
/* Write splitters bypass */
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_BYPASS_MASK 0x00000003
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_BYPASS_SHIFT 0
/* Write splitters store and forward */
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_ST_FW_MASK 0x0000000C
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_ST_FW_SHIFT 2
/* Write splitters unmodify snoop type */
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_UNMODIFY_SNP_MASK 0x00000030
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_UNMODIFY_SNP_SHIFT 4
/* Write splitters unsplit non-coherent access */
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_UNSPLIT_NOSNP_MASK 0x000000C0
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_UNSPLIT_NOSNP_SHIFT 6
/* Write splitter rate limit. */
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR0_SPLT_RATE_LIMIT_MASK 0x00001F00
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR0_SPLT_RATE_LIMIT_SHIFT 8
/* Write splitter rate limit  */
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR1_SPLT_RATE_LIMIT_MASK 0x0003E000
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR1_SPLT_RATE_LIMIT_SHIFT 13
/* Clear is not supported */
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_CLEAR_MASK 0xC0000000
#define NB_GLOBAL_IO_WR_SPLIT_CONTROL_WR_SPLT_CLEAR_SHIFT 30

/**** IO_Rd_ROB_Control register ****/
/* Read ROB Bypass[0] Rd ROB 0 bypass enable */
#define NB_GLOBAL_IO_RD_ROB_CONTROL_RD_ROB_BYPASS_MASK 0x00000003
#define NB_GLOBAL_IO_RD_ROB_CONTROL_RD_ROB_BYPASS_SHIFT 0
/* Read ROB in order */
#define NB_GLOBAL_IO_RD_ROB_CONTROL_RD_ROB_INORDER_MASK 0x0000000C
#define NB_GLOBAL_IO_RD_ROB_CONTROL_RD_ROB_INORDER_SHIFT 2
/* Read splitter rate limit */
#define NB_GLOBAL_IO_RD_ROB_CONTROL_RD0_ROB_RATE_LIMIT_MASK 0x00001F00
#define NB_GLOBAL_IO_RD_ROB_CONTROL_RD0_ROB_RATE_LIMIT_SHIFT 8
/* Read splitter rate limit */
#define NB_GLOBAL_IO_RD_ROB_CONTROL_RD1_ROB_RATE_LIMIT_MASK 0x0003E000
#define NB_GLOBAL_IO_RD_ROB_CONTROL_RD1_ROB_RATE_LIMIT_SHIFT 13

/**** SB_PoS_Error_Log_1 register ****/
/* Error Log 1[7:0] address_high[16:8] request id[18:17] bresp  ... */
#define NB_GLOBAL_SB_POS_ERROR_LOG_1_ERR_LOG_MASK 0x7FFFFFFF
#define NB_GLOBAL_SB_POS_ERROR_LOG_1_ERR_LOG_SHIFT 0
/* Valid logged errorSet on SB PoS error occurance on capturing  ... */
#define NB_GLOBAL_SB_POS_ERROR_LOG_1_VALID (1 << 31)

/**** MSIx_Error_Log register ****/
/* Error Log 
Corresponds to MSIx address message [30:0]. */
#define NB_GLOBAL_MSIX_ERROR_LOG_ERR_LOG_MASK 0x7FFFFFFF
#define NB_GLOBAL_MSIX_ERROR_LOG_ERR_LOG_SHIFT 0
/* Valid logged error */
#define NB_GLOBAL_MSIX_ERROR_LOG_VALID   (1 << 31)

/**** Error_Cause register ****/
/* PCIe emulation: inbound pcie read latch timeout */
#define NB_GLOBAL_ERROR_CAUSE_PCIE_RD_LOG_0_TIMEOUT (1 << 0)
/* PCIe emulation: inbound pcie read latch timeout */
#define NB_GLOBAL_ERROR_CAUSE_PCIE_RD_LOG_1_TIMEOUT (1 << 1)
/* Received msix is not mapped to local GIC or IO-GIC spin */
#define NB_GLOBAL_ERROR_CAUSE_MSIX_ERR_INT (1 << 2)
/* Coresight timestamp overflow */
#define NB_GLOBAL_ERROR_CAUSE_CORESIGHT_TS_OVERFLOW (1 << 3)
/* Write data parity error from SB channel 0. */
#define NB_GLOBAL_ERROR_CAUSE_SB0_WRDATA_PERR (1 << 4)
/* Write data parity error from SB channel 1. */
#define NB_GLOBAL_ERROR_CAUSE_SB1_WRDATA_PERR (1 << 5)
/* Read data parity error from SB slaves. */
#define NB_GLOBAL_ERROR_CAUSE_SB_SLV_RDATA_PERR (1 << 6)
/* Reserved. Read undefined; must write as zeros. */
#define NB_GLOBAL_ERROR_CAUSE_RESERVED_31_7_MASK 0xFFFFFF80
#define NB_GLOBAL_ERROR_CAUSE_RESERVED_31_7_SHIFT 7

/**** QoS_Peak_Control register ****/
/* Peak Read Low ThresholdWhen the number of outstanding read tr ... */
#define NB_GLOBAL_QOS_PEAK_CONTROL_PEAK_RD_L_THRESHOLD_MASK 0x0000007F
#define NB_GLOBAL_QOS_PEAK_CONTROL_PEAK_RD_L_THRESHOLD_SHIFT 0
/* Peak Read High ThresholdWhen the number of outstanding read t ... */
#define NB_GLOBAL_QOS_PEAK_CONTROL_PEAK_RD_H_THRESHOLD_MASK 0x00007F00
#define NB_GLOBAL_QOS_PEAK_CONTROL_PEAK_RD_H_THRESHOLD_SHIFT 8
/* Peak Write Low ThresholdWhen the number of outstanding write  ... */
#define NB_GLOBAL_QOS_PEAK_CONTROL_PEAK_WR_L_THRESHOLD_MASK 0x007F0000
#define NB_GLOBAL_QOS_PEAK_CONTROL_PEAK_WR_L_THRESHOLD_SHIFT 16
/* Peak Write High ThresholdWhen the number of outstanding write ... */
#define NB_GLOBAL_QOS_PEAK_CONTROL_PEAK_WR_H_THRESHOLD_MASK 0x7F000000
#define NB_GLOBAL_QOS_PEAK_CONTROL_PEAK_WR_H_THRESHOLD_SHIFT 24

/**** QoS_Set_Control register ****/
/* CPU Low priority Read QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_ARQOS_MASK 0x0000000F
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_ARQOS_SHIFT 0
/* CPU High priority Read QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_ARQOS_MASK 0x000000F0
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_ARQOS_SHIFT 4
/* CPU Low priority Write QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_AWQOS_MASK 0x00000F00
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_AWQOS_SHIFT 8
/* CPU High priority Write QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_AWQOS_MASK 0x0000F000
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_AWQOS_SHIFT 12
/* SB Low priority Read QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_SB_LP_ARQOS_MASK 0x000F0000
#define NB_GLOBAL_QOS_SET_CONTROL_SB_LP_ARQOS_SHIFT 16
/* SB Low-priority Write QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_SB_LP_AWQOS_MASK 0x00F00000
#define NB_GLOBAL_QOS_SET_CONTROL_SB_LP_AWQOS_SHIFT 20

/**** DDR_QoS register ****/
/* High Priority Read ThresholdLimits the number of outstanding  ... */
#define NB_GLOBAL_DDR_QOS_HIGH_PRIO_THRESHOLD_MASK 0x0000007F
#define NB_GLOBAL_DDR_QOS_HIGH_PRIO_THRESHOLD_SHIFT 0
/* DDR Low Priority QoSFabric priority below this value is mappe ... */
#define NB_GLOBAL_DDR_QOS_LP_QOS_MASK    0x00000F00
#define NB_GLOBAL_DDR_QOS_LP_QOS_SHIFT   8

/**** ACF_Misc register ****/
/* Disable DDR Write ChopPerformance optimitation feature to cho ... */
#define NB_GLOBAL_ACF_MISC_DDR_WR_CHOP_DIS (1 << 0)
/* Disable SB-2-SB path through NB fabric. */
#define NB_GLOBAL_ACF_MISC_SB2SB_PATH_DIS (1 << 1)
/* Disable ETR tracing to non-DDR. */
#define NB_GLOBAL_ACF_MISC_ETR2SB_PATH_DIS (1 << 2)
/* Disable ETR tracing to non-DDR. */
#define NB_GLOBAL_ACF_MISC_CPU2MSIX_DIS  (1 << 3)
/* Disable CPU generation of MSIx By default, the CPU can set an ... */
#define NB_GLOBAL_ACF_MISC_MSIX_TERMINATE_DIS (1 << 4)
/* Disable snoop override for MSIxBy default, an MSIx transactio ... */
#define NB_GLOBAL_ACF_MISC_MSIX_SNOOPOVRD_DIS (1 << 5)
/* POS bypass */
#define NB_GLOBAL_ACF_MISC_POS_BYPASS    (1 << 6)
/* PoS ReadStronglyOrdered enableSO read forces flushing of all  ... */
#define NB_GLOBAL_ACF_MISC_POS_RSO_EN    (1 << 7)
/* WRAP to INC transfer enable */
#define NB_GLOBAL_ACF_MISC_POS_WRAP2INC  (1 << 8)
/* PoS DSB flush DisableOn DSB from CPU, PoS blocks the progress ... */
#define NB_GLOBAL_ACF_MISC_POS_DSB_FLUSH_DIS (1 << 9)
/* PoS DMB Flush DisableOn DMB from CPU, the PoS blocks the prog ... */
#define NB_GLOBAL_ACF_MISC_POS_DMB_FLUSH_DIS (1 << 10)
/* change DMB functionality to DSB (block and drain) */
#define NB_GLOBAL_ACF_MISC_POS_DMB_TO_DSB_EN (1 << 11)
/* Disable write after read stall when accessing IO fabric slave ... */
#define NB_GLOBAL_ACF_MISC_M0_WAR_STALL_DIS (1 << 12)
/* Disable write after read stall when accessing DDR  */
#define NB_GLOBAL_ACF_MISC_M1_WAR_STALL_DIS (1 << 13)
/* spare configuration bits[14]: disable pos change to disable/e ... */
#define NB_GLOBAL_ACF_MISC_CONFIG_SPARE_MASK 0x1FFFC000
#define NB_GLOBAL_ACF_MISC_CONFIG_SPARE_SHIFT 14
/* Enable CPU WriteUnique to WriteNoSnoop trasform */
#define NB_GLOBAL_ACF_MISC_CPU_WU2WNS_EN (1 << 29)
/* Disable device after device check */
#define NB_GLOBAL_ACF_MISC_WR_POS_DEV_AFTER_DEV_DIS (1 << 30)
/* Disable wrap to inc on write */
#define NB_GLOBAL_ACF_MISC_WR_INC2WRAP_EN (1 << 31)

/**** Config_Bus_Control register ****/
/* Write slave error enable */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_WR_SLV_ERR_EN (1 << 0)
/* Write decode error enable */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_WR_DEC_ERR_EN (1 << 1)
/* Read slave error enable */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_RD_SLV_ERR_EN (1 << 2)
/* Read decode error enable */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_RD_DEC_ERR_EN (1 << 3)
/* Ignore Write ID */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_IGNORE_WR_ID (1 << 4)
/* Timeout limit before terminating configuration bus access wit ... */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_TIMEOUT_LIMIT_MASK 0xFFFFFF00
#define NB_GLOBAL_CONFIG_BUS_CONTROL_TIMEOUT_LIMIT_SHIFT 8

/**** Coresight_PD register ****/
/* ETF0 RAM force power down */
#define NB_GLOBAL_CORESIGHT_PD_ETF0_RAM_FORCE_PD (1 << 0)
/* ETF1 RAM force power down */
#define NB_GLOBAL_CORESIGHT_PD_ETF1_RAM_FORCE_PD (1 << 1)
/* ETF0 RAM force clock gate */
#define NB_GLOBAL_CORESIGHT_PD_ETF0_RAM_FORCE_CG (1 << 2)
/* ETF1 RAM force clock gate */
#define NB_GLOBAL_CORESIGHT_PD_ETF1_RAM_FORCE_CG (1 << 3)
/* APBIC clock enable */
#define NB_GLOBAL_CORESIGHT_PD_APBICLKEN (1 << 4)
/* DAP system clock enable */
#define NB_GLOBAL_CORESIGHT_PD_DAP_SYS_CLKEN (1 << 5)

/**** Coresight_INTERNAL_0 register ****/

#define NB_GLOBAL_CORESIGHT_INTERNAL_0_CTIAPBSBYPASS (1 << 0)
/* CA15 CTM and Coresight CTI operate at same clock, bypass mode ... */
#define NB_GLOBAL_CORESIGHT_INTERNAL_0_CISBYPASS (1 << 1)
/* CA15 CTM and Coresight CTI operate according to the same cloc ... */
#define NB_GLOBAL_CORESIGHT_INTERNAL_0_CIHSBYPASS_MASK 0x0000003C
#define NB_GLOBAL_CORESIGHT_INTERNAL_0_CIHSBYPASS_SHIFT 2

/**** Coresight_DBGROMADDR register ****/
/* Valid signal for DBGROMADDR.
Connected to DBGROMADDRV */
#define NB_GLOBAL_CORESIGHT_DBGROMADDR_VALID (1 << 0)
/* Specifies bits [39:12] of the ROM table physical address. */
#define NB_GLOBAL_CORESIGHT_DBGROMADDR_ADDR_39_12_MASK 0x3FFFFFFC
#define NB_GLOBAL_CORESIGHT_DBGROMADDR_ADDR_39_12_SHIFT 2

/**** Coresight_DBGSELFADDR register ****/
/* Valid signal for DBGROMADDR.
Connected to DBGROMADDRV */
#define NB_GLOBAL_CORESIGHT_DBGSELFADDR_VALID (1 << 0)
/* Specifies bits [18:17] of the two’s complement signed offset  ... */
#define NB_GLOBAL_CORESIGHT_DBGSELFADDR_ADDR_18_17_MASK 0x00000180
#define NB_GLOBAL_CORESIGHT_DBGSELFADDR_ADDR_18_17_SHIFT 7
/* Specifies bits [39:19] of the two’s complement signed offset  ... */
#define NB_GLOBAL_CORESIGHT_DBGSELFADDR_ADDR_39_19_MASK 0x3FFFFE00
#define NB_GLOBAL_CORESIGHT_DBGSELFADDR_ADDR_39_19_SHIFT 9

/**** Cnt_Control register ****/
/* System counter enable
Counter is enabled after reset. */
#define NB_SYSTEM_COUNTER_CNT_CONTROL_EN (1 << 0)
/* System counter restartInitial value is reloaded from Counter_ ... */
#define NB_SYSTEM_COUNTER_CNT_CONTROL_RESTART (1 << 1)
/* System counter tickSpecifies the counter tick rate relative t ... */
#define NB_SYSTEM_COUNTER_CNT_CONTROL_SCALE_MASK 0x0000FF00
#define NB_SYSTEM_COUNTER_CNT_CONTROL_SCALE_SHIFT 8

/**** CA15_RF_Misc register ****/

#define NB_RAMS_CONTROL_MISC_CA15_RF_MISC_NONECPU_RF_MISC_MASK 0x0000000F
#define NB_RAMS_CONTROL_MISC_CA15_RF_MISC_NONECPU_RF_MISC_SHIFT 0

#define NB_RAMS_CONTROL_MISC_CA15_RF_MISC_CPU_RF_MISC_MASK 0x00FFFF00
#define NB_RAMS_CONTROL_MISC_CA15_RF_MISC_CPU_RF_MISC_SHIFT 8
/* Pause for CPUs from the time all power is up to the time the  ... */
#define NB_RAMS_CONTROL_MISC_CA15_RF_MISC_PWR_UP_PAUSE_MASK 0xF8000000
#define NB_RAMS_CONTROL_MISC_CA15_RF_MISC_PWR_UP_PAUSE_SHIFT 27

/**** NB_RF_Misc register ****/
/* SMMU TLB RAMs force power down */
#define NB_RAMS_CONTROL_MISC_NB_RF_MISC_SMMU_RAM_FORCE_PD (1 << 0)

/**** Lockn register ****/
/* Semaphore LockCPU reads it:If current value ==0,  return 0 to ... */
#define NB_SEMAPHORES_LOCKN_LOCK         (1 << 0)

/**** CA15_outputs_1 register ****/

#define NB_DEBUG_CA15_OUTPUTS_1_STANDBYWFI_MASK 0x0000000F
#define NB_DEBUG_CA15_OUTPUTS_1_STANDBYWFI_SHIFT 0

#define NB_DEBUG_CA15_OUTPUTS_1_CPU_PWR_DN_ACK_MASK 0x000000F0
#define NB_DEBUG_CA15_OUTPUTS_1_CPU_PWR_DN_ACK_SHIFT 4

#define NB_DEBUG_CA15_OUTPUTS_1_IRQOUT_N_MASK 0x00000F00
#define NB_DEBUG_CA15_OUTPUTS_1_IRQOUT_N_SHIFT 8

#define NB_DEBUG_CA15_OUTPUTS_1_FIQOUT_N_MASK 0x0000F000
#define NB_DEBUG_CA15_OUTPUTS_1_FIQOUT_N_SHIFT 12

#define NB_DEBUG_CA15_OUTPUTS_1_CNTHPIRQ_N_MASK 0x000F0000
#define NB_DEBUG_CA15_OUTPUTS_1_CNTHPIRQ_N_SHIFT 16

#define NB_DEBUG_CA15_OUTPUTS_1_NCNTPNSIRQ_N_MASK 0x00F00000
#define NB_DEBUG_CA15_OUTPUTS_1_NCNTPNSIRQ_N_SHIFT 20

#define NB_DEBUG_CA15_OUTPUTS_1_NCNTPSIRQ_N_MASK 0x0F000000
#define NB_DEBUG_CA15_OUTPUTS_1_NCNTPSIRQ_N_SHIFT 24

#define NB_DEBUG_CA15_OUTPUTS_1_NCNTVIRQ_N_MASK 0xF0000000
#define NB_DEBUG_CA15_OUTPUTS_1_NCNTVIRQ_N_SHIFT 28

/**** CA15_outputs_2 register ****/

#define NB_DEBUG_CA15_OUTPUTS_2_STANDBYWFIL2 (1 << 0)

#define NB_DEBUG_CA15_OUTPUTS_2_L2RAM_PWR_DN_ACK (1 << 1)

/**** cpu_msg register ****/
/* status/ascii code */
#define NB_DEBUG_CPU_MSG_STATUS_MASK     0x000000FF
#define NB_DEBUG_CPU_MSG_STATUS_SHIFT    0
/* toggle with each ascii write */
#define NB_DEBUG_CPU_MSG_ASCII_TOGGLE    (1 << 8)
/* signals ascii */
#define NB_DEBUG_CPU_MSG_ASCII           (1 << 9)

#define NB_DEBUG_CPU_MSG_RESERVED_11_10_MASK 0x00000C00
#define NB_DEBUG_CPU_MSG_RESERVED_11_10_SHIFT 10
/* Signals new section started in S/W */
#define NB_DEBUG_CPU_MSG_SECTION_START   (1 << 12)

#define NB_DEBUG_CPU_MSG_RESERVED_13     (1 << 13)
/* Signals a single CPU is done. */
#define NB_DEBUG_CPU_MSG_CPU_DONE        (1 << 14)
/* Signals test is done */
#define NB_DEBUG_CPU_MSG_TEST_DONE       (1 << 15)

/**** ddrc register ****/
/* External DLL calibration request */
#define NB_DEBUG_DDRC_DLL_CALIB_EXT_REQ  (1 << 0)
/* External request to perform short (long isperformed during in ... */
#define NB_DEBUG_DDRC_ZQ_SHORT_CALIB_EXT_REQ (1 << 1)
/* External request to perform a refresh command to a specific b ... */
#define NB_DEBUG_DDRC_RANK_REFRESH_EXT_REQ_MASK 0x0000003C
#define NB_DEBUG_DDRC_RANK_REFRESH_EXT_REQ_SHIFT 2

/**** ddrc_phy_smode_control register ****/
/* DDR PHY special mode */
#define NB_DEBUG_DDRC_PHY_SMODE_CONTROL_CTL_MASK 0x0000FFFF
#define NB_DEBUG_DDRC_PHY_SMODE_CONTROL_CTL_SHIFT 0

/**** ddrc_phy_smode_status register ****/
/* DDR PHY special mode */
#define NB_DEBUG_DDRC_PHY_SMODE_STATUS_STT_MASK 0x0000FFFF
#define NB_DEBUG_DDRC_PHY_SMODE_STATUS_STT_SHIFT 0

/**** pmc register ****/
/* Enable system control on NB DRO */
#define NB_DEBUG_PMC_SYS_EN              (1 << 0)
/* NB PMC HVT35 counter value */
#define NB_DEBUG_PMC_HVT35_VAL_14_0_MASK 0x0000FFFE
#define NB_DEBUG_PMC_HVT35_VAL_14_0_SHIFT 1
/* NB PMC SVT31 counter value */
#define NB_DEBUG_PMC_SVT31_VAL_14_0_MASK 0x7FFF0000
#define NB_DEBUG_PMC_SVT31_VAL_14_0_SHIFT 16

/**** cpus_int_out register ****/
/* Defines which CPUs' IRQ will be triggered out through the cpu ... */
#define NB_DEBUG_CPUS_INT_OUT_FIQ_EN_MASK 0x0000000F
#define NB_DEBUG_CPUS_INT_OUT_FIQ_EN_SHIFT 0
/* Defines which CPUs' FIQ will be triggered out through the cpu ... */
#define NB_DEBUG_CPUS_INT_OUT_IRQ_EN_MASK 0x000000F0
#define NB_DEBUG_CPUS_INT_OUT_IRQ_EN_SHIFT 4

/**** track_dump_ctrl register ****/
/* [24:16]: queue entry pointer[2] target queue:  1'b0: HazardTr ... */
#define NB_DEBUG_TRACK_DUMP_CTRL_PTR_MASK 0x7FFFFFFF
#define NB_DEBUG_TRACK_DUMP_CTRL_PTR_SHIFT 0
/* Track Dump RequestIf set, queue entry info is latched on trac ... */
#define NB_DEBUG_TRACK_DUMP_CTRL_REQ     (1 << 31)

/**** track_dump_rdata_0 register ****/
/* valid */
#define NB_DEBUG_TRACK_DUMP_RDATA_0_VALID (1 << 0)
/* low data */
#define NB_DEBUG_TRACK_DUMP_RDATA_0_DATA_MASK 0xFFFFFFFE
#define NB_DEBUG_TRACK_DUMP_RDATA_0_DATA_SHIFT 1

/**** pos_track_dump_ctrl register ****/
/* [24:16]: queue entry pointer */
#define NB_DEBUG_POS_TRACK_DUMP_CTRL_PTR_MASK 0x7FFFFFFF
#define NB_DEBUG_POS_TRACK_DUMP_CTRL_PTR_SHIFT 0
/* Track Dump RequestIf set, queue entry info is latched on trac ... */
#define NB_DEBUG_POS_TRACK_DUMP_CTRL_REQ (1 << 31)

/**** pos_track_dump_rdata_0 register ****/
/* valid */
#define NB_DEBUG_POS_TRACK_DUMP_RDATA_0_VALID (1 << 0)
/* low data */
#define NB_DEBUG_POS_TRACK_DUMP_RDATA_0_DATA_MASK 0xFFFFFFFE
#define NB_DEBUG_POS_TRACK_DUMP_RDATA_0_DATA_SHIFT 1

/**** c2swb_track_dump_ctrl register ****/
/* [24:16]: queue entry pointer */
#define NB_DEBUG_C2SWB_TRACK_DUMP_CTRL_PTR_MASK 0x7FFFFFFF
#define NB_DEBUG_C2SWB_TRACK_DUMP_CTRL_PTR_SHIFT 0
/* Track Dump RequestIf set, queue entry info is latched on trac ... */
#define NB_DEBUG_C2SWB_TRACK_DUMP_CTRL_REQ (1 << 31)

/**** c2swb_track_dump_rdata_0 register ****/
/* valid */
#define NB_DEBUG_C2SWB_TRACK_DUMP_RDATA_0_VALID (1 << 0)
/* low data */
#define NB_DEBUG_C2SWB_TRACK_DUMP_RDATA_0_DATA_MASK 0xFFFFFFFE
#define NB_DEBUG_C2SWB_TRACK_DUMP_RDATA_0_DATA_SHIFT 1

/**** c2swb_bar_ovrd_high register ****/
/* Read barrier is progress downstream when not terminated in th ... */
#define NB_DEBUG_C2SWB_BAR_OVRD_HIGH_RD_ADDR_OVRD_EN (1 << 0)
/* address bits 39:32 */
#define NB_DEBUG_C2SWB_BAR_OVRD_HIGH_ADDR_39_32_MASK 0x00FF0000
#define NB_DEBUG_C2SWB_BAR_OVRD_HIGH_ADDR_39_32_SHIFT 16

/**** Config register ****/
/* Individual processor control of the endianness configuration  ... */
#define NB_CPUN_CONFIG_STATUS_CONFIG_ENDIAN (1 << 0)
/* Individual processor control of the default exception handlin ... */
#define NB_CPUN_CONFIG_STATUS_CONFIG_TE  (1 << 1)
/* Individual processor control of the location of the exception ... */
#define NB_CPUN_CONFIG_STATUS_CONFIG_VINITHI (1 << 2)
/* Individual processor control to disable write access to some  ... */
#define NB_CPUN_CONFIG_STATUS_CONFIG_CP15DISABLE (1 << 3)

/**** Power_Ctrl register ****/
/* Individual CPU power mode transition requestIf requested to e ... */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_MASK 0x00000003
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_SHIFT 0
/* Normal power mode state */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_NORMAL \
		(0x0 << NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_SHIFT)
/* Dormant power mode state */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_DEEP_IDLE \
		(0x2 << NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_SHIFT)
/* Powered-off power mode */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_POWEREDOFF \
		(0x3 << NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_SHIFT)
/* Power down regret disableWhen power down regret is enabled, t ... */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PWRDN_RGRT_DIS (1 << 16)
/* Power down emulation enableIf set, the entire power down sequ ... */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PWRDN_EMULATE (1 << 17)
/* Disable wakeup from Local--GIC FIQ. */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_WU_LGIC_FIQ_DIS (1 << 18)
/* Disable wakeup from Local-GIC IRQ. */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_WU_LGIC_IRQ_DIS (1 << 19)
/* Disable wakeup from IO-GIC FIQ. */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_WU_IOGIC_FIQ_DIS (1 << 20)
/* Disable wakeup from IO-GIC IRQ. */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_WU_IOGIC_IRQ_DIS (1 << 21)

/**** Power_Status register ****/
/* Read-only bits that reflect the individual CPU power mode sta ... */
#define NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_MASK 0x00000003
#define NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_SHIFT 0
/* Normal power mode state */
#define NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_NORMAL \
		(0x0 << NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_SHIFT)
/* Idle power mode state (WFI) */
#define NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_IDLE \
		(0x1 << NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_SHIFT)
/* Dormant power mode state */
#define NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_DEEP_IDLE \
		(0x2 << NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_SHIFT)
/* Powered-off power mode */
#define NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_POWEREDOFF \
		(0x3 << NB_CPUN_CONFIG_STATUS_POWER_STATUS_CPU_PM_SHIFT)
/* WFI status */
#define NB_CPUN_CONFIG_STATUS_POWER_STATUS_WFI (1 << 2)
/* WFE status */
#define NB_CPUN_CONFIG_STATUS_POWER_STATUS_WFE (1 << 3)

/**** PMU_Control register ****/
/* Disable all countersWhen this bit is clear, counter state is  ... */
#define NB_MC_PMU_PMU_CONTROL_DISABLE_ALL (1 << 0)
/* Pause all counters */
#define NB_MC_PMU_PMU_CONTROL_PAUSE_ALL  (1 << 1)
/* Overflow interrupt enable. */
#define NB_MC_PMU_PMU_CONTROL_OVRF_INTR_EN (1 << 2)
/* Number of monitored events supported by the PMU */
#define NB_MC_PMU_PMU_CONTROL_NUM_OF_EVENTS_MASK 0x00F80000
#define NB_MC_PMU_PMU_CONTROL_NUM_OF_EVENTS_SHIFT 19
/* Number of counters impemented by PMU. */
#define NB_MC_PMU_PMU_CONTROL_NUM_OF_CNTS_MASK 0x0F000000
#define NB_MC_PMU_PMU_CONTROL_NUM_OF_CNTS_SHIFT 24

/**** Cfg register ****/
/* Event select */
#define NB_MC_PMU_COUNTERS_CFG_EVENT_SEL_MASK 0x0000003F
#define NB_MC_PMU_COUNTERS_CFG_EVENT_SEL_SHIFT 0
/* Enable setting of counter low overflow status bit. */
#define NB_MC_PMU_COUNTERS_CFG_OVRF_LOW_STT_EN (1 << 6)
/* Enable setting of counter high overflow status bit. */
#define NB_MC_PMU_COUNTERS_CFG_OVRF_HIGH_STT_EN (1 << 7)
/* Enable pause on trigger in assertion. */
#define NB_MC_PMU_COUNTERS_CFG_TRIGIN_PAUSE_EN (1 << 8)
/* Enable increment trigger out for trace */
#define NB_MC_PMU_COUNTERS_CFG_TRIGOUT_EN (1 << 9)
/* Trigger out granule valueSpecifies the number of events count ... */
#define NB_MC_PMU_COUNTERS_CFG_TRIGOUT_GRANULA_MASK 0x00007C00
#define NB_MC_PMU_COUNTERS_CFG_TRIGOUT_GRANULA_SHIFT 10
/* Pause on overflow bitmaskIf set for counter <i>, current coun ... */
#define NB_MC_PMU_COUNTERS_CFG_PAUSE_ON_OVRF_BITMASK_MASK 0x000F0000
#define NB_MC_PMU_COUNTERS_CFG_PAUSE_ON_OVRF_BITMASK_SHIFT 16

/**** Cntl register ****/
/* Set the counter state to disable, enable, or pause. */
#define NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_MASK 0x00000003
#define NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_SHIFT 0
/* Disable counter. */
#define NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_DISABLE \
		(0x0 << NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_SHIFT)
/* Enable counter.  */
#define NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_ENABLE \
		(0x1 << NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_SHIFT)
/* Pause counter.  */
#define NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_PAUSE \
		(0x3 << NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_SHIFT)

/**** High register ****/
/* Counter high value */
#define NB_MC_PMU_COUNTERS_HIGH_COUNTER_MASK 0x0000FFFF
#define NB_MC_PMU_COUNTERS_HIGH_COUNTER_SHIFT 0

/**** version register ****/
/*  Revision number (Minor) */
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MINOR_MASK 0x000000FF
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MINOR_SHIFT 0
/*  Revision number (Major) */
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_MASK 0x0000FF00
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_SHIFT 8
/*  Date of release */
#define NB_NB_VERSION_VERSION_DATE_DAY_MASK 0x001F0000
#define NB_NB_VERSION_VERSION_DATE_DAY_SHIFT 16
/*  Month of release */
#define NB_NB_VERSION_VERSION_DATA_MONTH_MASK 0x01E00000
#define NB_NB_VERSION_VERSION_DATA_MONTH_SHIFT 21
/*  Year of release (starting from 2000) */
#define NB_NB_VERSION_VERSION_DATE_YEAR_MASK 0x3E000000
#define NB_NB_VERSION_VERSION_DATE_YEAR_SHIFT 25
/*  Reserved */
#define NB_NB_VERSION_VERSION_RESERVED_MASK 0xC0000000
#define NB_NB_VERSION_VERSION_RESERVED_SHIFT 30

/**** cpu_vmid register ****/
/* target VMID */
#define NB_SRIOV_CPU_VMID_VAL_MASK      0x000000FF
#define NB_SRIOV_CPU_VMID_VAL_SHIFT     0

/**** control register ****/
/* Enable write accesses logging to FIFO instance 0When an inbou ... */
#define NB_PCIE_LOGGING_CONTROL_WR_EN    (1 << 0)
/* Enable read accesses loggingWhen an inbound read from PCIe hi ... */
#define NB_PCIE_LOGGING_CONTROL_RD_EN    (1 << 1)
/* Enable write accesses logging to FIFO instance 1 When an inbo ... */
#define NB_PCIE_LOGGING_CONTROL_WR_EN_1  (1 << 2)
/* Enable read accesses logging to PCIe 1 and 2When an inbound r ... */
#define NB_PCIE_LOGGING_CONTROL_RD_EN_1  (1 << 3)
/* Enable loggin the original transaction strobes */
#define NB_PCIE_LOGGING_CONTROL_STRB_EN  (1 << 4)
/* When this bit is set, read will always progress forward (will ... */
#define NB_PCIE_LOGGING_CONTROL_FREE_RD_ON_WR_EMPTY_EN (1 << 5)
/* Free stalled read whenever write fifo head pointer bit[31] is ... */
#define NB_PCIE_LOGGING_CONTROL_FREE_RD_ON_WR_FIFO_PTR_UPD_EN (1 << 6)
/* Push pended latched read notification to the current pushed w ... */
#define NB_PCIE_LOGGING_CONTROL_WR_FIFO_PUSH_LATCH_RD_STATUS_EN (1 << 7)
/* Read latch timeout enable */
#define NB_PCIE_LOGGING_CONTROL_RD_TIMEOUT_EN (1 << 8)
/* Logging window low  */
#define NB_PCIE_LOGGING_WR_WINDOW_LOW_LOW_MASK 0xFFFFFFC0
#define NB_PCIE_LOGGING_WR_WINDOW_LOW_LOW_SHIFT 6

/**** Wr_Window_High register ****/
/* Window high address bits
Supports 40-bits memory addressing */
#define NB_PCIE_LOGGING_WR_WINDOW_HIGH_HIGH_MASK 0x000000FF
#define NB_PCIE_LOGGING_WR_WINDOW_HIGH_HIGH_SHIFT 0
/* Size maskCorresponds to window low bits 31:6 */
#define NB_PCIE_LOGGING_WR_WINDOW_SIZE_SIZE_MASK_MASK 0xFFFFFFC0
#define NB_PCIE_LOGGING_WR_WINDOW_SIZE_SIZE_MASK_SHIFT 6
/* FIFO base address. 
Must be alighed to 4KB */
#define NB_PCIE_LOGGING_FIFO_BASE_ADDR_MASK 0xFFFFF000
#define NB_PCIE_LOGGING_FIFO_BASE_ADDR_SHIFT 12

/**** FIFO_Size register ****/
/* FIFO size maskCorresponds to FIFO base address bits 19:12 */
#define NB_PCIE_LOGGING_FIFO_SIZE_MASK_19_12_MASK 0x000000FF
#define NB_PCIE_LOGGING_FIFO_SIZE_MASK_19_12_SHIFT 0
/* Logging window low  */
#define NB_PCIE_LOGGING_WR_WINDOW_LOW_1_LOW_MASK 0xFFFFFFC0
#define NB_PCIE_LOGGING_WR_WINDOW_LOW_1_LOW_SHIFT 6

/**** Wr_Window_High_1 register ****/
/* Window high address bits
Supports 40-bits memory addressing */
#define NB_PCIE_LOGGING_WR_WINDOW_HIGH_1_HIGH_MASK 0x000000FF
#define NB_PCIE_LOGGING_WR_WINDOW_HIGH_1_HIGH_SHIFT 0
/* Size maskCorresponds to window low bits 31:6 */
#define NB_PCIE_LOGGING_WR_WINDOW_SIZE_1_SIZE_MASK_MASK 0xFFFFFFC0
#define NB_PCIE_LOGGING_WR_WINDOW_SIZE_1_SIZE_MASK_SHIFT 6
/* FIFO base address. 
Must be alighed to 4KB */
#define NB_PCIE_LOGGING_FIFO_BASE_1_ADDR_MASK 0xFFFFF000
#define NB_PCIE_LOGGING_FIFO_BASE_1_ADDR_SHIFT 12

/**** FIFO_Size_1 register ****/
/* FIFO size maskCorresponds to FIFO base address bits 19:12 */
#define NB_PCIE_LOGGING_FIFO_SIZE_1_MASK_19_12_MASK 0x000000FF
#define NB_PCIE_LOGGING_FIFO_SIZE_1_MASK_19_12_SHIFT 0
/* Logging window low  */
#define NB_PCIE_LOGGING_RD_WINDOW_LOW_LOW_MASK 0xFFFFFFC0
#define NB_PCIE_LOGGING_RD_WINDOW_LOW_LOW_SHIFT 6

/**** Rd_Window_High register ****/
/* Window high address bits
Supports 40-bits memory addressing */
#define NB_PCIE_LOGGING_RD_WINDOW_HIGH_HIGH_MASK 0x000000FF
#define NB_PCIE_LOGGING_RD_WINDOW_HIGH_HIGH_SHIFT 0
/* Size maskCorresponds to window low bits 31:6 */
#define NB_PCIE_LOGGING_RD_WINDOW_SIZE_SIZE_MASK_MASK 0xFFFFFFC0
#define NB_PCIE_LOGGING_RD_WINDOW_SIZE_SIZE_MASK_SHIFT 6

/**** Read_Latch register ****/
/* Set by hardware when the read address is stalled and latched  ... */
#define NB_PCIE_LOGGING_READ_LATCH_VALID (1 << 0)
/* Latched read address [30:0] */
#define NB_PCIE_LOGGING_READ_LATCH_ADDR_MASK 0xFFFFFFFE
#define NB_PCIE_LOGGING_READ_LATCH_ADDR_SHIFT 1
/* Logging window low  */
#define NB_PCIE_LOGGING_RD_WINDOW_LOW_1_LOW_MASK 0xFFFFFFC0
#define NB_PCIE_LOGGING_RD_WINDOW_LOW_1_LOW_SHIFT 6

/**** Rd_Window_High_1 register ****/
/* Window high address bits
Supports 40-bits memory addressing */
#define NB_PCIE_LOGGING_RD_WINDOW_HIGH_1_HIGH_MASK 0x000000FF
#define NB_PCIE_LOGGING_RD_WINDOW_HIGH_1_HIGH_SHIFT 0
/* Size maskCorresponds to window low bits 31:6 */
#define NB_PCIE_LOGGING_RD_WINDOW_SIZE_1_SIZE_MASK_MASK 0xFFFFFFC0
#define NB_PCIE_LOGGING_RD_WINDOW_SIZE_1_SIZE_MASK_SHIFT 6

/**** Read_Latch_1 register ****/
/* Set by hardware when the read address is stalled and latched  ... */
#define NB_PCIE_LOGGING_READ_LATCH_1_VALID (1 << 0)
/* Latched read address [30:0] */
#define NB_PCIE_LOGGING_READ_LATCH_1_ADDR_MASK 0xFFFFFFFE
#define NB_PCIE_LOGGING_READ_LATCH_1_ADDR_SHIFT 1

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_NB_REG_H */

/** @} end of ... group */


