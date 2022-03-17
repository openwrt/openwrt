/*
* Description
*
* Copyright (C) 2016-2020 Qin.Xia <qin.xia@siflower.com.cn>
*
* Siflower software
*/

#include "gsw_swconfig.h"

extern GSW_return_t gsw_reg_rd(void *pdev, u16 Offset, u16 Shift, u16 Size, ur *value);
extern GSW_return_t gsw_reg_wr(void *pdev, u16 Offset, u16 Shift, u16 Size, u32 value);
extern GSW_return_t GSW_MDIO_DataRead(void *pdev, GSW_MDIO_data_t *parm);
extern GSW_return_t GSW_MDIO_DataWrite(void *pdev, GSW_MDIO_data_t *parm);
extern GSW_return_t GSW_MmdDataRead(void *pdev, GSW_MMD_data_t *parm);
extern GSW_return_t GSW_MmdDataWrite(void *pdev, GSW_MMD_data_t *parm);
extern GSW_return_t GSW_RMON_Port_Get(void *pdev, GSW_RMON_Port_cnt_t *parm);
extern GSW_return_t GSW_RMON_Clear(void *pdev, GSW_RMON_clear_t *parm);

int intel_phy_rd(struct intel_gsw *gsw, GSW_MDIO_data_t *parm) {
	return GSW_MDIO_DataRead((void*)&gsw->pd, parm);
}

int intel_phy_wr(struct intel_gsw *gsw, GSW_MDIO_data_t *parm) {
	return GSW_MDIO_DataWrite((void*)&gsw->pd, parm);
}

int intel_mdio_rd(struct intel_gsw *gsw, u16 Offset, u16 Shift, u16 Size, ur *value) {
	return gsw_reg_rd((void*)&gsw->pd, Offset, Shift, Size, value);
}

int intel_mdio_wr(struct intel_gsw *gsw, u16 Offset, u16 Shift, u16 Size, u32 value) {
	return gsw_reg_wr((void*)&gsw->pd, Offset, Shift, Size, value);
}

int intel_mmd_rd(struct intel_gsw *gsw, GSW_MMD_data_t *parm) {
	return GSW_MmdDataRead((void *)&gsw->pd, parm);
}

int intel_mmd_wr(struct intel_gsw *gsw, GSW_MMD_data_t *parm) {
	return GSW_MmdDataWrite((void *)&gsw->pd, parm);
}

int intel_count_rd(struct intel_gsw *gsw, GSW_RMON_Port_cnt_t *parm) {
	return GSW_RMON_Port_Get((void*)&gsw->pd, parm);
}

int intel_count_clear(struct intel_gsw *gsw, GSW_RMON_clear_t *parm) {
	return GSW_RMON_Clear((void*)&gsw->pd, parm);
}

#ifdef CONFIG_SWCONFIG
static int
intel_get_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	struct switch_port *port;
	GSW_VLAN_portMemberRead_t parm;
	int i;

	if (val->port_vlan < 0)
		return -EINVAL;

	memset((void *)&parm, 0, sizeof(GSW_VLAN_portMemberRead_t));
	parm.nVId = val->port_vlan;
	mutex_lock(&gsw->reg_mutex);
	GSW_VLAN_PortMemberRead((void *)&gsw->pd, &parm);
	mutex_unlock(&gsw->reg_mutex);

	port = &val->value.ports[0];
	val->len = 0;
	for (i = 0; i < INTEL_SWITCH_PORT_NUM; i++) {
		if (!(parm.nPortId & BIT(i)))
			continue;

		port->id = i;
		port->flags = (parm.nTagId & BIT(i)) ? BIT(SWITCH_PORT_FLAG_TAGGED) : 0;
		val->len++;
		port++;
	}

	return 0;
}

static int
intel_set_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	GSW_VLAN_IdCreate_t vlan;
	GSW_VLAN_portMemberAdd_t parm;
	int i = 0;

	if (val->port_vlan < 0 || val->port_vlan >= INTEL_NUM_VLANS || val->len >= INTEL_SWITCH_PORT_NUM)
		return -EINVAL;

	memset((void *)&vlan, 0, sizeof(GSW_VLAN_IdCreate_t));
	memset((void *)&parm, 0, sizeof(GSW_VLAN_portMemberAdd_t));
	vlan.nVId = val->port_vlan;
	vlan.nFId = vlan.nVId; // RM#8744 fix lan-wan bridge fail
	mutex_lock(&gsw->reg_mutex);
	GSW_VLAN_IdCreate((void *)&gsw->pd, &vlan);
	mutex_unlock(&gsw->reg_mutex);

	parm.nVId = val->port_vlan;
	for (i = 0; i < val->len; i++) {
		struct switch_port *p = &val->value.ports[i];
		if (p->id > INTEL_SWITCH_PORT_NUM)
			return -EINVAL;

		parm.nPortId = p->id;
		if (p->flags & BIT(SWITCH_PORT_FLAG_TAGGED))
			parm.bVLAN_TagEgress = 1;
		else
			parm.bVLAN_TagEgress = 0;

		mutex_lock(&gsw->reg_mutex);
		GSW_VLAN_PortMemberAdd((void *)&gsw->pd, &parm);
		mutex_unlock(&gsw->reg_mutex);
	}

	return 0;
}

static int intel_get_port_pvid(struct switch_dev *dev, int port, int *val)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	GSW_return_t s;
	ur nPortVId;

	mutex_lock(&gsw->reg_mutex);
	s = gsw_reg_rd((void *)&gsw->pd,
	               (PCE_DEFPVID_PVID_OFFSET + (10 * port)),
	               PCE_DEFPVID_PVID_SHIFT,
	               PCE_DEFPVID_PVID_SIZE, &nPortVId);
	mutex_unlock(&gsw->reg_mutex);
	if (s != GSW_statusOk)
		return s;

	*val = nPortVId;
	return 0;
}

static int intel_set_port_pvid(struct switch_dev *dev, int port, int pvid)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	GSW_return_t s;

	mutex_lock(&gsw->reg_mutex);
	s = gsw_reg_wr((void *)&gsw->pd,
	               (PCE_DEFPVID_PVID_OFFSET + (10 * port)),
	               PCE_DEFPVID_PVID_SHIFT,
	               PCE_DEFPVID_PVID_SIZE, pvid);
	mutex_unlock(&gsw->reg_mutex);
	if (s != GSW_statusOk)
		return s;

	return 0;
}

static int intel_get_port_link(struct switch_dev *dev,  int port,
                               struct switch_port_link *link)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);

	GSW_portLinkCfg_t parm;

	memset((void *)&parm, 0, sizeof(GSW_portLinkCfg_t));
	parm.nPortId = port;
	mutex_lock(&gsw->reg_mutex);
	GSW_PortLinkCfgGet((void *)&gsw->pd, &parm);
	mutex_unlock(&gsw->reg_mutex);

	link->link = !parm.eLink;
	link->duplex = !parm.eDuplex;
	link->speed = parm.eSpeed;
	// printk("get port:%d linkStatus:%d duplex:%d speed:%d\n",
	// port, parm.eLink, parm.eDuplex, parm.eSpeed);

	return 0;
}

static int intel_apply_vlan_config(struct switch_dev *dev)
{
	return 0;
}

static int intel_reset_vlan(struct switch_dev *dev)
{
	return 0;
}

static int intel_get_vlan_enable(struct switch_dev *dev,
                                 const struct switch_attr *attr,
                                 struct switch_val *val)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	ur r;
	GSW_return_t s;
	mutex_lock(&gsw->reg_mutex);
	s = gsw_reg_rd((void *)&gsw->pd,
	               PCE_GCTRL_0_VLAN_OFFSET,
	               PCE_GCTRL_0_VLAN_SHIFT,
	               PCE_GCTRL_0_VLAN_SIZE, &r);
	mutex_unlock(&gsw->reg_mutex);
	if (s != GSW_statusOk)
		return s;

	val->value.i = r;
	return 0;
}

static int intel_set_vlan_enable(struct switch_dev *dev,
                                 const struct switch_attr *attr,
                                 struct switch_val *val)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	// enable vlan aware
	GSW_return_t s;
	mutex_lock(&gsw->reg_mutex);
	s = gsw_reg_wr((void *)&gsw->pd,
	               PCE_GCTRL_0_VLAN_OFFSET,
	               PCE_GCTRL_0_VLAN_SHIFT,
	               PCE_GCTRL_0_VLAN_SIZE, val->value.i);
	mutex_unlock(&gsw->reg_mutex);
	if (s != GSW_statusOk)
		return s;

	return 0;
}

static int intel_get_vlan_fid(struct switch_dev *dev,
                              const struct switch_attr *attr,
                              struct switch_val *val)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	GSW_VLAN_IdGet_t parm;

	memset((void *)&parm, 0, sizeof(GSW_VLAN_IdGet_t));
	parm.nVId = val->port_vlan;
	mutex_lock(&gsw->reg_mutex);
	GSW_VLAN_IdGet((void *)&gsw->pd, &parm);
	mutex_unlock(&gsw->reg_mutex);

	val->value.i = parm.nFId;

	return 0;
}

static int intel_set_vlan_fid(struct switch_dev *dev,
                              const struct switch_attr *attr,
                              struct switch_val *val)
{
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	GSW_VLAN_IdCreate_t parm;

	memset((void *)&parm, 0, sizeof(GSW_VLAN_IdCreate_t));
	parm.nVId = val->port_vlan;
	parm.nFId = val->value.i;
	mutex_lock(&gsw->reg_mutex);
	GSW_VLAN_IdCreate((void *)&gsw->pd, &parm);
	mutex_unlock(&gsw->reg_mutex);

	return 0;
}

static int intel_get_port_mib(struct switch_dev *dev,
                              const struct switch_attr *attr,
                              struct switch_val *val)
{
	static char buf[4096];
	GSW_RMON_Port_cnt_t parm;
	struct intel_gsw *gsw = container_of(dev, struct intel_gsw, swdev);
	int len = 0;

	if (val->port_vlan >= INTEL_SWITCH_PORT_NUM)
		return -EINVAL;

	len += snprintf(buf + len, sizeof(buf) - len,
	                "Port %d MIB counters\n", val->port_vlan);

	parm.nPortId = val->port_vlan;
	mutex_lock(&gsw->reg_mutex);
	intel_count_rd(gsw, &parm);
	mutex_unlock(&gsw->reg_mutex);

	len += sprintf(buf + len,
	               "TxDrop      : %u\n"
	               "TxUni       : %u\n"
	               "TxMulti     : %u\n"
	               "TxBroad     : %u\n"
	               "TxCollision : %u\n"
	               "TxSingleCol : %u\n"
	               "TxMultiCol  : %u\n"
	               "TxLateCol   : %u\n"
	               "TxPause     : %u\n"
	               "Tx64Byte    : %u\n"
	               "Tx65Byte    : %u\n"
	               "Tx128Byte   : %u\n"
	               "Tx256Byte   : %u\n"
	               "Tx512Byte   : %u\n"
	               "Tx1024Byte  : %u\n"
	               "TxByte      : %llu\n",
	               parm.nTxDroppedPkts,
	               parm.nTxUnicastPkts,
	               parm.nTxMulticastPkts,
	               parm.nTxBroadcastPkts,
	               parm.nTxCollCount,
	               parm.nTxSingleCollCount,
	               parm.nTxMultCollCount,
	               parm.nTxLateCollCount,
	               parm.nTxPauseCount,
	               parm.nTx64BytePkts,
	               parm.nRx127BytePkts,
	               parm.nRx255BytePkts,
	               parm.nRx511BytePkts,
	               parm.nRx1023BytePkts,
	               parm.nRxMaxBytePkts,
	               parm.nTxGoodBytes);

	len += sprintf(buf + len,
	               "RxDrop      : %u\n"
	               "RxFiltered  : %u\n"
	               "RxUni       : %u\n"
	               "RxMulti     : %u\n"
	               "RxBroad     : %u\n"
	               "RxAlignErr  : %u\n"
	               "RxUnderSize : %u\n"
	               "RxOverSize  : %u\n"
	               "RxPause     : %u\n"
	               "Rx64Byte    : %u\n"
	               "Rx65Byte    : %u\n"
	               "Rx128Byte   : %u\n"
	               "Rx256Byte   : %u\n"
	               "Rx512Byte   : %u\n"
	               "Rx1024Byte  : %u\n"
	               "RxByte      : %llu\n",
	               parm.nRxDroppedPkts,
	               parm.nRxFilteredPkts,
	               parm.nRxUnicastPkts,
	               parm.nRxMulticastPkts,
	               parm.nRxBroadcastPkts,
	               parm.nRxAlignErrorPkts,
	               parm.nRxUnderSizeGoodPkts,
	               parm.nRxOversizeGoodPkts,
	               parm.nRxGoodPausePkts,
	               parm.nRx64BytePkts,
	               parm.nRx127BytePkts,
	               parm.nRx255BytePkts,
	               parm.nRx511BytePkts,
	               parm.nRx1023BytePkts,
	               parm.nRxMaxBytePkts,
	               parm.nRxGoodBytes);

	val->value.s = buf;
	val->len = len;
	return 0;
}

static struct switch_attr intel_globals[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.get = intel_get_vlan_enable,
		.set = intel_set_vlan_enable,
		.max = 1,
		.ofs = 1
	}
};

static struct switch_attr intel_port[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get MIB counters for port",
		.get = intel_get_port_mib,
		.set = NULL,
	},
};

static struct switch_attr intel_vlan[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "fid",
		.description = "Get/Set vlan FID",
		.set = intel_set_vlan_fid,
		.get = intel_get_vlan_fid,
	},
};

struct switch_dev_ops intel_switch_ops = {
	.attr_global = {
		.attr = intel_globals,
		.n_attr = ARRAY_SIZE(intel_globals),
	},
	.attr_port = {
		.attr = intel_port,
		.n_attr = ARRAY_SIZE(intel_port),
	},
	.attr_vlan = {
		.attr = intel_vlan,
		.n_attr = ARRAY_SIZE(intel_vlan),
	},

	.get_vlan_ports = intel_get_vlan_ports,
	.set_vlan_ports = intel_set_vlan_ports,
	.get_port_pvid = intel_get_port_pvid,
	.set_port_pvid = intel_set_port_pvid,
	.get_port_link = intel_get_port_link,
	.apply_config = intel_apply_vlan_config,
	.reset_switch = intel_reset_vlan,
};

int intel_swconfig_init(struct intel_gsw *gsw)
{
	struct switch_dev *swdev;
	int ret;

	mutex_init(&gsw->reg_mutex);

	gsw->cpu_port = RGMII_PORT0;

	swdev = &gsw->swdev;

	swdev->name = "gsw150";
	swdev->alias = "gsw150";
	swdev->cpu_port = gsw->cpu_port;
	swdev->ports = INTEL_SWITCH_PORT_NUM;
	swdev->vlans = INTEL_NUM_VLANS;
	swdev->ops = &intel_switch_ops;

	ret = register_switch(swdev, NULL);
	if (ret) {
		dev_notice(gsw->dev, "Failed to register switch %s\n",
		           swdev->name);
		return ret;
	}

	intel_apply_vlan_config(swdev);

	return 0;
}

void intel_swconfig_destroy(struct intel_gsw *gsw)
{
	mutex_destroy(&gsw->reg_mutex);
	unregister_switch(&gsw->swdev);
}

#endif

#if 0
int intel_check_phy_linkup(int port)
{
	ur r;
	GSW_return_t s;

	SF_MDIO_LOCK();
	s = gsw_reg_rd((void *)&gsw->pd,
	               (MAC_PSTAT_LSTAT_OFFSET + (0xC * port)),
	               MAC_PSTAT_LSTAT_SHIFT,
	               MAC_PSTAT_LSTAT_SIZE, &r);
	SF_MDIO_UNLOCK();
	if (s != GSW_statusOk)
		return s;

	return r ? 1 : 0;
}
#endif

void intel_led_init(struct intel_gsw *gsw, int led_mode)
{
	ethsw_api_dev_t *pedev = &gsw->pd;
	GSW_MMD_data_t md;
	int i = 0, mode = 0;

	if (led_mode == LED_ALL_ON)
		mode = GROUND_MODE;
	else if (led_mode == LED_ALL_BLINK)
		return; // switch not support, please use all on/off
	else
		mode = POWER_MODE;

	// port[0:4] <--> led[5:9]
	gsw_reg_wr((void *)pedev, LED_MD_CFG_LED5_OFFSET,
	           LED_MD_CFG_LED5_SHIFT,
	           LED_MD_CFG_LED5_SIZE,
	           mode);
	gsw_reg_wr((void *)pedev, LED_MD_CFG_LED6_OFFSET,
	           LED_MD_CFG_LED6_SHIFT,
	           LED_MD_CFG_LED6_SIZE,
	           mode);
	gsw_reg_wr((void *)pedev, LED_MD_CFG_LED7_OFFSET,
	           LED_MD_CFG_LED7_SHIFT,
	           LED_MD_CFG_LED7_SIZE,
	           mode);
	gsw_reg_wr((void *)pedev, LED_MD_CFG_LED8_OFFSET,
	           LED_MD_CFG_LED8_SHIFT,
	           LED_MD_CFG_LED8_SIZE,
	           mode);
	gsw_reg_wr((void *)pedev, LED_MD_CFG_LED9_OFFSET,
	           LED_MD_CFG_LED9_SHIFT,
	           LED_MD_CFG_LED9_SIZE,
	           mode);

	// config led when link is 10/100/1000Mbit/s
	for (i = 0; i < INTEL_PHY_PORT_NUM; i++) {
		md.nAddressDev = i;
		md.nAddressReg = 0x1F01E4;
		if (led_mode == LED_ALL_OFF)
			md.nData = 0x0; // no blink
		else
			md.nData = 0x70; // active on 10/100/1000

		GSW_MmdDataWrite((void *)&gsw->pd, &md);
	}
}

#if 0
void intel_ifg_init(void)
{
	// set ifg to 80 bit time
	intel_mdio_wr((MAC_CTRL_1_IPG_OFFSET + (RGMII_PORT0 * 0xc)),
	              MAC_CTRL_1_IPG_SHIFT,
	              MAC_CTRL_1_IPG_SIZE,
	              0xa);
}

void intel_disable_single_phy(int port_num)
{
	GSW_MDIO_data_t parm;
	int i;

	i = port_num;
	SF_MDIO_LOCK();
	parm.nAddressReg = 0;
	parm.nAddressDev = i;
	intel_phy_rd(&parm);
	parm.nData |= PHY_CTRL_ENABLE_POWER_DOWN;
	intel_phy_wr(&parm);
	SF_MDIO_UNLOCK();
}
#endif

void intel_disable_all_phy(struct intel_gsw *gsw)
{
	GSW_MDIO_data_t parm;
	int i;

	parm.nAddressReg = 0;
	for (i = 0; i < INTEL_PHY_PORT_NUM; i++) {
		parm.nAddressDev = i;
		intel_phy_rd(gsw, &parm);
		parm.nData |= PHY_CTRL_ENABLE_POWER_DOWN;
		intel_phy_wr(gsw, &parm);
	}
}

#if 0
void intel_enable_single_phy(struct sf_eswitch_priv *pesw_priv, int port_num)
{
	GSW_MDIO_data_t parm;
	int i;

	SF_MDIO_LOCK();
	parm.nAddressReg = 0;
	i = port_num;
	if (!check_port_in_portlist(pesw_priv, i)) {
		SF_MDIO_UNLOCK();
		return;
	}
	parm.nAddressDev = i;
	intel_phy_rd(&parm);
	parm.nData &= ~PHY_CTRL_ENABLE_POWER_DOWN;
	intel_phy_wr(&parm);
	SF_MDIO_UNLOCK();
}
#endif

void intel_enable_all_phy(struct intel_gsw *gsw)
{
	GSW_MDIO_data_t parm;
	int i;

	parm.nAddressReg = 0;
	for (i = 0; i < INTEL_PHY_PORT_NUM; i++) {
		parm.nAddressDev = i;
		intel_phy_rd(gsw, &parm);
		parm.nData &= ~PHY_CTRL_ENABLE_POWER_DOWN;
		intel_phy_wr(gsw, &parm);
	}
}


static int intel_rgmii_init(struct intel_gsw *gsw, int port)
{
	int s;
	GSW_MDIO_data_t md;
	uint r;

	s = intel_mdio_rd(gsw, (MAC_PSTAT_PACT_OFFSET + (0xC * port)),
	                  MAC_PSTAT_PACT_SHIFT,
	                  MAC_PSTAT_PACT_SIZE, &r);
	if (s != 0)
		return s;

	if (r) {
		uint pa, fd, pc;
		s = intel_mdio_rd(gsw, (PHY_ADDR_0_ADDR_OFFSET - port),
		                  PHY_ADDR_0_ADDR_SHIFT,
		                  PHY_ADDR_0_ADDR_SIZE, &pa);
		if (s != 0)
			return s;

		fd = 1; //DUPLEX_FULL
		pc = PHY_AN_ADV_1000FDX;
		md.nAddressReg = 9;
		md.nAddressDev = pa;
		s = intel_phy_rd(gsw, &md);
		if (s != 0)
			return s;
		md.nData &= ~(PHY_AN_ADV_1000HDX
		              | PHY_AN_ADV_1000FDX);
		md.nData |= pc;
		md.nAddressDev = pa;
		md.nAddressReg = 9;
		s = intel_phy_wr(gsw, &md);
		if (s != 0)
			return s;
		md.nAddressReg = 4;
		md.nAddressDev = pa;
		s = intel_phy_rd(gsw, &md);
		if (s != 0)
			return s;
		md.nData &= ~(PHY_AN_ADV_10HDX
		              | PHY_AN_ADV_10FDX
		              | PHY_AN_ADV_100HDX
		              | PHY_AN_ADV_100FDX);
		md.nAddressReg = 4;
		md.nAddressDev = pa;
		s = intel_phy_wr(gsw, &md);
		if (s != 0)
			return s;

		md.nAddressDev = pa;
		md.nAddressReg = 0;
		s = intel_phy_rd(gsw, &md);
		if (s != 0)
			return s;
		md.nData = 0x1200;
		s = intel_phy_wr(gsw, &md);
		if (s != 0)
			return s;
	} else {
		uint pr;
		s = intel_mdio_rd(gsw, (PHY_ADDR_0_ADDR_OFFSET - port),
		                  PHY_ADDR_0_ADDR_SHIFT,
		                  PHY_ADDR_0_REG_SIZE,
		                  &pr);
		if (s != 0)
			return s;

		//set GSW_DUPLEX_FULL
		pr &= ~(3 << 9);
		pr |= (1 << 9);

		//set GSW_PORT_LINK_UP
		pr &= ~(3 << 13);
		pr |= (1 << 13);

		//set GSW_PORT_SPEED_1000
		pr &= ~(3 << 11);
		pr |= (2 << 11);
		s = intel_mdio_wr(gsw, (PHY_ADDR_0_ADDR_OFFSET - port),
		                  PHY_ADDR_0_ADDR_SHIFT,
		                  PHY_ADDR_0_REG_SIZE,
		                  pr);
		if (s != 0)
			return s;
	}
	//'Low Power Idle' Support for 'Energy Efficient Ethernet'. default dsiable.
	s = intel_mdio_wr(gsw, (MAC_CTRL_4_LPIEN_OFFSET + (0xC * port)),
	                  MAC_CTRL_4_LPIEN_SHIFT,
	                  MAC_CTRL_4_LPIEN_SIZE, 0);
	return 0;
}

#if 1
static int intel_port_rgmii_dalay_set(struct intel_gsw *gsw, int port, int txDelay, int rxDelay)
{
	if ((txDelay > 7) || (rxDelay > 7))
		return -1;

	if(port == RGMII_PORT0) {
		intel_mdio_wr(gsw, PCDU_5_RXDLY_OFFSET,
		              PCDU_5_RXDLY_SHIFT,
		              PCDU_5_RXDLY_SIZE,
		              rxDelay);
		intel_mdio_wr(gsw, PCDU_5_TXDLY_OFFSET,
		              PCDU_5_TXDLY_SHIFT,
		              PCDU_5_TXDLY_SIZE,
		              txDelay);
	} else if(port == RGMII_PORT1) {
		intel_mdio_wr(gsw, PCDU_6_RXDLY_OFFSET,
		              PCDU_6_RXDLY_SHIFT,
		              PCDU_6_RXDLY_SIZE,
		              rxDelay);
		intel_mdio_wr(gsw, PCDU_6_TXDLY_OFFSET,
		              PCDU_6_TXDLY_SHIFT,
		              PCDU_6_TXDLY_SIZE,
		              txDelay);
	} else
		return -1;

	return 0;
}
#endif

#if 0
u32 intel_get_cpu_port_rx_mib(void)
{
	GSW_RMON_clear_t mp;
	GSW_RMON_Port_cnt_t parm;

	SF_MDIO_LOCK();
	parm.nPortId = RGMII_PORT0;
	intel_count_rd(&parm);
	// clear mib count after read
	mp.nRmonId = RGMII_PORT0;
	mp.eRmonType = GSW_RMON_ALL_TYPE;
	intel_count_clear(&mp);
	SF_MDIO_UNLOCK();
	return parm.nRxGoodPkts;
}

int intel_set_cpu_port_self_mirror(struct sf_eswitch_priv *pesw_priv, int port, int enable)
{
	GSW_portCfg_t pCfg;
	GSW_monitorPortCfg_t parm;
	GSW_MDIO_data_t mdioData;
	int i;

	SF_MDIO_LOCK();
	parm.nPortId = RGMII_PORT0;
	pCfg.nPortId = RGMII_PORT0;
	mdioData.nAddressReg = 0;
	GSW_PortCfgGet((void *)&gsw->pd, &pCfg);
	GSW_MonitorPortCfgGet((void *)&gsw->pd, &parm);

	if (enable) {
		parm.bMonitorPort = 1;
		pCfg.ePortMonitor = GSW_PORT_MONITOR_RX;
		// disable vlan
		gsw_reg_wr((void *)&gsw->pd,
		           PCE_GCTRL_0_VLAN_OFFSET,
		           PCE_GCTRL_0_VLAN_SHIFT,
		           PCE_GCTRL_0_VLAN_SIZE, 0);
		// disable all phy
		for (i = 0; i < INTEL_PHY_PORT_NUM; i++) {
			mdioData.nAddressDev = i;
			intel_phy_rd(&mdioData);
			mdioData.nData |= PHY_CTRL_ENABLE_POWER_DOWN;
			intel_phy_wr(&mdioData);
		}
	} else {
		parm.bMonitorPort = 0;
		pCfg.ePortMonitor = GSW_PORT_MONITOR_NONE;
		// enable vlan
		gsw_reg_wr((void *)&gsw->pd,
		           PCE_GCTRL_0_VLAN_OFFSET,
		           PCE_GCTRL_0_VLAN_SHIFT,
		           PCE_GCTRL_0_VLAN_SIZE, 1);
		// enable all phy
		for (i = 0; i < INTEL_PHY_PORT_NUM; i++) {
			if (!check_port_in_portlist(pesw_priv, i))
				continue;

			mdioData.nAddressDev = i;
			intel_phy_rd(&mdioData);
			mdioData.nData &= ~PHY_CTRL_ENABLE_POWER_DOWN;
			intel_phy_wr(&mdioData);
		}
	}
	GSW_MonitorPortCfgSet((void *)&gsw->pd, &parm);
	GSW_PortCfgSet((void *)&gsw->pd, &pCfg);
	SF_MDIO_UNLOCK();
	return 0;
}
#endif

void intel_init(struct intel_gsw *gsw)
{
	GSW_MMD_data_t md;
#ifdef CONFIG_SWCONFIG
	GSW_VLAN_portCfg_t pVlanCfg;
	int i;
#endif
	GSW_HW_Init_t pHwInit = {
		.eInitMode = GSW_HW_INIT_WR,
	};

	GSW_HW_Init(&gsw->pd, &pHwInit);

	intel_rgmii_init(gsw, RGMII_PORT0);
#if 1
	intel_port_rgmii_dalay_set(gsw, RGMII_PORT0, 2, 0);
#endif
	intel_enable_all_phy(gsw);

	intel_led_init(gsw, LED_ALL_DEFAULT);

	/* RM#9120 disable auto downspeed */
	for (i = 0; i < INTEL_PHY_PORT_NUM; i++) {
		md.nAddressDev = i;
		md.nAddressReg = 0x1F01EF;
		md.nData = 0x2;
		GSW_MmdDataWrite((void *)&gsw->pd, &md);
	}
#ifdef CONFIG_SWCONFIG
	/* port config init */
	for (i = 0; i < INTEL_SWITCH_PORT_NUM; i++) {
		pVlanCfg.nPortId = i;
		GSW_VLAN_PortCfgGet((void *)&gsw->pd, &pVlanCfg);

		pVlanCfg.eAdmitMode = GSW_VLAN_ADMIT_ALL;
		pVlanCfg.eVLAN_MemberViolation = GSW_VLAN_MEMBER_VIOLATION_BOTH;
		GSW_VLAN_PortCfgSet((void *)&gsw->pd, &pVlanCfg);
	}
#endif
}

void intel_deinit(struct intel_gsw *gsw)
{
	intel_disable_all_phy(gsw);
}

#if 0

int sf_intel_getAsicReg(unsigned int Offset, unsigned int *value)
{
	u16 Shift = 0, Size = 16;
	return intel_mdio_rd(Offset, Shift, Size, value);
}

int sf_intel_setAsicReg(unsigned int Offset, unsigned int value)
{
	u16 Shift = 0, Size = 16;
	return intel_mdio_wr(Offset, Shift, Size, value);
}
#endif
