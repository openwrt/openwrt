/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_serdes_REG_H
#define __AL_serdes_REG_H

/*
* Unit Registers
*/

struct serdes_intc {
	uint32_t rsrvd[0x100 / sizeof(uint32_t)];
};

struct serdes_gen {
	uint32_t version;               /* SERDES registers Version */
	uint32_t rsrvd1[0x0c / sizeof(uint32_t)];
	uint32_t reg_addr;              /* SERDES register file address */
	uint32_t reg_data;              /* SERDES register file data */
	uint32_t rsrvd2[0x08 / sizeof(uint32_t)];
	uint32_t ictl_multi_bist;       /* SERDES control */
	uint32_t ictl_pcs;              /* SERDES control */
	uint32_t ictl_pma;              /* SERDES control */
	uint32_t rsrvd3;
	uint32_t ipd_multi_synth;       /* SERDES control */
	uint32_t irst;                  /* SERDES control */
	uint32_t octl_multi_synthready; /* SERDES control */
	uint32_t octl_multi_synthstatus; /* SERDES control */
	uint32_t clk_out;               /* SERDES control */
	uint32_t rsrvd[47];
};
struct serdes_lane {
	uint32_t rsrvd1[0x10 / sizeof(uint32_t)];
	uint32_t octl_pma;              /* SERDES status */
	uint32_t ictl_multi_andme;      /* SERDES control */
	uint32_t ictl_multi_lb;         /* SERDES control */
	uint32_t ictl_multi_rxbist;     /* SERDES control */
	uint32_t ictl_multi_txbist;     /* SERDES control */
	uint32_t ictl_multi;            /* SERDES control */
	uint32_t ictl_multi_rxeq;       /* SERDES control */
	uint32_t ictl_multi_rxeq_l_low; /* SERDES control */
	uint32_t ictl_multi_rxeq_l_high; /* SERDES control */
	uint32_t ictl_multi_rxeyediag;  /* SERDES control */
	uint32_t ictl_multi_txdeemph;   /* SERDES control */
	uint32_t ictl_multi_txmargin;   /* SERDES control */
	uint32_t ictl_multi_txswing;    /* SERDES control */
	uint32_t idat_multi;            /* SERDES control */
	uint32_t ipd_multi;             /* SERDES control */
	uint32_t octl_multi_rxbist;     /* SERDES control */
	uint32_t octl_multi;            /* SERDES control */
	uint32_t octl_multi_rxeyediag;  /* SERDES control */
	uint32_t odat_multi_rxbist;     /* SERDES control */
	uint32_t odat_multi_rxeq;       /* SERDES control */
	uint32_t multi_rx_dvalid;       /* SERDES control */
	uint32_t reserved;              /* SERDES control */
	uint32_t rsrvd[6];
};

struct al_serdes_regs {
	struct serdes_intc intc;
	struct serdes_gen gen;
	struct serdes_lane lane[4];
};

/*
* Registers Fields
*/

/**** ICTL_MULTI_BIST register ****/

/**** ICTL_PCS register ****/

/**** ICTL_PMA register ****/

/**** IPD_MULTI_SYNTH register ****/

/**** IRST register ****/

#define SERDES_GEN_IRST_PIPE_RST_L0_B_A  (1 << 3)

#define SERDES_GEN_IRST_POR_B_A          (1 << 12)

#define SERDES_GEN_IRST_PIPE_RST_L0_B_A_SEL (1 << 19)

/**** OCTL_MULTI_SYNTHREADY register ****/

/**** OCTL_MULTI_SYNTHSTATUS register ****/

/**** clk_out register ****/

/**** OCTL_PMA register ****/

/**** ICTL_MULTI_ANDME register ****/

/**** ICTL_MULTI_LB register ****/

/**** ICTL_MULTI_RXBIST register ****/

/**** ICTL_MULTI_TXBIST register ****/

/**** ICTL_MULTI register ****/

/**** ICTL_MULTI_RXEQ register ****/

#define SERDES_LANE_ICTL_MULTI_RXEQ_START_L_A (1 << 1)

/**** ICTL_MULTI_RXEQ_L_high register ****/

/**** ICTL_MULTI_RXEYEDIAG register ****/

/**** ICTL_MULTI_TXDEEMPH register ****/

/**** ICTL_MULTI_TXMARGIN register ****/

/**** ICTL_MULTI_TXSWING register ****/

/**** IDAT_MULTI register ****/

/**** IPD_MULTI register ****/

/**** OCTL_MULTI_RXBIST register ****/

/**** OCTL_MULTI register ****/

#define SERDES_LANE_OCTL_MULTI_RXEQ_DONE_L_A (1 << 1)

/**** OCTL_MULTI_RXEYEDIAG register ****/

/**** ODAT_MULTI_RXBIST register ****/

/**** ODAT_MULTI_RXEQ register ****/

/**** MULTI_RX_DVALID register ****/

/**** reserved register ****/

#endif /* __AL_serdes_REG_H */
