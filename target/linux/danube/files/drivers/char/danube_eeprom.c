/******************************************************************************
**
** FILE NAME    : danube_eeprom.c
** PROJECT      : Danube
** MODULES      : EEPROM
**
** DATE         : 31 DEC 2004
** AUTHOR       : Liu Peng
** DESCRIPTION  : X25040 EEPROM Driver
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 31 DEC 2004  Liu Peng        Initiate Version
** 25 OCT 2006  Xu Liang        Add GPL header.
*******************************************************************************/

#define IFAP_EEPROM_DRV_VERSION "0.0.1"

/*
 **************************************************
 *
 * This driver was originally based on the INCA-IP driver, but due to
 * fundamental conceptual drawbacks there has been changed a lot.
 *
 * Based on INCA-IP driver Copyright (c) 2003 Gary Jennejohn <gj@denx.de>
 * Based on the VxWorks drivers Copyright (c) 2002, Infineon Technologies.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif
#include <linux/config.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include <asm/danube/danube.h>
#include <asm/danube/irq.h>
#include <asm/danube/ifx_ssc_defines.h>
#include <asm/danube/ifx_ssc.h>
#if CONFIG_MODVERSIONS==1
# include <linux/modversions.h>
#endif

#define AMAZON_EEPROM_EMSG(fmt,args...) printk("%s:" fmt, __FUNCTION__, ##args)

#if 0
#define AMAZON_EEPROM_DMSG(fmt,args...) printk("%s:" fmt, __FUNCTION__, ##args)
#else
#define AMAZON_EEPROM_DMSG(fmt,args...)
#endif

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Peng Liu");
MODULE_DESCRIPTION ("IFAP EEPROM driver");
MODULE_SUPPORTED_DEVICE ("danube_eeprom");
MODULE_PARM (maj, "i");
MODULE_PARM_DESC (maj, "Major device number");

/* allow the user to set the major device number */
static int maj = 0;

static ssize_t danube_eeprom_read (struct file *, char *, size_t, loff_t *);
static ssize_t danube_eeprom_write (struct file *, const char *, size_t,
				    loff_t *);
static int danube_eeprom_ioctl (struct inode *, struct file *, unsigned int,
				unsigned long);
static int danube_eeprom_open (struct inode *, struct file *);
static int danube_eeprom_close (struct inode *, struct file *);

//ifx_ssc.c
extern int ifx_ssc_init (void);
extern int ifx_ssc_open (struct inode *inode, struct file *filp);
extern int ifx_ssc_close (struct inode *inode, struct file *filp);
extern void ifx_ssc_cleanup_module (void);
extern int ifx_ssc_ioctl (struct inode *inode, struct file *filp,
			  unsigned int cmd, unsigned long data);
extern ssize_t ifx_ssc_kwrite (int port, const char *kbuf, size_t len);
extern ssize_t ifx_ssc_kread (int port, char *kbuf, size_t len);

extern int ifx_ssc_cs_low (u32 pin);
extern int ifx_ssc_cs_high (u32 pin);
extern int ifx_ssc_txrx (char *tx_buf, u32 tx_len, char *rx_buf, u32 rx_len);
extern int ifx_ssc_tx (char *tx_buf, u32 tx_len);
extern int ifx_ssc_rx (char *rx_buf, u32 rx_len);

/* CS: board dependant */
#define EEPROM_CS IFX_SSC_WHBGPOSTAT_OUT0_POS

/* commands for EEPROM, x25160, x25140 */
#define EEPROM_WREN			((u8)0x06)
#define EEPROM_WRDI			((u8)0x04)
#define EEPROM_RDSR			((u8)0x05)
#define EEPROM_WRSR			((u8)0x01)
#define EEPROM_READ			((u8)0x03)
#define EEPROM_WRITE			((u8)0x02)
#define EEPROM_PAGE_SIZE		4
#define EEPROM_SIZE			512

/* Brief:	EEPROM_RDSR command
 * Return:	0      OK
 *		error code
 */
static int
eeprom_rdsr (char *status)
{
	int ret = 0;
	unsigned char cmd = EEPROM_RDSR;

	unsigned long flag;
	local_irq_save (flag);
	if ((ret = ifx_ssc_cs_low (EEPROM_CS))) {
		local_irq_restore (flag);
		return ret;
	}
	if ((ret = ifx_ssc_txrx (&cmd, 1, status, 1)) < 0) {
		ifx_ssc_cs_high (EEPROM_CS);
		local_irq_restore (flag);
		return ret;
	}

	if ((ret = ifx_ssc_cs_high (EEPROM_CS))) {
		local_irq_restore (flag);
		return ret;
	}
	local_irq_restore (flag);
	return ret;
}

/* Brief: wait for WIP is over
 * Return:
 *	0	WIP is over
 *	<0	Error code
 */
static inline int
eeprom_wip_over (void)
{
	int ret = 0;
	u8 status;
	while (1) {
		ret = eeprom_rdsr (&status);
		AMAZON_EEPROM_DMSG ("status %x \n", status);
		if (ret) {
			AMAZON_EEPROM_EMSG ("read back status fails %d\n",
					    ret);
			break;
		}
		else if (((status) & 1) != 0) {
			AMAZON_EEPROM_DMSG ("read back status not zero %x\n",
					    status);
		}
		else {
			break;
		}
	}
	return ret;
}

/* Brief:	EEPROM_WREN command
 * Return:	0      OK
 *		error code
 */
static int
eeprom_wren (void)
{
	unsigned char cmd = EEPROM_WREN;
	int ret = 0;

	unsigned long flag;
	local_irq_save (flag);
	if ((ret = ifx_ssc_cs_low (EEPROM_CS))) {
		local_irq_restore (flag);
		return ret;
	}

	if ((ret = ifx_ssc_tx (&cmd, 1)) < 0) {
		ifx_ssc_cs_high (EEPROM_CS);
		local_irq_restore (flag);
		return ret;
	}

	if ((ret = ifx_ssc_cs_high (EEPROM_CS))) {
		local_irq_restore (flag);
		return ret;
	}
	local_irq_restore (flag);
	eeprom_wip_over ();
	return ret;

}

/* Brief:	EEPROM_WRSR command
 * Return:	0      OK
 *		error code
 */
static int
eeprom_wrsr (void)
{
	int ret = 0;
	unsigned char cmd[2];
	cmd[0] = EEPROM_WRSR;
	cmd[1] = 0;
	if ((ret = eeprom_wren ())) {
		AMAZON_EEPROM_EMSG ("eeprom_wren fails\n");
		return ret;
	}
	unsigned long flag;
	local_irq_save (flag);
	if ((ret = ifx_ssc_cs_low (EEPROM_CS))) {
		goto eeprom_wrsr_err_out;
	}

	if ((ret = ifx_ssc_tx (cmd, 2)) < 0) {
		ifx_ssc_cs_high (EEPROM_CS);
		goto eeprom_wrsr_err_out;
	}

	if ((ret = ifx_ssc_cs_low (EEPROM_CS))) {
		goto eeprom_wrsr_err_out;
	}
	local_irq_restore (flag);
	eeprom_wip_over ();
	return ret;
      eeprom_wrsr_err_out:
	local_irq_restore (flag);
	eeprom_wip_over ();
	return ret;
}

/* Brief:	read EEPROM
 * Parameter:
 *	addr
 *	len
 *	buf
 * Return:	 ok
 *		-EFAULT
 */
static int
eeprom_read (u32 addr, unsigned char *buf, u32 len)
{
	int ret = 0;
	unsigned char write_buf[2];
	u32 eff = 0;

	unsigned long flag;

	while (1) {
		eeprom_wip_over ();
		eff = EEPROM_PAGE_SIZE - (addr % EEPROM_PAGE_SIZE);
		eff = (eff < len) ? eff : len;
		//Format:
		//CMD + ADDR
		local_irq_save (flag);
		if ((ret = ifx_ssc_cs_low (EEPROM_CS)) < 0) {
			goto eeprom_read_err_out;
		}

		write_buf[0] = EEPROM_READ | ((u8) ((addr & 0x100) >> 5));
		write_buf[1] = (addr & 0xff);

		if ((ret = ifx_ssc_txrx (write_buf, 2, buf, eff)) != eff) {
			AMAZON_EEPROM_EMSG ("ssc_txrx fails %d\n", ret);
			ifx_ssc_cs_high (EEPROM_CS);
			goto eeprom_read_err_out;
		}
		buf += ret;
		len -= ret;
		addr += ret;
		if ((ret = ifx_ssc_cs_high (EEPROM_CS))) {
			goto eeprom_read_err_out;
		}
		local_irq_restore (flag);
		if (len <= 0) {
			break;
		}
	}

	return ret;
      eeprom_read_err_out:
	local_irq_restore (flag);
	return ret;
}

/* Brief:	 write EEPROm
 * Parameter:
 *	addr
 *	len
 *	buf
 * Return:	0 ok
 *		-EFAULT
 */
static int
eeprom_write (u32 addr, unsigned char *buf, u32 len)
{
	int ret = 0;
	u32 eff = 0;
	unsigned char write_buf[2];

	//unsigned char rx_buf[EEPROM_PAGE_SIZE];
	//Format:
	//CMD + ADDR + DATA (up to EEPROM_PAGE_SIZE)
	while (1) {
		eeprom_wip_over ();
		if ((ret = eeprom_wren ())) {
			AMAZON_EEPROM_EMSG ("eeprom_wren fails\n");
			return ret;
		}
		write_buf[0] = EEPROM_WRITE | ((u8) ((addr & 0x100) >> 5));
		write_buf[1] = (addr & 0xff);

		eff = EEPROM_PAGE_SIZE - (addr % EEPROM_PAGE_SIZE);
		eff = (eff < len) ? eff : len;
		AMAZON_EEPROM_DMSG ("EEPROM Write:\n");
#if 0
		int i;
		for (i = 0; i < eff; i++) {
			printk ("%2x ", buf[i]);
			if (i % 16 == 15)
				printk ("\n");
		}
		printk ("\n");
#endif

		if ((ret = ifx_ssc_cs_low (EEPROM_CS))) {
			return ret;
		}

		if ((ret = ifx_ssc_tx (write_buf, 2)) < 0) {
			AMAZON_EEPROM_EMSG ("ssc_tx fails %d\n", ret);
			ifx_ssc_cs_high (EEPROM_CS);
			return ret;
		}

		if ((ret = ifx_ssc_tx (buf, eff)) != eff) {
			AMAZON_EEPROM_EMSG ("ssc_tx fails %d\n", ret);
			ifx_ssc_cs_high (EEPROM_CS);
			return ret;
		}

		buf += ret;
		len -= ret;
		addr += ret;

		if ((ret = ifx_ssc_cs_high (EEPROM_CS))) {
			return ret;
		}
#if 0
		printk ("<==");
		eeprom_read ((addr - eff), rx_buf, eff);
		for (i = 0; i < eff; i++) {
			printk ("[%x]", rx_buf[i]);
		}
		printk ("\n");
#endif
		if (len <= 0)
			break;
	}

	return ret;
}

int
danube_eeprom_open (struct inode *inode, struct file *filp)
{
	filp->f_pos = 0;
	return 0;
}

int
danube_eeprom_close (struct inode *inode, struct file *filp)
{
	return 0;
}

int
danube_eeprom_ioctl (struct inode *inode, struct file *filp, unsigned int cmd,
		     unsigned long data)
{
	return 0;
}

ssize_t
danube_eeprom_kread (char *buf, size_t len, u32 addr)
{
	int ret = 0;
	u32 data;
	printk ("addr:=%d\n", addr);
	printk ("len:=%d\n", len);
	if ((addr + len) > EEPROM_SIZE) {
		AMAZON_EEPROM_EMSG ("invalid len\n");
		addr = 0;
		len = EEPROM_SIZE / 2;
	}

	if ((ret = ifx_ssc_open ((struct inode *) 0, NULL))) {
		AMAZON_EEPROM_EMSG ("danube_eeprom_open fails\n");
		goto read_err_out;
	}

	data = (u32) IFX_SSC_MODE_RXTX;
	if ((ret =
	     ifx_ssc_ioctl ((struct inode *) 0, NULL, IFX_SSC_RXTX_MODE_SET,
			    (unsigned long) &data))) {
		AMAZON_EEPROM_EMSG ("set RXTX mode fails\n");
		goto read_err_out;
	}

	if ((ret = eeprom_wrsr ())) {
		AMAZON_EEPROM_EMSG ("EEPROM reset fails\n");
		goto read_err_out;
	}

	if ((ret = eeprom_read (addr, buf, len))) {
		AMAZON_EEPROM_EMSG ("eeprom read fails\n");
		goto read_err_out;
	}

      read_err_out:
	if (ifx_ssc_close ((struct inode *) 0, NULL)) {
		AMAZON_EEPROM_EMSG ("danube_eeprom_close fails\n");
	}
	return len;

}

EXPORT_SYMBOL (danube_eeprom_kread);

static ssize_t
danube_eeprom_read (struct file *filp, char *ubuf, size_t len, loff_t * off)
{
	int ret = 0;
	unsigned char ssc_rx_buf[EEPROM_SIZE];
	long flag;
	if (*off >= EEPROM_SIZE)
		return 0;
	if (*off + len > EEPROM_SIZE)
		len = EEPROM_SIZE - *off;
	if (len == 0)
		return 0;
	local_irq_save (flag);
	if ((ret = danube_eeprom_kread (ssc_rx_buf, len, *off)) < 0) {
		AMAZON_EEPROM_EMSG ("read fails, err=%x\n", ret);
		local_irq_restore (flag);
		return ret;
	}
	if (copy_to_user ((void *) ubuf, ssc_rx_buf, ret) != 0) {
		local_irq_restore (flag);
		ret = -EFAULT;
	}
	local_irq_restore (flag);
	*off += len;
	return len;
}

ssize_t
danube_eeprom_kwrite (char *buf, size_t len, u32 addr)
{
	int ret = 0;
	u32 data;
	if ((ret = ifx_ssc_open ((struct inode *) 0, NULL))) {
		AMAZON_EEPROM_EMSG ("danube_eeprom_open fails\n");
		goto write_err_out;
	}

	data = (u32) IFX_SSC_MODE_RXTX;
	if ((ret =
	     ifx_ssc_ioctl ((struct inode *) 0, NULL, IFX_SSC_RXTX_MODE_SET,
			    (unsigned long) &data))) {
		AMAZON_EEPROM_EMSG ("set RXTX mode fails\n");
		goto write_err_out;
	}

	if ((ret = eeprom_wrsr ())) {
		AMAZON_EEPROM_EMSG ("EEPROM reset fails\n");
		goto write_err_out;
	}

	if ((ret = eeprom_write (addr, buf, len))) {
		AMAZON_EEPROM_EMSG ("eeprom write fails\n");
		goto write_err_out;
	}

      write_err_out:
	if (ifx_ssc_close ((struct inode *) 0, NULL)) {
		AMAZON_EEPROM_EMSG ("danube_eeprom_close fails\n");
	}
	return ret;
}

EXPORT_SYMBOL (danube_eeprom_kwrite);

static ssize_t
danube_eeprom_write (struct file *filp, const char *ubuf, size_t len,
		     loff_t * off)
{
	int ret = 0;
	unsigned char ssc_tx_buf[EEPROM_SIZE];
	if (*off >= EEPROM_SIZE)
		return 0;
	if (len + *off > EEPROM_SIZE)
		len = EEPROM_SIZE - *off;
	if ((ret = copy_from_user (ssc_tx_buf, ubuf, len))) {
		return EFAULT;
	}
	ret = danube_eeprom_kwrite (ssc_tx_buf, len, *off);
	if (ret > 0) {
		*off = ret;
	}
	return ret;
}

loff_t
danube_eeprom_llseek (struct file * filp, loff_t off, int whence)
{
	loff_t newpos;
	switch (whence) {
	case 0:		/*SEEK_SET */
		newpos = off;
		break;
	case 1:		/*SEEK_CUR */
		newpos = filp->f_pos + off;
		break;
	default:
		return -EINVAL;
	}
	if (newpos < 0)
		return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}

static struct file_operations danube_eeprom_fops = {
      owner:THIS_MODULE,
      llseek:danube_eeprom_llseek,
	/* llseek */
      read:danube_eeprom_read,	/* read */
      write:danube_eeprom_write,
	/* write */
      ioctl:danube_eeprom_ioctl,
	/* ioctl */
      open:danube_eeprom_open,	/* open */
      release:danube_eeprom_close,
	/* release */
};

int __init
danube_eeprom_init (void)
{
	int ret = 0;

	if ((ret = register_chrdev (maj, "eeprom", &danube_eeprom_fops)) < 0) {
		printk ("Unable to register major %d for the Infineon Amazon EEPROM\n", maj);
		if (maj == 0) {
			goto errout;
		}
		else {
			maj = 0;
			if ((ret =
			     register_chrdev (maj, "ssc",
					      &danube_eeprom_fops)) < 0) {
				printk ("Unable to register major 0 for the Infineon Amazon EEPROM\n");
				goto errout;
			}
		}
	}
	if (maj == 0)
		maj = ret;
      errout:
	return ret;
}

void __exit
danube_eeprom_cleanup_module (void)
{
	if (unregister_chrdev (maj, "eeprom")) {
		printk ("Unable to unregister major %d for the EEPROM\n",
			maj);
	}
}

module_exit (danube_eeprom_cleanup_module);
module_init (danube_eeprom_init);

#ifndef MODULE
static int __init
danube_eeprom_set_maj (char *str)
{
	maj = simple_strtol (str, NULL, 0);
	return 1;
}

__setup ("eeprom_maj=", danube_eeprom_set_maj);
#endif /* !MODULE */
