#ifndef __IDT_GPIO_H__
#define __IDT_GPIO_H__

/*******************************************************************************
 *
 * Copyright 2002 Integrated Device Technology, Inc.
 *		All rights reserved.
 *
 * GPIO register definition.
 *
 * File   : $Id: gpio.h,v 1.2 2002/06/06 18:34:04 astichte Exp $
 *
 * Author : ryan.holmQVist@idt.com
 * Date   : 20011005
 * Update :
 *	    $Log: gpio.h,v $
 *	    Revision 1.2  2002/06/06 18:34:04  astichte
 *	    Added XXX_PhysicalAddress and XXX_VirtualAddress
 *	
 *	    Revision 1.1  2002/05/29 17:33:22  sysarch
 *	    jba File moved from vcode/include/idt/acacia
 *	
 *
 ******************************************************************************/

enum
{
	GPIO0_PhysicalAddress	= 0x18050000,
	GPIO_PhysicalAddress	= GPIO0_PhysicalAddress,	// Default

	GPIO0_VirtualAddress	= 0xb8050000,
	GPIO_VirtualAddress	= GPIO0_VirtualAddress,		// Default
} ;

typedef struct
{
	u32   gpiofunc;   /* GPIO Function Register
			   * gpiofunc[x]==0 bit = gpio
			   * func[x]==1  bit = altfunc
			   */
	u32   gpiocfg;	  /* GPIO Configuration Register
			   * gpiocfg[x]==0 bit = input
			   * gpiocfg[x]==1 bit = output
			   */
	u32   gpiod;	  /* GPIO Data Register
			   * gpiod[x] read/write gpio pinX status
			   */
	u32   gpioilevel; /* GPIO Interrupt Status Register
			   * interrupt level (see gpioistat)
			   */
	u32   gpioistat;  /* Gpio Interrupt Status Register
			   * istat[x] = (gpiod[x] == level[x])
			   * cleared in ISR (STICKY bits)
			   */
	u32   gpionmien;  /* GPIO Non-maskable Interrupt Enable Register */
} volatile * GPIO_t ;

typedef enum
{
	GPIO_gpio_v		= 0,		// gpiofunc use pin as GPIO.
	GPIO_alt_v		= 1,		// gpiofunc use pin as alt.
	GPIO_input_v		= 0,		// gpiocfg use pin as input.
	GPIO_output_v		= 1,		// gpiocfg use pin as output.
	GPIO_pin0_b		= 0,
	GPIO_pin0_m		= 0x00000001,
	GPIO_pin1_b		= 1,
	GPIO_pin1_m		= 0x00000002,
	GPIO_pin2_b		= 2,
	GPIO_pin2_m		= 0x00000004,
	GPIO_pin3_b		= 3,
	GPIO_pin3_m		= 0x00000008,
	GPIO_pin4_b		= 4,
	GPIO_pin4_m		= 0x00000010,
	GPIO_pin5_b		= 5,
	GPIO_pin5_m		= 0x00000020,
	GPIO_pin6_b		= 6,
	GPIO_pin6_m		= 0x00000040,
	GPIO_pin7_b		= 7,
	GPIO_pin7_m		= 0x00000080,
	GPIO_pin8_b		= 8,
	GPIO_pin8_m		= 0x00000100,
	GPIO_pin9_b		= 9,
	GPIO_pin9_m		= 0x00000200,
	GPIO_pin10_b		= 10,
	GPIO_pin10_m		= 0x00000400,
	GPIO_pin11_b		= 11,
	GPIO_pin11_m		= 0x00000800,
	GPIO_pin12_b		= 12,
	GPIO_pin12_m		= 0x00001000,
	GPIO_pin13_b		= 13,
	GPIO_pin13_m		= 0x00002000,
	GPIO_pin14_b		= 14,
	GPIO_pin14_m		= 0x00004000,
	GPIO_pin15_b		= 15,
	GPIO_pin15_m		= 0x00008000,
	GPIO_pin16_b		= 16,
	GPIO_pin16_m		= 0x00010000,
	GPIO_pin17_b		= 17,
	GPIO_pin17_m		= 0x00020000,
	GPIO_pin18_b		= 18,
	GPIO_pin18_m		= 0x00040000,
	GPIO_pin19_b		= 19,
	GPIO_pin19_m		= 0x00080000,
	GPIO_pin20_b		= 20,
	GPIO_pin20_m		= 0x00100000,
	GPIO_pin21_b		= 21,
	GPIO_pin21_m		= 0x00200000,
	GPIO_pin22_b		= 22,
	GPIO_pin22_m		= 0x00400000,
	GPIO_pin23_b		= 23,
	GPIO_pin23_m		= 0x00800000,
	GPIO_pin24_b		= 24,
	GPIO_pin24_m		= 0x01000000,
	GPIO_pin25_b		= 25,
	GPIO_pin25_m		= 0x02000000,
	GPIO_pin26_b		= 26,
	GPIO_pin26_m		= 0x04000000,
	GPIO_pin27_b		= 27,
	GPIO_pin27_m		= 0x08000000,
	GPIO_pin28_b		= 28,
	GPIO_pin28_m		= 0x10000000,
	GPIO_pin29_b		= 29,
	GPIO_pin29_m		= 0x20000000,
	GPIO_pin30_b		= 30,
	GPIO_pin30_m		= 0x40000000,
	GPIO_pin31_b		= 31,
	GPIO_pin31_m		= 0x80000000,

// Alternate function pins.  Corrsponding gpiofunc bit set to GPIO_alt_v.

	GPIO_u0sout_b		= GPIO_pin0_b,		// UART 0 serial out.
	GPIO_u0sout_m		= GPIO_pin0_m,
		GPIO_u0sout_cfg_v	= GPIO_output_v,
	GPIO_u0sinp_b	= GPIO_pin1_b,			// UART 0 serial in.
	GPIO_u0sinp_m	= GPIO_pin1_m,
		GPIO_u0sinp_cfg_v	= GPIO_input_v,
	GPIO_u0rtsn_b	= GPIO_pin2_b,			// UART 0 req. to send.
	GPIO_u0rtsn_m	= GPIO_pin2_m,
		GPIO_u0rtsn_cfg_v	= GPIO_output_v,
	GPIO_u0ctsn_b	= GPIO_pin3_b,			// UART 0 clear to send.
	GPIO_u0ctsn_m	= GPIO_pin3_m,
		GPIO_u0ctsn_cfg_v	= GPIO_input_v,
	GPIO_maddr22_b		= GPIO_pin4_b, 	// M&P bus bit 22.
	GPIO_maddr22_m		= GPIO_pin4_m,
		GPIO_maddr22_cfg_v	= GPIO_output_v,

	GPIO_maddr23_b		= GPIO_pin5_b, 	// M&P bus bit 23.
	GPIO_maddr23_m		= GPIO_pin5_m,
		GPIO_maddr23_cfg_v	= GPIO_output_v,

	GPIO_maddr24_b		= GPIO_pin6_b, 	// M&P bus bit 24.
	GPIO_maddr24_m		= GPIO_pin6_m,
		GPIO_maddr24_cfg_v	= GPIO_output_v,

	GPIO_maddr25_b		= GPIO_pin7_b, 	// M&P bus bit 25.
	GPIO_maddr25_m		= GPIO_pin7_m,
		GPIO_maddr25_cfg_v	= GPIO_output_v,

	GPIO_cpu_b		= GPIO_pin8_b, 	// M&P bus bit 25.
	GPIO_cpu_m		= GPIO_pin8_m,
		GPIO_cpu_cfg_v	= GPIO_output_v,
	GPIO_afspare6_b 	= GPIO_pin9_b, 	// reserved.
	GPIO_afspare6_m 	= GPIO_pin9_m,
		GPIO_afspare6_cfg_v	= GPIO_input_v,
	GPIO_afspare4_b 	= GPIO_pin10_b, 	// reserved.
	GPIO_afspare4_m 	= GPIO_pin10_m,
		GPIO_afspare4_cfg_v	= GPIO_input_v,
	GPIO_afspare3_b 	= GPIO_pin11_b, 	// reserved.
	GPIO_afspare3_m 	= GPIO_pin11_m,
		GPIO_afspare3_cfg_v	= GPIO_input_v,
	GPIO_afspare2_b 	= GPIO_pin12_b, 	// reserved.
	GPIO_afspare2_m 	= GPIO_pin12_m,
		GPIO_afspare2_cfg_v	= GPIO_input_v,
	GPIO_pcimuintn_b	= GPIO_pin13_b, 	// PCI messaging int.
	GPIO_pcimuintn_m	= GPIO_pin13_m,
		GPIO_pcimuintn_cfg_v	= GPIO_output_v,

} GPIO_DEFS_t;

#endif	// __IDT_GPIO_H__

