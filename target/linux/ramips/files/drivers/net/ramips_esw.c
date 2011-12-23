#include <linux/ioport.h>

#include <rt305x_regs.h>
#include <rt305x_esw_platform.h>

#define RT305X_ESW_REG_FCT0		0x08
#define RT305X_ESW_REG_PFC1		0x14
#define RT305X_ESW_REG_PVIDC(_n)	(0x40 + 4 * (_n))
#define RT305X_ESW_REG_VLANI(_n)	(0x50 + 4 * (_n))
#define RT305X_ESW_REG_VMSC(_n)		(0x70 + 4 * (_n))
#define RT305X_ESW_REG_FPA		0x84
#define RT305X_ESW_REG_SOCPC		0x8c
#define RT305X_ESW_REG_POC1		0x90
#define RT305X_ESW_REG_POC2		0x94
#define RT305X_ESW_REG_POC3		0x98
#define RT305X_ESW_REG_SGC		0x9c
#define RT305X_ESW_REG_PCR0		0xc0
#define RT305X_ESW_REG_PCR1		0xc4
#define RT305X_ESW_REG_FPA2		0xc8
#define RT305X_ESW_REG_FCT2		0xcc
#define RT305X_ESW_REG_SGC2		0xe4
#define RT305X_ESW_REG_P0LED	0xa4
#define RT305X_ESW_REG_P1LED	0xa8
#define RT305X_ESW_REG_P2LED	0xac
#define RT305X_ESW_REG_P3LED	0xb0
#define RT305X_ESW_REG_P4LED	0xb4

#define RT305X_ESW_PCR0_WT_NWAY_DATA_S	16
#define RT305X_ESW_PCR0_WT_PHY_CMD	BIT(13)
#define RT305X_ESW_PCR0_CPU_PHY_REG_S	8

#define RT305X_ESW_PCR1_WT_DONE		BIT(0)

#define RT305X_ESW_PHY_TIMEOUT		(5 * HZ)

#define RT305X_ESW_PVIDC_PVID_M		0xfff
#define RT305X_ESW_PVIDC_PVID_S		12

#define RT305X_ESW_VLANI_VID_M		0xfff
#define RT305X_ESW_VLANI_VID_S		12

#define RT305X_ESW_VMSC_MSC_M		0xff
#define RT305X_ESW_VMSC_MSC_S		8

#define RT305X_ESW_SOCPC_DISUN2CPU_S	0
#define RT305X_ESW_SOCPC_DISMC2CPU_S	8
#define RT305X_ESW_SOCPC_DISBC2CPU_S	16
#define RT305X_ESW_SOCPC_CRC_PADDING	BIT(25)

#define RT305X_ESW_POC1_EN_BP_S		0
#define RT305X_ESW_POC1_EN_FC_S		8
#define RT305X_ESW_POC1_DIS_RMC2CPU_S	16
#define RT305X_ESW_POC1_DIS_PORT_S	23

#define RT305X_ESW_POC3_UNTAG_EN_S	0
#define RT305X_ESW_POC3_ENAGING_S	8
#define RT305X_ESW_POC3_DIS_UC_PAUSE_S	16

#define RT305X_ESW_PORT0		0
#define RT305X_ESW_PORT1		1
#define RT305X_ESW_PORT2		2
#define RT305X_ESW_PORT3		3
#define RT305X_ESW_PORT4		4
#define RT305X_ESW_PORT5		5
#define RT305X_ESW_PORT6		6

#define RT305X_ESW_PORTS_INTERNAL					\
		(BIT(RT305X_ESW_PORT0) | BIT(RT305X_ESW_PORT1) |	\
		 BIT(RT305X_ESW_PORT2) | BIT(RT305X_ESW_PORT3) |	\
		 BIT(RT305X_ESW_PORT4))

#define RT305X_ESW_PORTS_NOCPU	\
		(RT305X_ESW_PORTS_INTERNAL | BIT(RT305X_ESW_PORT5))

#define RT305X_ESW_PORTS_CPU	BIT(RT305X_ESW_PORT6)

#define RT305X_ESW_PORTS_ALL	\
		(RT305X_ESW_PORTS_NOCPU | RT305X_ESW_PORTS_CPU)

#define RT305X_ESW_NUM_VLANS	16
#define RT305X_ESW_NUM_PORTS	7

struct rt305x_esw {
	void __iomem *base;
	struct rt305x_esw_platform_data *pdata;
	spinlock_t reg_rw_lock;
};

static inline void
rt305x_esw_wr(struct rt305x_esw *esw, u32 val, unsigned reg)
{
	__raw_writel(val, esw->base + reg);
}

static inline u32
rt305x_esw_rr(struct rt305x_esw *esw, unsigned reg)
{
	return __raw_readl(esw->base + reg);
}

static inline void
rt305x_esw_rmw_raw(struct rt305x_esw *esw, unsigned reg, unsigned long mask,
		   unsigned long val)
{
	unsigned long t;

	t = __raw_readl(esw->base + reg) & ~mask;
	__raw_writel(t | val, esw->base + reg);
}

static void
rt305x_esw_rmw(struct rt305x_esw *esw, unsigned reg, unsigned long mask,
	       unsigned long val)
{
	unsigned long flags;

	spin_lock_irqsave(&esw->reg_rw_lock, flags);
	rt305x_esw_rmw_raw(esw, reg, mask, val);
	spin_unlock_irqrestore(&esw->reg_rw_lock, flags);
}

static u32
rt305x_mii_write(struct rt305x_esw *esw, u32 phy_addr, u32 phy_register,
		 u32 write_data)
{
	unsigned long t_start = jiffies;
	int ret = 0;

	while (1) {
		if (!(rt305x_esw_rr(esw, RT305X_ESW_REG_PCR1) &
		      RT305X_ESW_PCR1_WT_DONE))
			break;
		if (time_after(jiffies, t_start + RT305X_ESW_PHY_TIMEOUT)) {
			ret = 1;
			goto out;
		}
	}

	write_data &= 0xffff;
	rt305x_esw_wr(esw,
		      (write_data << RT305X_ESW_PCR0_WT_NWAY_DATA_S) |
		      (phy_register << RT305X_ESW_PCR0_CPU_PHY_REG_S) |
		      (phy_addr) | RT305X_ESW_PCR0_WT_PHY_CMD,
		      RT305X_ESW_REG_PCR0);

	t_start = jiffies;
	while (1) {
		if (rt305x_esw_rr(esw, RT305X_ESW_REG_PCR1) &
		    RT305X_ESW_PCR1_WT_DONE)
			break;

		if (time_after(jiffies, t_start + RT305X_ESW_PHY_TIMEOUT)) {
			ret = 1;
			break;
		}
	}
out:
	if (ret)
		printk(KERN_ERR "ramips_eth: MDIO timeout\n");
	return ret;
}

static void
rt305x_esw_set_vlan_id(struct rt305x_esw *esw, unsigned vlan, unsigned vid)
{
	unsigned s;

	s = RT305X_ESW_VLANI_VID_S * (vlan % 2);
	rt305x_esw_rmw(esw,
		       RT305X_ESW_REG_VLANI(vlan / 2),
		       RT305X_ESW_VLANI_VID_M << s,
		       (vid & RT305X_ESW_VLANI_VID_M) << s);
}

static void
rt305x_esw_set_pvid(struct rt305x_esw *esw, unsigned port, unsigned pvid)
{
	unsigned s;

	s = RT305X_ESW_PVIDC_PVID_S * (port % 2);
	rt305x_esw_rmw(esw,
		       RT305X_ESW_REG_PVIDC(port / 2),
		       RT305X_ESW_PVIDC_PVID_M << s,
		       (pvid & RT305X_ESW_PVIDC_PVID_M) << s);
}

static void
rt305x_esw_set_vmsc(struct rt305x_esw *esw, unsigned vlan, unsigned msc)
{
	unsigned s;

	s = RT305X_ESW_VMSC_MSC_S * (vlan % 4);
	rt305x_esw_rmw(esw,
		       RT305X_ESW_REG_VMSC(vlan / 4),
		       RT305X_ESW_VMSC_MSC_M << s,
		       (msc & RT305X_ESW_VMSC_MSC_M) << s);
}

static void
rt305x_esw_hw_init(struct rt305x_esw *esw)
{
	int i;

	/* vodoo from original driver */
	rt305x_esw_wr(esw, 0xC8A07850, RT305X_ESW_REG_FCT0);
	rt305x_esw_wr(esw, 0x00000000, RT305X_ESW_REG_SGC2);
	rt305x_esw_wr(esw, 0x00405555, RT305X_ESW_REG_PFC1);

	/* Enable Back Pressure, and Flow Control */
	rt305x_esw_wr(esw,
		      ((RT305X_ESW_PORTS_ALL << RT305X_ESW_POC1_EN_BP_S) |
		       (RT305X_ESW_PORTS_ALL << RT305X_ESW_POC1_EN_FC_S)),
		      RT305X_ESW_REG_POC1);

	/* Enable Aging, and VLAN TAG removal */
	rt305x_esw_wr(esw,
		      ((RT305X_ESW_PORTS_ALL << RT305X_ESW_POC3_ENAGING_S) |
		       (RT305X_ESW_PORTS_NOCPU << RT305X_ESW_POC3_UNTAG_EN_S)),
		      RT305X_ESW_REG_POC3);

	rt305x_esw_wr(esw, esw->pdata->reg_initval_fct2, RT305X_ESW_REG_FCT2);
	rt305x_esw_wr(esw, 0x0008a301, RT305X_ESW_REG_SGC);

	/* Setup SoC Port control register */
	rt305x_esw_wr(esw,
		      (RT305X_ESW_SOCPC_CRC_PADDING |
		       (RT305X_ESW_PORTS_CPU << RT305X_ESW_SOCPC_DISUN2CPU_S) |
		       (RT305X_ESW_PORTS_CPU << RT305X_ESW_SOCPC_DISMC2CPU_S) |
		       (RT305X_ESW_PORTS_CPU << RT305X_ESW_SOCPC_DISBC2CPU_S)),
		      RT305X_ESW_REG_SOCPC);

	rt305x_esw_wr(esw, esw->pdata->reg_initval_fpa2, RT305X_ESW_REG_FPA2);
	rt305x_esw_wr(esw, 0x00000000, RT305X_ESW_REG_FPA);

	/* Force Link/Activity on ports */
	rt305x_esw_wr(esw, 0x00000005, RT305X_ESW_REG_P0LED);
	rt305x_esw_wr(esw, 0x00000005, RT305X_ESW_REG_P1LED);
	rt305x_esw_wr(esw, 0x00000005, RT305X_ESW_REG_P2LED);
	rt305x_esw_wr(esw, 0x00000005, RT305X_ESW_REG_P3LED);
	rt305x_esw_wr(esw, 0x00000005, RT305X_ESW_REG_P4LED);

	rt305x_mii_write(esw, 0, 31, 0x8000);
	for (i = 0; i < 5; i++) {
		/* TX10 waveform coefficient */
		rt305x_mii_write(esw, i, 0, 0x3100);
		/* TX10 waveform coefficient */
		rt305x_mii_write(esw, i, 26, 0x1601);
		/* TX100/TX10 AD/DA current bias */
		rt305x_mii_write(esw, i, 29, 0x7058);
		/* TX100 slew rate control */
		rt305x_mii_write(esw, i, 30, 0x0018);
	}

	/* PHY IOT */
	/* select global register */
	rt305x_mii_write(esw, 0, 31, 0x0);
	/* tune TP_IDL tail and head waveform */
	rt305x_mii_write(esw, 0, 22, 0x052f);
	/* set TX10 signal amplitude threshold to minimum */
	rt305x_mii_write(esw, 0, 17, 0x0fe0);
	/* set squelch amplitude to higher threshold */
	rt305x_mii_write(esw, 0, 18, 0x40ba);
	/* longer TP_IDL tail length */
	rt305x_mii_write(esw, 0, 14, 0x65);
	/* select local register */
	rt305x_mii_write(esw, 0, 31, 0x8000);

	for (i = 0; i < RT305X_ESW_NUM_VLANS; i++) {
		rt305x_esw_set_vlan_id(esw, i, 0);
		rt305x_esw_set_vmsc(esw, i, 0);
	}

	for (i = 0; i < RT305X_ESW_NUM_PORTS; i++)
		rt305x_esw_set_pvid(esw, i, 1);

	switch (esw->pdata->vlan_config) {
	case RT305X_ESW_VLAN_CONFIG_NONE:
		break;

	case RT305X_ESW_VLAN_CONFIG_BYPASS:
		/* Pass all vlan tags to all ports */
		for (i = 0; i < RT305X_ESW_NUM_VLANS; i++) {
			rt305x_esw_set_vlan_id(esw, i, i+1);
			rt305x_esw_set_vmsc(esw, i, RT305X_ESW_PORTS_ALL);
		}
		/* Disable VLAN TAG removal, keep aging on. */
		rt305x_esw_wr(esw,
			      RT305X_ESW_PORTS_ALL << RT305X_ESW_POC3_ENAGING_S,
			      RT305X_ESW_REG_POC3);
		break;

	case RT305X_ESW_VLAN_CONFIG_LLLLW:
		rt305x_esw_set_vlan_id(esw, 0, 1);
		rt305x_esw_set_vlan_id(esw, 1, 2);
		rt305x_esw_set_pvid(esw, RT305X_ESW_PORT4, 2);

		rt305x_esw_set_vmsc(esw, 0,
				BIT(RT305X_ESW_PORT0) | BIT(RT305X_ESW_PORT1) |
				BIT(RT305X_ESW_PORT2) | BIT(RT305X_ESW_PORT3) |
				BIT(RT305X_ESW_PORT6));
		rt305x_esw_set_vmsc(esw, 1,
				BIT(RT305X_ESW_PORT4) | BIT(RT305X_ESW_PORT6));
		break;

	case RT305X_ESW_VLAN_CONFIG_WLLLL:
		rt305x_esw_set_vlan_id(esw, 0, 1);
		rt305x_esw_set_vlan_id(esw, 1, 2);
		rt305x_esw_set_pvid(esw, RT305X_ESW_PORT0, 2);

		rt305x_esw_set_vmsc(esw, 0,
				BIT(RT305X_ESW_PORT1) | BIT(RT305X_ESW_PORT2) |
				BIT(RT305X_ESW_PORT3) | BIT(RT305X_ESW_PORT4) |
				BIT(RT305X_ESW_PORT6));
		rt305x_esw_set_vmsc(esw, 1,
				BIT(RT305X_ESW_PORT0) | BIT(RT305X_ESW_PORT6));
		break;

	default:
		BUG();
	}
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

	esw = kzalloc(sizeof(struct rt305x_esw), GFP_KERNEL);
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
	spin_lock_init(&esw->reg_rw_lock);
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

static void
rt305x_esw_exit(void)
{
	platform_driver_unregister(&rt305x_esw_driver);
}
