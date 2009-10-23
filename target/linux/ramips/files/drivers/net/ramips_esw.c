#define GPIO_PRUPOSE           0x60
#define GPIO_MDIO_BIT          (1<<7)
#define RT305X_ESW_PHY_WRITE		(1 << 13)
#define RT305X_ESW_PHY_TOUT			(5 * HZ)
#define RT305X_ESW_PHY_CONTROL_0	0xC0
#define RT305X_ESW_PHY_CONTROL_1	0xC4

static void __iomem *ramips_esw_base = 0;

static inline void
ramips_esw_wr(u32 val, unsigned reg)
{
	__raw_writel(val, ramips_esw_base + reg);
}

static inline u32
ramips_esw_rr(unsigned reg)
{
	return __raw_readl(ramips_esw_base + reg);
}

static void
ramips_enable_mdio(int s)
{
	u32 gpio = rt305x_sysc_rr(GPIO_PRUPOSE);
	if(s)
		gpio &= ~GPIO_MDIO_BIT;
	else
		gpio |= GPIO_MDIO_BIT;
	rt305x_sysc_wr(gpio, GPIO_PRUPOSE);
}

u32
mii_mgr_write(u32 phy_addr, u32 phy_register, u32 write_data)
{
	unsigned long volatile t_start = jiffies;
	int ret = 0;

	ramips_enable_mdio(1);
	while(1)
	{
		if(!(ramips_esw_rr(RT305X_ESW_PHY_CONTROL_1) & (0x1 << 0)))
			break;
		if(time_after(jiffies, t_start + RT305X_ESW_PHY_TOUT))
		{
			ret = 1;
			goto out;
		}
	}
	ramips_esw_wr(((write_data & 0xFFFF) << 16) | (phy_register << 8) |
		(phy_addr) | RT305X_ESW_PHY_WRITE, RT305X_ESW_PHY_CONTROL_0);
	t_start = jiffies;
	while(1)
	{
		if(ramips_esw_rr(RT305X_ESW_PHY_CONTROL_1) & (0x1 << 0))
			break;
		if(time_after(jiffies, t_start + RT305X_ESW_PHY_TOUT))
		{
			ret = 1;
			break;
		}
	}
out:
	ramips_enable_mdio(0);
	if(ret)
		printk(KERN_ERR "ramips_eth: MDIO timeout\n");
	return ret;
}

static int
rt305x_esw_init(void)
{
	int i;

	ramips_esw_base = ioremap_nocache(RT305X_SWITCH_BASE, PAGE_SIZE);
	if(!ramips_esw_base)
		return -ENOMEM;

	/* vodoo from original driver */
	ramips_esw_wr(0xC8A07850, 0x08);
	ramips_esw_wr(0x00000000, 0xe4);
	ramips_esw_wr(0x00405555, 0x14);
	ramips_esw_wr(0x00002001, 0x50);
	ramips_esw_wr(0x00007f7f, 0x90);
	ramips_esw_wr(0x00007f3f, 0x98);
	ramips_esw_wr(0x00d6500c, 0xcc);
	ramips_esw_wr(0x0008a301, 0x9c);
	ramips_esw_wr(0x02404040, 0x8c);
	ramips_esw_wr(0x00001002, 0x48);
	ramips_esw_wr(0x3f502b28, 0xc8);
	ramips_esw_wr(0x00000000, 0x84);

	mii_mgr_write(0, 31, 0x8000);
	for(i = 0; i < 5; i++)
	{
		mii_mgr_write(i, 0, 0x3100);   //TX10 waveform coefficient
		mii_mgr_write(i, 26, 0x1601);   //TX10 waveform coefficient
		mii_mgr_write(i, 29, 0x7058);   //TX100/TX10 AD/DA current bias
		mii_mgr_write(i, 30, 0x0018);   //TX100 slew rate control
	}
	/* PHY IOT */
	mii_mgr_write(0, 31, 0x0);      //select global register
	mii_mgr_write(0, 22, 0x052f);   //tune TP_IDL tail and head waveform
	mii_mgr_write(0, 17, 0x0fe0);   //set TX10 signal amplitude threshold to minimum
	mii_mgr_write(0, 18, 0x40ba);   //set squelch amplitude to higher threshold
	mii_mgr_write(0, 14, 0x65);     //longer TP_IDL tail length
	mii_mgr_write(0, 31, 0x8000);   //select local register

	/* Port 5 Disabled */
	rt305x_sysc_wr(rt305x_sysc_rr(0x60) | (1 << 9), 0x60); //set RGMII to GPIO mode (GPIO41-GPIO50)
	rt305x_sysc_wr(0xfff, 0x674); //GPIO41-GPIO50 output mode
	rt305x_sysc_wr(0x0, 0x670); //GPIO41-GPIO50 output low

	/* set default vlan */
	ramips_esw_wr(0x2001, 0x50);
	ramips_esw_wr(0x5041, 0x70);

	return 0;
}
