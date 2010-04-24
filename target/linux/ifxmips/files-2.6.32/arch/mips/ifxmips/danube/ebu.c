#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/gpio.h>

#include <ifxmips.h>
#include <ifxmips_ebu.h>

#define IFXMIPS_EBU_BUSCON	0x1e7ff
#define IFXMIPS_EBU_WP		0x80000000

static int shadow = 0;
static void __iomem *virt;

static int
ifxmips_ebu_direction_output(struct gpio_chip *chip, unsigned offset, int value)
{
	return 0;
}

static void
ifxmips_ebu_set(struct gpio_chip *chip, unsigned offset, int value)
{
	unsigned long flags;
	if(value)
		shadow |= (1 << offset);
	else
		shadow &= ~(1 << offset);
	spin_lock_irqsave(&ebu_lock, flags);
	ifxmips_w32(IFXMIPS_EBU_BUSCON, IFXMIPS_EBU_BUSCON1);
	*((__u16*)virt) = shadow;
	ifxmips_w32(IFXMIPS_EBU_BUSCON | IFXMIPS_EBU_WP, IFXMIPS_EBU_BUSCON1);
	spin_unlock_irqrestore(&ebu_lock, flags);
}

static struct gpio_chip
ifxmips_ebu_chip =
{
	.label = "ifxmips_ebu",
	.direction_output = ifxmips_ebu_direction_output,
	.set = ifxmips_ebu_set,
	.base = 32,
	.ngpio = 16,
	.can_sleep = 1,
	.owner = THIS_MODULE,
};

static int __devinit
ifxmips_ebu_probe(struct platform_device *pdev)
{
	ifxmips_w32(pdev->resource->start | 0x1, IFXMIPS_EBU_ADDRSEL1);
	ifxmips_w32(IFXMIPS_EBU_BUSCON | IFXMIPS_EBU_WP, IFXMIPS_EBU_BUSCON1);
	virt = ioremap_nocache(pdev->resource->start, pdev->resource->end);
	if(gpiochip_add(&ifxmips_ebu_chip))
		return -EINVAL;
	shadow = (int) pdev->dev.platform_data;
	printk("IFXMIPS: ebu-gpio loaded\n");
	return 0;
}

static int
ifxmips_ebu_remove(struct platform_device *dev)
{
	return gpiochip_remove(&ifxmips_ebu_chip);
}

static struct platform_driver
ifxmips_ebu_driver = {
	.probe = ifxmips_ebu_probe,
	.remove = ifxmips_ebu_remove,
	.driver = {
		.name = "ifxmips_ebu",
		.owner = THIS_MODULE,
	},
};

static int __init
ifxmips_ebu_init(void)
{
	return platform_driver_register(&ifxmips_ebu_driver);
}

static void __exit
ifxmips_ebu_exit(void)
{
	platform_driver_unregister(&ifxmips_ebu_driver);
}

module_init(ifxmips_ebu_init);
module_exit(ifxmips_ebu_exit);

MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("ifxmips - EBU Latch GPIO-Expander");
