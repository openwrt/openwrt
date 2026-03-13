// SPDX-License-Identifier: GPL-2.0-or-later

// https://gist.github.com/Noltari/f75b254ceee9683fb2ed#file-3380_intr-h

#ifndef __DT_BINDINGS_INTERRUPT_CONTROLLER_BCM3380_H
#define __DT_BINDINGS_INTERRUPT_CONTROLLER_BCM3380_H

// Logical Peripheral Interrupt IDs
#define BCM3380_IRQ_TIMER               0
#define BCM3380_IRQ_SPI                 1
#define BCM3380_IRQ_UART                2
#define BCM3380_IRQ_UART1               3
#define BCM3380_IRQ_SIMCARD0            4
#define BCM3380_IRQ_SIMCARD1            5
#define BCM3380_IRQ_I2C                 6
#define BCM3380_IRQ_HS_SPI              7
#define BCM3380_IRQ_RING_OSC            8
#define BCM3380_IRQ_PERIPH_ERR          9
#define BCM3380_IRQ_RESERVED_10         10
#define BCM3380_IRQ_RESERVED_11         11
#define BCM3380_IRQ_RESERVED_12         12
#define BCM3380_IRQ_RESERVED_13         13
#define BCM3380_IRQ_RESERVED_14         14
#define BCM3380_IRQ_PCIE_RC             15
#define BCM3380_IRQ_PCIE_EP_LNK_RST     16
#define BCM3380_IRQ_BRG_UBUS0           17
#define BCM3380_IRQ_BRG_UBUS1           18
#define BCM3380_IRQ_FPM                 19
#define BCM3380_IRQ_USB0                20
#define BCM3380_IRQ_USB1                21
#define BCM3380_IRQ_APM                 22
#define BCM3380_IRQ_APM_DMA             23
#define BCM3380_IRQ_UNI_IRQ2            24
#define BCM3380_IRQ_UNI_IRQ             25
#define BCM3380_IRQ_GPHY_IRQB           26
#define BCM3380_IRQ_DAVIC               27
#define BCM3380_IRQ_OB                  28
#define BCM3380_IRQ_RESERVED_29         29
#define BCM3380_IRQ_RESERVED_30         30
#define BCM3380_IRQ_EXT_IRQ             31

#define BCM3380_IRQ_IOP_UTP             0
#define BCM3380_IRQ_IOP_DTP             1
#define BCM3380_IRQ_IOP_MEP             2
#define BCM3380_IRQ_IOP_MSP             3
#define BCM3380_IRQ_IOP_FAP             4

// These might not be necessary
#define INTERRUPT_ID_LAST   BCM3380_IRQ_MSP_SW_IRQ
#define INTERRUPT_ID_MPI    BCM3380_IRQ_EXT_IRQ

// We dont know these as of 20241012
#define BCM3380_EXTIRQ_0		0 /* GPIO 24 */
#define BCM3380_EXTIRQ_1		1 /* GPIO 25 */
#define BCM3380_EXTIRQ_2		2 /* GPIO 26 */
#define BCM3380_EXTIRQ_3		3 /* GPIO 27 */

#endif /* __DT_BINDINGS_INTERRUPT_CONTROLLER_BCM3380_H */
