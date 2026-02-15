/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef __DT_BINDINGS_CLOCK_BCM3380_H
#define __DT_BINDINGS_CLOCK_BCM3380_H

// From U-boot, also see IntControlClkControlLo in IntControl.h
#define BCM3380_CLK0_DDR		0
#define BCM3380_CLK0_FPM		1
#define BCM3380_CLK0_CRYPTO		2
// #define BCM3380_CLK0_EPHY	3
#define BCM3380_CLK0_DTP		3
#define BCM3380_CLK0_UTP		4
#define BCM3380_CLK0_FAP		5
#define BCM3380_CLK0_MEP		6
#define BCM3380_CLK0_MSP		7

#define BCM3380_CLK0_D3DSMAC	8
#define BCM3380_CLK0_USMAC20	9
#define BCM3380_CLK0_USMAC30	10
#define BCM3380_CLK0_SEGDMA		11
#define BCM3380_CLK0_TC0		12
#define BCM3380_CLK0_TC1		13
#define BCM3380_CLK0_TC2		14
#define BCM3380_CLK0_TC3		15

#define BCM3380_CLK0_PCIE		16
#define BCM3380_CLK0_SPI		17
// #define BCM3380_CLK0_ENET0	18
#define BCM3380_CLK0_SPIGLEES	18
// #define BCM3380_CLK0_ENET1	19
#define BCM3380_CLK0_UNIMAC0	19
#define BCM3380_CLK0_UNIMAC1	20
#define BCM3380_CLK0_DAVIC		21
#define BCM3380_CLK0_BRGRBUS	22
#define BCM3380_CLK0_MIPS		23

#define BCM3380_CLK0_APMPLL		24
#define BCM3380_CLK0_APM		25
#define BCM3380_CLK0_BMU		26
#define BCM3380_CLK0_PCM		27
#define BCM3380_CLK0_NTP		28
#define BCM3380_CLK0_AUDIOCOM	29
#define BCM3380_CLK0_AUDIOACLK	30
#define BCM3380_CLK0_AUDIOBCLK	31

// I added these
#define BCM3380_CLK1_PCIEPLL	0
#define BCM3380_CLK1_GPHYPLL	1
#define BCM3380_CLK1_DS0		2
#define BCM3380_CLK1_DS1		3
#define BCM3380_CLK1_DS2		4
#define BCM3380_CLK1_DS3		5
#define BCM3380_CLK1_DS4B		6
#define BCM3380_CLK1_DS5A		7
#define BCM3380_CLK1_DS6		8
#define BCM3380_CLK1_DS7		9
#define BCM3380_CLK1_USTOP		10
#define BCM3380_CLK1_OB			11
#define BCM3380_CLK1_DSTUNNER	12
#define BCM3380_CLK1_USB0		13
#define BCM3380_CLK1_USB1		14
#define BCM3380_CLK1_GPHY		15

// TODO: remind Alvaro about the mismatch
// TODO: may need to patch drivers/clk/bcm/clk-bcm63xx-gate.c
// https://github.com/torvalds/linux/blob/master/drivers/clk/bcm/clk-bcm63xx-gate.c

#endif /* __DT_BINDINGS_CLOCK_BCM3380_H */
