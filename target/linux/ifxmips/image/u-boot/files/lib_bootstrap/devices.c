/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <stdarg.h>
#include <malloc.h>
#include <devices.h>
#include <serial.h>
#ifdef CONFIG_LOGBUFFER
#include <logbuff.h>
#endif
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
#include <i2c.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

list_t devlist = 0;
device_t *stdio_devices[] = { NULL, NULL, NULL };
char *stdio_names[MAX_FILES] = { "stdin", "stdout", "stderr" };

#if defined(CONFIG_SPLASH_SCREEN) && !defined(CFG_DEVICE_NULLDEV)
#define	CFG_DEVICE_NULLDEV	1
#endif


#ifdef CFG_DEVICE_NULLDEV
void nulldev_putc(const char c)
{
  /* nulldev is empty! */
}

void nulldev_puts(const char *s)
{
  /* nulldev is empty! */
}

int nulldev_input(void)
{
  /* nulldev is empty! */
  return 0;
}
#endif

/**************************************************************************
 * SYSTEM DRIVERS
 **************************************************************************
 */

static void drv_system_init (void)
{
	device_t dev;

	memset (&dev, 0, sizeof (dev));

	strcpy (dev.name, "serial");
	dev.flags = DEV_FLAGS_OUTPUT | DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
#ifdef CONFIG_SERIAL_SOFTWARE_FIFO
	dev.putc = serial_buffered_putc;
	dev.puts = serial_buffered_puts;
	dev.getc = serial_buffered_getc;
	dev.tstc = serial_buffered_tstc;
#else
	dev.putc = serial_putc;
	dev.puts = serial_puts;
	dev.getc = serial_getc;
	dev.tstc = serial_tstc;
#endif

	device_register (&dev);

#ifdef CFG_DEVICE_NULLDEV
	memset (&dev, 0, sizeof (dev));

	strcpy (dev.name, "nulldev");
	dev.flags = DEV_FLAGS_OUTPUT | DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
	dev.putc = nulldev_putc;
	dev.puts = nulldev_puts;
	dev.getc = nulldev_input;
	dev.tstc = nulldev_input;

	device_register (&dev);
#endif
}

/**************************************************************************
 * DEVICES
 **************************************************************************
 */

int device_register (device_t * dev)
{
	ListInsertItem (devlist, dev, LIST_END);
	return 0;
}

/* deregister the device "devname".
 * returns 0 if success, -1 if device is assigned and 1 if devname not found
 */
#ifdef	CFG_DEVICE_DEREGISTER
int device_deregister(char *devname)
{
	int i,l,dev_index;
	device_t *dev = NULL;
	char temp_names[3][8];

	dev_index=-1;
	for (i=1; i<=ListNumItems(devlist); i++) {
		dev = ListGetPtrToItem (devlist, i);
		if(strcmp(dev->name,devname)==0) {
			dev_index=i;
			break;
		}
	}
	if(dev_index<0) /* device not found */
		return 0;
	/* get stdio devices (ListRemoveItem changes the dev list) */
	for (l=0 ; l< MAX_FILES; l++) {
		if (stdio_devices[l] == dev) {
			/* Device is assigned -> report error */
			return -1;
		}
		memcpy (&temp_names[l][0],
			stdio_devices[l]->name,
			sizeof(stdio_devices[l]->name));
	}
	ListRemoveItem(devlist,NULL,dev_index);
	/* reassign Device list */
	for (i=1; i<=ListNumItems(devlist); i++) {
		dev = ListGetPtrToItem (devlist, i);
		for (l=0 ; l< MAX_FILES; l++) {
			if(strcmp(dev->name,temp_names[l])==0) {
				stdio_devices[l] = dev;
			}
		}
	}
	return 0;
}
#endif	/* CFG_DEVICE_DEREGISTER */

int devices_init (void)
{
#ifndef CONFIG_ARM     /* already relocated for current ARM implementation */
	ulong relocation_offset = gd->reloc_off;
	int i;

	/* relocate device name pointers */
	for (i = 0; i < (sizeof (stdio_names) / sizeof (char *)); ++i) {
		stdio_names[i] = (char *) (((ulong) stdio_names[i]) +
						relocation_offset);
	}
#endif

	/* Initialize the list */
	devlist = ListCreate (sizeof (device_t));

	if (devlist == NULL) {
		eputs ("Cannot initialize the list of devices!\n");
		return -1;
	}
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
	i2c_init (CFG_I2C_SPEED, CFG_I2C_SLAVE);
#endif
#ifdef CONFIG_LCD
	drv_lcd_init ();
#endif
#if defined(CONFIG_VIDEO) || defined(CONFIG_CFB_CONSOLE)
	drv_video_init ();
#endif
#ifdef CONFIG_KEYBOARD
	drv_keyboard_init ();
#endif
#ifdef CONFIG_LOGBUFFER
	drv_logbuff_init ();
#endif
	drv_system_init ();
#ifdef CONFIG_SERIAL_MULTI
	serial_devices_init ();
#endif
#ifdef CONFIG_USB_TTY
	drv_usbtty_init ();
#endif
#ifdef CONFIG_NETCONSOLE
	drv_nc_init ();
#endif

	return (0);
}

int devices_done (void)
{
	ListDispose (devlist);

	return 0;
}
