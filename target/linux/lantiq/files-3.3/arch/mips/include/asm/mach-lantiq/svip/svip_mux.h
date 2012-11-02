/************************************************************************
 *
 * Copyright (c) 2007
 * Infineon Technologies AG
 * St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 ************************************************************************/

#ifndef __SVIP_MUX_H
#define __SVIP_MUX_H

#define LTQ_MUX_P0_PINS			20
#define LTQ_MUX_P1_PINS			20
#define LTQ_MUX_P2_PINS			19
#define LTQ_MUX_P3_PINS			20
#define LTQ_MUX_P4_PINS			24

struct ltq_mux_pin {
	int dirin;
	int puen;
	int altsel0;
	int altsel1;
};

struct ltq_mux_settings {
	const struct ltq_mux_pin *mux_p0;
	const struct ltq_mux_pin *mux_p1;
	const struct ltq_mux_pin *mux_p2;
	const struct ltq_mux_pin *mux_p3;
	const struct ltq_mux_pin *mux_p4;
};

#define LTQ_MUX_P0_19_EXINT16		{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_19			{ 0, 0, 1, 0 }

#define LTQ_MUX_P0_18_EJ_BRKIN		{ 1, 0, 0, 0 }
#define LTQ_MUX_P0_18			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_18_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P0_17_EXINT10		{ 1, 0, 0, 0 }
#define LTQ_MUX_P0_17			{ 0, 0, 0, 0 }
#define LTQ_MUX_P0_17_ASC1_RXD		{ 1, 0, 1, 0 }

#define LTQ_MUX_P0_16_EXINT9		{ 1, 0, 0, 0 }
#define LTQ_MUX_P0_16			{ 0, 0, 0, 0 }
#define LTQ_MUX_P0_16_ASC1_TXD		{ 0, 0, 1, 0 }

#define LTQ_MUX_P0_15_EXINT8		{ 1, 0, 0, 0 }
#define LTQ_MUX_P0_15			{ 0, 0, 0, 0 }
#define LTQ_MUX_P0_15_ASC0_RXD		{ 1, 0, 1, 0 }

#define LTQ_MUX_P0_14_EXINT7		{ 1, 0, 0, 0 }
#define LTQ_MUX_P0_14			{ 0, 0, 0, 0 }
#define LTQ_MUX_P0_14_ASC0_TXD		{ 1, 0, 1, 0 }

#define LTQ_MUX_P0_13_SSC0_CS7		{ 0, 1, 0, 0 }
#define LTQ_MUX_P0_13_EXINT6		{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_13			{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_13_SSC1_CS7		{ 0, 0, 0, 1 }
#define LTQ_MUX_P0_13_SSC1_INT		{ 0, 0, 1, 1 }

#define LTQ_MUX_P0_12_SSC0_CS6		{ 0, 1, 0, 0 }
#define LTQ_MUX_P0_12_EXINT5		{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_12			{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_12_SSC1_CS6		{ 0, 0, 0, 1 }

#define LTQ_MUX_P0_11_SSC0_CS5		{ 0, 1, 0, 0 }
#define LTQ_MUX_P0_11_EXINT4		{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_11			{ 1, 0, 0, 0 }
#define LTQ_MUX_P0_11_SSC1_CS5		{ 0, 0, 0, 1 }

#define LTQ_MUX_P0_10_SSC0_CS4		{ 0, 1, 0, 0 }
#define LTQ_MUX_P0_10_EXINT3		{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_10			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_10_SSC1_CS4		{ 0, 0, 0, 1 }

#define LTQ_MUX_P0_9_SSC0_CS3		{ 0, 1, 0, 0 }
#define LTQ_MUX_P0_9_EXINT2		{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_9			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_9_SSC1_CS3		{ 0, 0, 0, 1 }

#define LTQ_MUX_P0_8_SSC0_CS2		{ 0, 1, 0, 0 }
#define LTQ_MUX_P0_8_EXINT1		{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_8			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_8_SSC1_CS2		{ 0, 0, 0, 1 }

#define LTQ_MUX_P0_7_SSC0_CS1		{ 0, 1, 0, 0 }
#define LTQ_MUX_P0_7_EXINT0		{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_7			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_7_SSC1_CS1		{ 0, 0, 0, 1 }
#define LTQ_MUX_P0_7_SSC1_CS0		{ 1, 0, 0, 1 }
#define LTQ_MUX_P0_7_SSC2_CS0		{ 1, 0, 1, 1 }

#define LTQ_MUX_P0_6_SSC0_CS0		{ 0, 1, 0, 0 }
#define LTQ_MUX_P0_6			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_6_IN			{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_6_SSC1_CS0		{ 0, 0, 0, 1 }

#define LTQ_MUX_P0_5_SSC1_SCLK		{ 0, 0, 0, 0 }
#define LTQ_MUX_P0_5			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_5_IN			{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_5_SSC2_CLK		{ 1, 0, 0, 1 }

#define LTQ_MUX_P0_4_SSC1_MRST		{ 1, 0, 0, 0 }
#define LTQ_MUX_P0_4			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_4_IN			{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_4_SSC2_MRST		{ 0, 0, 0, 1 }

#define LTQ_MUX_P0_3_SSC1_MTSR		{ 0, 0, 0, 0 }
#define LTQ_MUX_P0_3			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_3_IN			{ 1, 0, 1, 0 }
#define LTQ_MUX_P0_3_SSC2_MTSR		{ 0, 0, 0, 1 }

#define LTQ_MUX_P0_2_SSC0_SCLK		{ 0, 0, 0, 0 }
#define LTQ_MUX_P0_2			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_2_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P0_1_SSC0_MRST		{ 1, 0, 0, 0 }
#define LTQ_MUX_P0_1			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_1_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P0_0_SSC0_MTSR		{ 0, 0, 0, 0 }
#define LTQ_MUX_P0_0			{ 0, 0, 1, 0 }
#define LTQ_MUX_P0_0_IN			{ 1, 0, 1, 0 }


#define LTQ_MUX_P1_19_PCM3_TC1		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_19_EXINT15		{ 1, 0, 1, 0 }
#define LTQ_MUX_P1_19			{ 0, 0, 1, 0 }

#define LTQ_MUX_P1_18_PCM3_FSC		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_18_EXINT11		{ 1, 0, 1, 0 }
#define LTQ_MUX_P1_18			{ 0, 0, 1, 0 }

#define LTQ_MUX_P1_17_PCM3_PCL		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_17_EXINT12		{ 1, 0, 1, 0 }
#define LTQ_MUX_P1_17			{ 0, 0, 1, 0 }

#define LTQ_MUX_P1_16_PCM3_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_16_EXINT13		{ 1, 0, 1, 0 }
#define LTQ_MUX_P1_16			{ 0, 0, 1, 0 }

#define LTQ_MUX_P1_15_PCM3_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_15_EXINT14		{ 1, 0, 1, 0 }
#define LTQ_MUX_P1_15			{ 0, 0, 1, 0 }

#define LTQ_MUX_P1_14_PCM2_TC1		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_14			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_14_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_13_PCM2_FSC		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_13			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_13_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_12_PCM2_PCL		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_12			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_12_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_11_PCM2_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_11			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_11_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_10_PCM2_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_10			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_10_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_9_PCM1_TC1		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_9			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_9_IN			{ 0, 0, 1, 0 }

#define LTQ_MUX_P1_8_PCM1_FSC		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_8			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_8_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_7_PCM1_PCL		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_7			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_7_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_6_PCM1_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_6			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_6_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_5_PCM1_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_5			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_5_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_4_PCM0_TC1		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_4			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_4_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_3_PCM0_FSC		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_3			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_3_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_2_PCM0_PCL		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_2			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_2_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_1_PCM0_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_1			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_1_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P1_0_PCM0_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P1_0			{ 0, 0, 1, 0 }
#define LTQ_MUX_P1_0_IN			{ 1, 0, 1, 0 }


#define LTQ_MUX_P2_18_EBU_BC1		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_18			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_18_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_17_EBU_BC0		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_17			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_17_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_16_EBU_RDBY		{ 1, 0, 0, 0 }
#define LTQ_MUX_P2_16			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_16_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_15_EBU_WAIT		{ 1, 0, 0, 0 }
#define LTQ_MUX_P2_15			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_15_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_14_EBU_ALE		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_14			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_14_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_13_EBU_WR		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_13			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_13_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_12_EBU_RD		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_12			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_12_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_11_EBU_A11		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_11			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_11_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_10_EBU_A10		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_10			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_10_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_9_EBU_A9		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_9			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_9_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_8_EBU_A8		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_8			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_8_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_7_EBU_A7		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_7			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_7_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_6_EBU_A6		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_6			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_6_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_5_EBU_A5		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_5			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_5_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_4_EBU_A4		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_4			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_4_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_3_EBU_A3		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_3			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_3_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_2_EBU_A2		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_2			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_2_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_1_EBU_A1		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_1			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_1_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P2_0_EBU_A0		{ 0, 0, 0, 0 }
#define LTQ_MUX_P2_0			{ 0, 0, 1, 0 }
#define LTQ_MUX_P2_0_IN			{ 1, 0, 1, 0 }


#define LTQ_MUX_P3_19_EBU_CS3		{ 0, 0, 0, 0 }
#define LTQ_MUX_P3_19			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_19_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_18_EBU_CS2		{ 0, 0, 0, 0 }
#define LTQ_MUX_P3_18			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_18_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_17_EBU_CS1		{ 0, 0, 0, 0 }
#define LTQ_MUX_P3_17			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_17_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_16_EBU_CS0		{ 0, 0, 0, 0 }
#define LTQ_MUX_P3_16			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_16_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_15_EBU_AD15		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_15			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_15_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_14_EBU_AD14		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_14			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_14_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_13_EBU_AD13		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_13			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_13_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_12_EBU_AD12		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_12			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_12_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_11_EBU_AD11		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_11			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_11_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_10_EBU_AD10		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_10			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_10_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_9_EBU_AD9		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_9			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_9_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_8_EBU_AD8		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_8			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_8_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_7_EBU_AD7		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_7			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_7_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_6_EBU_AD6		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_6			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_6_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_5_EBU_AD5		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_5			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_5_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_4_EBU_AD4		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_4			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_4_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_3_EBU_AD3		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_3			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_3_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_2_EBU_AD2		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_2			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_2_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_1_EBU_AD1		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_1			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_1_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P3_0_EBU_AD0		{ 1, 0, 0, 0 }
#define LTQ_MUX_P3_0			{ 0, 0, 1, 0 }
#define LTQ_MUX_P3_0_IN			{ 1, 0, 1, 0 }


#define LTQ_MUX_P4_23_SSLIC7_CLK	{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_23			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_23_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_22_SSLIC7_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_22			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_22_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_21_SSLIC7_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_21			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_21_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_20_SSLIC6_CLK	{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_20			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_20_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_19_SSLIC6_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_19			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_19_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_18_SSLIC6_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_18			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_18_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_17_SSLIC5_CLK	{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_17			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_17_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_16_SSLIC5_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_16			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_16_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_15_SSLIC5_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_15			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_15_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_14_SSLIC4_CLK	{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_14			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_14_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_13_SSLIC4_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_13			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_13_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_12_SSLIC4_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_12			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_12_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_11_SSLIC3_CLK	{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_11			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_11_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_10_SSLIC3_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_10			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_10_IN		{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_9_SSLIC3_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_9			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_9_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_8_SSLIC2_CLK		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_8			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_8_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_7_SSLIC2_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_7			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_7_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_6_SSLIC2_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_6			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_6_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_5_SSLIC1_CLK		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_5			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_5_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_4_SSLIC1_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_4			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_4_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_3_SSLIC1_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_3			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_3_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_2_SSLIC0_CLK		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_2			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_2_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_1_SSLIC0_RX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_1			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_1_IN			{ 1, 0, 1, 0 }

#define LTQ_MUX_P4_0_SSLIC0_TX		{ 0, 0, 0, 0 }
#define LTQ_MUX_P4_0			{ 0, 0, 1, 0 }
#define LTQ_MUX_P4_0_IN			{ 1, 0, 1, 0 }

#endif
