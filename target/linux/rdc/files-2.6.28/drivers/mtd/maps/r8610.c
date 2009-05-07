/*
 * Flash memory access on RDC R8610 Evaluation board
 *
 * (C) 2009, Florian Fainelli
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

static struct map_info r8610_map = {
	.name		= "r8610",
	.size		= CONFIG_MTD_RDC3210_SIZE,
	.bankwidth	= CONFIG_MTD_RDC3210_BUSWIDTH,
};

static struct mtd_partition r8610_partitions[] = {
        {
                .name = "Kernel",
                .size = 0x001f0000,
                .offset = 0
        },{
                .name = "Config",
                .size = 0x10000,
		.offset = MTDPART_OFS_APPEND,
        },{
                .name = "Initrd",
		.size = 0x1E0000, 
		.offset = MTDPART_OFS_APPEND,
        },{
		.name = "Redboot",
		.size = 0x20000,
		.offset = MTDPART_OFS_APPEND,
		.mask_flags = MTD_WRITEABLE
	}
};

static struct mtd_info *mymtd;

int __init r8610_mtd_init(void)
{
	struct mtd_partition *parts;
	int nb_parts = 0;

	/*
	 * Static partition definition selection
	 */
	parts = r8610_partitions;
	nb_parts = ARRAY_SIZE(r8610_partitions);

	/*
	 * Now let's probe for the actual flash.  Do it here since
	 * specific machine settings might have been set above.
	 */
	r8610_map.phys = -r8610_map.size;
	printk(KERN_NOTICE "r8610: flash device: %lx at %x\n", r8610_map.size, r8610_map.phys);

	r8610_map.map_priv_1 = (unsigned long)(r8610_map.virt = ioremap_nocache(r8610_map.phys, r8610_map.size));
	if (!r8610_map.map_priv_1) {
                printk(KERN_ERR "Failed to ioremap\n");
                return -EIO;
	}

	mymtd = do_map_probe("cfi_probe", &r8610_map);
	if (!mymtd) {
		iounmap(r8610_map.virt);
		return -ENXIO;
	}
	mymtd->owner = THIS_MODULE;

	add_mtd_partitions(mymtd, parts, nb_parts);

	return 0;
}

static void __exit r8610_mtd_cleanup(void)
{
	if (mymtd) {
		del_mtd_partitions(mymtd);
		map_destroy(mymtd);
		iounmap(r8610_map.virt);
	}
}

module_init(r8610_mtd_init);
module_exit(r8610_mtd_cleanup);

MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("RDC R8610 MTD driver");
MODULE_LICENSE("GPL");
