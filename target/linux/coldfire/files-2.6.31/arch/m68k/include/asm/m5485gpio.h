/*
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * File:	mcf548x_gpio.h
 * Purpose:	Register and bit definitions for the MCF548X
 *
 * Notes:
 *
 */

#ifndef _M5485GPIO_H_
#define _M5485GPIO_H_

/*********************************************************************
*
* General Purpose I/O (GPIO)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_GPIO_PODR_FBCTL         MCF_REG08(0x000A00)
#define MCF_GPIO_PODR_FBCS          MCF_REG08(0x000A01)
#define MCF_GPIO_PODR_DMA           MCF_REG08(0x000A02)
#define MCF_GPIO_PODR_FEC0H         MCF_REG08(0x000A04)
#define MCF_GPIO_PODR_FEC0L         MCF_REG08(0x000A05)
#define MCF_GPIO_PODR_FEC1H         MCF_REG08(0x000A06)
#define MCF_GPIO_PODR_FEC1L         MCF_REG08(0x000A07)
#define MCF_GPIO_PODR_FECI2C        MCF_REG08(0x000A08)
#define MCF_GPIO_PODR_PCIBG         MCF_REG08(0x000A09)
#define MCF_GPIO_PODR_PCIBR         MCF_REG08(0x000A0A)
#define MCF_GPIO_PODR_PSC3PSC2      MCF_REG08(0x000A0C)
#define MCF_GPIO_PODR_PSC1PSC0      MCF_REG08(0x000A0D)
#define MCF_GPIO_PODR_DSPI          MCF_REG08(0x000A0E)
#define MCF_GPIO_PDDR_FBCTL         MCF_REG08(0x000A10)
#define MCF_GPIO_PDDR_FBCS          MCF_REG08(0x000A11)
#define MCF_GPIO_PDDR_DMA           MCF_REG08(0x000A12)
#define MCF_GPIO_PDDR_FEC0H         MCF_REG08(0x000A14)
#define MCF_GPIO_PDDR_FEC0L         MCF_REG08(0x000A15)
#define MCF_GPIO_PDDR_FEC1H         MCF_REG08(0x000A16)
#define MCF_GPIO_PDDR_FEC1L         MCF_REG08(0x000A17)
#define MCF_GPIO_PDDR_FECI2C        MCF_REG08(0x000A18)
#define MCF_GPIO_PDDR_PCIBG         MCF_REG08(0x000A19)
#define MCF_GPIO_PDDR_PCIBR         MCF_REG08(0x000A1A)
#define MCF_GPIO_PDDR_PSC3PSC2      MCF_REG08(0x000A1C)
#define MCF_GPIO_PDDR_PSC1PSC0      MCF_REG08(0x000A1D)
#define MCF_GPIO_PDDR_DSPI          MCF_REG08(0x000A1E)
#define MCF_GPIO_PPDSDR_FBCTL       MCF_REG08(0x000A20)
#define MCF_GPIO_PPDSDR_FBCS        MCF_REG08(0x000A21)
#define MCF_GPIO_PPDSDR_DMA         MCF_REG08(0x000A22)
#define MCF_GPIO_PPDSDR_FEC0H       MCF_REG08(0x000A24)
#define MCF_GPIO_PPDSDR_FEC0L       MCF_REG08(0x000A25)
#define MCF_GPIO_PPDSDR_FEC1H       MCF_REG08(0x000A26)
#define MCF_GPIO_PPDSDR_FEC1L       MCF_REG08(0x000A27)
#define MCF_GPIO_PPDSDR_FECI2C      MCF_REG08(0x000A28)
#define MCF_GPIO_PPDSDR_PCIBG       MCF_REG08(0x000A29)
#define MCF_GPIO_PPDSDR_PCIBR       MCF_REG08(0x000A2A)
#define MCF_GPIO_PPDSDR_PSC3PSC2    MCF_REG08(0x000A2C)
#define MCF_GPIO_PPDSDR_PSC1PSC0    MCF_REG08(0x000A2D)
#define MCF_GPIO_PPDSDR_DSPI        MCF_REG08(0x000A2E)
#define MCF_GPIO_PCLRR_FBCTL        MCF_REG08(0x000A30)
#define MCF_GPIO_PCLRR_FBCS         MCF_REG08(0x000A31)
#define MCF_GPIO_PCLRR_DMA          MCF_REG08(0x000A32)
#define MCF_GPIO_PCLRR_FEC0H        MCF_REG08(0x000A34)
#define MCF_GPIO_PCLRR_FEC0L        MCF_REG08(0x000A35)
#define MCF_GPIO_PCLRR_FEC1H        MCF_REG08(0x000A36)
#define MCF_GPIO_PCLRR_FEC1L        MCF_REG08(0x000A37)
#define MCF_GPIO_PCLRR_FECI2C       MCF_REG08(0x000A38)
#define MCF_GPIO_PCLRR_PCIBG        MCF_REG08(0x000A39)
#define MCF_GPIO_PCLRR_PCIBR        MCF_REG08(0x000A3A)
#define MCF_GPIO_PCLRR_PSC3PSC2     MCF_REG08(0x000A3C)
#define MCF_GPIO_PCLRR_PSC1PSC0     MCF_REG08(0x000A3D)
#define MCF_GPIO_PCLRR_DSPI         MCF_REG08(0x000A3E)
#define MCF_GPIO_PAR_FBCTL          MCF_REG16(0x000A40)
#define MCF_GPIO_PAR_FBCS           MCF_REG08(0x000A42)
#define MCF_GPIO_PAR_DMA            MCF_REG08(0x000A43)
#define MCF_GPIO_PAR_FECI2CIRQ      MCF_REG16(0x000A44)
#define MCF_GPIO_PAR_PCIBG          MCF_REG16(0x000A48)
#define MCF_GPIO_PAR_PCIBR          MCF_REG16(0x000A4A)
#define MCF_GPIO_PAR_PSC3           MCF_REG08(0x000A4C)
#define MCF_GPIO_PAR_PSC2           MCF_REG08(0x000A4D)
#define MCF_GPIO_PAR_PSC1           MCF_REG08(0x000A4E)
#define MCF_GPIO_PAR_PSC0           MCF_REG08(0x000A4F)
#define MCF_GPIO_PAR_DSPI           MCF_REG16(0x000A50)
#define MCF_GPIO_PAR_TIMER          MCF_REG08(0x000A52)

/* Bit definitions and macros for MCF_GPIO_PODR_FBCTL */
#define MCF_GPIO_PODR_FBCTL_PODRFBCTL0              (0x01)
#define MCF_GPIO_PODR_FBCTL_PODRFBCTL1              (0x02)
#define MCF_GPIO_PODR_FBCTL_PODRFBCTL2              (0x04)
#define MCF_GPIO_PODR_FBCTL_PODRFBCTL3              (0x08)
#define MCF_GPIO_PODR_FBCTL_PODRFBCTL4              (0x10)
#define MCF_GPIO_PODR_FBCTL_PODRFBCTL5              (0x20)
#define MCF_GPIO_PODR_FBCTL_PODRFBCTL6              (0x40)
#define MCF_GPIO_PODR_FBCTL_PODRFBCTL7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FBCS */
#define MCF_GPIO_PODR_FBCS_PODRFBCS1                (0x02)
#define MCF_GPIO_PODR_FBCS_PODRFBCS2                (0x04)
#define MCF_GPIO_PODR_FBCS_PODRFBCS3                (0x08)
#define MCF_GPIO_PODR_FBCS_PODRFBCS4                (0x10)
#define MCF_GPIO_PODR_FBCS_PODRFBCS5                (0x20)

/* Bit definitions and macros for MCF_GPIO_PODR_DMA */
#define MCF_GPIO_PODR_DMA_PODRDMA0                  (0x01)
#define MCF_GPIO_PODR_DMA_PODRDMA1                  (0x02)
#define MCF_GPIO_PODR_DMA_PODRDMA2                  (0x04)
#define MCF_GPIO_PODR_DMA_PODRDMA3                  (0x08)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC0H */
#define MCF_GPIO_PODR_FEC0H_PODRFEC0H0              (0x01)
#define MCF_GPIO_PODR_FEC0H_PODRFEC0H1              (0x02)
#define MCF_GPIO_PODR_FEC0H_PODRFEC0H2              (0x04)
#define MCF_GPIO_PODR_FEC0H_PODRFEC0H3              (0x08)
#define MCF_GPIO_PODR_FEC0H_PODRFEC0H4              (0x10)
#define MCF_GPIO_PODR_FEC0H_PODRFEC0H5              (0x20)
#define MCF_GPIO_PODR_FEC0H_PODRFEC0H6              (0x40)
#define MCF_GPIO_PODR_FEC0H_PODRFEC0H7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC0L */
#define MCF_GPIO_PODR_FEC0L_PODRFEC0L0              (0x01)
#define MCF_GPIO_PODR_FEC0L_PODRFEC0L1              (0x02)
#define MCF_GPIO_PODR_FEC0L_PODRFEC0L2              (0x04)
#define MCF_GPIO_PODR_FEC0L_PODRFEC0L3              (0x08)
#define MCF_GPIO_PODR_FEC0L_PODRFEC0L4              (0x10)
#define MCF_GPIO_PODR_FEC0L_PODRFEC0L5              (0x20)
#define MCF_GPIO_PODR_FEC0L_PODRFEC0L6              (0x40)
#define MCF_GPIO_PODR_FEC0L_PODRFEC0L7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC1H */
#define MCF_GPIO_PODR_FEC1H_PODRFEC1H0              (0x01)
#define MCF_GPIO_PODR_FEC1H_PODRFEC1H1              (0x02)
#define MCF_GPIO_PODR_FEC1H_PODRFEC1H2              (0x04)
#define MCF_GPIO_PODR_FEC1H_PODRFEC1H3              (0x08)
#define MCF_GPIO_PODR_FEC1H_PODRFEC1H4              (0x10)
#define MCF_GPIO_PODR_FEC1H_PODRFEC1H5              (0x20)
#define MCF_GPIO_PODR_FEC1H_PODRFEC1H6              (0x40)
#define MCF_GPIO_PODR_FEC1H_PODRFEC1H7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FEC1L */
#define MCF_GPIO_PODR_FEC1L_PODRFEC1L0              (0x01)
#define MCF_GPIO_PODR_FEC1L_PODRFEC1L1              (0x02)
#define MCF_GPIO_PODR_FEC1L_PODRFEC1L2              (0x04)
#define MCF_GPIO_PODR_FEC1L_PODRFEC1L3              (0x08)
#define MCF_GPIO_PODR_FEC1L_PODRFEC1L4              (0x10)
#define MCF_GPIO_PODR_FEC1L_PODRFEC1L5              (0x20)
#define MCF_GPIO_PODR_FEC1L_PODRFEC1L6              (0x40)
#define MCF_GPIO_PODR_FEC1L_PODRFEC1L7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_FECI2C */
#define MCF_GPIO_PODR_FECI2C_PODRFECI2C0            (0x01)
#define MCF_GPIO_PODR_FECI2C_PODRFECI2C1            (0x02)
#define MCF_GPIO_PODR_FECI2C_PODRFECI2C2            (0x04)
#define MCF_GPIO_PODR_FECI2C_PODRFECI2C3            (0x08)

/* Bit definitions and macros for MCF_GPIO_PODR_PCIBG */
#define MCF_GPIO_PODR_PCIBG_PODRPCIBG0              (0x01)
#define MCF_GPIO_PODR_PCIBG_PODRPCIBG1              (0x02)
#define MCF_GPIO_PODR_PCIBG_PODRPCIBG2              (0x04)
#define MCF_GPIO_PODR_PCIBG_PODRPCIBG3              (0x08)
#define MCF_GPIO_PODR_PCIBG_PODRPCIBG4              (0x10)

/* Bit definitions and macros for MCF_GPIO_PODR_PCIBR */
#define MCF_GPIO_PODR_PCIBR_PODRPCIBR0              (0x01)
#define MCF_GPIO_PODR_PCIBR_PODRPCIBR1              (0x02)
#define MCF_GPIO_PODR_PCIBR_PODRPCIBR2              (0x04)
#define MCF_GPIO_PODR_PCIBR_PODRPCIBR3              (0x08)
#define MCF_GPIO_PODR_PCIBR_PODRPCIBR4              (0x10)

/* Bit definitions and macros for MCF_GPIO_PODR_PSC3PSC2 */
#define MCF_GPIO_PODR_PSC3PSC2_PODRPSC3PSC20        (0x01)
#define MCF_GPIO_PODR_PSC3PSC2_PODRPSC3PSC21        (0x02)
#define MCF_GPIO_PODR_PSC3PSC2_PODRPSC3PSC22        (0x04)
#define MCF_GPIO_PODR_PSC3PSC2_PODRPSC3PSC23        (0x08)
#define MCF_GPIO_PODR_PSC3PSC2_PODRPSC3PSC24        (0x10)
#define MCF_GPIO_PODR_PSC3PSC2_PODRPSC3PSC25        (0x20)
#define MCF_GPIO_PODR_PSC3PSC2_PODRPSC3PSC26        (0x40)
#define MCF_GPIO_PODR_PSC3PSC2_PODRPSC3PSC27        (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_PSC1PSC0 */
#define MCF_GPIO_PODR_PSC1PSC0_PODRPSC1PSC00        (0x01)
#define MCF_GPIO_PODR_PSC1PSC0_PODRPSC1PSC01        (0x02)
#define MCF_GPIO_PODR_PSC1PSC0_PODRPSC1PSC02        (0x04)
#define MCF_GPIO_PODR_PSC1PSC0_PODRPSC1PSC03        (0x08)
#define MCF_GPIO_PODR_PSC1PSC0_PODRPSC1PSC04        (0x10)
#define MCF_GPIO_PODR_PSC1PSC0_PODRPSC1PSC05        (0x20)
#define MCF_GPIO_PODR_PSC1PSC0_PODRPSC1PSC06        (0x40)
#define MCF_GPIO_PODR_PSC1PSC0_PODRPSC1PSC07        (0x80)

/* Bit definitions and macros for MCF_GPIO_PODR_DSPI */
#define MCF_GPIO_PODR_DSPI_PODRDSPI0                (0x01)
#define MCF_GPIO_PODR_DSPI_PODRDSPI1                (0x02)
#define MCF_GPIO_PODR_DSPI_PODRDSPI2                (0x04)
#define MCF_GPIO_PODR_DSPI_PODRDSPI3                (0x08)
#define MCF_GPIO_PODR_DSPI_PODRDSPI4                (0x10)
#define MCF_GPIO_PODR_DSPI_PODRDSPI5                (0x20)
#define MCF_GPIO_PODR_DSPI_PODRDSPI6                (0x40)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBCTL */
#define MCF_GPIO_PDDR_FBCTL_PDDRFBCTL0              (0x01)
#define MCF_GPIO_PDDR_FBCTL_PDDRFBCTL1              (0x02)
#define MCF_GPIO_PDDR_FBCTL_PDDRFBCTL2              (0x04)
#define MCF_GPIO_PDDR_FBCTL_PDDRFBCTL3              (0x08)
#define MCF_GPIO_PDDR_FBCTL_PDDRFBCTL4              (0x10)
#define MCF_GPIO_PDDR_FBCTL_PDDRFBCTL5              (0x20)
#define MCF_GPIO_PDDR_FBCTL_PDDRFBCTL6              (0x40)
#define MCF_GPIO_PDDR_FBCTL_PDDRFBCTL7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FBCS */
#define MCF_GPIO_PDDR_FBCS_PDDRFBCS1                (0x02)
#define MCF_GPIO_PDDR_FBCS_PDDRFBCS2                (0x04)
#define MCF_GPIO_PDDR_FBCS_PDDRFBCS3                (0x08)
#define MCF_GPIO_PDDR_FBCS_PDDRFBCS4                (0x10)
#define MCF_GPIO_PDDR_FBCS_PDDRFBCS5                (0x20)

/* Bit definitions and macros for MCF_GPIO_PDDR_DMA */
#define MCF_GPIO_PDDR_DMA_PDDRDMA0                  (0x01)
#define MCF_GPIO_PDDR_DMA_PDDRDMA1                  (0x02)
#define MCF_GPIO_PDDR_DMA_PDDRDMA2                  (0x04)
#define MCF_GPIO_PDDR_DMA_PDDRDMA3                  (0x08)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC0H */
#define MCF_GPIO_PDDR_FEC0H_PDDRFEC0H0              (0x01)
#define MCF_GPIO_PDDR_FEC0H_PDDRFEC0H1              (0x02)
#define MCF_GPIO_PDDR_FEC0H_PDDRFEC0H2              (0x04)
#define MCF_GPIO_PDDR_FEC0H_PDDRFEC0H3              (0x08)
#define MCF_GPIO_PDDR_FEC0H_PDDRFEC0H4              (0x10)
#define MCF_GPIO_PDDR_FEC0H_PDDRFEC0H5              (0x20)
#define MCF_GPIO_PDDR_FEC0H_PDDRFEC0H6              (0x40)
#define MCF_GPIO_PDDR_FEC0H_PDDRFEC0H7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC0L */
#define MCF_GPIO_PDDR_FEC0L_PDDRFEC0L0              (0x01)
#define MCF_GPIO_PDDR_FEC0L_PDDRFEC0L1              (0x02)
#define MCF_GPIO_PDDR_FEC0L_PDDRFEC0L2              (0x04)
#define MCF_GPIO_PDDR_FEC0L_PDDRFEC0L3              (0x08)
#define MCF_GPIO_PDDR_FEC0L_PDDRFEC0L4              (0x10)
#define MCF_GPIO_PDDR_FEC0L_PDDRFEC0L5              (0x20)
#define MCF_GPIO_PDDR_FEC0L_PDDRFEC0L6              (0x40)
#define MCF_GPIO_PDDR_FEC0L_PDDRFEC0L7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC1H */
#define MCF_GPIO_PDDR_FEC1H_PDDRFEC1H0              (0x01)
#define MCF_GPIO_PDDR_FEC1H_PDDRFEC1H1              (0x02)
#define MCF_GPIO_PDDR_FEC1H_PDDRFEC1H2              (0x04)
#define MCF_GPIO_PDDR_FEC1H_PDDRFEC1H3              (0x08)
#define MCF_GPIO_PDDR_FEC1H_PDDRFEC1H4              (0x10)
#define MCF_GPIO_PDDR_FEC1H_PDDRFEC1H5              (0x20)
#define MCF_GPIO_PDDR_FEC1H_PDDRFEC1H6              (0x40)
#define MCF_GPIO_PDDR_FEC1H_PDDRFEC1H7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FEC1L */
#define MCF_GPIO_PDDR_FEC1L_PDDRFEC1L0              (0x01)
#define MCF_GPIO_PDDR_FEC1L_PDDRFEC1L1              (0x02)
#define MCF_GPIO_PDDR_FEC1L_PDDRFEC1L2              (0x04)
#define MCF_GPIO_PDDR_FEC1L_PDDRFEC1L3              (0x08)
#define MCF_GPIO_PDDR_FEC1L_PDDRFEC1L4              (0x10)
#define MCF_GPIO_PDDR_FEC1L_PDDRFEC1L5              (0x20)
#define MCF_GPIO_PDDR_FEC1L_PDDRFEC1L6              (0x40)
#define MCF_GPIO_PDDR_FEC1L_PDDRFEC1L7              (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_FECI2C */
#define MCF_GPIO_PDDR_FECI2C_PDDRFECI2C0            (0x01)
#define MCF_GPIO_PDDR_FECI2C_PDDRFECI2C1            (0x02)
#define MCF_GPIO_PDDR_FECI2C_PDDRFECI2C2            (0x04)
#define MCF_GPIO_PDDR_FECI2C_PDDRFECI2C3            (0x08)

/* Bit definitions and macros for MCF_GPIO_PDDR_PCIBG */
#define MCF_GPIO_PDDR_PCIBG_PDDRPCIBG0              (0x01)
#define MCF_GPIO_PDDR_PCIBG_PDDRPCIBG1              (0x02)
#define MCF_GPIO_PDDR_PCIBG_PDDRPCIBG2              (0x04)
#define MCF_GPIO_PDDR_PCIBG_PDDRPCIBG3              (0x08)
#define MCF_GPIO_PDDR_PCIBG_PDDRPCIBG4              (0x10)

/* Bit definitions and macros for MCF_GPIO_PDDR_PCIBR */
#define MCF_GPIO_PDDR_PCIBR_PDDRPCIBR0              (0x01)
#define MCF_GPIO_PDDR_PCIBR_PDDRPCIBR1              (0x02)
#define MCF_GPIO_PDDR_PCIBR_PDDRPCIBR2              (0x04)
#define MCF_GPIO_PDDR_PCIBR_PDDRPCIBR3              (0x08)
#define MCF_GPIO_PDDR_PCIBR_PDDRPCIBR4              (0x10)

/* Bit definitions and macros for MCF_GPIO_PDDR_PSC3PSC2 */
#define MCF_GPIO_PDDR_PSC3PSC2_PDDRPSC3PSC20        (0x01)
#define MCF_GPIO_PDDR_PSC3PSC2_PDDRPSC3PSC21        (0x02)
#define MCF_GPIO_PDDR_PSC3PSC2_PDDRPSC3PSC22        (0x04)
#define MCF_GPIO_PDDR_PSC3PSC2_PDDRPSC3PSC23        (0x08)
#define MCF_GPIO_PDDR_PSC3PSC2_PDDRPSC3PSC24        (0x10)
#define MCF_GPIO_PDDR_PSC3PSC2_PDDRPSC3PSC25        (0x20)
#define MCF_GPIO_PDDR_PSC3PSC2_PDDRPSC3PSC26        (0x40)
#define MCF_GPIO_PDDR_PSC3PSC2_PDDRPSC3PSC27        (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_PSC1PSC0 */
#define MCF_GPIO_PDDR_PSC1PSC0_PDDRPSC1PSC00        (0x01)
#define MCF_GPIO_PDDR_PSC1PSC0_PDDRPSC1PSC01        (0x02)
#define MCF_GPIO_PDDR_PSC1PSC0_PDDRPSC1PSC02        (0x04)
#define MCF_GPIO_PDDR_PSC1PSC0_PDDRPSC1PSC03        (0x08)
#define MCF_GPIO_PDDR_PSC1PSC0_PDDRPSC1PSC04        (0x10)
#define MCF_GPIO_PDDR_PSC1PSC0_PDDRPSC1PSC05        (0x20)
#define MCF_GPIO_PDDR_PSC1PSC0_PDDRPSC1PSC06        (0x40)
#define MCF_GPIO_PDDR_PSC1PSC0_PDDRPSC1PSC07        (0x80)

/* Bit definitions and macros for MCF_GPIO_PDDR_DSPI */
#define MCF_GPIO_PDDR_DSPI_PDDRDSPI0                (0x01)
#define MCF_GPIO_PDDR_DSPI_PDDRDSPI1                (0x02)
#define MCF_GPIO_PDDR_DSPI_PDDRDSPI2                (0x04)
#define MCF_GPIO_PDDR_DSPI_PDDRDSPI3                (0x08)
#define MCF_GPIO_PDDR_DSPI_PDDRDSPI4                (0x10)
#define MCF_GPIO_PDDR_DSPI_PDDRDSPI5                (0x20)
#define MCF_GPIO_PDDR_DSPI_PDDRDSPI6                (0x40)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBCTL */
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDRFBCTL0          (0x01)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDRFBCTL1          (0x02)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDRFBCTL2          (0x04)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDRFBCTL3          (0x08)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDRFBCTL4          (0x10)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDRFBCTL5          (0x20)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDRFBCTL6          (0x40)
#define MCF_GPIO_PPDSDR_FBCTL_PPDSDRFBCTL7          (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FBCS */
#define MCF_GPIO_PPDSDR_FBCS_PPDSDRFBCS1            (0x02)
#define MCF_GPIO_PPDSDR_FBCS_PPDSDRFBCS2            (0x04)
#define MCF_GPIO_PPDSDR_FBCS_PPDSDRFBCS3            (0x08)
#define MCF_GPIO_PPDSDR_FBCS_PPDSDRFBCS4            (0x10)
#define MCF_GPIO_PPDSDR_FBCS_PPDSDRFBCS5            (0x20)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_DMA */
#define MCF_GPIO_PPDSDR_DMA_PPDSDRDMA0              (0x01)
#define MCF_GPIO_PPDSDR_DMA_PPDSDRDMA1              (0x02)
#define MCF_GPIO_PPDSDR_DMA_PPDSDRDMA2              (0x04)
#define MCF_GPIO_PPDSDR_DMA_PPDSDRDMA3              (0x08)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC0H */
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDRFEC0H0          (0x01)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDRFEC0H1          (0x02)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDRFEC0H2          (0x04)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDRFEC0H3          (0x08)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDRFEC0H4          (0x10)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDRFEC0H5          (0x20)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDRFEC0H6          (0x40)
#define MCF_GPIO_PPDSDR_FEC0H_PPDSDRFEC0H7          (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC0L */
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDRFEC0L0          (0x01)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDRFEC0L1          (0x02)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDRFEC0L2          (0x04)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDRFEC0L3          (0x08)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDRFEC0L4          (0x10)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDRFEC0L5          (0x20)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDRFEC0L6          (0x40)
#define MCF_GPIO_PPDSDR_FEC0L_PPDSDRFEC0L7          (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC1H */
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDRFEC1H0          (0x01)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDRFEC1H1          (0x02)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDRFEC1H2          (0x04)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDRFEC1H3          (0x08)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDRFEC1H4          (0x10)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDRFEC1H5          (0x20)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDRFEC1H6          (0x40)
#define MCF_GPIO_PPDSDR_FEC1H_PPDSDRFEC1H7          (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FEC1L */
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDRFEC1L0          (0x01)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDRFEC1L1          (0x02)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDRFEC1L2          (0x04)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDRFEC1L3          (0x08)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDRFEC1L4          (0x10)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDRFEC1L5          (0x20)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDRFEC1L6          (0x40)
#define MCF_GPIO_PPDSDR_FEC1L_PPDSDRFEC1L7          (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_FECI2C */
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDRFECI2C0        (0x01)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDRFECI2C1        (0x02)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDRFECI2C2        (0x04)
#define MCF_GPIO_PPDSDR_FECI2C_PPDSDRFECI2C3        (0x08)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PCIBG */
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDRPCIBG0          (0x01)
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDRPCIBG1          (0x02)
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDRPCIBG2          (0x04)
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDRPCIBG3          (0x08)
#define MCF_GPIO_PPDSDR_PCIBG_PPDSDRPCIBG4          (0x10)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PCIBR */
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDRPCIBR0          (0x01)
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDRPCIBR1          (0x02)
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDRPCIBR2          (0x04)
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDRPCIBR3          (0x08)
#define MCF_GPIO_PPDSDR_PCIBR_PPDSDRPCIBR4          (0x10)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PSC3PSC2 */
#define MCF_GPIO_PPDSDR_PSC3PSC2_PPDSDRPSC3PSC20    (0x01)
#define MCF_GPIO_PPDSDR_PSC3PSC2_PPDSDRPSC3PSC21    (0x02)
#define MCF_GPIO_PPDSDR_PSC3PSC2_PPDSDRPSC3PSC22    (0x04)
#define MCF_GPIO_PPDSDR_PSC3PSC2_PPDSDRPSC3PSC23    (0x08)
#define MCF_GPIO_PPDSDR_PSC3PSC2_PDDRPSC3PSC24      (0x10)
#define MCF_GPIO_PPDSDR_PSC3PSC2_PDDRPSC3PSC25      (0x20)
#define MCF_GPIO_PPDSDR_PSC3PSC2_PPDSDRPSC3PSC26    (0x40)
#define MCF_GPIO_PPDSDR_PSC3PSC2_PPDSDRPSC3PSC27    (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_PSC1PSC0 */
#define MCF_GPIO_PPDSDR_PSC1PSC0_PPDSDRPSC1PSC00    (0x01)
#define MCF_GPIO_PPDSDR_PSC1PSC0_PDDRPSC1PSC01      (0x02)
#define MCF_GPIO_PPDSDR_PSC1PSC0_PPDSDRPSC1PSC02    (0x04)
#define MCF_GPIO_PPDSDR_PSC1PSC0_PDDRPSC1PSC03      (0x08)
#define MCF_GPIO_PPDSDR_PSC1PSC0_PPDSDRPSC1PSC04    (0x10)
#define MCF_GPIO_PPDSDR_PSC1PSC0_PPDSDRPSC1PSC05    (0x20)
#define MCF_GPIO_PPDSDR_PSC1PSC0_PPDSDRPSC1PSC06    (0x40)
#define MCF_GPIO_PPDSDR_PSC1PSC0_PPDSDRPSC1PSC07    (0x80)

/* Bit definitions and macros for MCF_GPIO_PPDSDR_DSPI */
#define MCF_GPIO_PPDSDR_DSPI_PPDSDRDSPI0            (0x01)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDRDSPI1            (0x02)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDRDSPI2            (0x04)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDRDSPI3            (0x08)
#define MCF_GPIO_PPDSDR_DSPI_PDDRDSPI4              (0x10)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDRDSPI5            (0x20)
#define MCF_GPIO_PPDSDR_DSPI_PPDSDRDSPI6            (0x40)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBCTL */
#define MCF_GPIO_PCLRR_FBCTL_PCLRRFBCTL0            (0x01)
#define MCF_GPIO_PCLRR_FBCTL_PCLRRFBCTL1            (0x02)
#define MCF_GPIO_PCLRR_FBCTL_PCLRRFBCTL2            (0x04)
#define MCF_GPIO_PCLRR_FBCTL_PCLRRFBCTL3            (0x08)
#define MCF_GPIO_PCLRR_FBCTL_PCLRRFBCTL4            (0x10)
#define MCF_GPIO_PCLRR_FBCTL_PCLRRFBCTL5            (0x20)
#define MCF_GPIO_PCLRR_FBCTL_PCLRRFBCTL6            (0x40)
#define MCF_GPIO_PCLRR_FBCTL_PCLRRFBCTL7            (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FBCS */
#define MCF_GPIO_PCLRR_FBCS_PCLRRFBCS1              (0x02)
#define MCF_GPIO_PCLRR_FBCS_PCLRRFBCS2              (0x04)
#define MCF_GPIO_PCLRR_FBCS_PCLRRFBCS3              (0x08)
#define MCF_GPIO_PCLRR_FBCS_PCLRRFBCS4              (0x10)
#define MCF_GPIO_PCLRR_FBCS_PCLRRFBCS5              (0x20)

/* Bit definitions and macros for MCF_GPIO_PCLRR_DMA */
#define MCF_GPIO_PCLRR_DMA_PCLRRDMA0                (0x01)
#define MCF_GPIO_PCLRR_DMA_PCLRRDMA1                (0x02)
#define MCF_GPIO_PCLRR_DMA_PCLRRDMA2                (0x04)
#define MCF_GPIO_PCLRR_DMA_PCLRRDMA3                (0x08)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC0H */
#define MCF_GPIO_PCLRR_FEC0H_PCLRRFEC0H0            (0x01)
#define MCF_GPIO_PCLRR_FEC0H_PCLRRFEC0H1            (0x02)
#define MCF_GPIO_PCLRR_FEC0H_PCLRRFEC0H2            (0x04)
#define MCF_GPIO_PCLRR_FEC0H_PCLRRFEC0H3            (0x08)
#define MCF_GPIO_PCLRR_FEC0H_PCLRRFEC0H4            (0x10)
#define MCF_GPIO_PCLRR_FEC0H_PCLRRFEC0H5            (0x20)
#define MCF_GPIO_PCLRR_FEC0H_PCLRRFEC0H6            (0x40)
#define MCF_GPIO_PCLRR_FEC0H_PCLRRFEC0H7            (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC0L */
#define MCF_GPIO_PCLRR_FEC0L_PCLRRFEC0L0            (0x01)
#define MCF_GPIO_PCLRR_FEC0L_PODRFEC0L1             (0x02)
#define MCF_GPIO_PCLRR_FEC0L_PCLRRFEC0L2            (0x04)
#define MCF_GPIO_PCLRR_FEC0L_PCLRRFEC0L3            (0x08)
#define MCF_GPIO_PCLRR_FEC0L_PODRFEC0L4             (0x10)
#define MCF_GPIO_PCLRR_FEC0L_PODRFEC0L5             (0x20)
#define MCF_GPIO_PCLRR_FEC0L_PODRFEC0L6             (0x40)
#define MCF_GPIO_PCLRR_FEC0L_PCLRRFEC0L7            (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC1H */
#define MCF_GPIO_PCLRR_FEC1H_PCLRRFEC1H0            (0x01)
#define MCF_GPIO_PCLRR_FEC1H_PCLRRFEC1H1            (0x02)
#define MCF_GPIO_PCLRR_FEC1H_PCLRRFEC1H2            (0x04)
#define MCF_GPIO_PCLRR_FEC1H_PODRFEC1H3             (0x08)
#define MCF_GPIO_PCLRR_FEC1H_PODRFEC1H4             (0x10)
#define MCF_GPIO_PCLRR_FEC1H_PCLRRFEC1H5            (0x20)
#define MCF_GPIO_PCLRR_FEC1H_PCLRRFEC1H6            (0x40)
#define MCF_GPIO_PCLRR_FEC1H_PCLRRFEC1H7            (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FEC1L */
#define MCF_GPIO_PCLRR_FEC1L_PCLRRFEC1L0            (0x01)
#define MCF_GPIO_PCLRR_FEC1L_PCLRRFEC1L1            (0x02)
#define MCF_GPIO_PCLRR_FEC1L_PCLRRFEC1L2            (0x04)
#define MCF_GPIO_PCLRR_FEC1L_PCLRRFEC1L3            (0x08)
#define MCF_GPIO_PCLRR_FEC1L_PODRFEC1L4             (0x10)
#define MCF_GPIO_PCLRR_FEC1L_PCLRRFEC1L5            (0x20)
#define MCF_GPIO_PCLRR_FEC1L_PCLRRFEC1L6            (0x40)
#define MCF_GPIO_PCLRR_FEC1L_PCLRRFEC1L7            (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_FECI2C */
#define MCF_GPIO_PCLRR_FECI2C_PCLRRFECI2C0          (0x01)
#define MCF_GPIO_PCLRR_FECI2C_PCLRRFECI2C1          (0x02)
#define MCF_GPIO_PCLRR_FECI2C_PODRFECI2C2           (0x04)
#define MCF_GPIO_PCLRR_FECI2C_PCLRRFECI2C3          (0x08)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PCIBG */
#define MCF_GPIO_PCLRR_PCIBG_PODRPCIBG0             (0x01)
#define MCF_GPIO_PCLRR_PCIBG_PODRPCIBG1             (0x02)
#define MCF_GPIO_PCLRR_PCIBG_PODRPCIBG2             (0x04)
#define MCF_GPIO_PCLRR_PCIBG_PCLRRPCIBG3            (0x08)
#define MCF_GPIO_PCLRR_PCIBG_PCLRRPCIBG4            (0x10)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PCIBR */
#define MCF_GPIO_PCLRR_PCIBR_PCLRRPCIBR0            (0x01)
#define MCF_GPIO_PCLRR_PCIBR_PCLRRPCIBR1            (0x02)
#define MCF_GPIO_PCLRR_PCIBR_PCLRRPCIBR2            (0x04)
#define MCF_GPIO_PCLRR_PCIBR_PODRPCIBR3             (0x08)
#define MCF_GPIO_PCLRR_PCIBR_PODRPCIBR4             (0x10)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PSC3PSC2 */
#define MCF_GPIO_PCLRR_PSC3PSC2_PODRPSC3PSC20       (0x01)
#define MCF_GPIO_PCLRR_PSC3PSC2_PODRPSC3PSC21       (0x02)
#define MCF_GPIO_PCLRR_PSC3PSC2_PCLRRPSC3PSC22      (0x04)
#define MCF_GPIO_PCLRR_PSC3PSC2_PCLRRPSC3PSC23      (0x08)
#define MCF_GPIO_PCLRR_PSC3PSC2_PCLRRPSC3PSC24      (0x10)
#define MCF_GPIO_PCLRR_PSC3PSC2_PODRPSC3PSC25       (0x20)
#define MCF_GPIO_PCLRR_PSC3PSC2_PODRPSC3PSC26       (0x40)
#define MCF_GPIO_PCLRR_PSC3PSC2_PCLRRPSC3PSC27      (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_PSC1PSC0 */
#define MCF_GPIO_PCLRR_PSC1PSC0_PCLRRPSC1PSC00      (0x01)
#define MCF_GPIO_PCLRR_PSC1PSC0_PCLRRPSC1PSC01      (0x02)
#define MCF_GPIO_PCLRR_PSC1PSC0_PCLRRPSC1PSC02      (0x04)
#define MCF_GPIO_PCLRR_PSC1PSC0_PCLRRPSC1PSC03      (0x08)
#define MCF_GPIO_PCLRR_PSC1PSC0_PCLRRPSC1PSC04      (0x10)
#define MCF_GPIO_PCLRR_PSC1PSC0_PCLRRPSC1PSC05      (0x20)
#define MCF_GPIO_PCLRR_PSC1PSC0_PODRPSC1PSC06       (0x40)
#define MCF_GPIO_PCLRR_PSC1PSC0_PCLRRPSC1PSC07      (0x80)

/* Bit definitions and macros for MCF_GPIO_PCLRR_DSPI */
#define MCF_GPIO_PCLRR_DSPI_PCLRRDSPI0              (0x01)
#define MCF_GPIO_PCLRR_DSPI_PCLRRDSPI1              (0x02)
#define MCF_GPIO_PCLRR_DSPI_PCLRRDSPI2              (0x04)
#define MCF_GPIO_PCLRR_DSPI_PCLRRDSPI3              (0x08)
#define MCF_GPIO_PCLRR_DSPI_PCLRRDSPI4              (0x10)
#define MCF_GPIO_PCLRR_DSPI_PCLRRDSPI5              (0x20)
#define MCF_GPIO_PCLRR_DSPI_PCLRRDSPI6              (0x40)

/* Bit definitions and macros for MCF_GPIO_PAR_FBCTL */
#define MCF_GPIO_PAR_FBCTL_PAR_TS(x)                (((x)&0x0003)<<0)
#define MCF_GPIO_PAR_FBCTL_PAR_TA                   (0x0004)
#define MCF_GPIO_PAR_FBCTL_PAR_RWB                  (0x0010)
#define MCF_GPIO_PAR_FBCTL_PAR_OE                   (0x0040)
#define MCF_GPIO_PAR_FBCTL_PAR_BWE0                 (0x0100)
#define MCF_GPIO_PAR_FBCTL_PAR_BWE1                 (0x0400)
#define MCF_GPIO_PAR_FBCTL_PAR_BWE2                 (0x1000)
#define MCF_GPIO_PAR_FBCTL_PAR_BWE3                 (0x4000)
#define MCF_GPIO_PAR_FBCTL_PAR_TS_GPIO              (0)
#define MCF_GPIO_PAR_FBCTL_PAR_TS_TBST              (2)
#define MCF_GPIO_PAR_FBCTL_PAR_TS_TS                (3)

/* Bit definitions and macros for MCF_GPIO_PAR_FBCS */
#define MCF_GPIO_PAR_FBCS_PAR_CS1                   (0x02)
#define MCF_GPIO_PAR_FBCS_PAR_CS2                   (0x04)
#define MCF_GPIO_PAR_FBCS_PAR_CS3                   (0x08)
#define MCF_GPIO_PAR_FBCS_PAR_CS4                   (0x10)
#define MCF_GPIO_PAR_FBCS_PAR_CS5                   (0x20)

/* Bit definitions and macros for MCF_GPIO_PAR_DMA */
#define MCF_GPIO_PAR_DMA_PAR_DREQ0(x)               (((x)&0x03)<<0)
#define MCF_GPIO_PAR_DMA_PAR_DREQ1(x)               (((x)&0x03)<<2)
#define MCF_GPIO_PAR_DMA_PAR_DACK0(x)               (((x)&0x03)<<4)
#define MCF_GPIO_PAR_DMA_PAR_DACK1(x)               (((x)&0x03)<<6)
#define MCF_GPIO_PAR_DMA_PAR_DACKx_GPIO             (0)
#define MCF_GPIO_PAR_DMA_PAR_DACKx_TOUT             (2)
#define MCF_GPIO_PAR_DMA_PAR_DACKx_DACK             (3)
#define MCF_GPIO_PAR_DMA_PAR_DREQx_GPIO             (0)
#define MCF_GPIO_PAR_DMA_PAR_DREQx_TIN              (2)
#define MCF_GPIO_PAR_DMA_PAR_DREQx_DREQ             (3)

/* Bit definitions and macros for MCF_GPIO_PAR_FECI2CIRQ */
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_IRQ5             (0x0001)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_IRQ6             (0x0002)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_SCL              (0x0004)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_SDA              (0x0008)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MDC(x)         (((x)&0x0003)<<6)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO(x)        (((x)&0x0003)<<8)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MII            (0x0400)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E17              (0x0800)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E0MDC            (0x1000)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E0MDIO           (0x2000)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E0MII            (0x4000)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E07              (0x8000)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO_CANRX     (0x0000)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO_SDA       (0x0200)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MDIO_EMDIO     (0x0300)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MDC_CANTX      (0x0000)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MDC_SCL        (0x0080)
#define MCF_GPIO_PAR_FECI2CIRQ_PAR_E1MDC_EMDC       (0x00C0)

/* Bit definitions and macros for MCF_GPIO_PAR_PCIBG */
#define MCF_GPIO_PAR_PCIBG_PAR_PCIBG0(x)            (((x)&0x0003)<<0)
#define MCF_GPIO_PAR_PCIBG_PAR_PCIBG1(x)            (((x)&0x0003)<<2)
#define MCF_GPIO_PAR_PCIBG_PAR_PCIBG2(x)            (((x)&0x0003)<<4)
#define MCF_GPIO_PAR_PCIBG_PAR_PCIBG3(x)            (((x)&0x0003)<<6)
#define MCF_GPIO_PAR_PCIBG_PAR_PCIBG4(x)            (((x)&0x0003)<<8)

/* Bit definitions and macros for MCF_GPIO_PAR_PCIBR */
#define MCF_GPIO_PAR_PCIBR_PAR_PCIBG0(x)            (((x)&0x0003)<<0)
#define MCF_GPIO_PAR_PCIBR_PAR_PCIBG1(x)            (((x)&0x0003)<<2)
#define MCF_GPIO_PAR_PCIBR_PAR_PCIBG2(x)            (((x)&0x0003)<<4)
#define MCF_GPIO_PAR_PCIBR_PAR_PCIBG3(x)            (((x)&0x0003)<<6)
#define MCF_GPIO_PAR_PCIBR_PAR_PCIBR4(x)            (((x)&0x0003)<<8)

/* Bit definitions and macros for MCF_GPIO_PAR_PSC3 */
#define MCF_GPIO_PAR_PSC3_PAR_TXD3                  (0x04)
#define MCF_GPIO_PAR_PSC3_PAR_RXD3                  (0x08)
#define MCF_GPIO_PAR_PSC3_PAR_RTS3(x)               (((x)&0x03)<<4)
#define MCF_GPIO_PAR_PSC3_PAR_CTS3(x)               (((x)&0x03)<<6)
#define MCF_GPIO_PAR_PSC3_PAR_CTS3_GPIO             (0x00)
#define MCF_GPIO_PAR_PSC3_PAR_CTS3_BCLK             (0x80)
#define MCF_GPIO_PAR_PSC3_PAR_CTS3_CTS              (0xC0)
#define MCF_GPIO_PAR_PSC3_PAR_RTS3_GPIO             (0x00)
#define MCF_GPIO_PAR_PSC3_PAR_RTS3_FSYNC            (0x20)
#define MCF_GPIO_PAR_PSC3_PAR_RTS3_RTS              (0x30)
#define MCF_GPIO_PAR_PSC3_PAR_CTS2_CANRX            (0x40)

/* Bit definitions and macros for MCF_GPIO_PAR_PSC2 */
#define MCF_GPIO_PAR_PSC2_PAR_TXD2                  (0x04)
#define MCF_GPIO_PAR_PSC2_PAR_RXD2                  (0x08)
#define MCF_GPIO_PAR_PSC2_PAR_RTS2(x)               (((x)&0x03)<<4)
#define MCF_GPIO_PAR_PSC2_PAR_CTS2(x)               (((x)&0x03)<<6)
#define MCF_GPIO_PAR_PSC2_PAR_CTS2_GPIO             (0x00)
#define MCF_GPIO_PAR_PSC2_PAR_CTS2_BCLK             (0x80)
#define MCF_GPIO_PAR_PSC2_PAR_CTS2_CTS              (0xC0)
#define MCF_GPIO_PAR_PSC2_PAR_RTS2_GPIO             (0x00)
#define MCF_GPIO_PAR_PSC2_PAR_RTS2_CANTX            (0x10)
#define MCF_GPIO_PAR_PSC2_PAR_RTS2_FSYNC            (0x20)
#define MCF_GPIO_PAR_PSC2_PAR_RTS2_RTS              (0x30)
#define MCF_GPIO_PAR_PSC2_PAR_RTS2_CANRX            (0x40)

/* Bit definitions and macros for MCF_GPIO_PAR_PSC1 */
#define MCF_GPIO_PAR_PSC1_PAR_TXD1                  (0x04)
#define MCF_GPIO_PAR_PSC1_PAR_RXD1                  (0x08)
#define MCF_GPIO_PAR_PSC1_PAR_RTS1(x)               (((x)&0x03)<<4)
#define MCF_GPIO_PAR_PSC1_PAR_CTS1(x)               (((x)&0x03)<<6)
#define MCF_GPIO_PAR_PSC1_PAR_CTS1_GPIO             (0x00)
#define MCF_GPIO_PAR_PSC1_PAR_CTS1_BCLK             (0x80)
#define MCF_GPIO_PAR_PSC1_PAR_CTS1_CTS              (0xC0)
#define MCF_GPIO_PAR_PSC1_PAR_RTS1_GPIO             (0x00)
#define MCF_GPIO_PAR_PSC1_PAR_RTS1_FSYNC            (0x20)
#define MCF_GPIO_PAR_PSC1_PAR_RTS1_RTS              (0x30)

/* Bit definitions and macros for MCF_GPIO_PAR_PSC0 */
#define MCF_GPIO_PAR_PSC0_PAR_TXD0                  (0x04)
#define MCF_GPIO_PAR_PSC0_PAR_RXD0                  (0x08)
#define MCF_GPIO_PAR_PSC0_PAR_RTS0(x)               (((x)&0x03)<<4)
#define MCF_GPIO_PAR_PSC0_PAR_CTS0(x)               (((x)&0x03)<<6)
#define MCF_GPIO_PAR_PSC0_PAR_CTS0_GPIO             (0x00)
#define MCF_GPIO_PAR_PSC0_PAR_CTS0_BCLK             (0x80)
#define MCF_GPIO_PAR_PSC0_PAR_CTS0_CTS              (0xC0)
#define MCF_GPIO_PAR_PSC0_PAR_RTS0_GPIO             (0x00)
#define MCF_GPIO_PAR_PSC0_PAR_RTS0_FSYNC            (0x20)
#define MCF_GPIO_PAR_PSC0_PAR_RTS0_RTS              (0x30)

/* Bit definitions and macros for MCF_GPIO_PAR_DSPI */
#define MCF_GPIO_PAR_DSPI_PAR_SOUT(x)               (((x)&0x0003)<<0)
#define MCF_GPIO_PAR_DSPI_PAR_SIN(x)                (((x)&0x0003)<<2)
#define MCF_GPIO_PAR_DSPI_PAR_SCK(x)                (((x)&0x0003)<<4)
#define MCF_GPIO_PAR_DSPI_PAR_CS0(x)                (((x)&0x0003)<<6)
#define MCF_GPIO_PAR_DSPI_PAR_CS2(x)                (((x)&0x0003)<<8)
#define MCF_GPIO_PAR_DSPI_PAR_CS3(x)                (((x)&0x0003)<<10)
#define MCF_GPIO_PAR_DSPI_PAR_CS5                   (0x1000)
#define MCF_GPIO_PAR_DSPI_PAR_CS3_GPIO              (0x0000)
#define MCF_GPIO_PAR_DSPI_PAR_CS3_CANTX             (0x0400)
#define MCF_GPIO_PAR_DSPI_PAR_CS3_TOUT              (0x0800)
#define MCF_GPIO_PAR_DSPI_PAR_CS3_DSPICS            (0x0C00)
#define MCF_GPIO_PAR_DSPI_PAR_CS2_GPIO              (0x0000)
#define MCF_GPIO_PAR_DSPI_PAR_CS2_CANTX             (0x0100)
#define MCF_GPIO_PAR_DSPI_PAR_CS2_TOUT              (0x0200)
#define MCF_GPIO_PAR_DSPI_PAR_CS2_DSPICS            (0x0300)
#define MCF_GPIO_PAR_DSPI_PAR_CS0_GPIO              (0x0000)
#define MCF_GPIO_PAR_DSPI_PAR_CS0_FSYNC             (0x0040)
#define MCF_GPIO_PAR_DSPI_PAR_CS0_RTS               (0x0080)
#define MCF_GPIO_PAR_DSPI_PAR_CS0_DSPICS            (0x00C0)
#define MCF_GPIO_PAR_DSPI_PAR_SCK_GPIO              (0x0000)
#define MCF_GPIO_PAR_DSPI_PAR_SCK_BCLK              (0x0010)
#define MCF_GPIO_PAR_DSPI_PAR_SCK_CTS               (0x0020)
#define MCF_GPIO_PAR_DSPI_PAR_SCK_SCK               (0x0030)
#define MCF_GPIO_PAR_DSPI_PAR_SIN_GPIO              (0x0000)
#define MCF_GPIO_PAR_DSPI_PAR_SIN_RXD               (0x0008)
#define MCF_GPIO_PAR_DSPI_PAR_SIN_SIN               (0x000C)
#define MCF_GPIO_PAR_DSPI_PAR_SOUT_GPIO             (0x0000)
#define MCF_GPIO_PAR_DSPI_PAR_SOUT_TXD              (0x0002)
#define MCF_GPIO_PAR_DSPI_PAR_SOUT_SOUT             (0x0003)

/* Bit definitions and macros for MCF_GPIO_PAR_TIMER */
#define MCF_GPIO_PAR_TIMER_PAR_TOUT2                (0x01)
#define MCF_GPIO_PAR_TIMER_PAR_TIN2(x)              (((x)&0x03)<<1)
#define MCF_GPIO_PAR_TIMER_PAR_TOUT3                (0x08)
#define MCF_GPIO_PAR_TIMER_PAR_TIN3(x)              (((x)&0x03)<<4)
#define MCF_GPIO_PAR_TIMER_PAR_TIN3_CANRX           (0x00)
#define MCF_GPIO_PAR_TIMER_PAR_TIN3_IRQ             (0x20)
#define MCF_GPIO_PAR_TIMER_PAR_TIN3_TIN             (0x30)
#define MCF_GPIO_PAR_TIMER_PAR_TIN2_CANRX           (0x00)
#define MCF_GPIO_PAR_TIMER_PAR_TIN2_IRQ             (0x04)
#define MCF_GPIO_PAR_TIMER_PAR_TIN2_TIN             (0x06)

/********************************************************************/

#endif /* _M5485GPIO_H_ */
