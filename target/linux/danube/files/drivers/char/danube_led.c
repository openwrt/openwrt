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
 *   Copyright (C) 2006 infineon
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/errno.h>
#include <asm/danube/danube.h>
#include <asm/danube/danube_gpio.h>

#define LED_CONFIG                      0x01

#define CONFIG_OPERATION_UPDATE_SOURCE  0x0001
#define CONFIG_OPERATION_BLINK          0x0002
#define CONFIG_OPERATION_UPDATE_CLOCK   0x0004
#define CONFIG_OPERATION_STORE_MODE     0x0008
#define CONFIG_OPERATION_SHIFT_CLOCK    0x0010
#define CONFIG_OPERATION_DATA_OFFSET    0x0020
#define CONFIG_OPERATION_NUMBER_OF_LED  0x0040
#define CONFIG_OPERATION_DATA           0x0080
#define CONFIG_OPERATION_MIPS0_ACCESS   0x0100
#define CONFIG_DATA_CLOCK_EDGE          0x0200

#define DANUBE_LED_CLK_EDGE				DANUBE_LED_FALLING
//#define DANUBE_LED_CLK_EDGE				DANUBE_LED_RISING

#define LED_SH_PORT                     0
#define LED_SH_PIN                      4
#define LED_SH_DIR                      1
#define LED_SH_ALTSEL0                  1
#define LED_SH_ALTSEL1                  0
#define LED_SH_OPENDRAIN                1
#define LED_D_PORT                      0
#define LED_D_PIN                       5
#define LED_D_DIR                       1
#define LED_D_ALTSEL0                   1
#define LED_D_ALTSEL1                   0
#define LED_D_OPENDRAIN                 1
#define LED_ST_PORT                     0
#define LED_ST_PIN                      6
#define LED_ST_DIR                      1
#define LED_ST_ALTSEL0                  1
#define LED_ST_ALTSEL1                  0
#define LED_ST_OPENDRAIN                1

#define LED_ADSL0_PORT                  0
#define LED_ADSL0_PIN                   4
#define LED_ADSL0_DIR                   1
#define LED_ADSL0_ALTSEL0               0
#define LED_ADSL0_ALTSEL1               1
#define LED_ADSL0_OPENDRAIN             1
#define LED_ADSL1_PORT                  0
#define LED_ADSL1_PIN                   5
#define LED_ADSL1_DIR                   1
#define LED_ADSL1_ALTSEL0               1
#define LED_ADSL1_ALTSEL1               1
#define LED_ADSL1_OPENDRAIN             1

#if (LED_SH_PORT == LED_ADSL0_PORT && LED_SH_PIN == LED_ADSL0_PIN)      \
    || (LED_D_PORT == LED_ADSL0_PORT && LED_D_PIN == LED_ADSL0_PIN)     \
    || (LED_ST_PORT == LED_ADSL0_PORT && LED_ST_PIN == LED_ADSL0_PIN)   \
    || (LED_SH_PORT == LED_ADSL1_PORT && LED_SH_PIN == LED_ADSL1_PIN)   \
    || (LED_D_PORT == LED_ADSL1_PORT && LED_D_PIN == LED_ADSL1_PIN)     \
    || (LED_ST_PORT == LED_ADSL1_PORT && LED_ST_PIN == LED_ADSL1_PIN)
  #define ADSL_LED_IS_EXCLUSIVE         1
#else
  #define ADSL_LED_IS_EXCLUSIVE         0
#endif

#if LED_SH_DIR
  #define LED_SH_DIR_SETUP              danube_port_set_dir_out
#else
  #define LED_SH_DIR_SETUP              danube_port_clear_dir_out
#endif
#if LED_SH_ALTSEL0
  #define LED_SH_ALTSEL0_SETUP          danube_port_set_altsel0
#else
  #define LED_SH_ALTSEL0_SETUP          danube_port_clear_altsel0
#endif
#if LED_SH_ALTSEL1
  #define LED_SH_ALTSEL1_SETUP          danube_port_set_altsel1
#else
  #define LED_SH_ALTSEL1_SETUP          danube_port_clear_altsel1
#endif
#if LED_SH_OPENDRAIN
  #define LED_SH_OPENDRAIN_SETUP        danube_port_set_open_drain
#else
  #define LED_SH_OPENDRAIN_SETUP        danube_port_clear_open_drain
#endif

#if LED_D_DIR
  #define LED_D_DIR_SETUP               danube_port_set_dir_out
#else
  #define LED_D_DIR_SETUP               danube_port_clear_dir_out
#endif
#if LED_D_ALTSEL0
  #define LED_D_ALTSEL0_SETUP           danube_port_set_altsel0
#else
  #define LED_D_ALTSEL0_SETUP           danube_port_clear_altsel0
#endif
#if LED_D_ALTSEL1
  #define LED_D_ALTSEL1_SETUP           danube_port_set_altsel1
#else
  #define LED_D_ALTSEL1_SETUP           danube_port_clear_altsel1
#endif
#if LED_D_OPENDRAIN
  #define LED_D_OPENDRAIN_SETUP         danube_port_set_open_drain
#else
  #define LED_D_OPENDRAIN_SETUP         danube_port_clear_open_drain
#endif

#if LED_ST_DIR
  #define LED_ST_DIR_SETUP              danube_port_set_dir_out
#else
  #define LED_ST_DIR_SETUP              danube_port_clear_dir_out
#endif
#if LED_ST_ALTSEL0
  #define LED_ST_ALTSEL0_SETUP          danube_port_set_altsel0
#else
  #define LED_ST_ALTSEL0_SETUP          danube_port_clear_altsel0
#endif
#if LED_ST_ALTSEL1
  #define LED_ST_ALTSEL1_SETUP          danube_port_set_altsel1
#else
  #define LED_ST_ALTSEL1_SETUP          danube_port_clear_altsel1
#endif
#if LED_ST_OPENDRAIN
  #define LED_ST_OPENDRAIN_SETUP        danube_port_set_open_drain
#else
  #define LED_ST_OPENDRAIN_SETUP        danube_port_clear_open_drain
#endif

#if LED_ADSL0_DIR
  #define LED_ADSL0_DIR_SETUP           danube_port_set_dir_out
#else
  #define LED_ADSL0_DIR_SETUP           danube_port_clear_dir_out
#endif
#if LED_ADSL0_ALTSEL0
  #define LED_ADSL0_ALTSEL0_SETUP       danube_port_set_altsel0
#else
  #define LED_ADSL0_ALTSEL0_SETUP       danube_port_clear_altsel0
#endif
#if LED_ADSL0_ALTSEL1
  #define LED_ADSL0_ALTSEL1_SETUP       danube_port_set_altsel1
#else
  #define LED_ADSL0_ALTSEL1_SETUP       danube_port_clear_altsel1
#endif
#if LED_ADSL0_OPENDRAIN
  #define LED_ADSL0_OPENDRAIN_SETUP     danube_port_set_open_drain
#else
  #define LED_ADSL0_OPENDRAIN_SETUP     danube_port_clear_open_drain
#endif

#if LED_ADSL1_DIR
  #define LED_ADSL1_DIR_SETUP           danube_port_set_dir_out
#else
  #define LED_ADSL1_DIR_SETUP           danube_port_clear_dir_out
#endif
#if LED_ADSL1_ALTSEL0
  #define LED_ADSL1_ALTSEL0_SETUP       danube_port_set_altsel0
#else
  #define LED_ADSL1_ALTSEL0_SETUP       danube_port_clear_altsel0
#endif
#if LED_ADSL1_ALTSEL1
  #define LED_ADSL1_ALTSEL1_SETUP       danube_port_set_altsel1
#else
  #define LED_ADSL1_ALTSEL1_SETUP       danube_port_clear_altsel1
#endif
#if LED_ADSL1_OPENDRAIN
  #define LED_ADSL1_OPENDRAIN_SETUP     danube_port_set_open_drain
#else
  #define LED_ADSL1_OPENDRAIN_SETUP     danube_port_clear_open_drain
#endif

#define SET_BITS(x, msb, lsb, value)    (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

static int danube_led_major;

static int
danube_led_setup_gpio (void)
{
	/*
	*  Set LED_ST
	*    I don't check the return value, because I'm sure the value is valid
	*    and the pins are reserved already.
	*/
	LED_ST_ALTSEL0_SETUP(LED_ST_PORT, LED_ST_PIN);
	LED_ST_ALTSEL1_SETUP(LED_ST_PORT, LED_ST_PIN);
	LED_ST_DIR_SETUP(LED_ST_PORT, LED_ST_PIN);
	LED_ST_OPENDRAIN_SETUP(LED_ST_PORT, LED_ST_PIN);

	/*
	*  Set LED_D
	*/
	LED_D_ALTSEL0_SETUP(LED_D_PORT, LED_D_PIN);
	LED_D_ALTSEL1_SETUP(LED_D_PORT, LED_D_PIN);
	LED_D_DIR_SETUP(LED_D_PORT, LED_D_PIN);
	LED_D_OPENDRAIN_SETUP(LED_D_PORT, LED_D_PIN);

	/*
	*  Set LED_SH
	*/
	LED_SH_ALTSEL0_SETUP(LED_SH_PORT, LED_SH_PIN);
	LED_SH_ALTSEL1_SETUP(LED_SH_PORT, LED_SH_PIN);
	LED_SH_DIR_SETUP(LED_SH_PORT, LED_SH_PIN);
	LED_SH_OPENDRAIN_SETUP(LED_SH_PORT, LED_SH_PIN);

	return 0;
}

static void
danube_led_enable (void)
{
	int err = 1000000;

	writel(readl(DANUBE_PMU_PWDCR) & ~DANUBE_PMU_PWDCR_LED, DANUBE_PMU_PWDCR);
	while (--err && (readl(DANUBE_PMU_PWDSR) & DANUBE_PMU_PWDCR_LED)) {}

	if (!err)
		panic("Activating LED in PMU failed!");
}

static inline void
danube_led_disable (void)
{
	writel(readl(DANUBE_PMU_PWDCR) | DANUBE_PMU_PWDCR_LED, DANUBE_PMU_PWDCR);
}

static int
led_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = -EINVAL;

	switch ( cmd )
	{
	case LED_CONFIG:
		break;
	}

	return ret;
}

static int
led_open (struct inode *inode, struct file *file)
{
	return 0;
}

static int
led_release (struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations danube_led_fops = {
	.owner = THIS_MODULE,
	.ioctl = led_ioctl,
	.open = led_open,
	.release = led_release
};


/*
Map for LED on reference board
	WLAN_READ     LED11   OUT1    15
	WARNING       LED12   OUT2    14
	FXS1_LINK     LED13   OUT3    13
	FXS2_LINK     LED14   OUT4    12
	FXO_ACT       LED15   OUT5    11
	USB_LINK      LED16   OUT6    10
	ADSL2_LINK    LED19   OUT7    9
	BT_LINK       LED17   OUT8    8
	SD_LINK       LED20   OUT9    7
	ADSL2_TRAFFIC LED31   OUT16   0
Map for hardware relay on reference board
	USB Power On          OUT11   5
	RELAY                 OUT12   4
*/


int __init
danube_led_init (void)
{
	int ret = 0;

	danube_led_setup_gpio();

	writel(0, DANUBE_LED_AR);
	writel(0xff00, DANUBE_LED_CPU0);
	writel(0, DANUBE_LED_CPU1);
	writel(0x8000ffff, DANUBE_LED_CON0);

	/* setup the clock edge that the shift register is triggered on */
	writel(readl(DANUBE_LED_CON0) & ~DANUBE_LED_EDGE_MASK, DANUBE_LED_CON0);
	writel(readl(DANUBE_LED_CON0) | DANUBE_LED_CLK_EDGE, DANUBE_LED_CON0);

	/* per default leds 15-0 are set */
	writel(DANUBE_LED_GROUP1 | DANUBE_LED_GROUP0, DANUBE_LED_CON1);

	/* leds are update periodically by the FPID */
	writel(readl(DANUBE_LED_CON1) & ~DANUBE_LED_UPD_MASK, DANUBE_LED_CON1);
	writel(readl(DANUBE_LED_CON1) | DANUBE_LED_UPD_SRC_FPI, DANUBE_LED_CON1);

	/* set led update speed */
	writel(readl(DANUBE_LED_CON1) & ~DANUBE_LED_MASK, DANUBE_LED_CON1);
	writel(readl(DANUBE_LED_CON1) | DANUBE_LED_8HZ, DANUBE_LED_CON1);

	/* adsl 0 and 1 leds are updated by the arc */
	writel(readl(DANUBE_LED_CON0) | DANUBE_LED_ADSL_SRC, DANUBE_LED_CON0);

	/* per default, the leds are turned on */
	danube_led_enable();

	danube_led_major = register_chrdev(0, "danube_led", &danube_led_fops);

	if (!danube_led_major)
	{
		printk("danube_led: Error! Could not register device. %d\n", danube_led_major);
		ret = -EINVAL;

		goto out;
	}

	printk(KERN_INFO "danube_led : device registered on major %d\n", danube_led_major);

out:
	return ret;
}

void __exit
danube_led_exit (void)
{
	unregister_chrdev(danube_led_major, "danube_led");
}

module_init(danube_led_init);
module_exit(danube_led_exit);
