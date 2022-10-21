#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/io.h>
#include <linux/memblock.h>
#include <linux/delay.h>
#include <linux/printk.h>
#include <asm/io.h>

#define RBUS_SYNC 0x0001A020

static void __iomem *rbus_addr = NULL;

static __init int rtk_rbus_init(void)
{
	struct device_node *node;
	const __be32 *addrp;
	u64 base, size;

	node = of_find_node_by_path("/soc/bus@98000000");

	if (WARN_ON(!node))
		return -EINVAL;

	addrp = of_get_address(node, 0, &size, NULL);

	if (!addrp)
		return -EINVAL;

	base = of_translate_address(node, addrp);

	rbus_addr = ioremap(base, size);

	return 0;
}

void rtk_bus_sync(void)
{
	if(rbus_addr)
	{
		writel_relaxed(0x00001234, rbus_addr + RBUS_SYNC);
		dsb(st);
	}
}
EXPORT_SYMBOL(rtk_bus_sync);

arch_initcall(rtk_rbus_init);
