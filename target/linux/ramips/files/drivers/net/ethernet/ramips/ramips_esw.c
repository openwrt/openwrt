#include <linux/ioport.h>
#include <linux/switch.h>
#include <linux/mii.h>

#include <rt305x_regs.h>
#include <rt305x_esw_platform.h>

/*
 * HW limitations for this switch:
 * - No large frame support (PKT_MAX_LEN at most 1536)
 * - Can't have untagged vlan and tagged vlan on one port at the same time,
 *   though this might be possible using the undocumented PPE.
 */

#define RT305X_ESW_REG_FCT0		0x08
#define RT305X_ESW_REG_PFC1		0x14
#define RT305X_ESW_REG_ATS		0x24
#define RT305X_ESW_REG_ATS0		0x28
#define RT305X_ESW_REG_ATS1		0x2c
#define RT305X_ESW_REG_ATS2		0x30
#define RT305X_ESW_REG_PVIDC(_n)	(0x40 + 4 * (_n))
#define RT305X_ESW_REG_VLANI(_n)	(0x50 + 4 * (_n))
#define RT305X_ESW_REG_VMSC(_n)		(0x70 + 4 * (_n))
#define RT305X_ESW_REG_POA		0x80
#define RT305X_ESW_REG_FPA		0x84
#define RT305X_ESW_REG_SOCPC		0x8c
#define RT305X_ESW_REG_POC0		0x90
#define RT305X_ESW_REG_POC1		0x94
#define RT305X_ESW_REG_POC2		0x98
#define RT305X_ESW_REG_SGC		0x9c
#define RT305X_ESW_REG_STRT		0xa0
#define RT305X_ESW_REG_PCR0		0xc0
#define RT305X_ESW_REG_PCR1		0xc4
#define RT305X_ESW_REG_FPA2		0xc8
#define RT305X_ESW_REG_FCT2		0xcc
#define RT305X_ESW_REG_SGC2		0xe4
#define RT305X_ESW_REG_P0LED		0xa4
#define RT305X_ESW_REG_P1LED		0xa8
#define RT305X_ESW_REG_P2LED		0xac
#define RT305X_ESW_REG_P3LED		0xb0
#define RT305X_ESW_REG_P4LED		0xb4
#define RT305X_ESW_REG_P0PC		0xe8
#define RT305X_ESW_REG_P1PC		0xec
#define RT305X_ESW_REG_P2PC		0xf0
#define RT305X_ESW_REG_P3PC		0xf4
#define RT305X_ESW_REG_P4PC		0xf8
#define RT305X_ESW_REG_P5PC		0xfc

#define RT305X_ESW_LED_LINK		0
#define RT305X_ESW_LED_100M		1
#define RT305X_ESW_LED_DUPLEX		2
#define RT305X_ESW_LED_ACTIVITY		3
#define RT305X_ESW_LED_COLLISION	4
#define RT305X_ESW_LED_LINKACT		5
#define RT305X_ESW_LED_DUPLCOLL		6
#define RT305X_ESW_LED_10MACT		7
#define RT305X_ESW_LED_100MACT		8
/* Additional led states not in datasheet: */
#define RT305X_ESW_LED_BLINK		10
#define RT305X_ESW_LED_ON		12

#define RT305X_ESW_LINK_S		25
#define RT305X_ESW_DUPLEX_S		9
#define RT305X_ESW_SPD_S		0

#define RT305X_ESW_PCR0_WT_NWAY_DATA_S	16
#define RT305X_ESW_PCR0_WT_PHY_CMD	BIT(13)
#define RT305X_ESW_PCR0_CPU_PHY_REG_S	8

#define RT305X_ESW_PCR1_WT_DONE		BIT(0)

#define RT305X_ESW_ATS_TIMEOUT		(5 * HZ)
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

#define RT305X_ESW_POC0_EN_BP_S		0
#define RT305X_ESW_POC0_EN_FC_S		8
#define RT305X_ESW_POC0_DIS_RMC2CPU_S	16
#define RT305X_ESW_POC0_DIS_PORT_M	0x7f
#define RT305X_ESW_POC0_DIS_PORT_S	23

#define RT305X_ESW_POC2_UNTAG_EN_M	0xff
#define RT305X_ESW_POC2_UNTAG_EN_S	0
#define RT305X_ESW_POC2_ENAGING_S	8
#define RT305X_ESW_POC2_DIS_UC_PAUSE_S	16

#define RT305X_ESW_SGC2_DOUBLE_TAG_M	0x7f
#define RT305X_ESW_SGC2_DOUBLE_TAG_S	0
#define RT305X_ESW_SGC2_LAN_PMAP_M	0x3f
#define RT305X_ESW_SGC2_LAN_PMAP_S	24

#define RT305X_ESW_PFC1_EN_VLAN_M	0xff
#define RT305X_ESW_PFC1_EN_VLAN_S	16
#define RT305X_ESW_PFC1_EN_TOS_S	24

#define RT305X_ESW_VLAN_NONE		0xfff

#define RT305X_ESW_PORT0		0
#define RT305X_ESW_PORT1		1
#define RT305X_ESW_PORT2		2
#define RT305X_ESW_PORT3		3
#define RT305X_ESW_PORT4		4
#define RT305X_ESW_PORT5		5
#define RT305X_ESW_PORT6		6

#define RT305X_ESW_PORTS_NONE		0

#define RT305X_ESW_PMAP_LLLLLL		0x3f
#define RT305X_ESW_PMAP_LLLLWL		0x2f
#define RT305X_ESW_PMAP_WLLLLL		0x3e

#define RT305X_ESW_PORTS_INTERNAL					\
		(BIT(RT305X_ESW_PORT0) | BIT(RT305X_ESW_PORT1) |	\
		 BIT(RT305X_ESW_PORT2) | BIT(RT305X_ESW_PORT3) |	\
		 BIT(RT305X_ESW_PORT4))

#define RT305X_ESW_PORTS_NOCPU						\
		(RT305X_ESW_PORTS_INTERNAL | BIT(RT305X_ESW_PORT5))

#define RT305X_ESW_PORTS_CPU	BIT(RT305X_ESW_PORT6)

#define RT305X_ESW_PORTS_ALL						\
		(RT305X_ESW_PORTS_NOCPU | RT305X_ESW_PORTS_CPU)

#define RT305X_ESW_NUM_VLANS		16
#define RT305X_ESW_NUM_VIDS		4096
#define RT305X_ESW_NUM_PORTS		7
#define RT305X_ESW_NUM_LANWAN		6
#define RT305X_ESW_NUM_LEDS		5

enum {
	/* Global attributes. */
	RT305X_ESW_ATTR_ENABLE_VLAN,
	RT305X_ESW_ATTR_ALT_VLAN_DISABLE,
	/* Port attributes. */
	RT305X_ESW_ATTR_PORT_DISABLE,
	RT305X_ESW_ATTR_PORT_DOUBLETAG,
	RT305X_ESW_ATTR_PORT_UNTAG,
	RT305X_ESW_ATTR_PORT_LED,
	RT305X_ESW_ATTR_PORT_LAN,
	RT305X_ESW_ATTR_PORT_RECV_BAD,
	RT305X_ESW_ATTR_PORT_RECV_GOOD,
};

struct rt305x_esw_port {
	bool	disable;
	bool	doubletag;
	bool	untag;
	u8	led;
	u16	pvid;
};

struct rt305x_esw_vlan {
	u8	ports;
	u16	vid;
};

struct rt305x_esw {
	void __iomem		*base;
	struct rt305x_esw_platform_data *pdata;
	/* Protects against concurrent register rmw operations. */
	spinlock_t		reg_rw_lock;

	struct switch_dev	swdev;
	bool			global_vlan_enable;
	bool			alt_vlan_disable;
	struct rt305x_esw_vlan vlans[RT305X_ESW_NUM_VLANS];
	struct rt305x_esw_port ports[RT305X_ESW_NUM_PORTS];

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

static unsigned
rt305x_esw_get_vlan_id(struct rt305x_esw *esw, unsigned vlan)
{
	unsigned s;
	unsigned val;

	s = RT305X_ESW_VLANI_VID_S * (vlan % 2);
	val = rt305x_esw_rr(esw, RT305X_ESW_REG_VLANI(vlan / 2));
	val = (val >> s) & RT305X_ESW_VLANI_VID_M;

	return val;
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

static unsigned
rt305x_esw_get_pvid(struct rt305x_esw *esw, unsigned port)
{
	unsigned s, val;

	s = RT305X_ESW_PVIDC_PVID_S * (port % 2);
	val = rt305x_esw_rr(esw, RT305X_ESW_REG_PVIDC(port / 2));
	return (val >> s) & RT305X_ESW_PVIDC_PVID_M;
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

static unsigned
rt305x_esw_get_vmsc(struct rt305x_esw *esw, unsigned vlan)
{
	unsigned s, val;

	s = RT305X_ESW_VMSC_MSC_S * (vlan % 4);
	val = rt305x_esw_rr(esw, RT305X_ESW_REG_VMSC(vlan / 4));
	val = (val >> s) & RT305X_ESW_VMSC_MSC_M;

	return val;
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

static unsigned
rt305x_esw_get_port_disable(struct rt305x_esw *esw)
{
	unsigned reg;
	reg = rt305x_esw_rr(esw, RT305X_ESW_REG_POC0);
	return (reg >> RT305X_ESW_POC0_DIS_PORT_S) &
	       RT305X_ESW_POC0_DIS_PORT_M;
}

static void
rt305x_esw_set_port_disable(struct rt305x_esw *esw, unsigned disable_mask)
{
	unsigned old_mask;
	unsigned enable_mask;
	unsigned changed;
	int i;

	old_mask = rt305x_esw_get_port_disable(esw);
	changed = old_mask ^ disable_mask;
	enable_mask = old_mask & disable_mask;

	/* enable before writing to MII */
	rt305x_esw_rmw(esw, RT305X_ESW_REG_POC0,
		       (RT305X_ESW_POC0_DIS_PORT_M <<
			RT305X_ESW_POC0_DIS_PORT_S),
		       enable_mask << RT305X_ESW_POC0_DIS_PORT_S);

	for (i = 0; i < RT305X_ESW_NUM_LEDS; i++) {
		if (!(changed & (1 << i)))
			continue;
		if (disable_mask & (1 << i)) {
			/* disable */
			rt305x_mii_write(esw, i, MII_BMCR,
					 BMCR_PDOWN);
		} else {
			/* enable */
			rt305x_mii_write(esw, i, MII_BMCR,
					 BMCR_FULLDPLX |
					 BMCR_ANENABLE |
					 BMCR_ANRESTART |
					 BMCR_SPEED100);
		}
	}

	/* disable after writing to MII */
	rt305x_esw_rmw(esw, RT305X_ESW_REG_POC0,
		       (RT305X_ESW_POC0_DIS_PORT_M <<
			RT305X_ESW_POC0_DIS_PORT_S),
		       disable_mask << RT305X_ESW_POC0_DIS_PORT_S);
}

static int
rt305x_esw_apply_config(struct switch_dev *dev);

static void
rt305x_esw_hw_init(struct rt305x_esw *esw)
{
	int i;
	u8 port_disable = 0;
	u8 port_map = RT305X_ESW_PMAP_LLLLLL;

	/* vodoo from original driver */
	rt305x_esw_wr(esw, 0xC8A07850, RT305X_ESW_REG_FCT0);
	rt305x_esw_wr(esw, 0x00000000, RT305X_ESW_REG_SGC2);
	/* Port priority 1 for all ports, vlan enabled. */
	rt305x_esw_wr(esw, 0x00005555 |
		      (RT305X_ESW_PORTS_ALL << RT305X_ESW_PFC1_EN_VLAN_S),
		      RT305X_ESW_REG_PFC1);

	/* Enable Back Pressure, and Flow Control */
	rt305x_esw_wr(esw,
		      ((RT305X_ESW_PORTS_ALL << RT305X_ESW_POC0_EN_BP_S) |
		       (RT305X_ESW_PORTS_ALL << RT305X_ESW_POC0_EN_FC_S)),
		      RT305X_ESW_REG_POC0);

	/* Enable Aging, and VLAN TAG removal */
	rt305x_esw_wr(esw,
		      ((RT305X_ESW_PORTS_ALL << RT305X_ESW_POC2_ENAGING_S) |
		       (RT305X_ESW_PORTS_NOCPU << RT305X_ESW_POC2_UNTAG_EN_S)),
		      RT305X_ESW_REG_POC2);

	rt305x_esw_wr(esw, esw->pdata->reg_initval_fct2, RT305X_ESW_REG_FCT2);

	/*
	 * 300s aging timer, max packet len 1536, broadcast storm prevention
	 * disabled, disable collision abort, mac xor48 hash, 10 packet back
	 * pressure jam, GMII disable was_transmit, back pressure disabled,
	 * 30ms led flash, unmatched IGMP as broadcast, rmc tb fault to all
	 * ports.
	 */
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

	/* Copy disabled port configuration from bootloader setup */
	port_disable = rt305x_esw_get_port_disable(esw);
	for (i = 0; i < 6; i++)
		esw->ports[i].disable = (port_disable & (1 << i)) != 0;

	rt305x_mii_write(esw, 0, 31, 0x8000);
	for (i = 0; i < 5; i++) {
		if (esw->ports[i].disable) {
			rt305x_mii_write(esw, i, MII_BMCR, BMCR_PDOWN);
		} else {
			rt305x_mii_write(esw, i, MII_BMCR,
					 BMCR_FULLDPLX |
					 BMCR_ANENABLE |
					 BMCR_SPEED100);
		}
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

	switch (esw->pdata->vlan_config) {
	case RT305X_ESW_VLAN_CONFIG_NONE:
		port_map = RT305X_ESW_PMAP_LLLLLL;
		break;
	case RT305X_ESW_VLAN_CONFIG_LLLLW:
		port_map = RT305X_ESW_PMAP_LLLLWL;
		break;
	case RT305X_ESW_VLAN_CONFIG_WLLLL:
		port_map = RT305X_ESW_PMAP_WLLLLL;
		break;
	default:
		BUG();
	}

	/*
	 * Unused HW feature, but still nice to be consistent here...
	 * This is also exported to userspace ('lan' attribute) so it's
	 * conveniently usable to decide which ports go into the wan vlan by
	 * default.
	 */
	rt305x_esw_rmw(esw, RT305X_ESW_REG_SGC2,
		       RT305X_ESW_SGC2_LAN_PMAP_M << RT305X_ESW_SGC2_LAN_PMAP_S,
		       port_map << RT305X_ESW_SGC2_LAN_PMAP_S);

	/* make the switch leds blink */
	for (i = 0; i < RT305X_ESW_NUM_LEDS; i++)
		esw->ports[i].led = 0x05;

	/* Apply the empty config. */
	rt305x_esw_apply_config(&esw->swdev);
}

static int
rt305x_esw_apply_config(struct switch_dev *dev)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	int i;
	u8 disable = 0;
	u8 doubletag = 0;
	u8 en_vlan = 0;
	u8 untag = 0;

	for (i = 0; i < RT305X_ESW_NUM_VLANS; i++) {
		u32 vid, vmsc;
		if (esw->global_vlan_enable) {
			vid = esw->vlans[i].vid;
			vmsc = esw->vlans[i].ports;
		} else {
			vid = RT305X_ESW_VLAN_NONE;
			vmsc = RT305X_ESW_PORTS_NONE;
		}
		rt305x_esw_set_vlan_id(esw, i, vid);
		rt305x_esw_set_vmsc(esw, i, vmsc);
	}

	for (i = 0; i < RT305X_ESW_NUM_PORTS; i++) {
		u32 pvid;
		disable |= esw->ports[i].disable << i;
		if (esw->global_vlan_enable) {
			doubletag |= esw->ports[i].doubletag << i;
			en_vlan   |= 1                       << i;
			untag     |= esw->ports[i].untag     << i;
			pvid       = esw->ports[i].pvid;
		} else {
			int x = esw->alt_vlan_disable ? 0 : 1;
			doubletag |= x << i;
			en_vlan   |= x << i;
			untag     |= x << i;
			pvid       = 0;
		}
		rt305x_esw_set_pvid(esw, i, pvid);
		if (i < RT305X_ESW_NUM_LEDS)
			rt305x_esw_wr(esw, esw->ports[i].led,
				      RT305X_ESW_REG_P0LED + 4*i);
	}

	rt305x_esw_set_port_disable(esw, disable);
	rt305x_esw_rmw(esw, RT305X_ESW_REG_SGC2,
		       (RT305X_ESW_SGC2_DOUBLE_TAG_M <<
			RT305X_ESW_SGC2_DOUBLE_TAG_S),
		       doubletag << RT305X_ESW_SGC2_DOUBLE_TAG_S);
	rt305x_esw_rmw(esw, RT305X_ESW_REG_PFC1,
		       RT305X_ESW_PFC1_EN_VLAN_M << RT305X_ESW_PFC1_EN_VLAN_S,
		       en_vlan << RT305X_ESW_PFC1_EN_VLAN_S);
	rt305x_esw_rmw(esw, RT305X_ESW_REG_POC2,
		       RT305X_ESW_POC2_UNTAG_EN_M << RT305X_ESW_POC2_UNTAG_EN_S,
		       untag << RT305X_ESW_POC2_UNTAG_EN_S);

	if (!esw->global_vlan_enable) {
		/*
		 * Still need to put all ports into vlan 0 or they'll be
		 * isolated.
		 * NOTE: vlan 0 is special, no vlan tag is prepended
		 */
		rt305x_esw_set_vlan_id(esw, 0, 0);
		rt305x_esw_set_vmsc(esw, 0, RT305X_ESW_PORTS_ALL);
	}

	return 0;
}

static int
rt305x_esw_reset_switch(struct switch_dev *dev)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	esw->global_vlan_enable = 0;
	memset(esw->ports, 0, sizeof(esw->ports));
	memset(esw->vlans, 0, sizeof(esw->vlans));
	rt305x_esw_hw_init(esw);

	return 0;
}

static int
rt305x_esw_get_vlan_enable(struct switch_dev *dev,
			   const struct switch_attr *attr,
			   struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);

	val->value.i = esw->global_vlan_enable;

	return 0;
}

static int
rt305x_esw_set_vlan_enable(struct switch_dev *dev,
			   const struct switch_attr *attr,
			   struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);

	esw->global_vlan_enable = val->value.i != 0;

	return 0;
}

static int
rt305x_esw_get_alt_vlan_disable(struct switch_dev *dev,
				const struct switch_attr *attr,
				struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);

	val->value.i = esw->alt_vlan_disable;

	return 0;
}

static int
rt305x_esw_set_alt_vlan_disable(struct switch_dev *dev,
				const struct switch_attr *attr,
				struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);

	esw->alt_vlan_disable = val->value.i != 0;

	return 0;
}

static int
rt305x_esw_get_port_link(struct switch_dev *dev,
			 int port,
			 struct switch_port_link *link)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	u32 speed, poa;

	if (port < 0 || port >= RT305X_ESW_NUM_PORTS)
		return -EINVAL;

	poa = rt305x_esw_rr(esw, RT305X_ESW_REG_POA) >> port;

	link->link = (poa >> RT305X_ESW_LINK_S) & 1;
	link->duplex = (poa >> RT305X_ESW_DUPLEX_S) & 1;
	if (port < RT305X_ESW_NUM_LEDS) {
		speed = (poa >> RT305X_ESW_SPD_S) & 1;
	} else {
		if (port == RT305X_ESW_NUM_PORTS - 1)
			poa >>= 1;
		speed = (poa >> RT305X_ESW_SPD_S) & 3;
	}
	switch (speed) {
	case 0:
		link->speed = SWITCH_PORT_SPEED_10;
		break;
	case 1:
		link->speed = SWITCH_PORT_SPEED_100;
		break;
	case 2:
	case 3: /* forced gige speed can be 2 or 3 */
		link->speed = SWITCH_PORT_SPEED_1000;
		break;
	default:
		link->speed = SWITCH_PORT_SPEED_UNKNOWN;
		break;
	}

	return 0;
}

static int
rt305x_esw_get_port_bool(struct switch_dev *dev,
			 const struct switch_attr *attr,
			 struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	int idx = val->port_vlan;
	u32 x, reg, shift;

	if (idx < 0 || idx >= RT305X_ESW_NUM_PORTS)
		return -EINVAL;

	switch (attr->id) {
	case RT305X_ESW_ATTR_PORT_DISABLE:
		reg = RT305X_ESW_REG_POC0;
		shift = RT305X_ESW_POC0_DIS_PORT_S;
		break;
	case RT305X_ESW_ATTR_PORT_DOUBLETAG:
		reg = RT305X_ESW_REG_SGC2;
		shift = RT305X_ESW_SGC2_DOUBLE_TAG_S;
		break;
	case RT305X_ESW_ATTR_PORT_UNTAG:
		reg = RT305X_ESW_REG_POC2;
		shift = RT305X_ESW_POC2_UNTAG_EN_S;
		break;
	case RT305X_ESW_ATTR_PORT_LAN:
		reg = RT305X_ESW_REG_SGC2;
		shift = RT305X_ESW_SGC2_LAN_PMAP_S;
		if (idx >= RT305X_ESW_NUM_LANWAN)
			return -EINVAL;
		break;
	default:
		return -EINVAL;
	}

	x = rt305x_esw_rr(esw, reg);
	val->value.i = (x >> (idx + shift)) & 1;

	return 0;
}

static int
rt305x_esw_set_port_bool(struct switch_dev *dev,
			 const struct switch_attr *attr,
			 struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	int idx = val->port_vlan;

	if (idx < 0 || idx >= RT305X_ESW_NUM_PORTS ||
	    val->value.i < 0 || val->value.i > 1)
		return -EINVAL;

	switch (attr->id) {
	case RT305X_ESW_ATTR_PORT_DISABLE:
		esw->ports[idx].disable = val->value.i;
		break;
	case RT305X_ESW_ATTR_PORT_DOUBLETAG:
		esw->ports[idx].doubletag = val->value.i;
		break;
	case RT305X_ESW_ATTR_PORT_UNTAG:
		esw->ports[idx].untag = val->value.i;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int
rt305x_esw_get_port_recv_badgood(struct switch_dev *dev,
				 const struct switch_attr *attr,
				 struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	int idx = val->port_vlan;
	int shift = attr->id == RT305X_ESW_ATTR_PORT_RECV_GOOD ? 0 : 16;
	u32 reg;

	if (idx < 0 || idx >= RT305X_ESW_NUM_LANWAN)
		return -EINVAL;

	reg = rt305x_esw_rr(esw, RT305X_ESW_REG_P0PC + 4*idx);
	val->value.i = (reg >> shift) & 0xffff;

	return 0;
}

static int
rt305x_esw_get_port_led(struct switch_dev *dev,
			const struct switch_attr *attr,
			struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	int idx = val->port_vlan;

	if (idx < 0 || idx >= RT305X_ESW_NUM_PORTS ||
	    idx >= RT305X_ESW_NUM_LEDS)
		return -EINVAL;

	val->value.i = rt305x_esw_rr(esw, RT305X_ESW_REG_P0LED + 4*idx);

	return 0;
}

static int
rt305x_esw_set_port_led(struct switch_dev *dev,
			const struct switch_attr *attr,
			struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	int idx = val->port_vlan;

	if (idx < 0 || idx >= RT305X_ESW_NUM_LEDS)
		return -EINVAL;

	esw->ports[idx].led = val->value.i;

	return 0;
}

static int
rt305x_esw_get_port_pvid(struct switch_dev *dev, int port, int *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);

	if (port >= RT305X_ESW_NUM_PORTS)
		return -EINVAL;

	*val = rt305x_esw_get_pvid(esw, port);

	return 0;
}

static int
rt305x_esw_set_port_pvid(struct switch_dev *dev, int port, int val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);

	if (port >= RT305X_ESW_NUM_PORTS)
		return -EINVAL;

	esw->ports[port].pvid = val;

	return 0;
}

static int
rt305x_esw_get_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	u32 vmsc, poc2;
	int vlan_idx = -1;
	int i;

	val->len = 0;

	if (val->port_vlan < 0 || val->port_vlan >= RT305X_ESW_NUM_VIDS)
		return -EINVAL;

	/* valid vlan? */
	for (i = 0; i < RT305X_ESW_NUM_VLANS; i++) {
		if (rt305x_esw_get_vlan_id(esw, i) == val->port_vlan &&
		    rt305x_esw_get_vmsc(esw, i) != RT305X_ESW_PORTS_NONE) {
			vlan_idx = i;
			break;
		}
	}

	if (vlan_idx == -1)
		return -EINVAL;

	vmsc = rt305x_esw_get_vmsc(esw, vlan_idx);
	poc2 = rt305x_esw_rr(esw, RT305X_ESW_REG_POC2);

	for (i = 0; i < RT305X_ESW_NUM_PORTS; i++) {
		struct switch_port *p;
		int port_mask = 1 << i;

		if (!(vmsc & port_mask))
			continue;

		p = &val->value.ports[val->len++];
		p->id = i;
		if (poc2 & (port_mask << RT305X_ESW_POC2_UNTAG_EN_S))
			p->flags = 0;
		else
			p->flags = 1 << SWITCH_PORT_FLAG_TAGGED;
	}

	return 0;
}

static int
rt305x_esw_set_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct rt305x_esw *esw = container_of(dev, struct rt305x_esw, swdev);
	int ports;
	int vlan_idx = -1;
	int i;

	if (val->port_vlan < 0 || val->port_vlan >= RT305X_ESW_NUM_VIDS ||
	    val->len > RT305X_ESW_NUM_PORTS)
		return -EINVAL;

	/* one of the already defined vlans? */
	for (i = 0; i < RT305X_ESW_NUM_VLANS; i++) {
		if (esw->vlans[i].vid == val->port_vlan &&
		    esw->vlans[i].ports != RT305X_ESW_PORTS_NONE) {
			vlan_idx = i;
			break;
		}
	}

	/* select a free slot */
	for (i = 0; vlan_idx == -1 && i < RT305X_ESW_NUM_VLANS; i++) {
		if (esw->vlans[i].ports == RT305X_ESW_PORTS_NONE)
			vlan_idx = i;
	}

	/* bail if all slots are in use */
	if (vlan_idx == -1)
		return -EINVAL;

	ports = RT305X_ESW_PORTS_NONE;
	for (i = 0; i < val->len; i++) {
		struct switch_port *p = &val->value.ports[i];
		int port_mask = 1 << p->id;
		bool untagged = !(p->flags & (1 << SWITCH_PORT_FLAG_TAGGED));

		if (p->id >= RT305X_ESW_NUM_PORTS)
			return -EINVAL;

		ports |= port_mask;
		esw->ports[p->id].untag = untagged;
	}
	esw->vlans[vlan_idx].ports = ports;
	if (ports == RT305X_ESW_PORTS_NONE)
		esw->vlans[vlan_idx].vid = RT305X_ESW_VLAN_NONE;
	else
		esw->vlans[vlan_idx].vid = val->port_vlan;

	return 0;
}

static const struct switch_attr rt305x_esw_global[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "VLAN mode (1:enabled)",
		.max = 1,
		.id = RT305X_ESW_ATTR_ENABLE_VLAN,
		.get = rt305x_esw_get_vlan_enable,
		.set = rt305x_esw_set_vlan_enable,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "alternate_vlan_disable",
		.description = "Use en_vlan instead of doubletag to disable"
				" VLAN mode",
		.max = 1,
		.id = RT305X_ESW_ATTR_ALT_VLAN_DISABLE,
		.get = rt305x_esw_get_alt_vlan_disable,
		.set = rt305x_esw_set_alt_vlan_disable,
	},
};

static const struct switch_attr rt305x_esw_port[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "disable",
		.description = "Port state (1:disabled)",
		.max = 1,
		.id = RT305X_ESW_ATTR_PORT_DISABLE,
		.get = rt305x_esw_get_port_bool,
		.set = rt305x_esw_set_port_bool,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "doubletag",
		.description = "Double tagging for incoming vlan packets "
				"(1:enabled)",
		.max = 1,
		.id = RT305X_ESW_ATTR_PORT_DOUBLETAG,
		.get = rt305x_esw_get_port_bool,
		.set = rt305x_esw_set_port_bool,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "untag",
		.description = "Untag (1:strip outgoing vlan tag)",
		.max = 1,
		.id = RT305X_ESW_ATTR_PORT_UNTAG,
		.get = rt305x_esw_get_port_bool,
		.set = rt305x_esw_set_port_bool,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "led",
		.description = "LED mode (0:link, 1:100m, 2:duplex, 3:activity,"
				" 4:collision, 5:linkact, 6:duplcoll, 7:10mact,"
				" 8:100mact, 10:blink, 12:on)",
		.max = 15,
		.id = RT305X_ESW_ATTR_PORT_LED,
		.get = rt305x_esw_get_port_led,
		.set = rt305x_esw_set_port_led,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "lan",
		.description = "HW port group (0:wan, 1:lan)",
		.max = 1,
		.id = RT305X_ESW_ATTR_PORT_LAN,
		.get = rt305x_esw_get_port_bool,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "recv_bad",
		.description = "Receive bad packet counter",
		.id = RT305X_ESW_ATTR_PORT_RECV_BAD,
		.get = rt305x_esw_get_port_recv_badgood,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "recv_good",
		.description = "Receive good packet counter",
		.id = RT305X_ESW_ATTR_PORT_RECV_GOOD,
		.get = rt305x_esw_get_port_recv_badgood,
	},
};

static const struct switch_attr rt305x_esw_vlan[] = {
};

static const struct switch_dev_ops rt305x_esw_ops = {
	.attr_global = {
		.attr = rt305x_esw_global,
		.n_attr = ARRAY_SIZE(rt305x_esw_global),
	},
	.attr_port = {
		.attr = rt305x_esw_port,
		.n_attr = ARRAY_SIZE(rt305x_esw_port),
	},
	.attr_vlan = {
		.attr = rt305x_esw_vlan,
		.n_attr = ARRAY_SIZE(rt305x_esw_vlan),
	},
	.get_vlan_ports = rt305x_esw_get_vlan_ports,
	.set_vlan_ports = rt305x_esw_set_vlan_ports,
	.get_port_pvid = rt305x_esw_get_port_pvid,
	.set_port_pvid = rt305x_esw_set_port_pvid,
	.get_port_link = rt305x_esw_get_port_link,
	.apply_config = rt305x_esw_apply_config,
	.reset_switch = rt305x_esw_reset_switch,
};

static int
rt305x_esw_probe(struct platform_device *pdev)
{
	struct rt305x_esw_platform_data *pdata;
	struct rt305x_esw *esw;
	struct switch_dev *swdev;
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

	swdev = &esw->swdev;
	swdev->name = "rt305x-esw";
	swdev->alias = "rt305x";
	swdev->cpu_port = RT305X_ESW_PORT6;
	swdev->ports = RT305X_ESW_NUM_PORTS;
	swdev->vlans = RT305X_ESW_NUM_VIDS;
	swdev->ops = &rt305x_esw_ops;

	err = register_switch(swdev, NULL);
	if (err < 0) {
		dev_err(&pdev->dev, "register_switch failed\n");
		goto unmap_base;
	}

	platform_set_drvdata(pdev, esw);

	esw->pdata = pdata;
	spin_lock_init(&esw->reg_rw_lock);
	rt305x_esw_hw_init(esw);

	return 0;

unmap_base:
	iounmap(esw->base);
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
		unregister_switch(&esw->swdev);
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
