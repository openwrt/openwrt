/*
 * arch/ubicom32/mach-common/ubi32-gpio.c
 *   Ubicom gpio driver
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/version.h>

#if defined(CONFIG_PROC_FS)
#include <linux/proc_fs.h>
#endif

#include <linux/io.h>
#include <asm/ip5000.h>
#include <linux/gpio.h>

#define UBI_GPIO_CHECK_RANGE     0  /* !0 enables range checking */


/*
 * Each I/O port can be configured to operate in one of several
 * functional modes. One of these modes is GPIO, which causes the
 * entire port to function as a GPIO port.  Since the various port
 * registers serve the system with other important functions, such as
 * ethernet, serial, USB, etc., it isn't advantageous to set any of
 * the ports to be entirely dedicated for GPIO use.  The processor
 * alternatively allows individual bits of a port to be assigned to be
 * used as GPIO independently from the overall port function.  This
 * bit-by-bit assignment is selected by setting the corresponding bit
 * in the port's gpio_mask register.  When set, the selected bit is
 * then enabled as a GPIO.  If the corresponding bit is set in the
 * gpio_ctl register of the port, the bit is configured as a GPIO
 * output.  Otherwise, it is an input.
 *
 * NOTE: This driver uses the bit-by-bit GPIO function assignment
 * exclusively and *never* sets the port function registers to the
 * GPIO function.
 *
 * GPIO is not the main function of any of the I/O ports.  The port
 * bit widths are variable from one port to the next, determined by
 * the more common I/O functions of the ports.  For simplicity, this
 * driver assumes all the ports are 32 bits wide regardless of the
 * real bit width of the port.  GPIO bits are numbered from zero to
 * MAX_UBICOM_GPIOS.  Within a port, the least significant bit is
 * numbered bit zero, the most significant is bit 31.  Since the ports
 * are considered logically contiguous, GPIO #32 is the zeroth bit in
 * port #1, and so on.  Due to the hardware definition, there are
 * large gaps in the GPIO numbers representing real pins.
 *
 * NOTE: It is up to the programmer to refer to the processor data
 * sheet to determine which bits in which ports can be accessed and
 * used for GPIO.
 *
 */


/* There are 9 ports, A through I. Not all 32 bits in each
 * port can be a GPIO, but we pretend they are.  Its up to the
 * programmer to refer to the processor data sheet.
 */
#define MAX_UBICOM_GPIOS   (9 * 32) /* ARCH_NR_GPIOS */
#define NUM_GPIO_PORTS     (gpio_bank(MAX_UBICOM_GPIOS))


/* GPIO reservation bit map array */
static int reserved_gpio_map[NUM_GPIO_PORTS];


/* Array of hardware io_port addresses */
static struct ubicom32_io_port *gpio_bank_addr[NUM_GPIO_PORTS] =
{
	UBICOM32_IO_PORT(RA),
	UBICOM32_IO_PORT(RB),
	UBICOM32_IO_PORT(RC),
	UBICOM32_IO_PORT(RD),
	UBICOM32_IO_PORT(RE),
	UBICOM32_IO_PORT(RF),
	UBICOM32_IO_PORT(RG),
	UBICOM32_IO_PORT(RH),
	UBICOM32_IO_PORT(RI)
};


struct ubi_gpio_chip {
	/*
	 * Right now, nothing else lives here.
	 */
	struct gpio_chip gpio_chip;
};


#if UBI_GPIO_CHECK_RANGE
inline int check_gpio(unsigned gpio)
{
	if (gpio >= MAX_UBICOM_GPIOS)
		return -EINVAL;
	return 0;
}
#else
#define check_gpio(n)   (0)
#endif

/*
 * ubi_gpio_get_port
 *	Get the IO port associated with a certain gpio
 */
struct ubicom32_io_port *ubi_gpio_get_port(unsigned gpio)
{
	if (gpio_bank(gpio) > NUM_GPIO_PORTS) {
		return NULL;
	}
	return gpio_bank_addr[gpio_bank(gpio)];
}

/*
 * ubi_gpio_error()
 */
static void ubi_gpio_error(unsigned gpio)
{
	printk(KERN_ERR "ubicom-gpio: GPIO %d wasn't requested!\n", gpio);
}

/*
 * ubi_port_setup()
 */
static void ubi_port_setup(unsigned gpio, unsigned short usage)
{
	if (!check_gpio(gpio)) {
		if (usage) {
			UBICOM32_GPIO_ENABLE(gpio);
		} else {
			UBICOM32_GPIO_DISABLE(gpio);
		}
	}
}

/*
 * ubi_gpio_request()
 */
static int ubi_gpio_request(struct gpio_chip *chip, unsigned gpio)
{
	unsigned long flags;

	if (check_gpio(gpio) < 0)
		return -EINVAL;

	local_irq_save(flags);

	if (unlikely(reserved_gpio_map[gpio_bank(gpio)] & gpio_bit(gpio))) {
		printk(KERN_ERR "ubi-gpio: GPIO %d is already reserved!\n",
		       gpio);
		local_irq_restore(flags);
		return -EBUSY;
	}

	reserved_gpio_map[gpio_bank(gpio)] |= gpio_bit(gpio);

	ubi_port_setup(gpio, 1);

	local_irq_restore(flags);

	return 0;
}

/*
 * ubi_gpio_free()
 */
static void ubi_gpio_free(struct gpio_chip *chip, unsigned gpio)
{
	unsigned long flags;

	if (check_gpio(gpio) < 0)
		return;

	local_irq_save(flags);

	if (unlikely(!(reserved_gpio_map[gpio_bank(gpio)] & gpio_bit(gpio)))) {
		ubi_gpio_error(gpio);
		local_irq_restore(flags);
		return;
	}

	/* Assert the pin is no longer claimed */
	reserved_gpio_map[gpio_bank(gpio)] &= ~gpio_bit(gpio);

	/* Revert port bit to use specified by port->function */
	ubi_port_setup(gpio, 0);

	local_irq_restore(flags);
}

/*
 * ubi_gpio_direction_input()
 */
static int ubi_gpio_direction_input(struct gpio_chip *chip, unsigned gpio)
{
	unsigned long flags;

	if (!(reserved_gpio_map[gpio_bank(gpio)] & gpio_bit(gpio))) {
		ubi_gpio_error(gpio);
		return -EINVAL;
	}

	local_irq_save(flags);

	/* Configure pin as gpio */
	ubi_port_setup(gpio, 1);

	/* Assert pin is an input */
	UBICOM32_GPIO_SET_PIN_INPUT(gpio);

	local_irq_restore(flags);

	return 0;
}


/*
 * ubi_gpio_direction_output()
 */
static int ubi_gpio_direction_output(struct gpio_chip *chip,
				     unsigned gpio, int value)
{
	unsigned long flags;

	if (!(reserved_gpio_map[gpio_bank(gpio)] & gpio_bit(gpio))) {
		ubi_gpio_error(gpio);
		return -EINVAL;
	}

	local_irq_save(flags);

	/* Configure pin as gpio and set initial value in gpio_out register
	 * so that when we enable it as an output, it will have the correct
	 * initial value.
	 */
	ubi_port_setup(gpio, 1);
	if (value) {
		UBICOM32_GPIO_SET_PIN_HIGH(gpio);
	} else {
		UBICOM32_GPIO_SET_PIN_LOW(gpio);
	}

	/* Enable the pin as an output */
	UBICOM32_GPIO_SET_PIN_OUTPUT(gpio);

	local_irq_restore(flags);

	return 0;
}


/*
 * ubi_gpio_get_value()
 */
static int ubi_gpio_get_value(struct gpio_chip *chip, unsigned gpio)
{
	return 0 != (gpio_bank_addr[gpio_bank(gpio)]->gpio_in & gpio_bit(gpio));
}


/*
 * ubi_gpio_set_value()
 */
static void ubi_gpio_set_value(struct gpio_chip *chip, unsigned gpio,
			       int arg)
{
	unsigned long flags;
	local_irq_save(flags);

	if (arg) {
		UBICOM32_GPIO_SET_PIN_HIGH(gpio);
	} else {
		UBICOM32_GPIO_SET_PIN_LOW(gpio);
	}

	local_irq_restore(flags);
}


/*
 * ubi_gpio_to_irq()
 */
static int ubi_gpio_to_irq(struct gpio_chip *chip, unsigned gpio)
{
	return gpio_to_irq(gpio);
}


/*
 * ubi_gpio_init()
 */
int __init ubi_gpio_init(void)
{
	int k;
	int status;
	struct ubi_gpio_chip *chip;
	struct gpio_chip *gc;

	printk(KERN_INFO "Ubicom GPIO Controller\n");

	chip = kzalloc(sizeof(struct ubi_gpio_chip), GFP_KERNEL);
	if (chip == NULL)
		return -ENOMEM;

	gc = &chip->gpio_chip;
	gc->request          = ubi_gpio_request;
	gc->free             = ubi_gpio_free;
	gc->to_irq           = ubi_gpio_to_irq;
	gc->direction_input  = ubi_gpio_direction_input;
	gc->direction_output = ubi_gpio_direction_output;
	gc->get              = ubi_gpio_get_value;
	gc->set              = ubi_gpio_set_value;
	gc->can_sleep        = 0;
	gc->base             = 0;
	gc->ngpio            = MAX_UBICOM_GPIOS; /* ARCH_NR_GPIOS - 1 */
	gc->label            = "ubi_gpio";

	status = gpiochip_add(gc);
	if (status != 0) {
		kfree(chip);
		return status;
	}

	/* Assert all pins are free */
	for (k = 0; k < NUM_GPIO_PORTS; k++) {
		reserved_gpio_map[k] = 0;
	}

	return 0;
}

#if defined(CONFIG_PROC_FS)
/*
 * ubi_get_gpio_dir()
 */
static int ubi_get_gpio_dir(unsigned gpio)
{
	if (gpio_bank_addr[gpio_bank(gpio)]->gpio_ctl & gpio_bit(gpio))
		return 1;
	else
		return 0;
}

/*
 * gpio_proc_read()
 */
static int ubi_gpio_proc_read(char *buf, char **start, off_t offset,
			  int len, int *unused_i, void *unused_v)
{
	int c, outlen = 0;

	for (c = 0; c < MAX_UBICOM_GPIOS; c++) {
		if (!check_gpio(c) &&
		    (reserved_gpio_map[gpio_bank(c)] & gpio_bit(c))) {
			len = sprintf(buf, "GPIO_%d:\t\tGPIO %s\n", c,
				      ubi_get_gpio_dir(c) ? "OUTPUT" : "INPUT");
		} else {
			continue;
		}

		buf += len;
		outlen += len;
	}
	return outlen;
}

/*
 * ubi_gpio_register_proc()
 */
static __init int ubi_gpio_register_proc(void)
{
	struct proc_dir_entry *proc_gpio;

	proc_gpio = create_proc_entry("gpio", S_IRUGO, NULL);
	if (proc_gpio)
		proc_gpio->read_proc = ubi_gpio_proc_read;

	return proc_gpio != NULL;
}
device_initcall(ubi_gpio_register_proc);
#endif
