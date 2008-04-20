/*
 *  Miscellaneous functions for IDT EB434 board
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *  Copyright 2006 Phil Sutter <n0-1@freewrt.org>
 *  Copyright 2007 Florian Fainelli <florian@openwrt.org>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/platform_device.h>

#include <asm/addrspace.h>
#include <asm/gpio.h>

#include <asm/rc32434/rb.h>

#define GPIO_BADDR  0x18050000

static volatile unsigned char *devCtl3Base;
static unsigned char latchU5State;
static spinlock_t clu5Lock = SPIN_LOCK_UNLOCKED;

struct rb500_gpio_reg __iomem *rb500_gpio_reg0;
EXPORT_SYMBOL(rb500_gpio_reg0);

static struct resource rb500_gpio_reg0_res[] = {
	{
		.name 	= "gpio_reg0",
		.start 	= GPIO_BADDR,
		.end 	= GPIO_BADDR + sizeof(struct rb500_gpio_reg),
		.flags 	= IORESOURCE_MEM,
	}
};

void set434Reg(unsigned regOffs, unsigned bit, unsigned len, unsigned val)
{
	unsigned flags, data;
	unsigned i = 0;

	spin_lock_irqsave(&clu5Lock, flags);
	data = *(volatile unsigned *) (IDT434_REG_BASE + regOffs);
	for (i = 0; i != len; ++i) {
		if (val & (1 << i))
			data |= (1 << (i + bit));
		else
			data &= ~(1 << (i + bit));
	}
	*(volatile unsigned *) (IDT434_REG_BASE + regOffs) = data;
	spin_unlock_irqrestore(&clu5Lock, flags);
}
EXPORT_SYMBOL(set434Reg);

void changeLatchU5(unsigned char orMask, unsigned char nandMask)
{
	unsigned flags;

	spin_lock_irqsave(&clu5Lock, flags);
	latchU5State = (latchU5State | orMask) & ~nandMask;
	if (!devCtl3Base)
		devCtl3Base = (volatile unsigned char *)
		    KSEG1ADDR(*(volatile unsigned *)
			      KSEG1ADDR(0x18010030));
	*devCtl3Base = latchU5State;
	spin_unlock_irqrestore(&clu5Lock, flags);
}
EXPORT_SYMBOL(changeLatchU5);

unsigned char getLatchU5State(void)
{
	return latchU5State;
}
EXPORT_SYMBOL(getLatchU5State);

int rb500_gpio_get_value(unsigned gpio)
{
	return readl(&rb500_gpio_reg0->gpiod) & (1 << gpio);
}
EXPORT_SYMBOL(rb500_gpio_get_value);

void rb500_gpio_set_value(unsigned gpio, int value)
{
	unsigned tmp;

	tmp = readl(&rb500_gpio_reg0->gpiod) & ~(1 << gpio);
	if (value)
		tmp |= 1 << gpio;

	writel(tmp, (void *)&rb500_gpio_reg0->gpiod);
}
EXPORT_SYMBOL(rb500_gpio_set_value);

int rb500_gpio_direction_input(unsigned gpio)
{
	writel(readl(&rb500_gpio_reg0->gpiocfg) & ~(1 << gpio), (void *)&rb500_gpio_reg0->gpiocfg);

	return 0;
}
EXPORT_SYMBOL(rb500_gpio_direction_input);

int rb500_gpio_direction_output(unsigned gpio, int value)
{
	gpio_set_value(gpio, value);
	writel(readl(&rb500_gpio_reg0->gpiocfg) | (1 << gpio), (void *)&rb500_gpio_reg0->gpiocfg);

	return 0;
}
EXPORT_SYMBOL(rb500_gpio_direction_output);

void rb500_gpio_set_int_level(unsigned gpio, int value)
{
	unsigned tmp;

	tmp = readl(&rb500_gpio_reg0->gpioilevel) & ~(1 << gpio);
	if (value)
		tmp |= 1 << gpio;
	writel(tmp, (void *)&rb500_gpio_reg0->gpioilevel);
}
EXPORT_SYMBOL(rb500_gpio_set_int_level);

int rb500_gpio_get_int_level(unsigned gpio)
{
	return readl(&rb500_gpio_reg0->gpioilevel) & (1 << gpio);
}
EXPORT_SYMBOL(rb500_gpio_get_int_level);

void rb500_gpio_set_int_status(unsigned gpio, int value)
{
	unsigned tmp;

	tmp = readl(&rb500_gpio_reg0->gpioistat);
	if (value)
		tmp |= 1 << gpio;
	writel(tmp, (void *)&rb500_gpio_reg0->gpioistat);
}
EXPORT_SYMBOL(rb500_gpio_set_int_status);

int rb500_gpio_get_int_status(unsigned gpio)
{
	return readl(&rb500_gpio_reg0->gpioistat) & (1 << gpio);
}
EXPORT_SYMBOL(rb500_gpio_get_int_status);

void rb500_gpio_set_func(unsigned gpio, int value)
{
        unsigned tmp;

        tmp = readl(&rb500_gpio_reg0->gpiofunc);
        if (value)
                tmp |= 1 << gpio;
        writel(tmp, (void *)&rb500_gpio_reg0->gpiofunc);
}
EXPORT_SYMBOL(rb500_gpio_set_func);

int rb500_gpio_get_func(unsigned gpio)
{
        return readl(&rb500_gpio_reg0->gpiofunc) & (1 << gpio);
}
EXPORT_SYMBOL(rb500_gpio_get_func);

int __init rb500_gpio_init(void)
{
	rb500_gpio_reg0 = ioremap_nocache(rb500_gpio_reg0_res[0].start,
				rb500_gpio_reg0_res[0].end -
				rb500_gpio_reg0_res[0].start);

	if (!rb500_gpio_reg0) {
		printk(KERN_ERR "rb500: cannot remap GPIO register 0\n");
		return -ENXIO;
	}

	return 0;
}
arch_initcall(rb500_gpio_init);
