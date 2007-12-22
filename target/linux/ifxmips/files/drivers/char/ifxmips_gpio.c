/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2005 infineon
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_ioctl.h>

#define MAX_PORTS			2
#define PINS_PER_PORT		16

static unsigned int ifxmips_gpio_major = 0;

/* TODO do we need this ? */
static struct semaphore port_sem;

/* TODO do we really need this ? return in a define is forbidden by coding style */
#define IFXMIPS_GPIO_SANITY		{if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL; }

int
ifxmips_port_reserve_pin (unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	printk("%s : call to obseleted function\n", __func__);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_reserve_pin);

int
ifxmips_port_free_pin (unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	printk("%s : call to obseleted function\n", __func__);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_free_pin);

int
ifxmips_port_set_open_drain (unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_OD + (port * 0x30)) | (1 << pin), IFXMIPS_GPIO_P0_OD);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_open_drain);

int
ifxmips_port_clear_open_drain (unsigned int port, unsigned int pin)
{
	IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_OD + (port * 0x30)) & ~(1 << pin), IFXMIPS_GPIO_P0_OD);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_open_drain);

int
ifxmips_port_set_pudsel (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_PUDSEL + (port * 0x30)) | (1 << pin), IFXMIPS_GPIO_P0_PUDSEL);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_pudsel);

int
ifxmips_port_clear_pudsel (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_PUDSEL + (port * 0x30)) & ~(1 << pin), IFXMIPS_GPIO_P0_PUDSEL);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_pudsel);

int
ifxmips_port_set_puden (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_PUDEN + (port * 0x30)) | (1 << pin), IFXMIPS_GPIO_P0_PUDEN);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_puden);

int
ifxmips_port_clear_puden (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_PUDEN + (port * 0x30)) & ~(1 << pin), IFXMIPS_GPIO_P0_PUDEN);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_puden);

int
ifxmips_port_set_stoff (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_STOFF + (port * 0x30)) | (1 << pin), IFXMIPS_GPIO_P0_STOFF);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_stoff);

int
ifxmips_port_clear_stoff (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_STOFF + (port * 0x30)) & ~(1 << pin), IFXMIPS_GPIO_P0_STOFF);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_stoff);

int
ifxmips_port_set_dir_out (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_DIR + (port * 0x30)) | (1 << pin), IFXMIPS_GPIO_P0_DIR);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_dir_out);

int
ifxmips_port_set_dir_in (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_DIR + (port * 0x30)) & ~(1 << pin), IFXMIPS_GPIO_P0_DIR);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_dir_in);

int
ifxmips_port_set_output (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_OUT + (port * 0x30)) | (1 << pin), IFXMIPS_GPIO_P0_OUT);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_output);

int
ifxmips_port_clear_output (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_OUT + (port * 0x30)) & ~(1 << pin), IFXMIPS_GPIO_P0_OUT);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_output);

int
ifxmips_port_get_input (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;

	if (readl(IFXMIPS_GPIO_P0_IN + (port * 0x30)) & (1 << pin))
		return 0;
	else
		return 1;
}
EXPORT_SYMBOL(ifxmips_port_get_input);

int
ifxmips_port_set_altsel0 (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_ALTSEL0 + (port * 0x30)) | (1 << pin), IFXMIPS_GPIO_P0_ALTSEL0);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_altsel0);

int
ifxmips_port_clear_altsel0 (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_ALTSEL0 + (port * 0x30)) & ~(1 << pin), IFXMIPS_GPIO_P0_ALTSEL0);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_altsel0);

int
ifxmips_port_set_altsel1 (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_ALTSEL1 + (port * 0x30)) | (1 << pin), IFXMIPS_GPIO_P0_ALTSEL1);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_set_altsel1);

int
ifxmips_port_clear_altsel1 (unsigned int port, unsigned int pin)
{
    IFXMIPS_GPIO_SANITY;
	writel(readl(IFXMIPS_GPIO_P0_ALTSEL1 + (port * 0x30)) & ~(1 << pin), IFXMIPS_GPIO_P0_ALTSEL1);

	return 0;
}
EXPORT_SYMBOL(ifxmips_port_clear_altsel1);

long ifxmips_port_read_procmem_helper(char* tag, u32* in_reg, char *buf)
{
	u32 reg, bit = 0;
	unsigned int len, t;

	len = sprintf(buf, "\n%s: ", tag);
	reg = readl(in_reg);
	bit = 0x80000000;
	for (t = 0; t < 32; t++) {
		if ((reg & bit) > 0)
			len = len + sprintf(buf + len, "X");
		else
			len = len + sprintf(buf + len, " ");
		bit = bit >> 1;
	}

	return len;
}

int
ifxmips_port_read_procmem (char *buf, char **start, off_t offset, int count,
			  int *eof, void *data)
{
	long len = sprintf (buf, "\nIFXMips Port Settings\n");

	len += sprintf (buf + len,
			"         3         2         1         0\n");
	len += sprintf (buf + len,
			"        10987654321098765432109876543210\n");
	len += sprintf (buf + len,
			"----------------------------------------\n");

	len += ifxmips_port_read_procmem_helper("P0-OUT", IFXMIPS_GPIO_P0_OUT, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-OUT", IFXMIPS_GPIO_P1_OUT, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P0-IN ", IFXMIPS_GPIO_P0_IN, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-IN ", IFXMIPS_GPIO_P1_IN, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P0-DIR", IFXMIPS_GPIO_P0_DIR, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-DIR", IFXMIPS_GPIO_P1_DIR, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P0-STO ", IFXMIPS_GPIO_P0_STOFF, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-STO ", IFXMIPS_GPIO_P1_STOFF, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P0-PUDE", IFXMIPS_GPIO_P0_PUDEN, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-PUDE", IFXMIPS_GPIO_P1_PUDEN, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P0-OD  ", IFXMIPS_GPIO_P0_OD, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-OD  ", IFXMIPS_GPIO_P1_OD, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P0-PUDS", IFXMIPS_GPIO_P0_PUDSEL, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-PUDS", IFXMIPS_GPIO_P1_PUDSEL, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P0-ALT0", IFXMIPS_GPIO_P0_ALTSEL0, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-ALT0", IFXMIPS_GPIO_P1_ALTSEL0, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P0-ALT1", IFXMIPS_GPIO_P0_ALTSEL1, &buf[len]);
	len += ifxmips_port_read_procmem_helper("P1-ALT1", IFXMIPS_GPIO_P1_ALTSEL1, &buf[len]);
	len = len + sprintf (buf + len, "\n\n");

	*eof = 1;

	return len;
}

static int
ifxmips_port_open (struct inode *inode, struct file *filep)
{
	return 0;
}

static int
ifxmips_port_release (struct inode *inode, struct file *filelp)
{
	return 0;
}

static int
ifxmips_port_ioctl (struct inode *inode, struct file *filp,
			unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	volatile struct ifxmips_port_ioctl_parm parm;

	if (_IOC_TYPE (cmd) != IFXMIPS_PORT_IOC_MAGIC)
		return -EINVAL;

	if (_IOC_DIR (cmd) & _IOC_WRITE) {
		if (!access_ok
		    (VERIFY_READ, arg,
		     sizeof (struct ifxmips_port_ioctl_parm)))
			return -EFAULT;
		ret = copy_from_user ((void *) &parm, (void *) arg,
				      sizeof (struct ifxmips_port_ioctl_parm));
	}
	if (_IOC_DIR (cmd) & _IOC_READ) {
		if (!access_ok
		    (VERIFY_WRITE, arg,
		     sizeof (struct ifxmips_port_ioctl_parm)))
			return -EFAULT;
	}

	if (down_trylock (&port_sem) != 0)
		return -EBUSY;

	switch (cmd) {
	case IFXMIPS_PORT_IOCOD:
		if (parm.value == 0x00)
			ifxmips_port_clear_open_drain(parm.port, parm.pin);
		else
			ifxmips_port_set_open_drain(parm.port, parm.pin);
		break;

	case IFXMIPS_PORT_IOCPUDSEL:
		if (parm.value == 0x00)
			ifxmips_port_clear_pudsel(parm.port, parm.pin);
		else
			ifxmips_port_set_pudsel(parm.port, parm.pin);
		break;

	case IFXMIPS_PORT_IOCPUDEN:
		if (parm.value == 0x00)
			ifxmips_port_clear_puden(parm.port, parm.pin);
		else
			ifxmips_port_set_puden(parm.port, parm.pin);
		break;

	case IFXMIPS_PORT_IOCSTOFF:
		if (parm.value == 0x00)
			ifxmips_port_clear_stoff(parm.port, parm.pin);
		else
			ifxmips_port_set_stoff(parm.port, parm.pin);
		break;

	case IFXMIPS_PORT_IOCDIR:
		if (parm.value == 0x00)
			ifxmips_port_set_dir_in(parm.port, parm.pin);
		else
			ifxmips_port_set_dir_out(parm.port, parm.pin);
		break;

	case IFXMIPS_PORT_IOCOUTPUT:
		if (parm.value == 0x00)
			ifxmips_port_clear_output(parm.port, parm.pin);
		else
			ifxmips_port_set_output(parm.port, parm.pin);
		break;

	case IFXMIPS_PORT_IOCALTSEL0:
		if (parm.value == 0x00)
			ifxmips_port_clear_altsel0(parm.port, parm.pin);
		else
			ifxmips_port_set_altsel0(parm.port, parm.pin);
		break;

	case IFXMIPS_PORT_IOCALTSEL1:
		if (parm.value == 0x00)
			ifxmips_port_clear_altsel1(parm.port, parm.pin);
		else
			ifxmips_port_set_altsel1(parm.port, parm.pin);
		break;

	case IFXMIPS_PORT_IOCINPUT:
		parm.value = ifxmips_port_get_input(parm.port, parm.pin);
		copy_to_user((void*)arg, (void*)&parm,
			sizeof(struct ifxmips_port_ioctl_parm));
		break;

	default:
		ret = -EINVAL;
	}

	up (&port_sem);

	return ret;
}

static struct file_operations port_fops = {
      .open = ifxmips_port_open,
      .release = ifxmips_port_release,
      .ioctl = ifxmips_port_ioctl
};

int __init
ifxmips_gpio_init (void)
{
	int retval = 0;

	sema_init (&port_sem, 1);

	ifxmips_gpio_major = register_chrdev(0, "ifxmips_gpio", &port_fops);
	if (!ifxmips_gpio_major)
	{
		printk("ifxmips-port: Error! Could not register port device. #%d\n", ifxmips_gpio_major);
		retval = -EINVAL;
		goto out;
	}

	create_proc_read_entry("ifxmips_gpio", 0, NULL,
				ifxmips_port_read_procmem, NULL);

	printk("registered ifxmips gpio driver\n");

out:
	return retval;
}

void __exit
ifxmips_gpio_exit (void)
{
	unregister_chrdev(ifxmips_gpio_major, "ifxmips_gpio");
	remove_proc_entry("ifxmips_gpio", NULL);
}

module_init(ifxmips_gpio_init);
module_exit(ifxmips_gpio_exit);
