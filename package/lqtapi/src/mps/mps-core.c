#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <ifxmips.h>
#include <ifxmips_cgu.h>

#include <ifxmips_prom.h>
#include <ifxmips_irq.h>

#include "mps.h"

extern int mps_irq_init(struct mps *mps);
extern void mps_irq_exit(struct mps *mps);

#define MPS_CPU0_BOOT_RVEC	0x1c0
#define MPS_CPU0_BOOT_NVEC	0x1c4
#define MPS_CPU0_BOOT_EVEC	0x1c8
#define MPS_CPU0_CP0_STATUS	0x1cc
#define MPS_CPU0_CP0_EEPC	0x1d0
#define MPS_CPU0_CP0_EPC	0x1d4
#define MPS_CPU0_BOOT_SIZE	0x1d8
#define MPS_CPU0_CFG_STAT	0x1dc
#define MPS_CPU1_BOOT_RVEC	0x1e0
#define MPS_CPU1_BOOT_NVEC	0x1e4
#define MPS_CPU1_BOOT_EVEC	0x1e8
#define MPS_CPU1_CP0_STATUS	0x1ec
#define MPS_CPU1_CP0_EEPC	0x1f0
#define MPS_CPU1_CP0_EPC	0x1f4
#define MPS_CPU1_BOOT_SIZE	0x1f8
#define MPS_CPU1_CFG_STAT	0x1fc

static void mps_reset(void)
{
	ifxmips_w32(ifxmips_r32(IFXMIPS_RCU_RST) | IFXMIPS_RCU_RST_CPU1,
		IFXMIPS_RCU_RST);
	smp_wmb();
}

static void mps_release(void)
{
	uint32_t val;
	val = ifxmips_r32(IFXMIPS_RCU_RST);
	val |= 0x20000000;
	val &= ~IFXMIPS_RCU_RST_CPU1;
	ifxmips_w32(val, IFXMIPS_RCU_RST);
	smp_wmb();
}

void mps_load_firmware(struct mps *mps, const void *data, size_t size,
	enum mps_boot_config config)
{
	uint32_t cfg = 0;
	uint32_t fw_size = size;

	if (config == MPS_BOOT_LEGACY) {
		cfg = 0x00020000;
		fw_size -= sizeof(uint32_t);
	} else {
		if(config == MPS_BOOT_ENCRYPTED) {
			cfg = __raw_readl(mps->mbox_base + MPS_CPU1_CFG_STAT);
			cfg |= 0x00700000;
		} else {
			printk("PANIC!\n");
		}
	}

	mps_reset();

	memcpy_toio(mps->cp1_base, data, size);

	__raw_writel(cfg, mps->mbox_base + MPS_CPU1_CFG_STAT);
	__raw_writel(fw_size, mps->mbox_base + MPS_CPU1_BOOT_SIZE);
	__raw_writel((uint32_t)mps->cp1_base, mps->mbox_base + MPS_CPU1_BOOT_RVEC);

	mps_release();
}
EXPORT_SYMBOL_GPL(mps_load_firmware);

void mps_configure_fifo(struct mps *mps, struct mps_fifo *fifo,
	const struct mps_fifo_config *config)
{
	mps_fifo_init(fifo, mps->mbox_base + config->base,
		mps->mbox_base + config->head_addr,
		mps->mbox_base + config->tail_addr,
		config->size);

	__raw_writel(config->size, mps->mbox_base + config->size_addr);
	__raw_writel(mps->mbox_res->start + config->base,
		mps->mbox_base + config->base_addr);
}
EXPORT_SYMBOL_GPL(mps_configure_fifo);

void mps_configure_mailbox(struct mps *mps, struct mps_mailbox *mbox,
	const struct mps_fifo_config *upstream_config,
	const struct mps_fifo_config *downstream_config)
{
	mps_configure_fifo(mps, &mbox->upstream, upstream_config);
	mps_configure_fifo(mps, &mbox->downstream, downstream_config);
}
EXPORT_SYMBOL_GPL(mps_configure_mailbox);

static int __devinit mps_probe(struct platform_device *pdev)
{
	int ret;
	struct mps *mps;
	struct resource *res;

	mps = kzalloc(sizeof(*mps), GFP_KERNEL);

	if (!mps)
		return -ENOMEM;

	mps->dev = &pdev->dev;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mem");
	if (!res) {
		dev_err(&pdev->dev, "Failed to get mem resource");
		ret = -ENOENT;
		goto err_free;
	}

	res = request_mem_region(res->start, resource_size(res),
		dev_name(&pdev->dev));

	if (!res) {
		dev_err(&pdev->dev, "Failed to request mem resource");
		ret = -EBUSY;
		goto err_free;
	}

	mps->base = ioremap_nocache(res->start, resource_size(res));

	if (!mps->base) {
		dev_err(&pdev->dev, "Failed to ioremap mem region\n");
		ret = -EBUSY;
		goto err_release_mem_region;
	}

	mps->res = res;


	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mailbox");
	if (!res) {
		dev_err(&pdev->dev, "Failed to get mailbox mem region\n");
		ret = -ENOENT;
		goto err_free;
	}

	res = request_mem_region(res->start, resource_size(res),
		dev_name(&pdev->dev));

	if (!res) {
		dev_err(&pdev->dev, "Failed to request mailbox mem region\n");
		ret = -EBUSY;
		goto err_free;
	}

	mps->mbox_base = ioremap_nocache(res->start, resource_size(res));

	if (!mps->mbox_base) {
		dev_err(&pdev->dev, "Failed to ioremap mailbox mem region\n");
		ret = -EBUSY;
		goto err_release_mem_region;
	}

	mps->mbox_res = res;

	mps->cp1_base = ioremap_nocache((unsigned int)pdev->dev.platform_data, 1 << 20);

	if (!mps->cp1_base) {
		dev_err(&pdev->dev, "Failed to ioremap cp1 address\n");
		ret = -EBUSY;
		goto err_release_mem_region;
	}

	mps->irq_ad0 = INT_NUM_IM4_IRL18;
	mps->irq_ad1 = INT_NUM_IM4_IRL19;
	mps->irq_base = 160;

	ret = mps_irq_init(mps);
	if (ret < 0)
		goto err_iounmap;

	platform_set_drvdata(pdev, mps);

	return 0;

err_iounmap:
	iounmap(mps->mbox_base);
err_release_mem_region:
	release_mem_region(res->start, resource_size(res));
err_free:
	kfree(mps);

	return ret;
}

static int __devexit mps_remove(struct platform_device *pdev)
{
	struct mps *mps = platform_get_drvdata(pdev);

	mps_irq_exit(mps);

	iounmap(mps->mbox_base);
	release_mem_region(mps->mbox_res->start, resource_size(mps->mbox_res));
	iounmap(mps->base);
	release_mem_region(mps->res->start, resource_size(mps->res));

	kfree(mps);
	return 0;
}

static struct platform_driver mps_driver = {
	.probe = mps_probe,
	.remove = __devexit_p(mps_remove),
	.driver = {
		.name = "mps",
		.owner = THIS_MODULE
	},
};

static int __init mps_init(void)
{
	return platform_driver_register(&mps_driver);
}
module_init(mps_init);

static void __exit mps_exit(void)
{
	platform_driver_unregister(&mps_driver);
}
module_exit(mps_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
