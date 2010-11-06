/************************************************************************
 *
 *    Copyright (c) 2005
 *    Infineon Technologies AG
 *    St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 ************************************************************************/

#ifndef  __ADM8668_H__
#define  __ADM8668_H__

#include <asm/addrspace.h>
#include "bsp_sup.h"

#define MEM_KSEG0_BASE		0x80000000
#define MEM_KSEG1_BASE		0xA0000000
#define MEM_SEG_MASK		0xE0000000
#define KVA2PA(_addr)		((unsigned long)(_addr) & ~MEM_SEG_MASK)

#define MIPS_KSEG0A(_addr)	(KVA2PA(_addr) | MEM_KSEG0_BASE)
#define MIPS_KSEG1A(_addr)	(KVA2PA(_addr) | MEM_KSEG1_BASE)

#define PA2VA(_addr)		(KVA2PA(_addr) | MEM_KSEG1_BASE)
#define PA2CACHEVA(_addr)	(KVA2PA(_addr) | MEM_KSEG0_BASE)


/*=======================  Physical Memory Map  ============================*/
#define ADM8668_SDRAM_BASE	0
#define ADM8668_SMEM1_BASE	0x10000000
#define ADM8668_MPMC_BASE	0x11000000
#define ADM8668_USB_BASE	0x11200000
#define ADM8668_CONFIG_BASE	0x11400000
#define ADM8668_WAN_BASE	0x11600000
#define ADM8668_WLAN_BASE	0x11800000
#define ADM8668_LAN_BASE	0x11A00000
#define ADM8668_INTC_BASE	0x1E000000
#define ADM8668_TMR_BASE	0x1E200000
#define ADM8668_UART0_BASE	0x1E400000
#define ADM8668_SMEM0_BASE	0x1FC00000
#define ADM8668_NAND_BASE	0x1fffff00

#define PCICFG_BASE		0x12200000
#define PCIDAT_BASE		0x12400000

/* for PCI bridge fixup ! */
#define PCI_BRIDGE_MASK		0x40

/* WLAN registers */
#define WCSR0			0x00
#define WCSR11A			0x5c

#define GPIO_REG		WCSR11A

#define ADM8668_WLAN_REG(_reg)		\
	(*((volatile unsigned int *)(PA2VA(ADM8668_WLAN_BASE + (_reg)))))

/* configuration registers */
#define ADM8668_CR0		0x00
#define ADM8668_CR1		0x04
#define ADM8668_CR2		0x08
#define ADM8668_CR3		0x0C
#define ADM8668_CR8		0x20
#define ADM8668_CR10		0x28
#define ADM8668_CR11		0x2C
#define ADM8668_CR12		0x30
#define ADM8668_CR13		0x34
#define ADM8668_CR14		0x38
#define ADM8668_CR15		0x3C
#define ADM8668_CR16		0x40
#define ADM8668_CR17		0x44
#define ADM8668_CR18		0x48
#define ADM8668_CR19		0x4C
#define ADM8668_CR20		0x50
#define ADM8668_CR21		0x54
#define ADM8668_CR22		0x58
#define ADM8668_CR23		0x5C
#define ADM8668_CR24		0x60
#define ADM8668_CR25		0x64
#define ADM8668_CR26		0x68
#define ADM8668_CR27		0x6C
#define ADM8668_CR28		0x70
#define ADM8668_CR29		0x74
#define ADM8668_CR30		0x78
#define ADM8668_CR31		0x7C
#define ADM8668_CR32		0x80
#define ADM8668_CR33		0x84
#define ADM8668_CR34		0x88
#define ADM8668_CR35		0x8C
#define ADM8668_CR36		0x90
#define ADM8668_CR37		0x94
#define ADM8668_CR38		0x98
#define ADM8668_CR39		0x9C
#define ADM8668_CR40		0xA0
#define ADM8668_CR41		0xA4
#define ADM8668_CR42		0xA8
#define ADM8668_CR43		0xAC
#define ADM8668_CR44		0xB0
#define ADM8668_CR45		0xB4
#define ADM8668_CR46		0xB8
#define ADM8668_CR47		0xBC
#define ADM8668_CR48		0xC0
#define ADM8668_CR49		0xC4
#define ADM8668_CR50		0xC8
#define ADM8668_CR51		0xCC
#define ADM8668_CR52		0xD0
#define ADM8668_CR53		0xD4
#define ADM8668_CR54		0xD8
#define ADM8668_CR55		0xDC
#define ADM8668_CR56		0xE0
#define ADM8668_CR57		0xE4
#define ADM8668_CR58		0xE8
#define ADM8668_CR59		0xEC
#define ADM8668_CR60		0xF0
#define ADM8668_CR61		0xF4
#define ADM8668_CR62		0xF8
#define ADM8668_CR63		0xFC
#define ADM8668_CR64		0x100
#define ADM8668_CR65		0x104
#define ADM8668_CR66		0x108
#define ADM8668_CR67		0x10C
#define ADM8668_CR68		0x110

#define CRGPIO_REG		ADM8668_CR8

#define ADM8668_CONFIG_REG(_reg)		\
	(*((volatile unsigned int *)(PA2VA(ADM8668_CONFIG_BASE + (_reg)))))
#define ADM8668_MPMC_REG(_reg)		\
	(*((volatile unsigned int *)(PA2VA(ADM8668_MPMC_BASE + (_reg)))))


/*==========================  Interrupt Controller ==========================*/
/* registers offset */
#define IRQ_STATUS_REG		0x00	/* Read */
#define IRQ_RAW_STATUS_REG	0x04	/* Read */
#define IRQ_ENABLE_REG		0x08	/* Read/Write */
#define IRQ_DISABLE_REG		0x0C	/* Write */
#define IRQ_SOFT_REG		0x10	/* Write */

#define FIQ_STATUS_REG		0x100	/* Read */
#define FIQ_RAW_STATUS_REG	0x104
#define FIQ_ENABLE_REG		0x108
#define FIQ_DISABLE_REG		0x10c


/*  Macro for accessing Interrupt controller register  */
#define ADM8668_INTC_REG(_reg)		\
	(*((volatile unsigned long *)(PA2VA(ADM8668_INTC_BASE + (_reg)))))


/* interrupt levels */
#define INT_LVL_SWI		1
#define INT_LVL_COMMS_RX	2
#define INT_LVL_COMMS_TX	3
#define INT_LVL_TIMERINT0	4
#define INT_LVL_TIMERINT1	5
#define INT_LVL_UART0		6
#define INT_LVL_LAN		7
#define INT_LVL_WAN		8
#define INT_LVL_WLAN		9
#define INT_LVL_GPIO		10
#define INT_LVL_IDE		11
#define INT_LVL_PCI2		12
#define INT_LVL_PCI1		13
#define INT_LVL_PCI0		14
#define INT_LVL_USB		15
#define INT_LVL_MAX		INT_LVL_USB

#define IRQ_MASK		0xffff

#define IRQ_SWI			(0x1<<INT_LVL_SWI)
#define IRQ_TIMERINT0		(0x1<<INT_LVL_TIMERINT0)
#define IRQ_TIMERINT1		(0x1<<INT_LVL_TIMERINT1)
#define IRQ_UART0		(0x1<<INT_LVL_UART0)
#define IRQ_LAN			(0x1<<INT_LVL_LAN)
#define IRQ_WAN			(0x1<<INT_LVL_WAN)
#define IRQ_WLAN		(0x1<<INT_LVL_WLAN)
#define IRQ_GPIO		(0x1<<INT_LVL_GPIO)
#define IRQ_IDEINT		(0x1<<INT_LVL_IDE)
#define IRQ_PCI2		(0x1<<INT_LVL_PCI2)
#define IRQ_PCI1		(0x1<<INT_LVL_PCI1)
#define IRQ_PCI0		(0x1<<INT_LVL_PCI0)
#define IRQ_USB			(0x1<<INT_LVL_USB)


/*===========================  UART Control Register ========================*/
#define UART_DR_REG		0x00
#define UART_RSR_REG		0x04
#define UART_ECR_REG		0x04
#define UART_LCR_H_REG		0x08
#define UART_LCR_M_REG		0x0c
#define UART_LCR_L_REG		0x10
#define UART_CR_REG		0x14
#define UART_FR_REG		0x18
#define UART_IIR_REG		0x1c
#define UART_ICR_REG		0x1C
#define UART_ILPR_REG		0x20

/*  rsr/ecr reg  */
#define UART_OVERRUN_ERR	0x08
#define UART_BREAK_ERR		0x04
#define UART_PARITY_ERR		0x02
#define UART_FRAMING_ERR	0x01
#define UART_RX_STATUS_MASK	0x0f
#define UART_RX_ERROR		( UART_BREAK_ERR	\
				| UART_PARITY_ERR	\
				| UART_FRAMING_ERR)

/*  lcr_h reg  */
#define UART_SEND_BREAK		0x01
#define UART_PARITY_EN		0x02
#define UART_EVEN_PARITY	0x04
#define UART_TWO_STOP_BITS	0x08
#define UART_ENABLE_FIFO	0x10

#define UART_WLEN_5BITS		0x00
#define UART_WLEN_6BITS		0x20
#define UART_WLEN_7BITS		0x40
#define UART_WLEN_8BITS		0x60
#define UART_WLEN_MASK		0x60

/*  cr reg  */
#define UART_PORT_EN			0x01
#define UART_SIREN			0x02
#define UART_SIRLP			0x04
#define UART_MODEM_STATUS_INT_EN	0x08
#define UART_RX_INT_EN			0x10
#define UART_TX_INT_EN			0x20
#define UART_RX_TIMEOUT_INT_EN		0x40
#define UART_LOOPBACK_EN		0x80

/*  fr reg  */
#define UART_CTS		0x01
#define UART_DSR		0x02
#define UART_DCD		0x04
#define UART_BUSY		0x08
#define UART_RX_FIFO_EMPTY	0x10
#define UART_TX_FIFO_FULL	0x20
#define UART_RX_FIFO_FULL	0x40
#define UART_TX_FIFO_EMPTY	0x80

/*  iir/icr reg  */
#define UART_MODEM_STATUS_INT	0x01
#define UART_RX_INT		0x02
#define UART_TX_INT		0x04
#define UART_RX_TIMEOUT_INT	0x08

#define UART_INT_MASK		0x0f

#ifdef _FPGA_
#define ADM8668_UARTCLK_FREQ	3686400
#else
#define ADM8668_UARTCLK_FREQ	62500000
#endif

#define UART_BAUDDIV(_rate)				\
	((unsigned long)(ADM8668_UARTCLK_FREQ/(16*(_rate)) - 1))

/*  uart_baudrate  */
#define UART_230400bps_DIVISOR	UART_BAUDDIV(230400)
#define UART_115200bps_DIVISOR	UART_BAUDDIV(115200)
#define UART_76800bps_DIVISOR	UART_BAUDDIV(76800)
#define UART_57600bps_DIVISOR	UART_BAUDDIV(57600)
#define UART_38400bps_DIVISOR	UART_BAUDDIV(38400)
#define UART_19200bps_DIVISOR	UART_BAUDDIV(19200)
#define UART_14400bps_DIVISOR	UART_BAUDDIV(14400)
#define UART_9600bps_DIVISOR	UART_BAUDDIV(9600)
#define UART_2400bps_DIVISOR	UART_BAUDDIV(2400)
#define UART_1200bps_DIVISOR	UART_BAUDDIV(1200)


/*===========================  Counter Timer ==============================*/
#define TIMER0_REG_BASE		ADM8668_TMR_BASE
#define TIMER1_REG_BASE		ADM8668_TMR_BASE+0x20

#define TIMER_LOAD_REG		0x00
#define TIMER_VALUE_REG		0x04
#define TIMER_CTRL_REG		0x08
#define TIMER_CLR_REG		0x0c

/* TIMER_LOAD_REG */
#ifdef _FPGA_
#define SYS_CLOCK		56000000
#else
#define SYS_CLOCK		175000000
#endif

#define SYS_PRESCALE		256

#define TMR_10MS_TICKS		(SYS_CLOCK/SYS_PRESCALE/100)

/* TIMER_CTRL_REG */
#define TMR_PRESCALE_1		0x00
#define TMR_PRESCALE_16		0x04
#define TMR_PRESCALE_256	0x08
#define TMR_MODE_PERIODIC	0x40
#define TMR_ENABLE		0x80

/* TIMER_CLR_REG */
#define TMR_CLEAR_BIT		1


/* Macro for access MPMC register */
#define ADM8668_TMR_REG(base, _offset)				\
		(*((volatile unsigned long *)(PA2VA(base + (_offset)))))


/* For GPIO control */
#define GPIO0_OUTPUT_ENABLE	0x1000
#define GPIO1_OUTPUT_ENABLE	0x2000
#define GPIO2_OUTPUT_ENABLE	0x4000
#define GPIO_OUTPUT_ENABLE_ALL	0x7000
#define GPIO0_OUTPUT_1		0x40
#define GPIO1_OUTPUT_1		0x80
#define GPIO2_OUTPUT_1		0x100
#define GPIO0_INPUT_1		0x1
#define GPIO1_INPUT_1		0x2
#define GPIO2_INPUT_1		0x4

#define GPIO_SET_HI(num)	\
	ADM8668_WLAN_REG(GPIO_REG) |= (1 << (6 + num))

#define GPIO_SET_LOW(num)	\
	ADM8668_WLAN_REG(GPIO_REG) &= ~(1 << (6 + num))

#define GPIO_TOGGLE(num)	\
	ADM8668_WLAN_REG(GPIO_REG) ^= (1 << (6 + num))

#define CRGPIO_SET_HI(num)	\
	ADM8668_CONFIG_REG(CRGPIO_REG) |= (1 << (6 + num))

#define CRGPIO_SET_LOW(num)	\
	ADM8668_CONFIG_REG(CRGPIO_REG) &= ~(1 << (6 + num))

#define CRGPIO_TOGGLE(num)	\
	ADM8668_CONFIG_REG(CRGPIO_REG) ^= (1 << (6 + num))


/*==========================================================================*/
/* Cache Controller */
#define ADM8668_CACHE_LINE_SIZE		16

#define BIT_0			0x00000001
#define BIT_1			0x00000002
#define BIT_2			0x00000004
#define BIT_3			0x00000008
#define BIT_4			0x00000010
#define BIT_5			0x00000020
#define BIT_6			0x00000040
#define BIT_7			0x00000080
#define BIT_8			0x00000100
#define BIT_9			0x00000200
#define BIT_10			0x00000400
#define BIT_11			0x00000800
#define BIT_12			0x00001000
#define BIT_13			0x00002000
#define BIT_14			0x00004000
#define BIT_15			0x00008000
#define BIT_16			0x00010000
#define BIT_17			0x00020000
#define BIT_18			0x00040000
#define BIT_19			0x00080000
#define BIT_20			0x00100000
#define BIT_21			0x00200000
#define BIT_22			0x00400000
#define BIT_23			0x00800000
#define BIT_24			0x01000000
#define BIT_25			0x02000000
#define BIT_26			0x04000000
#define BIT_27			0x08000000
#define BIT_28			0x10000000
#define BIT_29			0x20000000
#define BIT_30			0x40000000
#define BIT_31			0x80000000

/* network regs */
#define	NETCSR0			0x0
#define	NETCSR1			0x08
#define	NETCSR2			0x10
#define	NETCSR3			0x18
#define	NETCSR4			0x20
#define	NETCSR5			0x28
#define	NETCSR6			0x30
#define	NETCSR7			0x38
#define	NETCSR8			0x40
#define	NETCSR9			0x48
#define	NETCSR10		0x50
#define	NETCSR11		0x58
#define	NETCSR12		0x60
#define	NETCSR13		0x68
#define	NETCSR14		0x70
#define	NETCSR15		0x78
#define	NETCSR36		0xD0
#define	NETCSR36A		0xD4
#define	NETCSR36B		0xD8
#define	NETCSR36C		0xDC	// dummy
#define	NETCSR36D		0xE0
#define	NETCSR36E		0xE4
#define	NETCSR36F		0xE8
#define	NETCSR36G		0xEC
#define	NETCSR36H		0xF0
#define	NETCSR36I		0xF4
#define	NETCSR37		0xF8

/* for descriptor skip DWs */
#define NETDESCSKIP_1DW		BIT_2
#define NETDESCSKIP_2DW		BIT_3
#define NETDESCSKIP_3DW		(BIT_3|BIT_2)
#define NETDESCSKIP_4DW		BIT_4


#define ADM8668_LAN_REG(_reg)		\
	(*((volatile unsigned int *)(PA2VA(ADM8668_LAN_BASE + (_reg)))))
#define ADM8668_WAN_REG(_reg)		\
	(*((volatile unsigned int *)(PA2VA(ADM8668_WAN_BASE + (_reg)))))

#endif /* __ADM8668_H__ */
