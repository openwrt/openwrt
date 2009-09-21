/*
 * linux/arch/arm/mach-s3c2442/mach-gta02.c
 *
 * S3C2442 Machine Support for the Openmoko Freerunner (GTA02)
 *
 * Copyright (C) 2006-2007 by Openmoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 * All rights reserved.
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
 *
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/mmc/host.h>
#include <linux/leds.h>
#include <linux/gpio_keys.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>

#include <linux/i2c.h>
#include <linux/backlight.h>
#include <linux/regulator/machine.h>

#include <linux/mfd/pcf50633/core.h>
#include <linux/mfd/pcf50633/mbc.h>
#include <linux/mfd/pcf50633/adc.h>
#include <linux/mfd/pcf50633/gpio.h>
#include <linux/mfd/pcf50633/pmic.h>

#include <linux/lis302dl.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/hardware.h>
#include <mach/io.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include <mach/regs-irq.h>
#include <mach/regs-gpio.h>
#include <mach/regs-gpioj.h>
#include <mach/fb.h>
#include <mach/mci.h>
#include <mach/ts.h>
#include <mach/spi.h>
#include <mach/spi-gpio.h>
#include <mach/regs-mem.h>
#include <plat/pwm.h>
#include <mach/cpu.h>

#include <mach/gta02.h>

#include <plat/regs-serial.h>
#include <plat/nand.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <plat/pm.h>
#include <plat/udc.h>
#include <plat/iic.h>
#include <plat/usb-control.h>
#include <plat/regs-timer.h>
#include <plat/gpio-core.h>

#include <mach/gta02-pm-gsm.h>
#include <mach/gta02-pm-gps.h>
#include <mach/gta02-pm-wlan.h>

#include <linux/jbt6k74.h>
#include <linux/glamofb.h>
#include <linux/mfd/glamo.h>

#include <linux/hdq.h>
#include <linux/bq27000_battery.h>

#include <linux/touchscreen/ts_filter_chain.h>
#ifdef CONFIG_TOUCHSCREEN_FILTER
#include <linux/touchscreen/ts_filter_linear.h>
#include <linux/touchscreen/ts_filter_mean.h>
#include <linux/touchscreen/ts_filter_median.h>
#include <linux/touchscreen/ts_filter_group.h>
#endif

#include <asm/fiq.h>

#include <linux/gta02-vibrator.h>

/* -------------------------------------------------------------------------------
 * GTA02 FIQ related
 *
 * Calls into vibrator and hdq and based on the return values
 * determines if we the FIQ source be kept alive
 */

#define DIVISOR_FROM_US(x) ((x) << 3)

#ifdef CONFIG_HDQ_GPIO_BITBANG
#define FIQ_DIVISOR_HDQ DIVISOR_FROM_US(HDQ_SAMPLE_PERIOD_US)
extern int hdq_fiq_handler(void);
#endif

#ifdef CONFIG_LEDS_GTA02_VIBRATOR
#define FIQ_DIVISOR_VIBRATOR DIVISOR_FROM_US(100)
extern int gta02_vibrator_fiq_handler(void);
#endif

#if defined(CONFIG_LEDS_GTA02_VIBRATOR) || defined(CONFIG_HDQ_GPIO_BITBANG)

/* Global data related to our fiq source */
static u32 gta02_fiq_ack_mask;
static struct s3c2410_pwm gta02_fiq_pwm_timer;
static u16 gta02_fiq_timer_index;
static int gta02_fiq_irq;

static void gta02_fiq_handler(void)
{
	u16 divisor = 0xffff;

	/* Vibrator servicing */

	/* disable further timer interrupts if nobody has any work
	 * or adjust rate according to who still has work
	 *
	 * CAUTION: it means forground code must disable FIQ around
	 * its own non-atomic S3C2410_INTMSK changes... not common
	 * thankfully and taken care of by the fiq-basis patch
	 */

#ifdef CONFIG_LEDS_GTA02_VIBRATOR
	if (gta02_vibrator_fiq_handler())
		divisor = FIQ_DIVISOR_VIBRATOR;
#endif

#ifdef CONFIG_HDQ_GPIO_BITBANG
	if (hdq_fiq_handler())
		divisor = FIQ_DIVISOR_HDQ;
#endif

	if (divisor == 0xffff) /* mask the fiq irq source */
		__raw_writel(__raw_readl(S3C2410_INTMSK) | gta02_fiq_ack_mask,
				S3C2410_INTMSK);
	else /* still working, maybe at a different rate */
		__raw_writel(divisor, S3C2410_TCNTB(gta02_fiq_timer_index));

	__raw_writel(gta02_fiq_ack_mask, S3C2410_SRCPND);
}

static void gta02_fiq_kick(void)
{
	unsigned long flags;
	u32 tcon;

	/* we have to take care about FIQ because this modification is
	 * non-atomic, FIQ could come in after the read and before the
	 * writeback and its changes to the register would be lost
	 * (platform INTMSK mod code is taken care of already)
	 */
	local_save_flags(flags);
	local_fiq_disable();
	/* allow FIQs to resume */
	__raw_writel(__raw_readl(S3C2410_INTMSK) &
			~(1 << (gta02_fiq_irq - S3C2410_CPUIRQ_OFFSET)),
			S3C2410_INTMSK);
	tcon = __raw_readl(S3C2410_TCON) & ~S3C2410_TCON_T3START;
	/* fake the timer to a count of 1 */
	__raw_writel(1, S3C2410_TCNTB(gta02_fiq_timer_index));
	__raw_writel(tcon | S3C2410_TCON_T3MANUALUPD, S3C2410_TCON);
	__raw_writel(tcon | S3C2410_TCON_T3MANUALUPD | S3C2410_TCON_T3START,
			S3C2410_TCON);
	__raw_writel(tcon | S3C2410_TCON_T3START, S3C2410_TCON);
	local_irq_restore(flags);
}

static int gta02_fiq_enable(void)
{
	int irq_index_fiq = IRQ_TIMER3;
	int rc = 0;

	local_fiq_disable();

	gta02_fiq_irq = irq_index_fiq;
	gta02_fiq_ack_mask = 1 << (irq_index_fiq - S3C2410_CPUIRQ_OFFSET);
	gta02_fiq_timer_index = (irq_index_fiq - IRQ_TIMER0);

	/* set up the timer to operate as a pwm device */

	rc = s3c2410_pwm_init(&gta02_fiq_pwm_timer);
	if (rc)
		goto bail;

	gta02_fiq_pwm_timer.timerid = PWM0 + gta02_fiq_timer_index;
	gta02_fiq_pwm_timer.prescaler = (6 - 1) / 2;
	gta02_fiq_pwm_timer.divider = S3C2410_TCFG1_MUX3_DIV2;
	/* default rate == ~32us */
	gta02_fiq_pwm_timer.counter = gta02_fiq_pwm_timer.comparer = 3000;

	rc = s3c2410_pwm_enable(&gta02_fiq_pwm_timer);
	if (rc)
		goto bail;

	s3c2410_pwm_start(&gta02_fiq_pwm_timer);

	/* let our selected interrupt be a magic FIQ interrupt */
	__raw_writel(gta02_fiq_ack_mask, S3C2410_INTMOD);

	/* it's ready to go as soon as we unmask the source in S3C2410_INTMSK */
	local_fiq_enable();

	set_fiq_c_handler(gta02_fiq_handler);

	return 0;

bail:
	printk(KERN_ERR "Could not initialize FIQ for GTA02: %d\n", rc);

	return rc;
}

static void gta02_fiq_disable(void)
{
	__raw_writel(0, S3C2410_INTMOD);
	local_fiq_disable();
	gta02_fiq_irq = 0; /* no active source interrupt now either */

}
/* -------------------- /GTA02 FIQ Handler ------------------------------------- */

#endif

/*
 * this gets called every 1ms when we paniced.
 */

static long gta02_panic_blink(long count)
{
	long delay = 0;
	static long last_blink;
	static char led;

	if (count - last_blink < 100) /* 200ms period, fast blink */
		return 0;

	led ^= 1;
	s3c2410_gpio_cfgpin(GTA02_GPIO_AUX_LED, S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_setpin(GTA02_GPIO_AUX_LED, led);

	last_blink = count;
	return delay;
}


/**
 * returns PCB revision information in b9,b8 and b2,b1,b0
 * Pre-GTA02 A6 returns 0x000
 *     GTA02 A6 returns 0x101
 *     ...
 */

int gta02_get_pcb_revision(void)
{
	int n;
	int u = 0;
	static unsigned long pinlist[] = {
		GTA02_PCB_ID1_0,
		GTA02_PCB_ID1_1,
		GTA02_PCB_ID1_2,
		GTA02_PCB_ID2_0,
		GTA02_PCB_ID2_1,
	};
	static int pin_offset[] = {
		0, 1, 2, 8, 9
	};

	for (n = 0 ; n < ARRAY_SIZE(pinlist); n++) {
		/*
		 * set the PCB version GPIO to be pulled-down input
		 * force low briefly first
		 */
		s3c2410_gpio_cfgpin(pinlist[n], S3C2410_GPIO_OUTPUT);
		s3c2410_gpio_setpin(pinlist[n], 0);
		/* misnomer: it is a pullDOWN in 2442 */
		s3c2410_gpio_pullup(pinlist[n], 1);
		s3c2410_gpio_cfgpin(pinlist[n], S3C2410_GPIO_INPUT);

		udelay(10);

		if (s3c2410_gpio_getpin(pinlist[n]))
			u |= 1 << pin_offset[n];

		/*
		* when not being interrogated, all of the revision GPIO
		* are set to output HIGH without pulldown so no current flows
		* if they are NC or pulled up.
		*/
		s3c2410_gpio_setpin(pinlist[n], 1);
		s3c2410_gpio_cfgpin(pinlist[n], S3C2410_GPIO_OUTPUT);
		/* misnomer: it is a pullDOWN in 2442 */
		s3c2410_gpio_pullup(pinlist[n], 0);
	}

	return u;
}

struct platform_device gta02_resume_reason_device = {
	.name 		= "gta02-resume",
	.num_resources	= 0,
};

static struct map_desc gta02_iodesc[] __initdata = {
	{
		.virtual	= 0xe0000000,
		.pfn		= __phys_to_pfn(S3C2410_CS3+0x01000000),
		.length		= SZ_1M,
		.type		= MT_DEVICE
	},
};

#define UCON (S3C2410_UCON_DEFAULT | S3C2443_UCON_RXERR_IRQEN)
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

static struct s3c2410_uartcfg gta02_uartcfgs[] = {
	[0] = {
		.hwport	= 0,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
	[1] = {
		.hwport	= 1,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
	[2] = {
		.hwport	= 2,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},

};

struct pcf50633 *gta02_pcf;

#ifdef CONFIG_CHARGER_PCF50633
#ifdef CONFIG_HDQ_GPIO_BITBANG
static int gta02_get_charger_online_status(void)
{
	struct pcf50633 *pcf = gta02_pcf;

	return pcf50633_mbc_get_status(pcf) & PCF50633_MBC_USB_ONLINE;
}

static int gta02_get_charger_active_status(void)
{
	struct pcf50633 *pcf = gta02_pcf;

	return pcf50633_mbc_get_status(pcf) & PCF50633_MBC_USB_ACTIVE;
}
#endif

#define ADC_NOM_CHG_DETECT_1A 6
#define ADC_NOM_CHG_DETECT_USB 43

static void
gta02_configure_pmu_for_charger(struct pcf50633 *pcf, void *unused, int res)
{
	int  ma;

	/* Interpret charger type */
	if (res < ((ADC_NOM_CHG_DETECT_USB + ADC_NOM_CHG_DETECT_1A) / 2)) {

		/* Stop GPO driving out now that we have a IA charger */
		pcf50633_gpio_set(pcf, PCF50633_GPO, 0);

		ma = 1000;
	} else
		ma = 100;

	pcf50633_mbc_usb_curlim_set(pcf, ma);
}

static struct delayed_work gta02_charger_work;
static int gta02_usb_vbus_draw;

static void gta02_charger_worker(struct work_struct *work)
{
	struct pcf50633 *pcf = gta02_pcf;

	if (gta02_usb_vbus_draw) {
		pcf50633_mbc_usb_curlim_set(pcf, gta02_usb_vbus_draw);
		return;
	} else {
#ifdef CONFIG_PCF50633_ADC
		pcf50633_adc_async_read(pcf,
			PCF50633_ADCC1_MUX_ADCIN1,
			PCF50633_ADCC1_AVERAGE_16,
			gta02_configure_pmu_for_charger, NULL);
#else
		/* If the PCF50633 ADC is disabled we fallback to a 100mA limit for safety. */
		pcf50633_mbc_usb_curlim_set(pcf, 100);
#endif
		return;
	}
}

#define GTA02_CHARGER_CONFIGURE_TIMEOUT ((3000 * HZ) / 1000)
static void gta02_pmu_event_callback(struct pcf50633 *pcf, int irq)
{
	if (irq == PCF50633_IRQ_USBINS) {
		schedule_delayed_work(&gta02_charger_work,
				GTA02_CHARGER_CONFIGURE_TIMEOUT);
		return;
	} else if (irq == PCF50633_IRQ_USBREM) {
		cancel_delayed_work_sync(&gta02_charger_work);
		gta02_usb_vbus_draw = 0;
	}
}

static void gta02_pmu_force_shutdown(struct pcf50633 *pcf)
{
	pcf50633_reg_set_bit_mask(pcf, PCF50633_REG_OOCSHDWN,
			PCF50633_OOCSHDWN_GOSTDBY, PCF50633_OOCSHDWN_GOSTDBY);
}


static void gta02_udc_vbus_draw(unsigned int ma)
{
	if (!gta02_pcf)
		return;

	gta02_usb_vbus_draw = ma;

	schedule_delayed_work(&gta02_charger_work,
				GTA02_CHARGER_CONFIGURE_TIMEOUT);
}

static int gta02_udc_vbus_status(void)
{
	struct pcf50633 *pcf = gta02_pcf;
	if (!gta02_pcf)
		return -ENODEV;

	return !!(pcf50633_mbc_get_status(pcf) & PCF50633_MBC_USB_ONLINE);
}

#else /* !CONFIG_CHARGER_PCF50633 */
#ifdef CONFIG_HDQ_GPIO_BITBANG
#define gta02_get_charger_online_status NULL
#define gta02_get_charger_active_status NULL
#endif
#define gta02_pmu_event_callback	NULL
#define gta02_udc_vbus_draw		NULL
#define gta02_udc_vbus_status		NULL
#endif


static struct platform_device gta02_pm_gps_dev = {
	.name = "gta02-pm-gps",
};

static struct platform_device gta02_pm_bt_dev = {
	.name = "gta02-pm-bt",
};

static struct platform_device gta02_pm_gsm_dev = {
	.name = "gta02-pm-gsm",
};

static struct platform_device gta02_pm_wlan_dev = {
	.name = "gta02-pm-wlan",
};

/* this is called when pc50633 is probed, unfortunately quite late in the
 * day since it is an I2C bus device.  Here we can belatedly define some
 * platform devices with the advantage that we can mark the pcf50633 as the
 * parent.  This makes them get suspended and resumed with their parent
 * the pcf50633 still around.
 */

static struct platform_device gta02_glamo_dev;

static void gta02_pmu_attach_child_devices(struct pcf50633 *pcf);
static void gta02_pmu_regulator_registered(struct pcf50633 *pcf, int id);

static struct regulator_consumer_supply ldo4_consumers[] = {
	{
		.dev = &gta02_pm_bt_dev.dev,
		.supply = "BT_3V2",
	},
};

static struct regulator_consumer_supply ldo5_consumers[] = {
	{
		.dev = &gta02_pm_gps_dev.dev,
		.supply = "RF_3V",
	},
};

static struct regulator_consumer_supply hcldo_consumers[] = {
	{
		.dev = &gta02_glamo_dev.dev,
		.supply = "SD_3V3",
	},
};

static struct regulator_consumer_supply ldo6_consumers[] = {
	{ .supply = "VDC" },
	{ .supply = "VDDIO" },
};

static char *gta02_batteries[] = {
	"battery",
};

#ifdef CONFIG_CHARGER_PCF50633

struct pcf50633_platform_data gta02_pcf_pdata = {
	.resumers = {
		[0] = 	PCF50633_INT1_USBINS |
			PCF50633_INT1_USBREM |
			PCF50633_INT1_ALARM,
		[1] = 	PCF50633_INT2_ONKEYF,
		[2] = 	PCF50633_INT3_ONKEY1S,
		[3] = 	PCF50633_INT4_LOWSYS |
			PCF50633_INT4_LOWBAT |
			PCF50633_INT4_HIGHTMP,
	},

	.batteries = gta02_batteries,
	.num_batteries = ARRAY_SIZE(gta02_batteries),
	.charging_restart_interval = (900 * HZ),
	.chg_ref_current_ma = 1000,

	.reg_init_data = {
		[PCF50633_REGULATOR_AUTO] = {
			.constraints = {
				.name = "IO_3V3",
				.min_uV = 3300000,
				.max_uV = 3300000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.boot_on = 1,
				.always_on = 1,
				.apply_uV = 1,
				.state_mem = {
					.enabled = 1,
				},
			},
			.num_consumer_supplies = 0,
		},
		[PCF50633_REGULATOR_DOWN1] = {
			.constraints = {
				.name = "CORE_1V3",
				.min_uV = 1300000,
				.max_uV = 1600000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.boot_on = 1,
				.always_on = 1,
				.apply_uV = 1,
			},
			.num_consumer_supplies = 0,
		},
		[PCF50633_REGULATOR_DOWN2] = {
			.constraints = {
				.name = "IO_1V8",
				.min_uV = 1800000,
				.max_uV = 1800000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.apply_uV = 1,
				.boot_on = 1,
				.always_on = 1,
				.state_mem = {
					.enabled = 1,
				},
			},
			.num_consumer_supplies = 0,
		},
		[PCF50633_REGULATOR_HCLDO] = {
			.constraints = {
				.name = "SD_3V3",
				.min_uV = 2000000,
				.max_uV = 3300000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
				.boot_on = 1,
			},
			.num_consumer_supplies = ARRAY_SIZE(hcldo_consumers),
			.consumer_supplies = hcldo_consumers,
		},
		[PCF50633_REGULATOR_LDO1] = {
			.constraints = {
				.name = "GSENSOR_3V3",
				.min_uV = 3300000,
				.max_uV = 3300000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.apply_uV = 1,
				.always_on = 1,
			},
			.num_consumer_supplies = 0,
		},
		[PCF50633_REGULATOR_LDO2] = {
			.constraints = {
				.name = "CODEC_3V3",
				.min_uV = 3300000,
				.max_uV = 3300000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.apply_uV = 1,
				.always_on = 1,
			},
			.num_consumer_supplies = 0,
		},
		[PCF50633_REGULATOR_LDO3] = {
			.constraints = {
				.min_uV = 3000000,
				.max_uV = 3000000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.apply_uV = 1,
				.always_on = 1,
			},
			.num_consumer_supplies = 0,
		},
		[PCF50633_REGULATOR_LDO4] = {
			.constraints = {
				.name = "BT_3V2",
				.min_uV = 3200000,
				.max_uV = 3200000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.always_on = 1,
				.apply_uV = 1,
			},
			.num_consumer_supplies = ARRAY_SIZE(ldo4_consumers),
			.consumer_supplies = ldo4_consumers,
		},
		[PCF50633_REGULATOR_LDO5] = {
			.constraints = {
				.name = "RF_3V",
				.min_uV = 3000000,
				.max_uV = 3000000,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.apply_uV = 1,
				.state_mem = {
					.enabled = 1,
				},
			},
			.num_consumer_supplies = ARRAY_SIZE(ldo5_consumers),
			.consumer_supplies = ldo5_consumers,
		},
		[PCF50633_REGULATOR_LDO6] = {
			.constraints = {
				.name = "LCM_3V",
				.min_uV = 3000000,
				.max_uV = 3000000,
				.always_on = 0,
				.valid_modes_mask = REGULATOR_MODE_NORMAL,
				.apply_uV = 1,
			},
			.num_consumer_supplies = ARRAY_SIZE(ldo6_consumers),
			.consumer_supplies = ldo6_consumers,
		},
	},
	.probe_done = gta02_pmu_attach_child_devices,
	.regulator_registered = gta02_pmu_regulator_registered,
	.mbc_event_callback = gta02_pmu_event_callback,
	.force_shutdown = gta02_pmu_force_shutdown,
};

#endif

#ifdef CONFIG_HDQ_GPIO_BITBANG
/* BQ27000 Battery */

struct bq27000_platform_data bq27000_pdata = {
	.name = "battery",
	.rsense_mohms = 20,
	.hdq_read = hdq_read,
	.hdq_write = hdq_write,
	.hdq_initialized = hdq_initialized,
	.get_charger_online_status = gta02_get_charger_online_status,
	.get_charger_active_status = gta02_get_charger_active_status
};

struct platform_device bq27000_battery_device = {
	.name 		= "bq27000-battery",
	.dev = {
		.platform_data = &bq27000_pdata,
	},
};

/* HDQ */

static void gta02_hdq_attach_child_devices(struct device *parent_device)
{
	switch (S3C_SYSTEM_REV_ATAG) {
	case GTA02v5_SYSTEM_REV:
	case GTA02v6_SYSTEM_REV:
		bq27000_battery_device.dev.parent = parent_device;
		platform_device_register(&bq27000_battery_device);
		break;
	default:
		break;
	}
}

static void gta02_hdq_gpio_direction_out(void)
{
	s3c2410_gpio_cfgpin(GTA02v5_GPIO_HDQ, S3C2410_GPIO_OUTPUT);
}

static void gta02_hdq_gpio_direction_in(void)
{
	s3c2410_gpio_cfgpin(GTA02v5_GPIO_HDQ, S3C2410_GPIO_INPUT);
}

static void gta02_hdq_gpio_set_value(int val)
{

	s3c2410_gpio_setpin(GTA02v5_GPIO_HDQ, val);
}

static int gta02_hdq_gpio_get_value(void)
{
	return s3c2410_gpio_getpin(GTA02v5_GPIO_HDQ);
}

static struct resource gta02_hdq_resources[] = {
	[0] = {
		.start	= GTA02v5_GPIO_HDQ,
		.end	= GTA02v5_GPIO_HDQ,
	},
};

struct hdq_platform_data gta02_hdq_platform_data = {
	.attach_child_devices = gta02_hdq_attach_child_devices,
	.gpio_dir_out = gta02_hdq_gpio_direction_out,
	.gpio_dir_in = gta02_hdq_gpio_direction_in,
	.gpio_set = gta02_hdq_gpio_set_value,
	.gpio_get = gta02_hdq_gpio_get_value,

	.enable_fiq = gta02_fiq_enable,
	.disable_fiq = gta02_fiq_disable,
	.kick_fiq = gta02_fiq_kick,

};

struct platform_device gta02_hdq_device = {
	.name 		= "hdq",
	.num_resources	= 1,
	.resource	= gta02_hdq_resources,
	.dev		= {
		.platform_data = &gta02_hdq_platform_data,
	},
};
#endif


/* vibrator (child of FIQ) */

static struct resource gta02_vibrator_resources[] = {
	[0] = {
		.start	= GTA02_GPIO_VIBRATOR_ON,
		.end	= GTA02_GPIO_VIBRATOR_ON,
	},
};
struct gta02_vib_platform_data gta02_vib_pdata = {
	.enable_fiq = gta02_fiq_enable,
	.disable_fiq = gta02_fiq_disable,
	.kick_fiq = gta02_fiq_kick,
};

static struct platform_device gta02_vibrator_dev = {
	.name		= "gta02-vibrator",
	.num_resources	= ARRAY_SIZE(gta02_vibrator_resources),
	.resource	= gta02_vibrator_resources,
	.dev	 = {
		.platform_data = &gta02_vib_pdata,
	},
};

/* NOR Flash */

#define GTA02_FLASH_BASE	S3C2410_CS3 /* GCS3 */
#define GTA02_FLASH_SIZE	0x200000 /* 2MBytes */

static struct physmap_flash_data gta02_nor_flash_data = {
	.width		= 2,
};

static struct resource gta02_nor_flash_resource = {
	.start		= GTA02_FLASH_BASE,
	.end		= GTA02_FLASH_BASE + GTA02_FLASH_SIZE - 1,
	.flags		= IORESOURCE_MEM,
};

static struct platform_device gta02_nor_flash = {
	.name		= "physmap-flash",
	.id		= 0,
	.dev		= {
				.platform_data	= &gta02_nor_flash_data,
			},
	.resource	= &gta02_nor_flash_resource,
	.num_resources	= 1,
};


struct platform_device s3c24xx_pwm_device = {
	.name 		= "s3c24xx_pwm",
	.num_resources	= 0,
};

static struct i2c_board_info gta02_i2c_devs[] __initdata = {
	{
		I2C_BOARD_INFO("pcf50633", 0x73),
		.irq = GTA02_IRQ_PCF50633,
		.platform_data = &gta02_pcf_pdata,
	},
	{
		I2C_BOARD_INFO("wm8753", 0x1a),
	},
};

static struct s3c2410_nand_set gta02_nand_sets[] = {
	[0] = {
		.name		= "neo1973-nand",
		.nr_chips	= 1,
		.flags		= S3C2410_NAND_BBT,
	},
};

/* choose a set of timings derived from S3C@2442B MCP54 
 * data sheet (K5D2G13ACM-D075 MCP Memory)
 */

static struct s3c2410_platform_nand gta02_nand_info = {
	.tacls		= 0,
	.twrph0		= 25,
	.twrph1		= 15,
	.nr_sets	= ARRAY_SIZE(gta02_nand_sets),
	.sets		= gta02_nand_sets,
	.software_ecc	= 1,
};


static void gta02_s3c_mmc_set_power(unsigned char power_mode,
					unsigned short vdd)
{
	static int is_on = -1;
	int on;

	on = power_mode == MMC_POWER_ON || power_mode == MMC_POWER_UP;
	if (is_on != on)
		gta02_wlan_reset(!on);
	is_on = on;
}


static struct s3c24xx_mci_pdata gta02_s3c_mmc_cfg = {
	.set_power	= gta02_s3c_mmc_set_power,
};

static void gta02_udc_command(enum s3c2410_udc_cmd_e cmd)
{
	switch (cmd) {
	case S3C2410_UDC_P_ENABLE:
		printk(KERN_DEBUG "%s S3C2410_UDC_P_ENABLE\n", __func__);
		s3c2410_gpio_setpin(GTA02_GPIO_USB_PULLUP, 1);
		break;
	case S3C2410_UDC_P_DISABLE:
		printk(KERN_DEBUG "%s S3C2410_UDC_P_DISABLE\n", __func__);
		s3c2410_gpio_setpin(GTA02_GPIO_USB_PULLUP, 0);
		break;
	case S3C2410_UDC_P_RESET:
		printk(KERN_DEBUG "%s S3C2410_UDC_P_RESET\n", __func__);
		/* FIXME! */
		break;
	default:
		break;
	}
}

/* get PMU to set USB current limit accordingly */

static struct s3c2410_udc_mach_info gta02_udc_cfg = {
	.vbus_draw	= gta02_udc_vbus_draw,
	.udc_command	= gta02_udc_command,
/*	.get_vbus_status = gta02_udc_vbus_status,*/
};


/* Touchscreen configuration. */

#ifdef CONFIG_TOUCHSCREEN_FILTER
const static struct ts_filter_group_configuration gta02_ts_group = {
	.length = 12,
	.close_enough = 10,
	.threshold = 6,		/* At least half of the points in a group. */
	.attempts = 10,
};

const static struct ts_filter_median_configuration gta02_ts_median = {
	.extent = 20,
	.decimation_below = 3,
	.decimation_threshold = 8 * 3,
	.decimation_above = 4,
};

const static struct ts_filter_mean_configuration gta02_ts_mean = {
	.length = 4,
};

const static struct ts_filter_linear_configuration gta02_ts_linear = {
	.constants = {1, 0, 0, 0, 1, 0, 1},	/* Don't modify coords. */
	.coord0 = 0,
	.coord1 = 1,
};
#endif

const static struct ts_filter_chain_configuration gta02_filter_configuration[] =
{
#ifdef CONFIG_TOUCHSCREEN_FILTER
	{&ts_filter_group_api,		&gta02_ts_group.config},
	{&ts_filter_median_api,		&gta02_ts_median.config},
	{&ts_filter_mean_api,		&gta02_ts_mean.config},
	{&ts_filter_linear_api,		&gta02_ts_linear.config},
#endif
	{NULL, NULL},
};

const static struct s3c2410_ts_mach_info gta02_ts_cfg = {
	.delay = 10000,
	.presc = 0xff, /* slow as we can go */
	.filter_config = gta02_filter_configuration,
};



static void gta02_bl_set_intensity(int intensity)
{
	struct pcf50633 *pcf = gta02_pcf;
	int old_intensity = pcf50633_reg_read(pcf, PCF50633_REG_LEDOUT);
	int ret;

	/*
	 * One code path that leads here is from a kernel panic. Trying to turn
	 * the backlight on just gives us a nearly endless stream of complaints
	 * and accomplishes nothing. We can't win. Just give up.
	 *
	 * In the unlikely event that there's another path leading here while
	 * we're atomic, we print at least a warning.
	 */
	if (in_atomic()) {
		printk(KERN_ERR
		    "gta02_bl_set_intensity called while atomic\n");
		return;
	}

	if (!(pcf50633_reg_read(pcf, PCF50633_REG_LEDENA) & 3))
		old_intensity = 0;
	else
		old_intensity = pcf50633_reg_read(pcf, PCF50633_REG_LEDOUT);

	if (intensity == old_intensity)
		return;

	/* We can't do this anywhere else */
	pcf50633_reg_write(pcf, PCF50633_REG_LEDDIM, 5);

	/*
	 * The PCF50633 cannot handle LEDOUT = 0 (datasheet p60)
	 * if seen, you have to re-enable the LED unit
	 */
	if (!intensity || !old_intensity)
		pcf50633_reg_write(pcf, PCF50633_REG_LEDENA, 0);

	if (!intensity) /* illegal to set LEDOUT to 0 */
		ret = pcf50633_reg_set_bit_mask(pcf, PCF50633_REG_LEDOUT, 0x3f,
		2);
	else
		ret = pcf50633_reg_set_bit_mask(pcf, PCF50633_REG_LEDOUT, 0x3f,
						intensity);

	if (intensity)
		pcf50633_reg_write(pcf, PCF50633_REG_LEDENA, 2);

}

static struct generic_bl_info gta02_bl_info = {
	.name 			= "gta02-bl",
	.max_intensity 		= 0x3f,
	.default_intensity 	= 0x3f,
	.set_bl_intensity 	= gta02_bl_set_intensity,
};

static struct platform_device gta02_bl_dev = {
	.name		  = "generic-bl",
	.id		  = 1,
	.dev = {
		.platform_data = &gta02_bl_info,
	},
};

/* SPI: LCM control interface attached to Glamo3362 */

static void gta02_jbt6k74_reset(int devidx, int level)
{
	gpio_set_value(GTA02_GPIO_GLAMO(4), level);
}

static int jbt_probed = 0;

static void gta02_jbt6k74_probe_completed(struct device *dev)
{
	struct pcf50633 *pcf = gta02_pcf;
	if (!jbt_probed) {
	/* Switch on backlight. Qi does not do it for us */
	pcf50633_reg_write(pcf, PCF50633_REG_LEDOUT, 0x01);
	pcf50633_reg_write(pcf, PCF50633_REG_LEDENA, 0x00);
	pcf50633_reg_write(pcf, PCF50633_REG_LEDDIM, 0x01);
	pcf50633_reg_write(pcf, PCF50633_REG_LEDENA, 0x01);

	gta02_bl_dev.dev.parent = dev;
	platform_device_register(&gta02_bl_dev);
	jbt_probed = 1;
	}
}

const struct jbt6k74_platform_data jbt6k74_pdata = {
	.reset		= gta02_jbt6k74_reset,
	.probe_completed = gta02_jbt6k74_probe_completed,
};

/*----------- SPI: Accelerometers attached to SPI of s3c244x ----------------- */

void gta02_lis302dl_suspend_io(struct lis302dl_info *lis, int resume)
{
	struct lis302dl_platform_data *pdata = lis->pdata;

	if (!resume) {
		 /*
		 * we don't want to power them with a high level
		 * because GSENSOR_3V3 is not up during suspend
		 */
		s3c2410_gpio_setpin(pdata->pin_chip_select, 0);
		s3c2410_gpio_setpin(pdata->pin_clk, 0);
		s3c2410_gpio_setpin(pdata->pin_mosi, 0);
		/* misnomer: it is a pullDOWN in 2442 */
		s3c2410_gpio_pullup(pdata->pin_miso, 1);
		return;
	}

	/* back to normal */
	s3c2410_gpio_setpin(pdata->pin_chip_select, 1);
	s3c2410_gpio_setpin(pdata->pin_clk, 1);
	/* misnomer: it is a pullDOWN in 2442 */
	s3c2410_gpio_pullup(pdata->pin_miso, 0);

	s3c2410_gpio_cfgpin(pdata->pin_chip_select, S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_cfgpin(pdata->pin_clk, S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_cfgpin(pdata->pin_mosi, S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_cfgpin(pdata->pin_miso, S3C2410_GPIO_INPUT);

}

struct lis302dl_platform_data lis302_pdata_top = {
		.name		= "lis302-1 (top)",
		.pin_chip_select= S3C2410_GPD(12),
		.pin_clk	= S3C2410_GPG(7),
		.pin_mosi	= S3C2410_GPG(6),
		.pin_miso	= S3C2410_GPG(5),
		.interrupt	= GTA02_IRQ_GSENSOR_1,
		.open_drain	= 1, /* altered at runtime by PCB rev */
		.lis302dl_suspend_io = gta02_lis302dl_suspend_io,
};

struct lis302dl_platform_data lis302_pdata_bottom = {
		.name		= "lis302-2 (bottom)",
		.pin_chip_select= S3C2410_GPD(13),
		.pin_clk	= S3C2410_GPG(7),
		.pin_mosi	= S3C2410_GPG(6),
		.pin_miso	= S3C2410_GPG(5),
		.interrupt	= GTA02_IRQ_GSENSOR_2,
		.open_drain	= 1, /* altered at runtime by PCB rev */
		.lis302dl_suspend_io = gta02_lis302dl_suspend_io,
};

static struct spi_board_info gta02_spi_board_info[] = {
	{
		.modalias	= "jbt6k74",
		.platform_data	= &jbt6k74_pdata,
		.controller_data = (void*)GTA02_GPIO_GLAMO(12),
		/* irq */
		.max_speed_hz	= 100 * 1000,
		.bus_num	= 2,
		.chip_select = 0
	},
	{
		.modalias	= "lis302dl",
		/* platform_data */
		.platform_data	= &lis302_pdata_top,
		/* controller_data */
		/* irq */
		.max_speed_hz	= 100 * 1000,
		.bus_num	= 3,
		.chip_select	= 0,
	},

	{
		.modalias	= "lis302dl",
		/* platform_data */
		.platform_data	= &lis302_pdata_bottom,
		/* controller_data */
		/* irq */
		.max_speed_hz	= 100 * 1000,
		.bus_num	= 3,
		.chip_select	= 1,
	},

};

static void gta02_lis302_chip_select(struct s3c2410_spigpio_info *info, int csid, int cs)
{

	/*
	 * Huh... "quirk"... CS on this device is not really "CS" like you can
	 * expect.
	 *
	 * When it is 0 it selects SPI interface mode.
	 * When it is 1 it selects I2C interface mode.
	 *
	 * Because we have 2 devices on one interface we have to make sure
	 * that the "disabled" device (actually in I2C mode) don't think we're
	 * talking to it.
	 *
	 * When we talk to the "enabled" device, the "disabled" device sees
	 * the clocks as I2C clocks, creating havoc.
	 *
	 * I2C sees MOSI going LOW while CLK HIGH as a START action, thus we
	 * must ensure this is never issued.
	 */

	int cs_gpio, other_cs_gpio;

	cs_gpio = csid ? S3C2410_GPD(13) : S3C2410_GPD(12);
	other_cs_gpio = csid ? S3C2410_GPD(12) : S3C2410_GPD(13);


	if (cs == BITBANG_CS_ACTIVE) {
		s3c2410_gpio_setpin(other_cs_gpio, 1);
		s3c2410_gpio_setpin(cs_gpio, 1);
		s3c2410_gpio_setpin(info->pin_clk, 1);
		s3c2410_gpio_setpin(cs_gpio, 0);
	} else {
		s3c2410_gpio_setpin(cs_gpio, 1);
		s3c2410_gpio_setpin(other_cs_gpio, 1);
	}
}

static struct s3c2410_spigpio_info gta02_spigpio_cfg = {
	.pin_clk	= S3C2410_GPG(7),
	.pin_mosi	= S3C2410_GPG(6),
	.pin_miso	= S3C2410_GPG(5),
	.bus_num	= 3,
	.num_chipselect	= 2,
	.chip_select	= gta02_lis302_chip_select,
};

static struct platform_device gta02_spi_gpio_dev = {
	.name		= "spi_s3c24xx_gpio",
	.dev = {
		.platform_data = &gta02_spigpio_cfg,
	},
};

/*----------- / SPI: Accelerometers attached to SPI of s3c244x ----------------- */

static struct gpio_led gta02_gpio_leds[] = {
	{
		.name	= "gta02-power:orange",
		.gpio	= GTA02_GPIO_PWR_LED1,
	}, {
		.name	= "gta02-power:blue",
		.gpio	= GTA02_GPIO_PWR_LED2,
	}, {
		.name	= "gta02-aux:red",
		.gpio	= GTA02_GPIO_AUX_LED,
	},
};

static struct gpio_led_platform_data gta02_gpio_leds_pdata = {
	.leds = gta02_gpio_leds,
	.num_leds = ARRAY_SIZE(gta02_gpio_leds),
};

struct platform_device gta02_led_dev = {
	.name = "leds-gpio",
	.id   = -1,
	.dev = {
		.platform_data = &gta02_gpio_leds_pdata,
	},
};

static struct gpio_keys_button gta02_buttons[] = {
	{
		.gpio = GTA02_GPIO_AUX_KEY,
		.code = KEY_PHONE,
		.desc = "Aux",
		.type = EV_KEY,
		.debounce_interval = 100,
	},
	{
		.gpio = GTA02_GPIO_HOLD_KEY,
		.code = KEY_PAUSE,
		.desc = "Hold",
		.type = EV_KEY,
		.debounce_interval = 100,
	},
};

static struct gpio_keys_platform_data gta02_buttons_pdata = {
	.buttons = gta02_buttons,
	.nbuttons = ARRAY_SIZE(gta02_buttons),
};

static struct platform_device gta02_button_dev = {
	.name = "gpio-keys",
	.id = -1,
	.dev = {
		.platform_data = &gta02_buttons_pdata,
	},
};

static struct platform_device gta02_pm_usbhost_dev = {
	.name		= "gta02-pm-host",
};

/* USB */
static struct s3c2410_hcd_info gta02_usb_info = {
	.port[0]	= {
		.flags	= S3C_HCDFLG_USED,
	},
	.port[1]	= {
		.flags	= 0,
	},
};

/*
 * we crank down SD Card clock dynamically when GPS is powered
 */

static int gta02_glamo_mci_use_slow(void)
{
	return gta02_pm_gps_is_on();
}

static void gta02_glamo_external_reset(int level)
{
	s3c2410_gpio_setpin(GTA02_GPIO_3D_RESET, level);
	s3c2410_gpio_cfgpin(GTA02_GPIO_3D_RESET, S3C2410_GPIO_OUTPUT);
}

/*
static struct fb_videomode gta02_glamo_modes[] = {
	{
		.name = "480x640",
		.xres = 480,
		.yres = 640,
		.pixclock	= 40816,
		.left_margin	= 8,
		.right_margin	= 63,
		.upper_margin	= 2,
		.lower_margin	= 4,
		.hsync_len	= 8,
		.vsync_len	= 2,
		.vmode = FB_VMODE_NONINTERLACED,
	}, {
		.name = "240x320",
		.xres = 240,
		.yres = 320,
		.pixclock	= 40816,
		.left_margin	= 8,
		.right_margin	= 88,
		.upper_margin	= 2,
		.lower_margin	= 2,
		.hsync_len	= 8,
		.vsync_len	= 2,
		.vmode = FB_VMODE_NONINTERLACED,
	}
};*/

struct spi_gpio_platform_data spigpio_platform_data = {
	.sck = GTA02_GPIO_GLAMO(10),
	.mosi = GTA02_GPIO_GLAMO(11),
	.miso = GTA02_GPIO_GLAMO(5),
	.num_chipselect = 1,
};

static struct platform_device spigpio_device = {
	.name = "spi_gpio",
	.id   = 2,
	.dev = {
		.platform_data = &spigpio_platform_data,
	},
};

static void gta02_glamo_registered(struct device *dev)
{
	if (gpio_request(GTA02_GPIO_GLAMO(4), "jbt6k74 reset"))
		printk("gta02: Failed to request jbt6k74 reset pin\n");
	if (gpio_direction_output(GTA02_GPIO_GLAMO(4), 1))
		printk("gta02: Failed to configure jbt6k74 reset pin\n");
	spigpio_device.dev.parent = dev;
	platform_device_register(&spigpio_device);
}

static struct fb_videomode gta02_glamo_modes[] = {
	{
		.name = "480x640",
		.xres = 480,
		.yres = 640,
		.pixclock	= 40816,
		.left_margin	= 8,
		.right_margin	= 16,
		.upper_margin	= 2,
		.lower_margin	= 16,
		.hsync_len	= 8,
		.vsync_len	= 2,
		.vmode = FB_VMODE_NONINTERLACED,
	}, {
		.name = "240x320",
		.xres = 240,
		.yres = 320,
		.pixclock	= 40816,
		.left_margin	= 8,
		.right_margin	= 16,
		.upper_margin	= 2,
		.lower_margin	= 16,
		.hsync_len	= 8,
		.vsync_len	= 2,
		.vmode = FB_VMODE_NONINTERLACED,
	}
};


static struct glamo_fb_platform_data gta02_glamo_fb_pdata = {
	.width  = 43,
	.height = 58,

	.num_modes = ARRAY_SIZE(gta02_glamo_modes),
	.modes = gta02_glamo_modes,
};

static struct glamo_mmc_platform_data gta02_glamo_mmc_pdata = {
	.glamo_mmc_use_slow = gta02_glamo_mci_use_slow,
};

static struct glamo_platform_data gta02_glamo_pdata = {
	.fb_data   = &gta02_glamo_fb_pdata,
	.mmc_data  = &gta02_glamo_mmc_pdata,
	.gpio_base = GTA02_GPIO_GLAMO_BASE,

	.osci_clock_rate = 32768,

	.glamo_external_reset = gta02_glamo_external_reset,
	.registered = gta02_glamo_registered,
};

static struct resource gta02_glamo_resources[] = {
	[0] = {
		.start	= S3C2410_CS1,
		.end	= S3C2410_CS1 + 0x1000000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= GTA02_IRQ_3D,
		.end	= GTA02_IRQ_3D,
		.flags	= IORESOURCE_IRQ,
	},
	[2] = {
		.start = GTA02_GPIO_3D_RESET,
		.end   = GTA02_GPIO_3D_RESET,
	},
};

static struct platform_device gta02_glamo_dev = {
	.name		= "glamo3362",
	.num_resources	= ARRAY_SIZE(gta02_glamo_resources),
	.resource	= gta02_glamo_resources,
	.dev		= {
		.platform_data	= &gta02_glamo_pdata,
	},
};

static void __init gta02_map_io(void)
{
	s3c24xx_init_io(gta02_iodesc, ARRAY_SIZE(gta02_iodesc));
	s3c24xx_init_clocks(12000000);
	s3c24xx_init_uarts(gta02_uartcfgs, ARRAY_SIZE(gta02_uartcfgs));
}

static irqreturn_t gta02_modem_irq(int irq, void *param)
{
/*	printk(KERN_DEBUG "modem wakeup interrupt\n");*/
	gta_gsm_interrupts++;
	return IRQ_HANDLED;
}

static irqreturn_t ar6000_wow_irq(int irq, void *param)
{
/*	printk(KERN_DEBUG "ar6000_wow interrupt\n");*/
	return IRQ_HANDLED;
}

/*
 * hardware_ecc=1|0
 */
static char hardware_ecc_str[4] __initdata = "";

static int __init hardware_ecc_setup(char *str)
{
	if (str)
		strlcpy(hardware_ecc_str, str, sizeof(hardware_ecc_str));
	return 1;
}

__setup("hardware_ecc=", hardware_ecc_setup);

/* these are the guys that don't need to be children of PMU */

static struct platform_device *gta02_devices[] __initdata = {
	&s3c_device_i2c0,
	&s3c_device_usb,
	&s3c_device_wdt,
	&s3c_device_sdi,
	&s3c_device_usbgadget,
	&s3c_device_nand,
	&gta02_nor_flash,

	&s3c24xx_pwm_device,
	&gta02_led_dev,
	&gta02_pm_wlan_dev, /* not dependent on PMU */
	&s3c_device_iis,
};

/* these guys DO need to be children of PMU */

static struct platform_device *gta02_devices_pmu_children[] = {
	&s3c_device_ts, /* input 1 */
	&gta02_pm_gsm_dev,
	&gta02_pm_usbhost_dev,
	&gta02_spi_gpio_dev, /* input 2 and 3 */
	&gta02_button_dev, /* input 4 */
	&gta02_resume_reason_device,
};

static void gta02_register_glamo(void)
{
	platform_device_register(&gta02_glamo_dev);
}

static void gta02_pmu_regulator_registered(struct pcf50633 *pcf, int id)
{
	struct platform_device *regulator, *pdev;

	gta02_pcf = pcf;

	regulator = pcf->regulator_pdev[id];

	switch(id) {
		case PCF50633_REGULATOR_LDO4:
			pdev = &gta02_pm_bt_dev;
			break;
		case PCF50633_REGULATOR_LDO5:
			pdev = &gta02_pm_gps_dev;
			break;
		case PCF50633_REGULATOR_HCLDO:
			gta02_register_glamo();
			return;
		default:
			return;
	}

	pdev->dev.parent = &regulator->dev;
	platform_device_register(pdev);
}

/* this is called when pc50633 is probed, unfortunately quite late in the
 * day since it is an I2C bus device.  Here we can belatedly define some
 * platform devices with the advantage that we can mark the pcf50633 as the
 * parent.  This makes them get suspended and resumed with their parent
 * the pcf50633 still around.
 */

static void gta02_pmu_attach_child_devices(struct pcf50633 *pcf)
{
	int n;

	for (n = 0; n < ARRAY_SIZE(gta02_devices_pmu_children); n++)
		gta02_devices_pmu_children[n]->dev.parent = pcf->dev;

	platform_add_devices(gta02_devices_pmu_children,
					ARRAY_SIZE(gta02_devices_pmu_children));

	regulator_has_full_constraints();
}

static void gta02_poweroff(void)
{
	pcf50633_reg_set_bit_mask(gta02_pcf, PCF50633_REG_OOCSHDWN,
		  PCF50633_OOCSHDWN_GOSTDBY, PCF50633_OOCSHDWN_GOSTDBY);
}


/* On hardware rev 5 and earlier the leds are missing a resistor and reading
 * from their gpio pins will always return 0, so we have to shadow the
 * led states software */
static unsigned long gpb_shadow;
extern struct s3c_gpio_chip s3c24xx_gpios[];

static void gta02_gpb_set(struct gpio_chip *chip,
				unsigned offset, int value)
{
	void __iomem *base = S3C24XX_GPIO_BASE(S3C2410_GPB(0));
	unsigned long flags;
	unsigned long dat;

	local_irq_save(flags);

	dat = __raw_readl(base + 0x04) | gpb_shadow;
	dat &= ~(1 << offset);
	gpb_shadow &= ~(1 << offset);
	if (value) {
		dat |= 1 << offset;
		switch (offset) {
		case 0 ... 2:
			gpb_shadow |= 1 << offset;
			break;
		default:
			break;
		}
	}
	__raw_writel(dat, base + 0x04);

	local_irq_restore(flags);
}

static int gta02_gpb_get(struct gpio_chip *chip, unsigned offset)
{
	void __iomem *base = S3C24XX_GPIO_BASE(S3C2410_GPB(0));
	unsigned long val;

	val = __raw_readl(base + 0x04) | gpb_shadow;
	val >>= offset;
	val &= 1;

	return val;
}

static void gta02_hijack_gpb(void) {
	s3c24xx_gpios[1].chip.set = gta02_gpb_set;
	s3c24xx_gpios[1].chip.get = gta02_gpb_get;
}

static void __init gta02_machine_init(void)
{
	int rc;

	/* set the panic callback to make AUX blink fast */
	panic_blink = gta02_panic_blink;

	switch (S3C_SYSTEM_REV_ATAG) {
	case GTA02v6_SYSTEM_REV:
		/* we need push-pull interrupt from motion sensors */
		lis302_pdata_top.open_drain = 0;
		lis302_pdata_bottom.open_drain = 0;
		break;
	default:
		break;
	}
	if (S3C_SYSTEM_REV_ATAG <= GTA02v5_SYSTEM_REV)
		gta02_hijack_gpb();

#ifdef CONFIG_CHARGER_PCF50633
	INIT_DELAYED_WORK(&gta02_charger_work, gta02_charger_worker);
#endif

	/* Glamo chip select optimization */
/*	 *((u32 *)(S3C2410_MEMREG(((1 + 1) << 2)))) = 0x1280; */

	/* do not force soft ecc if we are asked to use hardware_ecc */
	if (hardware_ecc_str[0] == '1')
		gta02_nand_info.software_ecc = 0;

	s3c_device_usb.dev.platform_data = &gta02_usb_info;
	s3c_device_nand.dev.platform_data = &gta02_nand_info;
	s3c_device_sdi.dev.platform_data = &gta02_s3c_mmc_cfg;

	/* acc sensor chip selects */
	s3c2410_gpio_setpin(S3C2410_GPD(12), 1);
	s3c2410_gpio_cfgpin(S3C2410_GPD(12), S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_setpin(S3C2410_GPD(13), 1);
	s3c2410_gpio_cfgpin(S3C2410_GPD(13), S3C2410_GPIO_OUTPUT);


	s3c24xx_udc_set_platdata(&gta02_udc_cfg);
	s3c_i2c0_set_platdata(NULL);
	set_s3c2410ts_info(&gta02_ts_cfg);

	i2c_register_board_info(0, gta02_i2c_devs, ARRAY_SIZE(gta02_i2c_devs));
	spi_register_board_info(gta02_spi_board_info,
				ARRAY_SIZE(gta02_spi_board_info));

	platform_add_devices(gta02_devices, ARRAY_SIZE(gta02_devices));

	s3c_pm_init();

	/* Make sure the modem can wake us up */
	set_irq_type(GTA02_IRQ_MODEM, IRQ_TYPE_EDGE_RISING);
	rc = request_irq(GTA02_IRQ_MODEM, gta02_modem_irq, IRQF_DISABLED,
			 "modem", NULL);
	if (rc < 0)
		printk(KERN_ERR "GTA02: can't request GSM modem wakeup IRQ\n");
	enable_irq_wake(GTA02_IRQ_MODEM);

	/* Make sure the wifi module can wake us up*/
	set_irq_type(GTA02_IRQ_WLAN_GPIO1, IRQ_TYPE_EDGE_RISING);
	rc = request_irq(GTA02_IRQ_WLAN_GPIO1, ar6000_wow_irq, IRQF_DISABLED,
			"ar6000", NULL);

	if (rc < 0)
		printk(KERN_ERR "GTA02: can't request ar6k wakeup IRQ\n");
	enable_irq_wake(GTA02_IRQ_WLAN_GPIO1);

	pm_power_off = gta02_poweroff;

	/* Register the HDQ and vibrator as children of pwm device */
#ifdef CONFIG_HDQ_GPIO_BITBANG
	gta02_hdq_device.dev.parent = &s3c24xx_pwm_device.dev;
	platform_device_register(&gta02_hdq_device);
#endif
	gta02_vibrator_dev.dev.parent = &s3c24xx_pwm_device.dev;
	platform_device_register(&gta02_vibrator_dev);
}

MACHINE_START(NEO1973_GTA02, "GTA02")
	.phys_io	= S3C2410_PA_UART,
	.io_pg_offst	= (((u32)S3C24XX_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S3C2410_SDRAM_PA + 0x100,
	.map_io		= gta02_map_io,
	.init_irq	= s3c24xx_init_irq,
	.init_machine	= gta02_machine_init,
	.timer		= &s3c24xx_timer,
MACHINE_END
