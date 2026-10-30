/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_PBS_REG_H
#define __AL_PBS_REG_H

#include <mach/al_hal_common.h>

/*
* Unit Registers
*/

struct al_pbs_unit {
	uint32_t conf_bus;              /* conf_bus, configuration of ... */
	uint32_t dram_0_nb_bar_high;    /* PASW high */
	uint32_t dram_0_nb_bar_low;     /* PASW low */
	uint32_t dram_1_nb_bar_high;    /* PASW high */
	uint32_t dram_1_nb_bar_low;     /* PASW low */
	uint32_t dram_2_nb_bar_high;    /* PASW high */
	uint32_t dram_2_nb_bar_low;     /* PASW low */
	uint32_t dram_3_nb_bar_high;    /* PASW high */
	uint32_t dram_3_nb_bar_low;     /* PASW low */
	uint32_t msix_nb_bar_high;      /* PASW high */
	uint32_t msix_nb_bar_low;       /* PASW low */
	uint32_t dram_0_sb_bar_high;    /* PASW high */
	uint32_t dram_0_sb_bar_low;     /* PASW low */
	uint32_t dram_1_sb_bar_high;    /* PASW high */
	uint32_t dram_1_sb_bar_low;     /* PASW low */
	uint32_t dram_2_sb_bar_high;    /* PASW high */
	uint32_t dram_2_sb_bar_low;     /* PASW low */
	uint32_t dram_3_sb_bar_high;    /* PASW high */
	uint32_t dram_3_sb_bar_low;     /* PASW low */
	uint32_t msix_sb_bar_high;      /* PASW high */
	uint32_t msix_sb_bar_low;       /* PASW low */
	uint32_t pcie_mem0_bar_high;    /* PASW high */
	uint32_t pcie_mem0_bar_low;     /* PASW low */
	uint32_t pcie_mem1_bar_high;    /* PASW high */
	uint32_t pcie_mem1_bar_low;     /* PASW low */
	uint32_t pcie_mem2_bar_high;    /* PASW high */
	uint32_t pcie_mem2_bar_low;     /* PASW low */
	uint32_t pcie_ext_ecam0_bar_high; /* PASW high */
	uint32_t pcie_ext_ecam0_bar_low; /* PASW low */
	uint32_t pcie_ext_ecam1_bar_high; /* PASW high */
	uint32_t pcie_ext_ecam1_bar_low; /* PASW low */
	uint32_t pcie_ext_ecam2_bar_high; /* PASW high */
	uint32_t pcie_ext_ecam2_bar_low; /* PASW low */
	uint32_t pbs_nor_bar_high;      /* PASW high */
	uint32_t pbs_nor_bar_low;       /* PASW low */
	uint32_t pbs_spi_bar_high;      /* PASW high */
	uint32_t pbs_spi_bar_low;       /* PASW low */
	uint32_t rsrvd_0[3];
	uint32_t pbs_nand_bar_high;     /* PASW high */
	uint32_t pbs_nand_bar_low;      /* PASW low */
	uint32_t pbs_int_mem_bar_high;  /* PASW high */
	uint32_t pbs_int_mem_bar_low;   /* PASW low */
	uint32_t pbs_boot_bar_high;     /* PASW high */
	uint32_t pbs_boot_bar_low;      /* PASW low */
	uint32_t nb_int_bar_high;       /* PASW high */
	uint32_t nb_int_bar_low;        /* PASW low */
	uint32_t nb_stm_bar_high;       /* PASW high */
	uint32_t nb_stm_bar_low;        /* PASW low */
	uint32_t pcie_ecam_int_bar_high; /* PASW high */
	uint32_t pcie_ecam_int_bar_low; /* PASW low */
	uint32_t pcie_mem_int_bar_high; /* PASW high */
	uint32_t pcie_mem_int_bar_low;  /* PASW low */
	uint32_t winit_cntl;            /* control */
	uint32_t latch_bars;            /* control */
	uint32_t pcie_conf_0;           /* control */
	uint32_t pcie_conf_1;           /* control */
	uint32_t serdes_mux_pipe;       /* control */
	uint32_t dma_io_master_map;     /* control */
	uint32_t i2c_pld_status_high;   /* status */
	uint32_t i2c_pld_status_low;    /* status */
	uint32_t spi_dbg_status_high;   /* status */
	uint32_t spi_dbg_status_low;    /* status */
	uint32_t spi_mst_status_high;   /* status */
	uint32_t spi_mst_status_low;    /* status */
	uint32_t mem_pbs_parity_err_high; /* log */
	uint32_t mem_pbs_parity_err_low; /* log */
	uint32_t boot_strap;            /* log */
	uint32_t cfg_axi_conf_0;        /* conf */
	uint32_t cfg_axi_conf_1;        /* conf */
	uint32_t cfg_axi_conf_2;        /* conf */
	uint32_t cfg_axi_conf_3;        /* conf */
	uint32_t spi_mst_conf_0;        /* conf */
	uint32_t spi_mst_conf_1;        /* conf */
	uint32_t spi_slv_conf_0;        /* conf */
	uint32_t apb_mem_conf_int;      /* conf */
	uint32_t sb2nb_cfg_dram_remap;  /* PASW remap register */
	uint32_t pbs_mux_sel_0;         /* control */
	uint32_t pbs_mux_sel_1;         /* control */
	uint32_t pbs_mux_sel_2;         /* control */
	uint32_t pbs_mux_conf;          /* control */
	uint32_t sb_int_bar_high;       /* PASW high */
	uint32_t sb_int_bar_low;        /* PASW low */
	uint32_t ufc_pbs_parity_err_high; /* log */
	uint32_t ufc_pbs_parity_err_low; /* log */
	uint32_t gen_conf;              /* cntl */
	uint32_t cpu_debug;             /* cntl */
	uint32_t uart0_debug;           /* status */
	uint32_t uart1_debug;           /* status */
	uint32_t uart2_debug;           /* status */
	uint32_t uart3_debug;           /* status */
	uint32_t uart0_conf_status;     /* cntl */
	uint32_t uart1_conf_status;     /* cntl */
	uint32_t uart2_conf_status;     /* cntl */
	uint32_t uart3_conf_status;     /* cntl */
	uint32_t gpio0_conf_status;     /* cntl */
	uint32_t gpio1_conf_status;     /* cntl */
	uint32_t gpio2_conf_status;     /* cntl */
	uint32_t gpio3_conf_status;     /* cntl */
	uint32_t gpio4_conf_status;     /* cntl */
	uint32_t i2c_gen_conf_status;   /* cntl */
	uint32_t i2c_gen_debug;         /* cntl */
	uint32_t watch_dog_reset_out;   /* cntl */
	uint32_t otp_magic_num;         /* cntl */
	uint32_t otp_cntl;              /* cntl */
	uint32_t otp_cfg_0;             /* cfg */
	uint32_t otp_cfg_1;             /* cfg */
	uint32_t otp_cfg_3;             /* cfg */
	uint32_t cfg_nand_0;            /* cfg */
	uint32_t cfg_nand_1;            /* cfg */
	uint32_t cfg_nand_2;            /* cfg */
	uint32_t cfg_nand_3;            /* cfg */
	uint32_t nb_nic_regs_bar_high;  /* PASW high */
	uint32_t nb_nic_regs_bar_low;   /* PASW low */
	uint32_t sb_nic_regs_bar_high;  /* PASW high */
	uint32_t sb_nic_regs_bar_low;   /* PASW low */
	uint32_t serdes_mux_multi_0;    /* control */
	uint32_t serdes_mux_multi_1;    /* control */
	uint32_t pbs_ulpi_mux_conf;     /* control */
	uint32_t wr_once_dbg_dis_ovrd_reg; /* cntl */
	uint32_t gpio5_conf_status;     /* cntl */
	uint32_t rsrvd[6];
};

struct al_pbs_regs {
	struct al_pbs_unit unit;
};

/*
* Registers Fields
*/

#endif /* __AL_PBS_REG_H */
