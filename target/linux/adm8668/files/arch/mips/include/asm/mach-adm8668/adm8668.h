/************************************************************************
 *
 *    Copyright (c) 2005
 *    Infineon Technologies AG
 *    St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 ************************************************************************/

#ifndef  __ADM8668_H__
#define  __ADM8668_H__

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
#define ADM8668_NAND_BASE	0x1FFFFF00

#define ADM8668_PCICFG_BASE	0x12200000
#define ADM8668_PCIDAT_BASE	0x12400000

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

/* register access macros */
#define ADM8668_LAN_REG(_reg)		\
	(*((volatile unsigned int *)(KSEG1ADDR(ADM8668_LAN_BASE + (_reg)))))
#define ADM8668_WAN_REG(_reg)		\
	(*((volatile unsigned int *)(KSEG1ADDR(ADM8668_WAN_BASE + (_reg)))))
#define ADM8668_WLAN_REG(_reg)		\
	(*((volatile unsigned int *)(KSEG1ADDR(ADM8668_WLAN_BASE + (_reg)))))
#define ADM8668_CONFIG_REG(_reg)	\
	(*((volatile unsigned int *)(KSEG1ADDR(ADM8668_CONFIG_BASE + (_reg)))))

/* lan registers */
#define	NETCSR6			0x30
#define	NETCSR7			0x38
#define	NETCSR37		0xF8

/* known/used CPU configuration registers */
#define ADM8668_CR0		0x00
#define ADM8668_CR1		0x04
#define ADM8668_CR3		0x0C
#define ADM8668_CR66		0x108

/** For GPIO control **/
#define	GPIO_REG		0x5C	/* on WLAN */
#define CRGPIO_REG		0x20	/* on CPU */
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

void adm8668_init_clocks(void);

#endif /* __ADM8668_H__ */
