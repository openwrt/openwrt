/* Support for the OLPC DCON and OLPC EC access
 * Copyright (C) 2006, Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mc146818rtc.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

#include <asm/olpc.h>
#include <asm/ofw.h>

/* This is our new multi-purpose structure used to contain the
 * information about the platform that we detect
 */

struct olpc_platform_t olpc_platform_info;
EXPORT_SYMBOL_GPL(olpc_platform_info);

/*********************************************************************
 *		EC locking and access
 *********************************************************************/

static DEFINE_SPINLOCK(ec_lock);

/* what the timeout *should* be (in ms) */
#define EC_BASE_TIMEOUT 20

/* the timeout that bugs in the EC might force us to actually use */
static int ec_timeout = EC_BASE_TIMEOUT;

static int __init olpc_ec_timeout_set(char *str)
{
	if (get_option(&str, &ec_timeout) != 1) {
		ec_timeout = EC_BASE_TIMEOUT;
		printk(KERN_ERR "olpc-ec:  invalid argument to "
				"'olpc_ec_timeout=', ignoring!\n");
	}
	printk(KERN_DEBUG "olpc-ec:  using %d ms delay for EC commands.\n",
			ec_timeout);
	return 1;
}
__setup("olpc_ec_timeout=", olpc_ec_timeout_set);

/*
 * These *bf_status functions return whether the buffers are full or not.
 */

static inline unsigned int ibf_status(unsigned int port)
{
	return inb(port) & 0x02;
}

static inline unsigned int obf_status(unsigned int port)
{
	return inb(port) & 0x01;
}

#define wait_on_ibf(p, d) __wait_on_ibf(__LINE__, (p), (d))
static int __wait_on_ibf(unsigned int line, unsigned int port, int desired)
{
	unsigned int timeo;
	int state = ibf_status(port);

	for (timeo = ec_timeout; state != desired && timeo; timeo--) {
		mdelay(1);
		state = ibf_status(port);
	}

	if ((state == desired) && (ec_timeout > EC_BASE_TIMEOUT) &&
			timeo < (ec_timeout - EC_BASE_TIMEOUT)) {
		printk(KERN_WARNING "olpc-ec:  waited %u ms for IBF (%d)!\n",
				EC_BASE_TIMEOUT-timeo, line);
	}

	return !(state == desired);
}

#define wait_on_obf(p, d) __wait_on_obf(__LINE__, (p), (d))
static int __wait_on_obf(unsigned int line, unsigned int port, int desired)
{
	unsigned int timeo;
	int state = obf_status(port);

	for (timeo = ec_timeout; state != desired && timeo; timeo--) {
		mdelay(1);
		state = obf_status(port);
	}

	if ((state == desired) && (ec_timeout > EC_BASE_TIMEOUT) &&
			timeo < (ec_timeout - EC_BASE_TIMEOUT)) {
		printk(KERN_WARNING "olpc-ec:  waited %u ms for OBF (%d)!\n",
				EC_BASE_TIMEOUT-timeo, line);
	}

	return !(state == desired);
}

int olpc_ec_cmd(unsigned char cmd, unsigned char *inbuf, size_t inlen,
		unsigned char *outbuf,  size_t outlen)
{
	unsigned long flags;
	int ret = -EIO;
	int i;

	spin_lock_irqsave(&ec_lock, flags);

	if (wait_on_ibf(0x6c, 0)) {
		printk(KERN_ERR "olpc-ec:  timeout waiting for EC to "
				"quiesce!\n");
		goto err;
	}

restart:
	/*
	 * Note that if we time out during any IBF checks, that's a failure;
	 * we have to return.  There's no way for the kernel to clear that.
	 *
	 * If we time out during an OBF check, we can restart the command;
	 * reissuing it will clear the OBF flag, and we should be alright.
	 * The OBF flag will sometimes misbehave due to what we believe
	 * is a hardware quirk..
	 */
	printk(KERN_DEBUG "olpc-ec:  running cmd 0x%x\n", cmd);
	outb(cmd, 0x6c);

	if (wait_on_ibf(0x6c, 0)) {
		printk(KERN_ERR "olpc-ec:  timeout waiting for EC to read "
				"command!\n");
		goto err;
	}

	if (inbuf && inlen) {
		/* write data to EC */
		for (i = 0; i < inlen; i++) {
			if (wait_on_ibf(0x6c, 0)) {
				printk(KERN_ERR "olpc-ec:  timeout waiting for"
						" EC accept data!\n");
				goto err;
			}
			printk(KERN_DEBUG "olpc-ec:  sending cmd arg 0x%x\n",
					inbuf[i]);
			outb(inbuf[i], 0x68);
		}
	}
	if (outbuf && outlen) {
		/* read data from EC */
		for (i = 0; i < outlen; i++) {
			if (wait_on_obf(0x6c, 1)) {
				printk(KERN_ERR "olpc-ec:  timeout waiting for"
						" EC to provide data!\n");
				goto restart;
			}
			outbuf[i] = inb(0x68);
			printk(KERN_DEBUG "olpc-ec:  received 0x%x\n",
					outbuf[i]);
		}
	}

	ret = 0;
err:
	spin_unlock_irqrestore(&ec_lock, flags);
	return ret;
}
EXPORT_SYMBOL_GPL(olpc_ec_cmd);

/*********************************************************************
 *		DCON stuff
 *********************************************************************/

static void olpc_power_off(void)
{
	printk(KERN_INFO "OLPC power off sequence...\n");
	outb(0xff, 0x381);
	outb(0x14, 0x382);
	outb(0x01, 0x383);
	outb(0xff, 0x381);
	outb(0x14, 0x382);
	outb(0x00, 0x383);
}

static void __init
ec_detect(void)
{
	olpc_ec_cmd(0x08, NULL, 0, (unsigned char *) &olpc_platform_info.ecver, 1);
}

/* Check to see if this version of the OLPC board has VSA built
 * in, and set a flag
 */

static void __init vsa_detect(void)
{
	u16 rev;

	outw(0xFC53, 0xAC1C);
	outw(0x0003, 0xAC1C);

	rev = inw(0xAC1E);

	if (rev == 0x4132)
		olpc_platform_info.flags |= OLPC_F_VSA;
}

/* Map OFW revisions to what OLPC_REV_* */
static const char __initdata *olpc_boardrev_str[] = {
	"A1",
	"preB1",
	"B1",
	"preB2",
	"B2",
	"preB3",
	"B3",
	"B4",
	"C1",
	"R1",
};

static void __init platform_detect(char *revision, size_t len)
{
	size_t propsize;
	int i;

	BUG_ON(ARRAY_SIZE(olpc_boardrev_str) != OLPC_REV_UNKNOWN);

	if (ofw("getprop", 4, 1, NULL, "model", revision, len, &propsize)) {
		printk(KERN_ERR "ofw: getprop call failed!\n");
		goto failed;
	}
	if (len < propsize) {
		printk(KERN_ERR "ofw: revision string is too long!\n");
		goto failed;
	}

	for (i=0; i < ARRAY_SIZE(olpc_boardrev_str); i++) {
		if (strcmp(revision, olpc_boardrev_str[i]) == 0) {
			olpc_platform_info.boardrev = i;
			return;
		}
	}

failed:
	strncpy(revision, "Unknown", len);
	olpc_platform_info.boardrev = OLPC_REV_UNKNOWN;
}

static int olpc_dcon_present = -1;
module_param(olpc_dcon_present, int, 0444);

/* REV_A CMOS map:
 * bit 440;  DCON present bit
 */

#define OLPC_CMOS_DCON_OFFSET (440 / 8)
#define OLPC_CMOS_DCON_MASK   0x01

static int __init olpc_init(void)
{
	unsigned char *romsig;
	char revision[10];

	spin_lock_init(&ec_lock);

	romsig = ioremap(0xffffffc0, 16);

	if (!romsig)
		return 0;

	if (strncmp(romsig, "CL1   Q", 7))
		goto unmap;
	if (strncmp(romsig+6, romsig+13, 3)) {
		printk(KERN_INFO "OLPC BIOS signature looks invalid. Assuming not OLPC\n");
		goto unmap;
	}
	printk(KERN_INFO "OLPC board with OpenFirmware: %.16s\n", romsig);

	olpc_platform_info.flags |= OLPC_F_PRESENT;

	pm_power_off = olpc_power_off;

	/* Get the platform revision */
	platform_detect(revision, sizeof(revision));

	/* If olpc_dcon_present isn't set by the command line, then
	 * "detect" it
	 */

	if (olpc_dcon_present == -1) {
		/* B1 and greater always has a DCON */
		if (olpc_platform_info.boardrev >= OLPC_REV_B1 &&
				olpc_platform_info.boardrev < OLPC_REV_UNKNOWN)
			olpc_dcon_present = 1;
	}

	if (olpc_dcon_present)
		olpc_platform_info.flags |= OLPC_F_DCON;

	/* Get the EC revision */
	ec_detect();

	/* Check to see if the VSA exists */
	vsa_detect();

	printk(KERN_INFO "OLPC board revision: %s (EC=%x)\n", revision,
			olpc_platform_info.ecver);

 unmap:
	iounmap(romsig);

	return 0;
}

postcore_initcall(olpc_init);
