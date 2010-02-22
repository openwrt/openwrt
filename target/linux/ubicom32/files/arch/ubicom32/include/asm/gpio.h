/*
 * arch/ubicom32/include/asm/gpio.h
 *   Definitions for GPIO operations on Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_GPIO_H
#define _ASM_UBICOM32_GPIO_H

#include <linux/compiler.h>
#include <asm/irq.h>

#include <asm/ip5000.h>

#define ARCH_NR_GPIOS	         512
#define MAX_UBICOM_ONCHIP_GPIO   (9 * 32)

/*
 * Macros for manipulating GPIO numbers
 */
#define gpio_bit(gn)			(1 << (gn & 0x1f))
#define gpio_bank(gn)			(gn >> 5)

#define gpio_pin_index(gn)		(gn & 0x1f)
#define gpio_port_index(gn)		(gn >> 5)

#define GPIO_RA_0    ((32 * 0) + 0)
#define GPIO_RA_1    ((32 * 0) + 1)
#define GPIO_RA_2    ((32 * 0) + 2)
#define GPIO_RA_3    ((32 * 0) + 3)
#define GPIO_RA_4    ((32 * 0) + 4)
#define GPIO_RA_5    ((32 * 0) + 5)
#define GPIO_RA_6    ((32 * 0) + 6)
#define GPIO_RA_7    ((32 * 0) + 7)

#define GPIO_RB_0    ((32 * 1) + 0)
#define GPIO_RB_1    ((32 * 1) + 1)
#define GPIO_RB_2    ((32 * 1) + 2)
#define GPIO_RB_3    ((32 * 1) + 3)
#define GPIO_RB_4    ((32 * 1) + 4)
#define GPIO_RB_5    ((32 * 1) + 5)
#define GPIO_RB_6    ((32 * 1) + 6)
#define GPIO_RB_7    ((32 * 1) + 7)
#define GPIO_RB_8    ((32 * 1) + 8)
#define GPIO_RB_9    ((32 * 1) + 9)
#define GPIO_RB_10   ((32 * 1) + 10)
#define GPIO_RB_11   ((32 * 1) + 11)
#define GPIO_RB_12   ((32 * 1) + 12)
#define GPIO_RB_13   ((32 * 1) + 13)
#define GPIO_RB_14   ((32 * 1) + 14)
#define GPIO_RB_15   ((32 * 1) + 15)
#define GPIO_RB_16   ((32 * 1) + 16)
#define GPIO_RB_17   ((32 * 1) + 17)
#define GPIO_RB_18   ((32 * 1) + 18)
#define GPIO_RB_19   ((32 * 1) + 19)

#define GPIO_RC_0    ((32 * 2) + 0)
#define GPIO_RC_1    ((32 * 2) + 1)
#define GPIO_RC_2    ((32 * 2) + 2)
#define GPIO_RC_3    ((32 * 2) + 3)
#define GPIO_RC_4    ((32 * 2) + 4)
#define GPIO_RC_5    ((32 * 2) + 5)
#define GPIO_RC_6    ((32 * 2) + 6)
#define GPIO_RC_7    ((32 * 2) + 7)
#define GPIO_RC_8    ((32 * 2) + 8)
#define GPIO_RC_9    ((32 * 2) + 9)
#define GPIO_RC_10   ((32 * 2) + 10)
#define GPIO_RC_11   ((32 * 2) + 11)
#define GPIO_RC_12   ((32 * 2) + 12)
#define GPIO_RC_13   ((32 * 2) + 13)
#define GPIO_RC_14   ((32 * 2) + 14)
#define GPIO_RC_15   ((32 * 2) + 15)
#define GPIO_RC_16   ((32 * 2) + 16)
#define GPIO_RC_17   ((32 * 2) + 17)
#define GPIO_RC_18   ((32 * 2) + 18)
#define GPIO_RC_19   ((32 * 2) + 19)
#define GPIO_RC_20   ((32 * 2) + 20)
#define GPIO_RC_21   ((32 * 2) + 21)
#define GPIO_RC_22   ((32 * 2) + 22)
#define GPIO_RC_23   ((32 * 2) + 23)
#define GPIO_RC_24   ((32 * 2) + 24)
#define GPIO_RC_25   ((32 * 2) + 25)
#define GPIO_RC_26   ((32 * 2) + 26)
#define GPIO_RC_27   ((32 * 2) + 27)
#define GPIO_RC_28   ((32 * 2) + 28)
#define GPIO_RC_29   ((32 * 2) + 29)
#define GPIO_RC_30   ((32 * 2) + 30)
#define GPIO_RC_31   ((32 * 2) + 31)

#define GPIO_RD_0    ((32 * 3) + 0)
#define GPIO_RD_1    ((32 * 3) + 1)
#define GPIO_RD_2    ((32 * 3) + 2)
#define GPIO_RD_3    ((32 * 3) + 3)
#define GPIO_RD_4    ((32 * 3) + 4)
#define GPIO_RD_5    ((32 * 3) + 5)
#define GPIO_RD_6    ((32 * 3) + 6)
#define GPIO_RD_7    ((32 * 3) + 7)
#define GPIO_RD_8    ((32 * 3) + 8)
#define GPIO_RD_9    ((32 * 3) + 9)
#define GPIO_RD_10   ((32 * 3) + 10)
#define GPIO_RD_11   ((32 * 3) + 11)

#define GPIO_RE_0    ((32 * 4) + 0)
#define GPIO_RE_1    ((32 * 4) + 1)
#define GPIO_RE_2    ((32 * 4) + 2)
#define GPIO_RE_3    ((32 * 4) + 3)
#define GPIO_RE_4    ((32 * 4) + 4)
#define GPIO_RE_5    ((32 * 4) + 5)
#define GPIO_RE_6    ((32 * 4) + 6)
#define GPIO_RE_7    ((32 * 4) + 7)

#define GPIO_RF_0    ((32 * 5) + 0)
#define GPIO_RF_1    ((32 * 5) + 1)
#define GPIO_RF_2    ((32 * 5) + 2)
#define GPIO_RF_3    ((32 * 5) + 3)
#define GPIO_RF_4    ((32 * 5) + 4)
#define GPIO_RF_5    ((32 * 5) + 5)
#define GPIO_RF_6    ((32 * 5) + 6)
#define GPIO_RF_7    ((32 * 5) + 7)
#define GPIO_RF_8    ((32 * 5) + 8)
#define GPIO_RF_9    ((32 * 5) + 9)
#define GPIO_RF_10   ((32 * 5) + 10)
#define GPIO_RF_11   ((32 * 5) + 11)
#define GPIO_RF_12   ((32 * 5) + 12)
#define GPIO_RF_13   ((32 * 5) + 13)
#define GPIO_RF_14   ((32 * 5) + 14)
#define GPIO_RF_15   ((32 * 5) + 15)

#define GPIO_RG_0    ((32 * 6) + 0)
#define GPIO_RG_1    ((32 * 6) + 1)
#define GPIO_RG_2    ((32 * 6) + 2)
#define GPIO_RG_3    ((32 * 6) + 3)
#define GPIO_RG_4    ((32 * 6) + 4)
#define GPIO_RG_5    ((32 * 6) + 5)
#define GPIO_RG_6    ((32 * 6) + 6)
#define GPIO_RG_7    ((32 * 6) + 7)
#define GPIO_RG_8    ((32 * 6) + 8)
#define GPIO_RG_9    ((32 * 6) + 9)
#define GPIO_RG_10   ((32 * 6) + 10)
#define GPIO_RG_11   ((32 * 6) + 11)
#define GPIO_RG_12   ((32 * 6) + 12)
#define GPIO_RG_13   ((32 * 6) + 13)
#define GPIO_RG_14   ((32 * 6) + 14)
#define GPIO_RG_15   ((32 * 6) + 15)
#define GPIO_RG_16   ((32 * 6) + 16)
#define GPIO_RG_17   ((32 * 6) + 17)
#define GPIO_RG_18   ((32 * 6) + 18)
#define GPIO_RG_19   ((32 * 6) + 19)
#define GPIO_RG_20   ((32 * 6) + 20)
#define GPIO_RG_21   ((32 * 6) + 21)
#define GPIO_RG_22   ((32 * 6) + 22)
#define GPIO_RG_23   ((32 * 6) + 23)
#define GPIO_RG_24   ((32 * 6) + 24)
#define GPIO_RG_25   ((32 * 6) + 25)
#define GPIO_RG_26   ((32 * 6) + 26)
#define GPIO_RG_27   ((32 * 6) + 27)
#define GPIO_RG_28   ((32 * 6) + 28)
#define GPIO_RG_29   ((32 * 6) + 29)
#define GPIO_RG_30   ((32 * 6) + 30)
#define GPIO_RG_31   ((32 * 6) + 31)

#define GPIO_RH_0    ((32 * 7) + 0)
#define GPIO_RH_1    ((32 * 7) + 1)
#define GPIO_RH_2    ((32 * 7) + 2)
#define GPIO_RH_3    ((32 * 7) + 3)
#define GPIO_RH_4    ((32 * 7) + 4)
#define GPIO_RH_5    ((32 * 7) + 5)
#define GPIO_RH_6    ((32 * 7) + 6)
#define GPIO_RH_7    ((32 * 7) + 7)
#define GPIO_RH_8    ((32 * 7) + 8)
#define GPIO_RH_9    ((32 * 7) + 9)

#define GPIO_RI_0    ((32 * 8) + 0)
#define GPIO_RI_1    ((32 * 8) + 1)
#define GPIO_RI_2    ((32 * 8) + 2)
#define GPIO_RI_3    ((32 * 8) + 3)
#define GPIO_RI_4    ((32 * 8) + 4)
#define GPIO_RI_5    ((32 * 8) + 5)
#define GPIO_RI_6    ((32 * 8) + 6)
#define GPIO_RI_7    ((32 * 8) + 7)
#define GPIO_RI_8    ((32 * 8) + 8)
#define GPIO_RI_9    ((32 * 8) + 9)
#define GPIO_RI_10   ((32 * 8) + 10)
#define GPIO_RI_11   ((32 * 8) + 11)
#define GPIO_RI_12   ((32 * 8) + 12)
#define GPIO_RI_13   ((32 * 8) + 13)
#define GPIO_RI_14   ((32 * 8) + 14)
#define GPIO_RI_15   ((32 * 8) + 15)

/*
 * The following section defines extra GPIO available to some boards.
 * These GPIO are generally external to the processor (i.e. SPI/I2C
 * expander chips).
 *
 * Note that these defines show all possible GPIO available, however,
 * depending on the actual board configuration, some GPIO are not
 * available for use.
 */
#ifdef CONFIG_IP7500MEDIA
/*
 * U15
 */
#define IP7500MEDIA_U15_BASE	(32 * 10)
#define IP7500MEDIA_IO0		(IP7500MEDIA_U15_BASE + 0)
#define IP7500MEDIA_IO1		(IP7500MEDIA_U15_BASE + 1)
#define IP7500MEDIA_IO2		(IP7500MEDIA_U15_BASE + 2)
#define IP7500MEDIA_IO3		(IP7500MEDIA_U15_BASE + 3)
#define IP7500MEDIA_IO4		(IP7500MEDIA_U15_BASE + 4)
#define IP7500MEDIA_IO5		(IP7500MEDIA_U15_BASE + 5)
#define IP7500MEDIA_IO6		(IP7500MEDIA_U15_BASE + 6)
#define IP7500MEDIA_IO7		(IP7500MEDIA_U15_BASE + 7)

/*
 * U16
 */
#define IP7500MEDIA_U16_BASE	(32 * 11)
#define IP7500MEDIA_IO8		(IP7500MEDIA_U16_BASE + 0)
#define IP7500MEDIA_IO9		(IP7500MEDIA_U16_BASE + 1)
#define IP7500MEDIA_IO10	(IP7500MEDIA_U16_BASE + 2)
#define IP7500MEDIA_IO11	(IP7500MEDIA_U16_BASE + 3)
#define IP7500MEDIA_IO12	(IP7500MEDIA_U16_BASE + 4)
#define IP7500MEDIA_IO13	(IP7500MEDIA_U16_BASE + 5)
#define IP7500MEDIA_IO14	(IP7500MEDIA_U16_BASE + 6)
#define IP7500MEDIA_IO15	(IP7500MEDIA_U16_BASE + 7)

/*
 * U17
 */
#define IP7500MEDIA_U17_BASE	(32 * 12)
#define IP7500MEDIA_IO16	(IP7500MEDIA_U17_BASE + 0)
#define IP7500MEDIA_IO17	(IP7500MEDIA_U17_BASE + 1)
#define IP7500MEDIA_IO18	(IP7500MEDIA_U17_BASE + 2)
#define IP7500MEDIA_IO19	(IP7500MEDIA_U17_BASE + 3)
#define IP7500MEDIA_IO20	(IP7500MEDIA_U17_BASE + 4)
#define IP7500MEDIA_IO21	(IP7500MEDIA_U17_BASE + 5)
#define IP7500MEDIA_IO22	(IP7500MEDIA_U17_BASE + 6)
#define IP7500MEDIA_IO23	(IP7500MEDIA_U17_BASE + 7)

/*
 * U18
 */
#define IP7500MEDIA_U18_BASE	(32 * 13)
#define IP7500MEDIA_IO24	(IP7500MEDIA_U18_BASE + 0)
#define IP7500MEDIA_IO25	(IP7500MEDIA_U18_BASE + 1)
#define IP7500MEDIA_IO26	(IP7500MEDIA_U18_BASE + 2)
#define IP7500MEDIA_IO27	(IP7500MEDIA_U18_BASE + 3)
#define IP7500MEDIA_IO28	(IP7500MEDIA_U18_BASE + 4)
#define IP7500MEDIA_IO29	(IP7500MEDIA_U18_BASE + 5)
#define IP7500MEDIA_IO30	(IP7500MEDIA_U18_BASE + 6)
#define IP7500MEDIA_IO31	(IP7500MEDIA_U18_BASE + 7)
#endif

#ifdef CONFIG_IP7145DPF
/*
 * U48
 */
#define IP7145DPF_U48_BASE	(32 * 10)
#define IP7145DPF_IO0		(IP7145DPF_U48_BASE + 0)
#define IP7145DPF_IO1		(IP7145DPF_U48_BASE + 1)
#define IP7145DPF_IO2		(IP7145DPF_U48_BASE + 2)
#define IP7145DPF_IO3		(IP7145DPF_U48_BASE + 3)
#define IP7145DPF_IO4		(IP7145DPF_U48_BASE + 4)
#define IP7145DPF_IO5		(IP7145DPF_U48_BASE + 5)
#define IP7145DPF_IO6		(IP7145DPF_U48_BASE + 6)
#define IP7145DPF_IO7		(IP7145DPF_U48_BASE + 7)

/*
 * U72
 */
#define IP7145DPF_U72_BASE	(32 * 11)
#define IP7145DPF_IOB0		(IP7145DPF_U72_BASE + 0)
#define IP7145DPF_IOB1		(IP7145DPF_U72_BASE + 1)
#define IP7145DPF_IOB2		(IP7145DPF_U72_BASE + 2)
#define IP7145DPF_IOB3		(IP7145DPF_U72_BASE + 3)
#define IP7145DPF_IOB4		(IP7145DPF_U72_BASE + 4)
#define IP7145DPF_IOB5		(IP7145DPF_U72_BASE + 5)
#define IP7145DPF_IOB6		(IP7145DPF_U72_BASE + 6)
#define IP7145DPF_IOB7		(IP7145DPF_U72_BASE + 7)
#endif

#include <asm-generic/gpio.h>

/*
 * The following macros bypass gpiolib to generate direct references
 * to the port registers.  These assume, minimally, that either
 * gpio_direction_input() or gpio_direction_output() have already been
 * called to setup the pin direction and to enable the pin function to
 * be gpio.  These macros generate the hardware port address based on
 * the assumption that all ports are 32 bits wide (even though we know
 * they are not).  This is so we can efficiently turn pin numbers into
 * port addresses without a lookup.
 *
 * These operations must be done in one instruction to prevent clobbering
 * other thread's accesses to the same port.
 */
#define UBICOM32_GPIO_ENABLE(pin)				\
	do {							\
		asm volatile ("or.4 (%[port]), (%[port]), %[mask]\n\t"						\
				:										\
				: [port] "a" (&UBICOM32_IO_PORT(IO_BASE + (gpio_bank(pin) << 12))->gpio_mask),	\
				  [mask] "d" (gpio_bit(pin))							\
				: "cc", "memory"								\
		);												\
	} while (0);

#define UBICOM32_GPIO_DISABLE(pin)				\
	do {							\
		asm volatile ("and.4 (%[port]), (%[port]), %[mask]\n\t"						\
				:										\
				: [port] "a" (&UBICOM32_IO_PORT(IO_BASE + (gpio_bank(pin) << 12))->gpio_mask),	\
				  [mask] "d" (~gpio_bit(pin))							\
				: "cc", "memory"								\
		);												\
	} while (0);

#define UBICOM32_GPIO_SET_PIN_INPUT(pin)			\
	do {							\
		asm volatile ("and.4 (%[port]), (%[port]), %[mask]\n\t"						\
				:										\
				: [port] "a" (&UBICOM32_IO_PORT(IO_BASE + (gpio_bank(pin) << 12))->gpio_ctl),	\
				  [mask] "d" (~gpio_bit(pin))							\
				: "cc", "memory"								\
		);												\
	} while (0);

#define UBICOM32_GPIO_SET_PIN_OUTPUT(pin)			\
	do {							\
		asm volatile ("or.4 (%[port]), (%[port]), %[mask]\n\t"						\
				:										\
				: [port] "a" (&UBICOM32_IO_PORT(IO_BASE + (gpio_bank(pin) << 12))->gpio_ctl),	\
				  [mask] "d" (gpio_bit(pin))							\
				: "cc", "memory"								\
		);												\
	} while (0);

#define UBICOM32_GPIO_SET_PIN_TOGGLE(pin)			\
	do {							\
		asm volatile ("xor.4 (%[port]), (%[port]), %[mask]\n\t"						\
				:										\
				: [port] "a" (&UBICOM32_IO_PORT(IO_BASE + (gpio_bank(pin) << 12))->gpio_out),	\
				  [mask] "d" (gpio_bit(pin))							\
				: "cc", "memory"								\
		);												\
	} while (0);

#define UBICOM32_GPIO_SET_PIN_HIGH(pin)				\
	do {							\
		asm volatile ("or.4 (%[port]), (%[port]), %[mask]\n\t"						\
				:										\
				: [port] "a" (&UBICOM32_IO_PORT(IO_BASE + (gpio_bank(pin) << 12))->gpio_out),	\
				  [mask] "d" (gpio_bit(pin))							\
				: "cc", "memory"								\
		);												\
	} while (0);

#define UBICOM32_GPIO_SET_PIN_LOW(pin)				\
	do {							\
		asm volatile ("and.4 (%[port]), (%[port]), %[mask]\n\t"						\
				:										\
				: [port] "a" (&UBICOM32_IO_PORT(IO_BASE + (gpio_bank(pin) << 12))->gpio_out),	\
				  [mask] "d" (~gpio_bit(pin))							\
				: "cc", "memory"								\
		);												\
	} while (0);

#define UBICOM32_GPIO_SET_PIN(pin, val) \
  if ( val ) {                          \
    UBICOM32_GPIO_SET_PIN_HIGH(pin);    \
  } else {                              \
    UBICOM32_GPIO_SET_PIN_LOW(pin);	\
  }

#define UBICOM32_GPIO_GET_PIN(pin)                                    \
  (0 != (UBICOM32_IO_PORT(IO_BASE + (gpio_bank(pin) << 12))->gpio_in  \
	 & gpio_bit(pin)))


static inline int gpio_get_value(unsigned gpio)
{
  if (gpio <= MAX_UBICOM_ONCHIP_GPIO)
    return UBICOM32_GPIO_GET_PIN(gpio);
  else
    return __gpio_get_value(gpio);
}

static inline void gpio_set_value(unsigned gpio, int value)
{
  if (gpio <= MAX_UBICOM_ONCHIP_GPIO)
    {
      UBICOM32_GPIO_SET_PIN(gpio, value);
    }
  else
    {
      __gpio_set_value(gpio, value);
    }
}

static inline int gpio_cansleep(unsigned gpio)
{
  return __gpio_cansleep(gpio);
}

static inline int gpio_to_irq(unsigned gpio)
{
#if defined(IP5000) || defined(IP5000_REV2)
  if ((gpio >= GPIO_RA_4) && (gpio <= GPIO_RA_6))
    return 25;
  else
    return -ENXIO;

#elif defined(IP7000) || defined(IP7000_REV2)
  if ((gpio >= GPIO_RA_4) && (gpio <= GPIO_RA_6))
    return 44 + (gpio - GPIO_RA_4);
  else
    return -ENXIO;

#else
    return -ENXIO;

#endif
}

static inline int irq_to_gpio(unsigned gpio)
{
	return -ENXIO;
}

extern struct ubicom32_io_port *ubi_gpio_get_port(unsigned gpio);

extern int __init ubi_gpio_init(void);

#endif /* _ASM_UBICOM32_GPIO_H */
