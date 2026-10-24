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


#ifndef __AL_PBS_REG_H
#define __AL_PBS_REG_H

#include <mach/al_hal_common.h>

#ifdef __cplusplus
extern "C" {
#endif
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


/**** conf_bus register ****/
/* read slave error enable */
#define PBS_UNIT_CONF_BUS_RD_SLVERR_EN   (1 << 0)
/* write slave error enable */
#define PBS_UNIT_CONF_BUS_WR_SLVERR_EN   (1 << 1)
/* read decode error enable */
#define PBS_UNIT_CONF_BUS_RD_DECERR_EN   (1 << 2)
/* write decode error enable */
#define PBS_UNIT_CONF_BUS_WR_DECERR_EN   (1 << 3)
/* for debug clear the apb SM */
#define PBS_UNIT_CONF_BUS_CLR_APB_FSM    (1 << 4)
/* for debug clear the WFIFO */
#define PBS_UNIT_CONF_BUS_CLR_WFIFO_CLEAR (1 << 5)
/* Arbiter between read and write channel */
#define PBS_UNIT_CONF_BUS_WRR_CNT_MASK   0x000001C0
#define PBS_UNIT_CONF_BUS_WRR_CNT_SHIFT  6

/**** dram_0_nb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_DRAM_0_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_DRAM_0_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_DRAM_0_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_DRAM_0_NB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_DRAM_0_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_DRAM_0_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_1_nb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_DRAM_1_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_DRAM_1_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_DRAM_1_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_DRAM_1_NB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_DRAM_1_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_DRAM_1_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_2_nb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_DRAM_2_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_DRAM_2_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_DRAM_2_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_DRAM_2_NB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_DRAM_2_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_DRAM_2_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_3_nb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_DRAM_3_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_DRAM_3_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_DRAM_3_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_DRAM_3_NB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_DRAM_3_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_DRAM_3_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** msix_nb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_MSIX_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_MSIX_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_MSIX_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_MSIX_NB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_MSIX_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_MSIX_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_0_sb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_DRAM_0_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_DRAM_0_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_DRAM_0_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_DRAM_0_SB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_DRAM_0_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_DRAM_0_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_1_sb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_DRAM_1_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_DRAM_1_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_DRAM_1_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_DRAM_1_SB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_DRAM_1_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_DRAM_1_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_2_sb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_DRAM_2_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_DRAM_2_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_DRAM_2_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_DRAM_2_SB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_DRAM_2_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_DRAM_2_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_3_sb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_DRAM_3_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_DRAM_3_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_DRAM_3_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_DRAM_3_SB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_DRAM_3_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_DRAM_3_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** msix_sb_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_MSIX_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_MSIX_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_MSIX_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_MSIX_SB_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_MSIX_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_MSIX_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem0_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PCIE_MEM0_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PCIE_MEM0_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PCIE_MEM0_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PCIE_MEM0_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PCIE_MEM0_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PCIE_MEM0_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem1_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PCIE_MEM1_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PCIE_MEM1_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PCIE_MEM1_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PCIE_MEM1_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PCIE_MEM1_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PCIE_MEM1_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem2_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PCIE_MEM2_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PCIE_MEM2_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PCIE_MEM2_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PCIE_MEM2_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PCIE_MEM2_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PCIE_MEM2_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam0_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PCIE_EXT_ECAM0_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PCIE_EXT_ECAM0_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PCIE_EXT_ECAM0_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PCIE_EXT_ECAM0_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PCIE_EXT_ECAM0_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PCIE_EXT_ECAM0_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam1_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PCIE_EXT_ECAM1_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PCIE_EXT_ECAM1_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PCIE_EXT_ECAM1_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PCIE_EXT_ECAM1_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PCIE_EXT_ECAM1_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PCIE_EXT_ECAM1_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam2_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PCIE_EXT_ECAM2_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PCIE_EXT_ECAM2_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PCIE_EXT_ECAM2_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PCIE_EXT_ECAM2_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PCIE_EXT_ECAM2_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PCIE_EXT_ECAM2_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_nor_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PBS_NOR_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PBS_NOR_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PBS_NOR_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PBS_NOR_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PBS_NOR_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PBS_NOR_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_spi_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PBS_SPI_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PBS_SPI_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PBS_SPI_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PBS_SPI_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PBS_SPI_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PBS_SPI_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_nand_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PBS_NAND_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PBS_NAND_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PBS_NAND_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PBS_NAND_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PBS_NAND_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PBS_NAND_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_int_mem_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PBS_INT_MEM_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PBS_INT_MEM_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PBS_INT_MEM_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PBS_INT_MEM_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PBS_INT_MEM_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PBS_INT_MEM_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_boot_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PBS_BOOT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PBS_BOOT_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PBS_BOOT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PBS_BOOT_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PBS_BOOT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PBS_BOOT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** nb_int_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_NB_INT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_NB_INT_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_NB_INT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_NB_INT_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_NB_INT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_NB_INT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** nb_stm_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_NB_STM_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_NB_STM_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_NB_STM_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_NB_STM_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_NB_STM_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_NB_STM_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ecam_int_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PCIE_ECAM_INT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PCIE_ECAM_INT_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PCIE_ECAM_INT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PCIE_ECAM_INT_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PCIE_ECAM_INT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PCIE_ECAM_INT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem_int_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_PCIE_MEM_INT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_PCIE_MEM_INT_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_PCIE_MEM_INT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_PCIE_MEM_INT_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_PCIE_MEM_INT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_PCIE_MEM_INT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** winit_cntl register ****/
/* When set enable accsess to winit regs, in normal mode. */
#define PBS_UNIT_WINIT_CNTL_ENABLE_WINIT_REGS_ACCESS (1 << 0)
/* Rsrvd */
#define PBS_UNIT_WINIT_CNTL_RSRVD_MASK   0xFFFFFFFE
#define PBS_UNIT_WINIT_CNTL_RSRVD_SHIFT  1

/**** latch_bars register ****/
/* The SW should clear this bit before any bar update, and reset ... */
#define PBS_UNIT_LATCH_BARS_ENABLE       (1 << 0)
/* Rsrvd */
#define PBS_UNIT_LATCH_BARS_RSRVD_MASK   0xFFFFFFFE
#define PBS_UNIT_LATCH_BARS_RSRVD_SHIFT  1

/**** pcie_conf_0 register ****/
/* NOT_use, config internal inside each PCIe core */
#define PBS_UNIT_PCIE_CONF_0_DEVS_TYPE_MASK 0x00000FFF
#define PBS_UNIT_PCIE_CONF_0_DEVS_TYPE_SHIFT 0
/* sys_aux_det value */
#define PBS_UNIT_PCIE_CONF_0_SYS_AUX_PWR_DET_VEC_MASK 0x00007000
#define PBS_UNIT_PCIE_CONF_0_SYS_AUX_PWR_DET_VEC_SHIFT 12
/* Rsrvd */
#define PBS_UNIT_PCIE_CONF_0_RSRVD_MASK  0xFFFF8000
#define PBS_UNIT_PCIE_CONF_0_RSRVD_SHIFT 15

/**** pcie_conf_1 register ****/
/* which pcie exist, the PCIe device will be under reset untill  ... */
#define PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_MASK 0x00000007
#define PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_SHIFT 0
/* Rsrvd */
#define PBS_UNIT_PCIE_CONF_1_RSRVD_MASK  0xFFFFFFF8
#define PBS_UNIT_PCIE_CONF_1_RSRVD_SHIFT 3

/**** serdes_mux_pipe register ****/
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_2_MASK 0x00000007
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_2_SHIFT 0
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_3 (1 << 3)
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_3_MASK 0x00000070
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_3_SHIFT 4
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_7 (1 << 7)
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_0_MASK 0x00000300
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_0_SHIFT 8
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_1_MASK 0x00000C00
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_1_SHIFT 10
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_0_MASK 0x00003000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_0_SHIFT 12
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_1_MASK 0x0000C000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_1_SHIFT 14
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_USB_A_0_MASK 0x00030000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_USB_A_0_SHIFT 16
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_USB_B_0_MASK 0x000C0000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_USB_B_0_SHIFT 18
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_2_MASK 0x00300000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_2_SHIFT 20
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_23_22_MASK 0x00C00000
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_23_22_SHIFT 22
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_3_MASK 0x07000000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_3_SHIFT 24
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_MASK 0xF8000000
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_SHIFT 27

/**** dma_io_master_map register ****/
/* when set map all the dma_io transaction to the dram, regardle ... */
#define PBS_UNIT_DMA_IO_MASTER_MAP_CNTL  (1 << 0)
/* Rsrvd */
#define PBS_UNIT_DMA_IO_MASTER_MAP_RSRVD_MASK 0xFFFFFFFE
#define PBS_UNIT_DMA_IO_MASTER_MAP_RSRVD_SHIFT 1

/**** i2c_pld_status_high register ****/
/* i2c pre load status */
#define PBS_UNIT_I2C_PLD_STATUS_HIGH_STATUS_MASK 0x000000FF
#define PBS_UNIT_I2C_PLD_STATUS_HIGH_STATUS_SHIFT 0

/**** spi_dbg_status_high register ****/
/* spi dbg load status */
#define PBS_UNIT_SPI_DBG_STATUS_HIGH_STATUS_MASK 0x000000FF
#define PBS_UNIT_SPI_DBG_STATUS_HIGH_STATUS_SHIFT 0

/**** spi_mst_status_high register ****/
/* sp imst load status */
#define PBS_UNIT_SPI_MST_STATUS_HIGH_STATUS_MASK 0x000000FF
#define PBS_UNIT_SPI_MST_STATUS_HIGH_STATUS_SHIFT 0

/**** mem_pbs_parity_err_high register ****/
/* address latch in case of error */
#define PBS_UNIT_MEM_PBS_PARITY_ERR_HIGH_ADDR_MASK 0x000000FF
#define PBS_UNIT_MEM_PBS_PARITY_ERR_HIGH_ADDR_SHIFT 0

/**** cfg_axi_conf_0 register ****/
/* value */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_RD_ID_MASK 0x0000007F
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_RD_ID_SHIFT 0
/* value */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_WR_ID_MASK 0x00003F80
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_WR_ID_SHIFT 7
/* value */
#define PBS_UNIT_CFG_AXI_CONF_0_PLD_WR_ID_MASK 0x001FC000
#define PBS_UNIT_CFG_AXI_CONF_0_PLD_WR_ID_SHIFT 14
/* value */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_AWCACHE_MASK 0x01E00000
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_AWCACHE_SHIFT 21
/* value */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_ARCACHE_MASK 0x1E000000
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_ARCACHE_SHIFT 25
/* value */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_AXPROT_MASK 0xE0000000
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_AXPROT_SHIFT 29

/**** cfg_axi_conf_1 register ****/
/* value */
#define PBS_UNIT_CFG_AXI_CONF_1_DBG_ARUSER_MASK 0x03FFFFFF
#define PBS_UNIT_CFG_AXI_CONF_1_DBG_ARUSER_SHIFT 0
/* value */
#define PBS_UNIT_CFG_AXI_CONF_1_DBG_ARQOS_MASK 0x3C000000
#define PBS_UNIT_CFG_AXI_CONF_1_DBG_ARQOS_SHIFT 26

/**** cfg_axi_conf_2 register ****/
/* value */
#define PBS_UNIT_CFG_AXI_CONF_2_DBG_AWUSER_MASK 0x03FFFFFF
#define PBS_UNIT_CFG_AXI_CONF_2_DBG_AWUSER_SHIFT 0
/* value */
#define PBS_UNIT_CFG_AXI_CONF_2_DBG_AWQOS_MASK 0x3C000000
#define PBS_UNIT_CFG_AXI_CONF_2_DBG_AWQOS_SHIFT 26

/**** spi_mst_conf_0 register ****/
/* value */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SRL (1 << 0)
/* value */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SCPOL (1 << 1)
/* value */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SCPH (1 << 2)
/* value */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SER_MASK 0x00000078
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SER_SHIFT 3
/* value */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_BAUD_MASK 0x007FFF80
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_BAUD_SHIFT 7
/* value */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_RD_CMD_MASK 0x7F800000
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_RD_CMD_SHIFT 23

/**** spi_mst_conf_1 register ****/
/* value */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_WR_CMD_MASK 0x000000FF
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_WR_CMD_SHIFT 0
/* value */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_ADDR_BYTES_NUM_MASK 0x00000700
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_ADDR_BYTES_NUM_SHIFT 8
/* value */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_TMODE_MASK 0x00001800
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_TMODE_SHIFT 11
/* value */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_FAST_RD (1 << 13)

/**** spi_slv_conf_0 register ****/
/* value */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_BAUD_MASK 0x0000FFFF
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_BAUD_SHIFT 0
/* value. The reset va;ue is according to boot strap */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SCPOL (1 << 16)
/* value. The reset va;ue is according to boot strap */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SCPH (1 << 17)
/* value */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SER_MASK 0x03FC0000
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SER_SHIFT 18
/* value */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SRL (1 << 26)
/* value */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_TMODE_MASK 0x18000000
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_TMODE_SHIFT 27

/**** apb_mem_conf_int register ****/
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_PBS_WRR_CNT_MASK 0x00000007
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_PBS_WRR_CNT_SHIFT 0
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_I2C_PLD_APB_MIX_ARB (1 << 3)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_SPI_DBG_APB_MIX_ARB (1 << 4)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_SPI_MST_APB_MIX_ARB (1 << 5)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_I2C_PLD_CLEAR_FSM (1 << 6)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_SPI_DBG_CLEAR_FSM (1 << 7)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_SPI_MST_CLEAR_FSM (1 << 8)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_PBS_AXI_FSM_CLEAR (1 << 9)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_PBS_AXI_FIFOS_CLEAR (1 << 10)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_BOOTROM_PARITY_EN (1 << 11)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_RD_SLV_ERR_EN (1 << 12)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_RD_DEC_ERR_EN (1 << 13)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_WR_SLV_ERR_EN (1 << 14)
/* value */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_WR_DEC_ERR_EN (1 << 15)

/**** sb_int_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_SB_INT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_SB_INT_BAR_LOW_WIN_SIZE_SHIFT 0
/* reseved fiels */
#define PBS_UNIT_SB_INT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_SB_INT_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_SB_INT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_SB_INT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** ufc_pbs_parity_err_high register ****/
/* address latch in case of error */
#define PBS_UNIT_UFC_PBS_PARITY_ERR_HIGH_ADDR_MASK 0x000000FF
#define PBS_UNIT_UFC_PBS_PARITY_ERR_HIGH_ADDR_SHIFT 0

/**** uart0_conf_status register ****/
/* Conf:// [0] -- DSR_N RW bit// [1] -- DCD_N RW bit// [2] -- RI ... */
#define PBS_UNIT_UART0_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_UART0_CONF_STATUS_CONF_SHIFT 0
/* Status:// [16] -- dtr_n RO bit// [17] -- OUT1_N RO bit// [18] ... */
#define PBS_UNIT_UART0_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_UART0_CONF_STATUS_STATUS_SHIFT 16

/**** uart1_conf_status register ****/
/* Conf: // [0] -- DSR_N RW bit // [1] -- DCD_N RW bit // [2] -- ... */
#define PBS_UNIT_UART1_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_UART1_CONF_STATUS_CONF_SHIFT 0
/* Status: // [16] -- dtr_n RO bit // [17] -- OUT1_N RO bit // [ ... */
#define PBS_UNIT_UART1_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_UART1_CONF_STATUS_STATUS_SHIFT 16

/**** uart2_conf_status register ****/
/* Conf: // [0] -- DSR_N RW bit // [1] -- DCD_N RW bit // [2] -- ... */
#define PBS_UNIT_UART2_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_UART2_CONF_STATUS_CONF_SHIFT 0
/* Status: // [16] -- dtr_n RO bit // [17] -- OUT1_N RO bit // [ ... */
#define PBS_UNIT_UART2_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_UART2_CONF_STATUS_STATUS_SHIFT 16

/**** uart3_conf_status register ****/
/* Conf: // [0] -- DSR_N RW bit // [1] -- DCD_N RW bit // [2] -- ... */
#define PBS_UNIT_UART3_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_UART3_CONF_STATUS_CONF_SHIFT 0
/* Status: // [16] -- dtr_n RO bit // [17] -- OUT1_N RO bit // [ ... */
#define PBS_UNIT_UART3_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_UART3_CONF_STATUS_STATUS_SHIFT 16

/**** gpio0_conf_status register ****/
/* Cntl://  [7:0] nGPAFEN;              // from regfile//  [15:8 ... */
#define PBS_UNIT_GPIO0_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO0_CONF_STATUS_CONF_SHIFT 0
/* staus:
//  [24:16] GPAFIN;             // to regfile */
#define PBS_UNIT_GPIO0_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO0_CONF_STATUS_STATUS_SHIFT 16

/**** gpio1_conf_status register ****/
/* Cntl://  [7:0] nGPAFEN;              // from regfile//  [15:8 ... */
#define PBS_UNIT_GPIO1_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO1_CONF_STATUS_CONF_SHIFT 0
/* staus:
//  [24:16] GPAFIN;             // to regfile */
#define PBS_UNIT_GPIO1_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO1_CONF_STATUS_STATUS_SHIFT 16

/**** gpio2_conf_status register ****/
/* Cntl://  [7:0] nGPAFEN;              // from regfile//  [15:8 ... */
#define PBS_UNIT_GPIO2_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO2_CONF_STATUS_CONF_SHIFT 0
/* staus:
//  [24:16] GPAFIN;             // to regfile */
#define PBS_UNIT_GPIO2_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO2_CONF_STATUS_STATUS_SHIFT 16

/**** gpio3_conf_status register ****/
/* Cntl://  [7:0] nGPAFEN;              // from regfile//  [15:8 ... */
#define PBS_UNIT_GPIO3_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO3_CONF_STATUS_CONF_SHIFT 0
/* staus:
//  [24:16] GPAFIN;             // to regfile */
#define PBS_UNIT_GPIO3_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO3_CONF_STATUS_STATUS_SHIFT 16

/**** gpio4_conf_status register ****/
/* Cntl://  [7:0] nGPAFEN;              // from regfile//  [15:8 ... */
#define PBS_UNIT_GPIO4_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO4_CONF_STATUS_CONF_SHIFT 0
/* staus:
//  [24:16] GPAFIN;             // to regfile */
#define PBS_UNIT_GPIO4_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO4_CONF_STATUS_STATUS_SHIFT 16

/**** i2c_gen_conf_status register ****/
/* cntl
// [0] -- dma_tx_ack
// [1] -- dma_rx_ack */
#define PBS_UNIT_I2C_GEN_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_I2C_GEN_CONF_STATUS_CONF_SHIFT 0
/* Status// [16] – dma_tx_req RO bit// [17] -- dma_tx_single RO  ... */
#define PBS_UNIT_I2C_GEN_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_I2C_GEN_CONF_STATUS_STATUS_SHIFT 16

/**** watch_dog_reset_out register ****/
/* [0] if set to 1'b1, WD0 can not generate reset_out_n[1] if se ... */
#define PBS_UNIT_WATCH_DOG_RESET_OUT_DISABLE_MASK 0x0000000F
#define PBS_UNIT_WATCH_DOG_RESET_OUT_DISABLE_SHIFT 0

/**** otp_cntl register ****/
/* from reg file Config To bypass the copy from OTPW to OTPR */
#define PBS_UNIT_OTP_CNTL_IGNORE_OTPW    (1 << 0)
/* Not use comes from bond. */
#define PBS_UNIT_OTP_CNTL_IGNORE_PRELOAD (1 << 1)
/* margin read from the fuse box */
#define PBS_UNIT_OTP_CNTL_OTPW_MARGIN_READ (1 << 2)
/* Indicate when OTP busy  */
#define PBS_UNIT_OTP_CNTL_OTP_BUSY       (1 << 3)

/**** otp_cfg_0 register ****/
/* cfg to to OTP cntl. */
#define PBS_UNIT_OTP_CFG_0_CFG_OTPW_PWRDN_CNT_MASK 0x0000FFFF
#define PBS_UNIT_OTP_CFG_0_CFG_OTPW_PWRDN_CNT_SHIFT 0
/* cfg to to OTP cntl. */
#define PBS_UNIT_OTP_CFG_0_CFG_OTPW_READ_CNT_MASK 0xFFFF0000
#define PBS_UNIT_OTP_CFG_0_CFG_OTPW_READ_CNT_SHIFT 16

/**** otp_cfg_1 register ****/
/* cfg to to OTP cntl.  */
#define PBS_UNIT_OTP_CFG_1_CFG_OTPW_PGM_CNT_MASK 0x0000FFFF
#define PBS_UNIT_OTP_CFG_1_CFG_OTPW_PGM_CNT_SHIFT 0
/* cfg to to OTP cntl. */
#define PBS_UNIT_OTP_CFG_1_CFG_OTPW_PREP_CNT_MASK 0xFFFF0000
#define PBS_UNIT_OTP_CFG_1_CFG_OTPW_PREP_CNT_SHIFT 16

/**** otp_cfg_3 register ****/
/* cfg to to OTP cntl. */
#define PBS_UNIT_OTP_CFG_3_CFG_OTPW_PS18_CNT_MASK 0x0000FFFF
#define PBS_UNIT_OTP_CFG_3_CFG_OTPW_PS18_CNT_SHIFT 0
/* cfg to to OTP cntl. */
#define PBS_UNIT_OTP_CFG_3_CFG_OTPW_PWRUP_CNT_MASK 0xFFFF0000
#define PBS_UNIT_OTP_CFG_3_CFG_OTPW_PWRUP_CNT_SHIFT 16

/**** nb_nic_regs_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_NB_NIC_REGS_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_NB_NIC_REGS_BAR_LOW_WIN_SIZE_SHIFT 0
/* reserved fields */
#define PBS_UNIT_NB_NIC_REGS_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_NB_NIC_REGS_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_NB_NIC_REGS_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_NB_NIC_REGS_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** sb_nic_regs_bar_low register ****/
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_UNIT_SB_NIC_REGS_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_UNIT_SB_NIC_REGS_BAR_LOW_WIN_SIZE_SHIFT 0
/* reserved fields */
#define PBS_UNIT_SB_NIC_REGS_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_UNIT_SB_NIC_REGS_BAR_LOW_RSRVD_SHIFT 6
/* Rsrvd */
#define PBS_UNIT_SB_NIC_REGS_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_UNIT_SB_NIC_REGS_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** serdes_mux_multi_0 register ****/
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_8_MASK 0x00000007
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_8_SHIFT 0
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_3 (1 << 3)
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_9_MASK 0x00000070
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_9_SHIFT 4
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_7 (1 << 7)
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_10_MASK 0x00000700
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_10_SHIFT 8
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_11 (1 << 11)
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_11_MASK 0x00007000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_11_SHIFT 12
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_15 (1 << 15)
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_12_MASK 0x00030000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_12_SHIFT 16
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_13_MASK 0x000C0000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_13_SHIFT 18
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_14_MASK 0x00300000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_14_SHIFT 20
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_15_MASK 0x00C00000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_15_SHIFT 22
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_MASK 0xFF000000
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_SHIFT 24

/**** serdes_mux_multi_1 register ****/
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_A_0_MASK 0x00000003
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_A_0_SHIFT 0
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_3_2_MASK 0x0000000C
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_3_2_SHIFT 2
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_B_0_MASK 0x00000070
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_B_0_SHIFT 4
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_7 (1 << 7)
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_C_0_MASK 0x00000300
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_C_0_SHIFT 8
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_11_10_MASK 0x00000C00
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_11_10_SHIFT 10
/* serdes one hot mux control. */
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_D_0_MASK 0x00007000
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_D_0_SHIFT 12
/* Rsrvd */
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_MASK 0xFFFF8000
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_SHIFT 15

/**** pbs_ulpi_mux_conf register ****/
/* Value 0 – select dedicate pins for the USB-1 inputs */
#define PBS_UNIT_PBS_ULPI_MUX_CONF_SEL_UPLI_IN_PBSMUX_MASK 0x000007FF
#define PBS_UNIT_PBS_ULPI_MUX_CONF_SEL_UPLI_IN_PBSMUX_SHIFT 0
/* [3] - force to zero[2] == 1 - forcee register selection [1 :  ... */
#define PBS_UNIT_PBS_ULPI_MUX_CONF_REG_MDIO_BYPASS_SEL_MASK 0x0000F000
#define PBS_UNIT_PBS_ULPI_MUX_CONF_REG_MDIO_BYPASS_SEL_SHIFT 12
/* [0] set the clk_ulpi OE for USB0, 1'b0 set to input 1'b1 set  ... */
#define PBS_UNIT_PBS_ULPI_MUX_CONF_RSRVD_MASK 0xFFFF0000
#define PBS_UNIT_PBS_ULPI_MUX_CONF_RSRVD_SHIFT 16

/**** wr_once_dbg_dis_ovrd_reg register ****/
/* This register can be written only once.. */
#define PBS_UNIT_WR_ONCE_DBG_DIS_OVRD_REG_WR_ONCE_DBG_DIS_OVRD (1 << 0)

#define PBS_UNIT_WR_ONCE_DBG_DIS_OVRD_REG_RSRVD_MASK 0xFFFFFFFE
#define PBS_UNIT_WR_ONCE_DBG_DIS_OVRD_REG_RSRVD_SHIFT 1

/**** gpio5_conf_status register ****/
/* Cntl: //  [7:0] nGPAFEN;              // from regfile //  [15 ... */
#define PBS_UNIT_GPIO5_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO5_CONF_STATUS_CONF_SHIFT 0
/* staus:  //  [24:16] GPAFIN;             // to regfile */
#define PBS_UNIT_GPIO5_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO5_CONF_STATUS_STATUS_SHIFT 16

/**** pbs_sb2nb_cfg_dram_remap register ****/
#define PBS_UNIT_SB2NB_REMAP_BASE_ADDR_SHIFT		5
#define PBS_UNIT_SB2NB_REMAP_BASE_ADDR_MASK		0x0000FFE0
#define PBS_UNIT_SB2NB_REMAP_TRANSL_BASE_ADDR_SHIFT	21
#define PBS_UNIT_SB2NB_REMAP_TRANSL_BASE_ADDR_MASK	0xFFE00000

/* For remapping are used bits [39 - 29] of DRAM 40bit Physical address */
#define PBS_UNIT_DRAM_SRC_REMAP_BASE_ADDR_SHIFT	29
#define PBS_UNIT_DRAM_DST_REMAP_BASE_ADDR_SHIFT	29
#define PBS_UNIT_DRAM_REMAP_BASE_ADDR_MASK	0xFFE0000000


#ifdef __cplusplus
}
#endif

#endif /* __AL_PBS_REG_H */




