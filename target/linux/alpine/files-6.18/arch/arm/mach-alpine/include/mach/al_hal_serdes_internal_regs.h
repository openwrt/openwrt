/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */
#ifndef __AL_SERDES_INTERNAL_REGS_H__
#define  __AL_SERDES_INTERNAL_REGS_H__

/*******************************************************************************
 * Per lane register fields
 ******************************************************************************/
/*
 * RX and TX lane hard reset
 * 0 - Hard reset is asserted
 * 1 - Hard reset is de-asserted
 */
#define SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_REG_NUM			2
#define SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_MASK			0x01
#define SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_VAL_ASSERT		0x00
#define SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_VAL_DEASSERT		0x01

/*
 * RX and TX lane hard reset control
 * 0 - Hard reset is taken from the interface pins
 * 1 - Hard reset is taken from registers
 */
#define SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_REG_NUM		2
#define SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_MASK			0x02
#define SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_VAL_REGS		0x02

/* RX lane word width */
#define SERDES_IREG_FLD_PCSRX_DATAWIDTH_REG_NUM				5

/* TX lane word width */
#define SERDES_IREG_FLD_PCSTX_DATAWIDTH_REG_NUM				5

/* RX lane rate select */
#define SERDES_IREG_FLD_PCSRX_DIVRATE_REG_NUM				6

/* TX lane rate select */
#define SERDES_IREG_FLD_PCSTX_DIVRATE_REG_NUM				6

/*
 * PMA serial RX-to-TX loop-back enable (from AGC to IO Driver). Serial receive
 * to transmit loopback: 0 - Disables loopback 1 - Transmits the untimed,
 * partial equalized RX signal out the transmit IO pins
 */
#define SERDES_IREG_FLD_LB_RX2TXUNTIMEDEN_REG_NUM			7

/*
 * PMA TX-to-RX buffered serial loop-back enable (bypasses IO Driver). Serial
 * transmit to receive buffered loopback: 0 - Disables loopback 1 - Loops back
 * the TX serializer output into the CDR
 */
#define SERDES_IREG_FLD_LB_TX2RXBUFTIMEDEN_REG_NUM			7

/*
 * PMA TX-to-RX I/O serial loop-back enable (loop back done directly from TX to
 * RX pads). Serial IO loopback from the transmit lane IO pins to the receive
 * lane IO pins: 0 - Disables loopback 1 - Loops back the driver IO signal to
 * the RX IO pins
 */
#define SERDES_IREG_FLD_LB_TX2RXIOTIMEDEN_REG_NUM			7

/*
 * PMA Parallel RX-to-TX loop-back enable. Parallel loopback from the PMA
 * receive lane 20-bit data ports, to the transmit lane 20-bit data ports 0 -
 * Disables loopback 1 - Loops back the 20-bit receive data port to the
 * transmitter
 */
#define SERDES_IREG_FLD_LB_PARRX2TXTIMEDEN_REG_NUM			7

/*
 * PMA CDR recovered-clock loopback enable; asserted when PARRX2TXTIMEDEN is 1.
 * Transmit bit clock select: 0 - Selects synthesizer bit clock for transmit 1
 * - Selects CDR clock for transmit
 */
#define SERDES_IREG_FLD_LB_CDRCLK2TXEN_REG_NUM				7

/*
 * RX BIST error count overflow indicator. Indicates an overflow in the number
 * of byte errors identified during the course of the test. This word is stable
 * to sample when *_DONE_* signal has asserted
 */
#define SERDES_IREG_FLD_RXBIST_ERRCOUNT_OVERFLOW_REG_NUM		8

/*
 * RX BIST locked indicator 0 - Indicates BIST is not word locked and error
 * comparisons have not begun yet 1 - Indicates BIST is word locked and error
 * comparisons have begun
 */
#define SERDES_IREG_FLD_RXBIST_RXLOCKED_REG_NUM				8

/* Tx params */
#define SERDES_IREG_TX_DRV_1_REG_NUM					21
#define SERDES_IREG_TX_DRV_1_HLEV_MASK					0x7
#define SERDES_IREG_TX_DRV_1_HLEV_SHIFT					0
#define SERDES_IREG_TX_DRV_1_LEVN_MASK					0xf8
#define SERDES_IREG_TX_DRV_1_LEVN_SHIFT					3

#define SERDES_IREG_TX_DRV_2_REG_NUM					22
#define SERDES_IREG_TX_DRV_2_LEVNM1_MASK				0xf
#define SERDES_IREG_TX_DRV_2_LEVNM1_SHIFT				0
#define SERDES_IREG_TX_DRV_2_LEVNM2_MASK				0x30
#define SERDES_IREG_TX_DRV_2_LEVNM2_SHIFT				4

#define SERDES_IREG_TX_DRV_3_REG_NUM					23
#define SERDES_IREG_TX_DRV_3_LEVNP1_MASK				0x7
#define SERDES_IREG_TX_DRV_3_LEVNP1_SHIFT				0
#define SERDES_IREG_TX_DRV_3_SLEW_MASK					0x18
#define SERDES_IREG_TX_DRV_3_SLEW_SHIFT					3

/* Rx params */
#define SERDES_IREG_RX_CALEQ_1_REG_NUM					24
#define SERDES_IREG_RX_CALEQ_1_DCGAIN_MASK				0x7
#define SERDES_IREG_RX_CALEQ_1_DCGAIN_SHIFT				0
/* DFE post-shaping tap 3dB frequency */
#define SERDES_IREG_RX_CALEQ_1_DFEPSTAP3DB_MASK				0x38
#define SERDES_IREG_RX_CALEQ_1_DFEPSTAP3DB_SHIFT			3

#define SERDES_IREG_RX_CALEQ_2_REG_NUM					25
/* DFE post-shaping tap gain */
#define SERDES_IREG_RX_CALEQ_2_DFEPSTAPGAIN_MASK			0x7
#define SERDES_IREG_RX_CALEQ_2_DFEPSTAPGAIN_SHIFT			0
/* DFE first tap gain control */
#define SERDES_IREG_RX_CALEQ_2_DFETAP1GAIN_MASK				0x78
#define SERDES_IREG_RX_CALEQ_2_DFETAP1GAIN_SHIFT			3

#define SERDES_IREG_RX_CALEQ_3_REG_NUM					26
#define SERDES_IREG_RX_CALEQ_3_DFETAP2GAIN_MASK				0xf
#define SERDES_IREG_RX_CALEQ_3_DFETAP2GAIN_SHIFT			0
#define SERDES_IREG_RX_CALEQ_3_DFETAP3GAIN_MASK				0xf0
#define SERDES_IREG_RX_CALEQ_3_DFETAP3GAIN_SHIFT			4

#define SERDES_IREG_RX_CALEQ_4_REG_NUM					27
#define SERDES_IREG_RX_CALEQ_4_DFETAP4GAIN_MASK				0xf
#define SERDES_IREG_RX_CALEQ_4_DFETAP4GAIN_SHIFT			0
#define SERDES_IREG_RX_CALEQ_4_LOFREQAGCGAIN_MASK			0x70
#define SERDES_IREG_RX_CALEQ_4_LOFREQAGCGAIN_SHIFT			4

#define SERDES_IREG_RX_CALEQ_5_REG_NUM					28
#define SERDES_IREG_RX_CALEQ_5_PRECAL_CODE_SEL_MASK			0x7
#define SERDES_IREG_RX_CALEQ_5_PRECAL_CODE_SEL_SHIFT			0
#define SERDES_IREG_RX_CALEQ_5_HIFREQAGCCAP_MASK			0xf8
#define SERDES_IREG_RX_CALEQ_5_HIFREQAGCCAP_SHIFT			3

/* RX lane best eye point measurement result */
#define SERDES_IREG_RXEQ_BEST_EYE_MSB_VAL_REG_NUM			29
#define SERDES_IREG_RXEQ_BEST_EYE_LSB_VAL_REG_NUM			30
#define SERDES_IREG_RXEQ_BEST_EYE_LSB_VAL_MASK				0x3F

/*
 * Adaptive RX Equalization enable
 * 0 - Disables adaptive RX equalization.
 * 1 - Enables adaptive RX equalization.
 */
#define SERDES_IREG_FLD_PCSRXEQ_START_REG_NUM				31
#define SERDES_IREG_FLD_PCSRXEQ_START					(1 << 0)

/*
 * PHY Adaptive Equalization status
 * 0 - Indicates Adaptive Equalization results are not valid for sampling
 * 1 - Indicates Adaptive Equalization is complete and results are valid for
 *     sampling
 */
#define SERDES_IREG_FLD_RXCALROAMEYEMEASDONE_REG_NUM			32

#define SERDES_IREG_FLD_RXEQ_DONE					(1 << 3)

/*
 * Selects Eye to capture based on edge
 * 0 - Capture 1st Eye in Eye Diagram
 * 1 - Capture 2nd Eye in Eye Diagram measurement
 */
#define SERDES_IREG_FLD_RXROAM_XORBITSEL_REG_NUM			39
#define SERDES_IREG_FLD_RXROAM_XORBITSEL				(1 << 2)
#define SERDES_IREG_FLD_RXROAM_XORBITSEL_MASK				(1 << 2)

/*
 * RX Signal detect. 0 indicates no signal, 1 indicates signal detected.
 */
#define SERDES_IREG_FLD_RXRANDET_REG_NUM				41
#define SERDES_IREG_FLD_RXRANDET_STAT					0x20

/*
 * RX data polarity inversion control:
 * 1'b0: no inversion
 * 1'b1: invert polarity
 */
#define SERDES_IREG_FLD_POLARITY_RX_REG_NUM				46

/* LANEPCSPSTATE* override enable (Active low) */
#define SERDES_IREG_FLD_LANEPCSPSTATE_LOCWREN_REG_NUM			85

/* LB* override enable (Active low) */
#define SERDES_IREG_FLD_LB_LOCWREN_REG_NUM				85

/* PCSRX* override enable (Active low) */
#define SERDES_IREG_FLD_PCSRX_LOCWREN_REG_NUM				85

/* PCSRXBIST* override enable (Active low) */
#define SERDES_IREG_FLD_PCSRXBIST_LOCWREN_REG_NUM			85

/* PCSRXEQ* override enable (Active low) */
#define SERDES_IREG_FLD_PCSRXEQ_LOCWREN_REG_NUM				85
#define SERDES_IREG_FLD_PCSRXEQ_LOCWREN					(1 << 6)

/* PCSTX* override enable (Active low) */
#define SERDES_IREG_FLD_PCSTX_LOCWREN_REG_NUM				85

/*
 * group registers:
 * SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN,
 * SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN
 * SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN
 */
#define SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM				86

/* Override RX_CALCEQ through the internal registers (Active low) */
#define SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN_REG_NUM			86
#define SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN				(1 << 3)

#define SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN_REG_NUM		86
#define SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN			(1 << 4)

/* RXCALROAMEYEMEASIN* override enable - Active Low */
#define SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN_REG_NUM		86
#define SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN			(1 << 6)

/* RXCALROAMXADJUST* override enable - Active Low */
#define SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN_REG_NUM		86
#define SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN			(1 << 7)

/* RXCALROAMYADJUST* override enable - Active Low */
#define SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM		87
#define SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN			(1 << 0)

/* Override TX_DRV through the internal registers (Active low) */
#define SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN_REG_NUM			87
#define SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN				(1 << 5)

#define SERDES_IREG_FLD_RXEQ_COARSE_ITER_NUM_REG_NUM			30
#define SERDES_IREG_FLD_RXEQ_COARSE_ITER_NUM_MASK			0x7f
#define SERDES_IREG_FLD_RXEQ_COARSE_ITER_NUM_SHIFT			0

#define SERDES_IREG_FLD_RXEQ_FINE_ITER_NUM_REG_NUM			31
#define SERDES_IREG_FLD_RXEQ_FINE_ITER_NUM_MASK				0x7f
#define SERDES_IREG_FLD_RXEQ_FINE_ITER_NUM_SHIFT			0

#define SERDES_IREG_FLD_RXEQ_COARSE_RUN1_MASK_REG_NUM			32
#define SERDES_IREG_FLD_RXEQ_COARSE_RUN1_MASK_MASK			0xff
#define SERDES_IREG_FLD_RXEQ_COARSE_RUN1_MASK_SHIFT			0

#define SERDES_IREG_FLD_RXEQ_COARSE_RUN2_MASK_REG_NUM			33
#define SERDES_IREG_FLD_RXEQ_COARSE_RUN2_MASK_MASK			0x1
#define SERDES_IREG_FLD_RXEQ_COARSE_RUN2_MASK_SHIFT			0

#define SERDES_IREG_FLD_RXEQ_COARSE_STEP_REG_NUM			33
#define SERDES_IREG_FLD_RXEQ_COARSE_STEP_MASK				0x3e
#define SERDES_IREG_FLD_RXEQ_COARSE_STEP_SHIFT				1

#define SERDES_IREG_FLD_RXEQ_FINE_RUN1_MASK_REG_NUM			34
#define SERDES_IREG_FLD_RXEQ_FINE_RUN1_MASK_MASK			0xff
#define SERDES_IREG_FLD_RXEQ_FINE_RUN1_MASK_SHIFT			0

#define SERDES_IREG_FLD_RXEQ_FINE_RUN2_MASK_REG_NUM			35
#define SERDES_IREG_FLD_RXEQ_FINE_RUN2_MASK_MASK			0x1
#define SERDES_IREG_FLD_RXEQ_FINE_RUN2_MASK_SHIFT			0

#define SERDES_IREG_FLD_RXEQ_FINE_STEP_REG_NUM				35
#define SERDES_IREG_FLD_RXEQ_FINE_STEP_MASK				0x3e
#define SERDES_IREG_FLD_RXEQ_FINE_STEP_SHIFT				1

#define SERDES_IREG_FLD_RXEQ_LOOKUP_CODE_EN_REG_NUM			36
#define SERDES_IREG_FLD_RXEQ_LOOKUP_CODE_EN_MASK			0xff
#define SERDES_IREG_FLD_RXEQ_LOOKUP_CODE_EN_SHIFT			0

#define SERDES_IREG_FLD_RXEQ_LOOKUP_LASTCODE_REG_NUM			37
#define SERDES_IREG_FLD_RXEQ_LOOKUP_LASTCODE_MASK			0x7
#define SERDES_IREG_FLD_RXEQ_LOOKUP_LASTCODE_SHIFT			0

#define SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_REG_NUM			43
#define SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_MASK				0x7
#define SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_SHIFT				0

/* CMNPCIEGEN3* override enable (Active Low) */
#define SERDES_IREG_FLD_CMNPCIEGEN3_LOCWREN_REG_NUM			95

/* CMNPCS* override enable (Active Low) */
#define SERDES_IREG_FLD_CMNPCS_LOCWREN_REG_NUM				95

/* CMNPCSBIST* override enable (Active Low) */
#define SERDES_IREG_FLD_CMNPCSBIST_LOCWREN_REG_NUM			95

/* CMNPCSPSTATE* override enable (Active Low) */
#define SERDES_IREG_FLD_CMNPCSPSTATE_LOCWREN_REG_NUM			95

/* Eye diagram sample count */
#define SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_REG_NUM			150
#define SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_MASK			0xff
#define SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_SHIFT			0

#define SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_REG_NUM			151
#define SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_MASK			0xff
#define SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_SHIFT			0

#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD1_REG_NUM		623
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD1_MASK		0xff
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD1_SHIFT		0

#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD2_REG_NUM		624
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD2_MASK		0xff
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD2_SHIFT		0

/* X coarse scan step */
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_REG_NUM		627
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_MASK			0x7F
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_SHIFT		0

/* X fine scan step */
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_REG_NUM		628
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_MASK			0x7F
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_SHIFT			0

/* Y coarse scan step */
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_REG_NUM		629
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_MASK			0x0F
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_SHIFT		0

/* Y fine scan step */
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_REG_NUM		630
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_MASK			0x0F
#define SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_SHIFT			0

#endif /* __AL_serdes_REG_H */
