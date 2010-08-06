#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/physmap.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/etherdevice.h>
#include <linux/reboot.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/leds.h>

#include <asm/bootinfo.h>
#include <asm/irq.h>

#include <ifxmips.h>
#include <ifxmips_irq.h>

/* gpio leds */
#ifdef CONFIG_LEDS_GPIO
static struct gpio_led_platform_data ifxmips_gpio_led_data;

static struct platform_device ifxmips_gpio_leds =
{
	.name = "leds-gpio",
	.dev = {
		.platform_data = (void *) &ifxmips_gpio_led_data,
	}
};

void __init
ifxmips_register_gpio_leds(struct gpio_led *leds, int cnt)
{
	ifxmips_gpio_led_data.leds = leds;
	ifxmips_gpio_led_data.num_leds = cnt;
	platform_device_register(&ifxmips_gpio_leds);
}
#endif

/* leds */
static struct gpio_led_platform_data ifxmips_led_data;

static struct platform_device ifxmips_led =
{
	.name = "ifxmips_led",
	.dev = {
		.platform_data = (void *) &ifxmips_led_data,
	}
};

void __init
ifxmips_register_leds(struct gpio_led *leds, int cnt)
{
	ifxmips_led_data.leds = leds;
	ifxmips_led_data.num_leds = cnt;
	platform_device_register(&ifxmips_led);
}

/* mtd flash */
static struct resource ifxmips_mtd_resource =
{
	.start  = IFXMIPS_FLASH_START,
	.end    = IFXMIPS_FLASH_START + IFXMIPS_FLASH_MAX - 1,
	.flags  = IORESOURCE_MEM,
};

static struct platform_device ifxmips_mtd =
{
	.name			= "ifxmips_mtd",
	.resource		= &ifxmips_mtd_resource,
	.num_resources	= 1,
};

void __init
ifxmips_register_mtd(struct physmap_flash_data *pdata)
{
	ifxmips_mtd.dev.platform_data = pdata;
	platform_device_register(&ifxmips_mtd);
}

/* watchdog */
static struct resource ifxmips_wdt_resource =
{
	.start  = IFXMIPS_WDT_BASE_ADDR,
	.end    = IFXMIPS_WDT_BASE_ADDR + IFXMIPS_WDT_SIZE - 1,
	.flags  = IORESOURCE_MEM,
};

static struct platform_device ifxmips_wdt =
{
	.name			= "ifxmips_wdt",
	.resource		= &ifxmips_wdt_resource,
	.num_resources = 1,
};

void __init
ifxmips_register_wdt(void)
{
	platform_device_register(&ifxmips_wdt);
}

/* gpio */
static struct platform_device ifxmips_gpio0 =
{
	.name = "ifxmips_gpio",
};

static struct platform_device ifxmips_gpio1 =
{
	.name = "ifxmips_gpio1",
};

void __init
ifxmips_register_gpio(void)
{
	platform_device_register(&ifxmips_gpio0);
	platform_device_register(&ifxmips_gpio1);
}
