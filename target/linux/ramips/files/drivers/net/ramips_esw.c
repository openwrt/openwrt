#include <linux/ioport.h>

#include <rt305x_regs.h>
#include <rt305x_esw_platform.h>

#define RT305X_ESW_PHY_WRITE		(1 << 13)
#define RT305X_ESW_PHY_TOUT			(5 * HZ)
#define RT305X_ESW_PHY_CONTROL_0	0xC0
#define RT305X_ESW_PHY_CONTROL_1	0xC4

struct rt305x_esw {
	void __iomem *base;
	struct rt305x_esw_platform_data *pdata;
};

static inline void
ramips_esw_wr(struct rt305x_esw *esw, u32 val, unsigned reg)
{
	__raw_writel(val, esw->base + reg);
}

static inline u32
ramips_esw_rr(struct rt305x_esw *esw, unsigned reg)
{
	return __raw_readl(esw->base + reg);
}

u32
mii_mgr_write(struct rt305x_esw *esw, u32 phy_addr, u32 phy_register,
	      u32 write_data)
{
	unsigned long volatile t_start = jiffies;
	int ret = 0;

	while(1)
	{
		if(!(ramips_esw_rr(esw, RT305X_ESW_PHY_CONTROL_1) & (0x1 << 0)))
			break;
		if(time_after(jiffies, t_start + RT305X_ESW_PHY_TOUT))
		{
			ret = 1;
			goto out;
		}
	}
	ramips_esw_wr(esw, ((write_data & 0xFFFF) << 16) | (phy_register << 8) |
		(phy_addr) | RT305X_ESW_PHY_WRITE, RT305X_ESW_PHY_CONTROL_0);
	t_start = jiffies;
	while(1)
	{
		if(ramips_esw_rr(esw, RT305X_ESW_PHY_CONTROL_1) & (0x1 << 0))
			break;
		if(time_after(jiffies, t_start + RT305X_ESW_PHY_TOUT))
		{
			ret = 1;
			break;
		}
	}
out:
	if(ret)
		printk(KERN_ERR "ramips_eth: MDIO timeout\n");
	return ret;
}

static void
rt305x_esw_hw_init(struct rt305x_esw *esw)
{
	int i;

	/* vodoo from original driver */
	ramips_esw_wr(esw, 0xC8A07850, 0x08);
	ramips_esw_wr(esw, 0x00000000, 0xe4);
	ramips_esw_wr(esw, 0x00405555, 0x14);
	ramips_esw_wr(esw, 0x00002001, 0x50);
	ramips_esw_wr(esw, 0x00007f7f, 0x90);
	ramips_esw_wr(esw, 0x00007f3f, 0x98);
	ramips_esw_wr(esw, 0x00d6500c, 0xcc);
	ramips_esw_wr(esw, 0x0008a301, 0x9c);
	ramips_esw_wr(esw, 0x02404040, 0x8c);
	ramips_esw_wr(esw, 0x00001002, 0x48);
	ramips_esw_wr(esw, 0x3f502b28, 0xc8);
	ramips_esw_wr(esw, 0x00000000, 0x84);

	mii_mgr_write(esw, 0, 31, 0x8000);
	for(i = 0; i < 5; i++)
	{
		mii_mgr_write(esw, i, 0, 0x3100);   //TX10 waveform coefficient
		mii_mgr_write(esw, i, 26, 0x1601);   //TX10 waveform coefficient
		mii_mgr_write(esw, i, 29, 0x7058);   //TX100/TX10 AD/DA current bias
		mii_mgr_write(esw, i, 30, 0x0018);   //TX100 slew rate control
	}
	/* PHY IOT */
	mii_mgr_write(esw, 0, 31, 0x0);      //select global register
	mii_mgr_write(esw, 0, 22, 0x052f);   //tune TP_IDL tail and head waveform
	mii_mgr_write(esw, 0, 17, 0x0fe0);   //set TX10 signal amplitude threshold to minimum
	mii_mgr_write(esw, 0, 18, 0x40ba);   //set squelch amplitude to higher threshold
	mii_mgr_write(esw, 0, 14, 0x65);     //longer TP_IDL tail length
	mii_mgr_write(esw, 0, 31, 0x8000);   //select local register

	/* set default vlan */
	ramips_esw_wr(esw, 0x2001, 0x50);
	ramips_esw_wr(esw, 0x504f, 0x70);
}

static int
rt305x_esw_probe(struct platform_device *pdev)
{
	struct rt305x_esw_platform_data *pdata;
	struct rt305x_esw *esw;
	struct resource *res;
	int err;

	pdata = pdev->dev.platform_data;
	if (!pdata)
		return -EINVAL;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "no memory resource found\n");
		return -ENOMEM;
	}

	esw = kzalloc(sizeof (struct rt305x_esw), GFP_KERNEL);
	if (!esw) {
		dev_err(&pdev->dev, "no memory for private data\n");
		return -ENOMEM;
	}

	esw->base = ioremap(res->start, resource_size(res));
	if (!esw->base) {
		dev_err(&pdev->dev, "ioremap failed\n");
		err = -ENOMEM;
		goto free_esw;
	}

	platform_set_drvdata(pdev, esw);

	esw->pdata = pdata;
	rt305x_esw_hw_init(esw);

	return 0;

free_esw:
	kfree(esw);
	return err;
}

static int
rt305x_esw_remove(struct platform_device *pdev)
{
	struct rt305x_esw *esw;

	esw = platform_get_drvdata(pdev);
	if (esw) {
		platform_set_drvdata(pdev, NULL);
		iounmap(esw->base);
		kfree(esw);
	}

	return 0;
}

static struct platform_driver rt305x_esw_driver = {
	.probe = rt305x_esw_probe,
	.remove = rt305x_esw_remove,
	.driver = {
		.name = "rt305x-esw",
		.owner = THIS_MODULE,
	},
};

static int __init
rt305x_esw_init(void)
{
	return platform_driver_register(&rt305x_esw_driver);
}

static void __exit
rt305x_esw_exit(void)
{
	platform_driver_unregister(&rt305x_esw_driver);
}
