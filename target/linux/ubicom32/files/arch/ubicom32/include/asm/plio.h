/*
 * plio.h
 *	PLIO defines.
 *
 * Copyright © 2009 Ubicom Inc. <www.ubicom.com>.  All Rights Reserved.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 2 of the License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 * This file contains confidential information of Ubicom, Inc. and your use of
 * this file is subject to the Ubicom Software License Agreement distributed with
 * this file. If you are uncertain whether you are an authorized user or to report
 * any unauthorized use, please contact Ubicom, Inc. at +1-408-789-2200.
 * Unauthorized reproduction or distribution of this file is subject to civil and
 * criminal penalties.
 */

#ifndef __PLIO__H__
#define __PLIO__H__

#include <asm/ip5000.h>
#include <asm/thread.h>

#define PLIO_PORT		RD
#define PLIO_EXT_PORT		RI

#define TRANSMIT_FIFO_WATERMARK 8

/*
 * PLIO non-blocking register definitions
 */
#define PLIO_FN		2

typedef struct {
	unsigned	: 10;
	unsigned	rxfifo_thread_enable: 1;   /* allowed rxfifo thread enable */
	unsigned	: 1;
	unsigned	rxfifo_thread: 4;          /* allowed rxfifo thread access */
	unsigned	: 4;
	unsigned	br_thread: 4;              /* allowed blocking region thread access */
	unsigned	fn_reset: 4;               /* function reset bit vector */
	unsigned	rxfifo_sel: 1;             /* select between RXFIFO 0 and 1 */
	unsigned	fn_sel: 3;                 /* select port function */
} plio_io_function_t;

typedef struct {
	unsigned	: 24;
	unsigned	pin:8;
} plio_gpio_t;

typedef struct {
	unsigned 	: 16;
	unsigned 	txfifo_uf: 1;              /* TXFIFO underflow */
	unsigned 	txfifo_wm: 1;              /* TXFIFO watermark */
	unsigned 	rxfifo_of: 1;              /* RXFIFO overflow */
	unsigned 	rxfifo_wm: 1;              /* RXFIFO watermark */
	unsigned 	: 5;
	unsigned 	lreg_int_addr_rd: 1;       /* read from specified LREG address */
	unsigned 	lreg_int_addr_wr: 1;       /* write to specified LREG address */
	unsigned 	extctl_int: 4;             /* synchronized external interrupts */
	unsigned 	pfsm_int: 1;               /* state machine */
} plio_intstat_t;

typedef struct {
	unsigned 	txfifo_reset: 1;           /* TXFIFO reset for int_set only */
	unsigned 	rxfifo_reset: 1;           /* RXFIFO reset for int_set only */
	unsigned 	: 11;
	unsigned 	idif_txfifo_flush: 1;      /* flush TXFIFO and idif_txfifo */
	unsigned 	idif_rxfifo_flush: 1;      /* flush RXFIFO and idif_rxfifo */
	unsigned 	pfsm_start: 1;             /* input to fsm */
	unsigned 	txfifo_uf: 1;              /* TXFIFO underflow */
	unsigned 	txfifo_wm: 1;              /* TXFIFO watermark */
	unsigned 	rxfifo_of: 1;              /* RXFIFO overflow */
	unsigned 	rxfifo_wm: 1;              /* RXFIFO watermark */
	unsigned 	: 5;
	unsigned 	lreg_int_addr_rd: 1;       /* read from specified LREG address */
	unsigned 	lreg_int_addr_wr: 1;       /* write to specified LREG address */
	unsigned 	extctl_int: 4;             /* synchronized external interrupts */
	unsigned 	pfsm_int: 1;               /* state machine */
} plio_intset_t;

typedef enum {
	PLIO_PORT_MODE_D,
	PLIO_PORT_MODE_DE,
	PLIO_PORT_MODE_DI,
	PLIO_PORT_MODE_DEI,
	PLIO_PORT_MODE_DC,
} plio_port_mode_t;

typedef enum {
	PLIO_CLK_CORE,	/* CORE CLK */
	PLIO_CLK_IO,	/* IO CLK */
	PLIO_CLK_EXT, 	/* EXT CLK */
} plio_clk_src_t;
typedef struct {
	unsigned 		: 4;
	unsigned 		edif_iaena_sel: 1;         /* Input Address Enable Select */
	unsigned 		edif_iaclk_sel: 1;         /* Input Address Clock Select */
	unsigned 		edif_iald_inv: 1;          /* Input Address Strobe Invert */
	unsigned 		edif_idclk_sel: 1;         /* Input Data Clock Select */
	unsigned 		edif_idld_inv: 1;          /* Input Data Strobe Invert */
	unsigned 		edif_ds: 3;                /* specify IDR and ODR data shift */
	unsigned 		edif_cmp_mode: 1;          /* configure IDR comparator output */
	unsigned 		edif_idena_sel: 1;         /* Input Data Enable Select */
	unsigned 		ecif_extclk_ena: 1;        /* plio_extctl output select */
	unsigned 		idif_tx_fifo_cmd_sel: 1;   /* select pfsm_cmd data word position */
	unsigned 		ptif_porti_cfg: 2;         /* select port I pin configuration */
	unsigned 		ptif_portd_cfg: 3;         /* select port D pin configuration */
	plio_port_mode_t	ptif_port_mode: 3;      /* select other plio ports  */
	unsigned		icif_clk_plio_ext_inv: 1;  /* invert external plio clock when set */
	unsigned		icif_rst_plio: 1;          /* reset plio function and io fifos */
	plio_clk_src_t		icif_clk_src_sel: 2;      /* select plio clock source */
	unsigned		pfsm_prog: 1;              /* enable pfsm programming */
	unsigned 		pfsm_cmd: 3;               /* software input to pfsm */
} plio_fctl0_t;

typedef struct {
	unsigned 	: 2;
	unsigned 	idif_byteswap_tx: 3;       /* swap TXFIFO byte order */
	unsigned 	idif_byteswap_rx: 3;       /* swap RXFIFO byte order */
	unsigned 	: 1;
	unsigned 	lreg_ena: 1;               /* enable local register map */
	unsigned 	lreg_addr_fifo_cmp_ena: 1; /* enable a specific LREG address from/to TX/RX fifos */
	unsigned 	lreg_addr_fifo_cmp: 5;     /* LREG address routed from/to TX/RX fifos */
	unsigned 	: 1;
	unsigned 	dcod_iald_idld_sel: 2;     /* select address/data strobes */
	unsigned 	dcod_rw_src_sel: 1;        /* select LREG strobe source */
	unsigned 	dcod_rd_sel: 5;            /* select read strobe source */
	unsigned 	dcod_wr_sel: 5;            /* select write strobe source */
	unsigned 	dcod_rd_lvl: 1;            /* select active level of read strobe */
	unsigned 	dcod_wr_lvl: 1;            /* select active level of read strobe */
} plio_fctl1_t;

typedef struct {
	unsigned 	icif_eclk_div: 16;         /* external plio clock divider */
	unsigned 	icif_iclk_div: 16;         /* internal plio clock divider */
} plio_fctl2_t;

typedef struct {
	unsigned 	: 27;
	unsigned 	pfsm_state: 5;             /* current pfsm state */
} plio_stat_0_t;

typedef struct {
	unsigned 	: 3;
	unsigned 	lreg_r_int_addr: 5;
	unsigned 	: 11;
	unsigned 	lreg_w_int_addr: 5;
	unsigned 	lreg_w_int_data: 8;
} plio_stat_1_t;

typedef struct {
	unsigned 	: 32;
} plio_stat_2_t;

typedef struct {
	unsigned 	tx: 16;
	unsigned 	rx: 16;
} plio_io_fifo_wm_t, plio_io_fifo_lvl_t;


/* plio blocking region register definitions
 */
typedef struct {
	unsigned	 ns1: 5;
	unsigned	 ic1: 7;
	unsigned	 ec1: 4;
	unsigned	 ns0: 5;
	unsigned	 ic0: 7;
	unsigned	 ec0: 4;
} plio_sram_t;

typedef struct {
	unsigned	 : 2;
	unsigned	 s9: 3;
	unsigned	 s8: 3;
	unsigned	 s7: 3;
	unsigned	 s6: 3;
	unsigned	 s5: 3;
	unsigned	 s4: 3;
	unsigned	 s3: 3;
	unsigned	 s2: 3;
	unsigned	 s1: 3;
	unsigned	 s0: 3;
} plio_grpsel_t;

typedef struct {
	unsigned 	s7: 4;
	unsigned 	s6: 4;
	unsigned 	s5: 4;
	unsigned 	s4: 4;
	unsigned 	s3: 4;
	unsigned 	s2: 4;
	unsigned 	s1: 4;
	unsigned 	s0: 4;
} plio_cs_lut_t;

typedef struct {
	unsigned 	lut3: 8;
	unsigned 	lut2: 8;
	unsigned 	lut1: 8;
	unsigned 	lut0: 8;
} plio_extctl_t;

typedef struct {
	plio_grpsel_t	grpsel[4];
	u16_t		cv[16];
	plio_cs_lut_t	cs_lut[4];
	plio_extctl_t	extctl_o_lut[8];
} plio_pfsm_t;

typedef struct {
	u32_t		odr_oe_sel;
	u32_t		odr_oe;
	u32_t		cmp;
	u32_t		ncmp;
	u32_t		cmp_mask;
} plio_edif_t;

typedef enum {
	PLIO_ECIF_CLK_OUT	= 9,
	PLIO_ECIF_IALD		= 9,
	PLIO_ECIF_CLK_IN	= 8,
	PLIO_ECIF_IDLD		= 8,
	PLIO_ECIF_INT		= 2,
} plio_ecif_output_t;

typedef struct {
	u32_t 		bypass_sync;
	u32_t 		ift;
	u32_t 		output_type;
	u32_t 		output_ena;
	u32_t 		output_lvl;
} plio_ecif_t;

typedef struct {
	u32_t 		idr_addr_pos_mask;
	u32_t 		reserved;
	u32_t 		lreg_bar;
} plio_dcod_t;

typedef struct {
	u32_t 		addr_rd_ena;
	u32_t 		addr_wr_ena;
	u32_t 		addr_rd_int_ena;
	u32_t 		addr_wr_int_ena;
} plio_lcfg_t;


/*
 * PLIO configuration
 */
typedef struct {
	plio_fctl0_t 	fctl0;
	plio_fctl1_t 	fctl1;
	plio_fctl2_t 	fctl2;
} plio_fctl_t;

typedef struct {
	plio_pfsm_t 	pfsm;
	plio_edif_t 	edif;
	plio_ecif_t 	ecif;
	plio_dcod_t 	dcod;
	plio_lcfg_t 	lcfg;
} plio_config_t;

typedef struct {
	plio_io_function_t	function;
	plio_gpio_t		gpio_ctl;
	plio_gpio_t		gpio_out;
	plio_gpio_t		gpio_in;
	plio_intstat_t		intstat;
	plio_intstat_t		intmask;
	plio_intset_t		intset;
	plio_intstat_t		intclr;
	unsigned		tx_lo;
	unsigned		tx_hi;
	unsigned		rx_lo;
	unsigned		rx_hi;
	plio_fctl0_t		fctl0;
	plio_fctl1_t		fctl1;
	plio_fctl2_t		fctl2;
	plio_stat_0_t		stat0;
	plio_stat_1_t		stat1;
	plio_stat_2_t		stat2;
	plio_io_fifo_wm_t	fifo_wm;
	plio_io_fifo_lvl_t	fifo_lvl;
} plio_nbr_t;

typedef struct {
	u32_t			pfsm_sram[256];
	plio_config_t		config;
} plio_br_t;

#define PLIO_NBR	((plio_nbr_t *)(PLIO_PORT))
#define PLIO_BR		((plio_br_t *)((PLIO_PORT + IO_PORT_BR_OFFSET)))
#define PEXT_NBR	((plio_nbr_t *)(PLIO_EXT_PORT))

extern void plio_init(const plio_fctl_t *plio_fctl, const plio_config_t *plio_config, const plio_sram_t plio_sram_cfg[], int sram_cfg_size);

#endif // __PLIO__H__
