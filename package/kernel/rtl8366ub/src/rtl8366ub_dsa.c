// SPDX-License-Identifier: GPL-2.0
/*
 * This is a sparsely documented chip, the only viable documentation seems
 * to be a patched up code drop from the vendor that appear in various
 * GPL source trees.
 *
 * Copyright (C) 2024 Jianhui Zhao <jianhui.zhao@gl-inet.com>
 */

#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/debugfs.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/bitops.h>
#include <linux/regmap.h>
#include <linux/of_address.h>
#include <net/dsa.h>
#include <linux/gpio/consumer.h>

#include "rtk_switch.h"
#include "port.h"
#include "vlan.h"
#include "cpu.h"
#include "rtl8366ub_dsa.h"
#include "dal/smi.h"

struct rtl8366ub_dbg {
    const char *name;
    struct file_operations ops;
};

extern void rtk_set_mdc_mdio(struct mii_bus *bus, int id);

static int rtl8366ub_find_cpu_port(struct dsa_switch *ds)
{
    /* Find the connected cpu port. Valid port are 3 or 8 */

    if (dsa_is_cpu_port(ds, EXT_PORT0))
        return EXT_PORT0;

    if (dsa_is_cpu_port(ds, EXT_PORT1))
        return EXT_PORT1;

    return -EINVAL;
}

static enum dsa_tag_protocol rtl8366ub_sw_get_tag_protocol(struct dsa_switch *ds, int port, enum dsa_tag_protocol mp)
{
    struct rtl8366ub_priv *priv = ds->priv;
    if (port != EXT_PORT0 && port != EXT_PORT1) {
        dev_warn(priv->dev, "port not matched with tagging CPU port\n");
        return DSA_TAG_PROTO_NONE;
    } else {
        return DSA_TAG_PROTO_RTL8_4;
    }
}

static int rtl8366ub_cpu_mac_config(struct dsa_switch *ds, rtksw_port_t port, int phy_mode)
{
    struct rtl8366ub_priv *priv = ds->priv;
    rtksw_port_mac_ability_t mac_cfg;
    rtksw_mode_ext_t mode_ext;

    mode_ext = RTKSW_MODE_EXT_HSGMII;
    mac_cfg.forcemode = PORT_MAC_FORCE;
    mac_cfg.speed = RTKSW_PORT_SPEED_2500M;
    mac_cfg.duplex = RTKSW_PORT_FULL_DUPLEX;
    mac_cfg.link = RTKSW_PORT_LINKUP;
    mac_cfg.nway = RTKSW_DISABLED;
    mac_cfg.txpause = RTKSW_ENABLED;
    mac_cfg.rxpause = RTKSW_ENABLED;

    switch (phy_mode) {
        case PHY_INTERFACE_MODE_2500BASEX:
            mac_cfg.speed = RTKSW_PORT_SPEED_2500M;
            mode_ext = RTKSW_MODE_EXT_HSGMII;
            break;
        case PHY_INTERFACE_MODE_10GKR:
        case PHY_INTERFACE_MODE_USXGMII:
            mac_cfg.speed = RTKSW_PORT_SPEED_10G;
            mode_ext = RTKSW_MODE_EXT_USXGMII;
            break;
        default:
            dev_err(priv->dev, "phy mode %d not supported for CPU port\n", phy_mode);
    }
    rtk_port_macForceLinkExt_set(port, mode_ext, &mac_cfg);
    return 0;
}

static int rtl8366ub_sw_setup(struct dsa_switch *ds)
{
    struct rtl8366ub_priv *priv = ds->priv;
    rtksw_portmask_t portmask;
    rtksw_portmask_t cpu_portmask;
    int ret, phy_mode, i;
    u32 val;

    /* Reset whole chip through gpio pin or memory-mapped registers for
     * different type of hardware
     */
    gpiod_set_value_cansleep(priv->reset, 0);
    usleep_range(100000, 150000);
    gpiod_set_value_cansleep(priv->reset, 1);
    usleep_range(1000000, 1500000);

    /* Detect device */
    ret = reg_mdcmdio_read(0x4, &val);
    if (ret) {
        dev_err(priv->dev, "can't get chip ID (%d)\n", ret);
        return ret;
    }

    switch (val) {
        case 0x8366:
            dev_info(priv->dev, "found an RTL8366UB switch\n");
            break;
        default:
            dev_err(priv->dev, "found an Unknown Realtek switch (id=0x%04x)\n",
                    val);
            return -ENODEV;
            break;
    }

    priv->cpu_port = rtl8366ub_find_cpu_port(ds);
    if (priv->cpu_port < 0) {
        dev_err(priv->dev, "No cpu port configured in both cpu port3 and port8");
        return -EINVAL;
    }

    phy_mode = of_get_phy_mode(ds->ports[priv->cpu_port].dn);
    if (phy_mode < 0) {
        dev_err(priv->dev, "Can't find phy-mode for master device\n");
        return -ENODEV;
    }

    rtk_switch_init();
    rtk_vlan_init();

    RTKSW_PORTMASK_CLEAR(portmask);
    for (i = 0; i < RTL8366UB_NUM_PORTS; i++) {
        RTKSW_PORTMASK_CLEAR(cpu_portmask);
        RTKSW_PORTMASK_PORT_SET(cpu_portmask, priv->cpu_port);
        rtk_port_isolation_set(i, &cpu_portmask);

        RTKSW_PORTMASK_PORT_SET(portmask, i);
    }
    rtk_port_isolation_set(EXT_PORT0, &portmask);
    rtk_port_isolation_set(priv->cpu_port, &portmask);

    rtl8366ub_cpu_mac_config(ds, priv->cpu_port, phy_mode);
    rtk_cpu_tagPort_set(priv->cpu_port, CPU_INSERT_TO_ALL);
    rtk_cpu_enable_set(RTKSW_ENABLED);

    rtk_port_phyEnableAll_set(RTKSW_ENABLED);
    return 0;
}

static void rtl8366ub_sw_get_strings(struct dsa_switch *ds, int port, u32 stringset,
                                     uint8_t *data)
{
}

static int rtl8366ub_sw_phy_read(struct dsa_switch *ds, int port, int regnum)
{
    unsigned int val;

    rtk_port_phyReg_get(port, regnum, &val);
    return val;
}

static int rtl8366ub_sw_phy_write(struct dsa_switch *ds, int port, int regnum, u16 val)
{
    rtk_port_phyReg_set(port, regnum, val);
    return 0;
}

static void rtl8366ub_sw_get_ethtool_stats(struct dsa_switch *ds, int port, uint64_t *data)
{
}

static int rtl8366ub_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
    return 0;
}

static int rtl8366ub_sw_port_enable(struct dsa_switch *ds, int port,
                                    struct phy_device *phy)
{
    struct rtl8366ub_priv *priv = ds->priv;

    if (!dsa_is_user_port(ds, port))
        return 0;

    mutex_lock(&priv->reg_mutex);
    priv->ports[port].enable = true;
    rtk_port_phyReg_set(port, 0xa610, 0x2058);
    mutex_unlock(&priv->reg_mutex);
    return 0;
}

static void rtl8366ub_sw_port_disable(struct dsa_switch *ds, int port)
{
    struct rtl8366ub_priv *priv = ds->priv;
    if (!dsa_is_user_port(ds, port))
        return;

    mutex_lock(&priv->reg_mutex);

    /* Clear up all port matrix which could be restored in the next
     * enablement for the port.
     */
    priv->ports[port].enable = false;
    rtk_port_phyReg_set(port, 0xa610, 0x2858);

    mutex_unlock(&priv->reg_mutex);
}

static void rtl8366ub_sw_stp_state_set(struct dsa_switch *ds, int port, u8 state)
{
    u32 stp_state;

    if (dsa_is_unused_port(ds, port))
        return;

    switch (state) {
        case BR_STATE_DISABLED:
            stp_state = RTKSW_STP_STATE_DISABLED;
            break;
        case BR_STATE_BLOCKING:
            stp_state = RTKSW_STP_STATE_BLOCKING;
            break;
        case BR_STATE_LEARNING:
            stp_state = RTKSW_STP_STATE_LEARNING;
            break;
        case BR_STATE_FORWARDING:
        default:
            stp_state = RTKSW_STP_STATE_FORWARDING;
            break;
    }

    rtk_stp_mstpState_set(1, port, stp_state);
}

static int rtl8366ub_sw_port_bridge_join(struct dsa_switch *ds, int port, struct net_device *br)
{
    int i;
    rtksw_portmask_t portmask_tmp;
    rtksw_portmask_t portmask;
    rtk_port_isolation_get(port, &portmask);

    for (i = 0; i < RTL8366UB_NUM_PORTS; i++) {
        if (i == port)
            continue;

        if (dsa_to_port(ds, i)->bridge_dev != br)
            continue;

        /* Join this port to each other port on the bridge */
        rtk_port_isolation_get(i, &portmask_tmp);
        RTKSW_PORTMASK_PORT_SET(portmask_tmp, port);
        rtk_port_isolation_set(i, &portmask_tmp);

        RTKSW_PORTMASK_PORT_SET(portmask, i);
    }

    /* Join each other port on the bridge to this port */
    rtk_port_isolation_set(port, &portmask);

    return 0;
}

static void rtl8366ub_sw_port_bridge_leave(struct dsa_switch *ds, int port, struct net_device *br)
{
    int i;
    rtksw_portmask_t portmask_tmp;
    rtksw_portmask_t portmask;
    rtk_port_isolation_get(port, &portmask);

    for (i = 0; i < RTL8366UB_NUM_PORTS; i++) {
        if (i == port)
            continue;

        if (dsa_to_port(ds, i)->bridge_dev != br)
            continue;

        /* Remove this port from any other port on the bridge */
        rtk_port_isolation_get(i, &portmask_tmp);
        RTKSW_PORTMASK_PORT_CLEAR(portmask_tmp, port);
        rtk_port_isolation_set(i, &portmask_tmp);

        RTKSW_PORTMASK_PORT_CLEAR(portmask, i);
    }

    rtk_port_isolation_set(port, &portmask);
}

static void rtl8366ub_sw_port_vlan_add(struct dsa_switch *ds, int port,
                                       const struct switchdev_obj_port_vlan *vlan)
{
    struct rtl8366ub_priv *priv = ds->priv;
    u16 vid;
    rtksw_vlan_cfg_t vlan_entry;

    pr_info("add vlan, port%d, flags: 0x%x, vid: %d - %d\n", port, vlan->flags, vlan->vid_begin, vlan->vid_end);
    mutex_lock(&priv->reg_mutex);

    for (vid = vlan->vid_begin; vid <= vlan->vid_end; ++vid) {
        memset(&vlan_entry, 0x00, sizeof(rtksw_vlan_cfg_t));
        RTK_PORTMASK_PORT_SET(vlan_entry.mbr, port);

        if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED)
            RTK_PORTMASK_PORT_SET(vlan_entry.untag, port);

        rtk_vlan_set(vid, &vlan_entry);
    }

    if (vlan->flags & BRIDGE_VLAN_INFO_PVID) {
        rtk_vlan_portPvid_set(port, vlan->vid_end, 0);
        priv->ports[port].pvid = vlan->vid_end;
    }

    mutex_unlock(&priv->reg_mutex);
}

static int rtl8366ub_sw_port_vlan_del(struct dsa_switch *ds, int port,
                                      const struct switchdev_obj_port_vlan *vlan)
{
    struct rtl8366ub_priv *priv = ds->priv;
    u16 vid, pvid;
    rtksw_vlan_cfg_t vlan_entry;

    pr_info("del vlan, port%d, flags: 0x%x, vid: %d - %d\n", port, vlan->flags, vlan->vid_begin, vlan->vid_end);
    mutex_lock(&priv->reg_mutex);

    pvid = priv->ports[port].pvid;
    for (vid = vlan->vid_begin; vid <= vlan->vid_end; ++vid) {
        memset(&vlan_entry, 0x00, sizeof(rtksw_vlan_cfg_t));
        RTK_PORTMASK_PORT_CLEAR(vlan_entry.mbr, port);
        RTK_PORTMASK_PORT_CLEAR(vlan_entry.untag, port);

        rtk_vlan_set(vid, &vlan_entry);

        if (pvid == vid) {
            rtk_vlan_portPvid_set(port, 1, 0);
            priv->ports[port].pvid = 1;
        }
    }

    mutex_unlock(&priv->reg_mutex);
    return 0;
}

static int rtl8366ub_sw_port_vlan_prepare(struct dsa_switch *ds, int port,
        const struct switchdev_obj_port_vlan *vlan)
{
    struct rtl8366ub_priv *priv = ds->priv;

    mutex_lock(&priv->reg_mutex);
    rtk_vlan_init();
    mutex_unlock(&priv->reg_mutex);
    return 0;
}

static void rtl8366ub_sw_phylink_validate(struct dsa_switch *ds, int port,
        unsigned long *supported,
        struct phylink_link_state *state)
{
    switch (port) {
        case 0:
        case 1:
        case EXT_PORT0:
        case EXT_PORT1:
            phylink_set(supported, 10baseT_Half);
            phylink_set(supported, 10baseT_Full);
            phylink_set(supported, 100baseT_Half);
            phylink_set(supported, 100baseT_Full);
            phylink_set(supported, 1000baseX_Full);
            phylink_set(supported, 2500baseX_Full);
            break;
        case 2:
        case 3:
            phylink_set(supported, 10baseT_Half);
            phylink_set(supported, 10baseT_Full);
            phylink_set(supported, 100baseT_Half);
            phylink_set(supported, 100baseT_Full);
            phylink_set(supported, 1000baseX_Full);
            break;
    }
    return;
}

static int rtl8366ub_sw_phylink_mac_link_state(struct dsa_switch *ds, int port,
        struct phylink_link_state *state)
{
    rtksw_port_mac_ability_t status;

    state->speed = SPEED_UNKNOWN;
    state->duplex = DUPLEX_UNKNOWN;
    state->pause = 0;

    rtk_port_macStatus_get(port, &status);

    state->link = status.link;
    state->an_complete = state->link;

    if (!state->link)
        return 0;

    state->duplex = status.duplex;

    if (status.txpause || status.rxpause)
        state->pause = 1;

    switch (status.speed) {
        case PORT_SPEED_10M:
            state->speed = SPEED_10;
            break;
        case PORT_SPEED_100M:
            state->speed = SPEED_10;
            break;
        case PORT_SPEED_1000M:
            state->speed = SPEED_1000;
            break;
        case PORT_SPEED_2500M:
            state->speed = SPEED_2500;
            break;
    }

    return 1;
}

static void rtl8366ub_sw_phylink_mac_config(struct dsa_switch *ds, int port, unsigned int mode,
        const struct phylink_link_state *state)
{
    rtksw_port_phy_ability_t ability;

    if (state->pause == MLO_PAUSE_TX || state->pause == MLO_PAUSE_RX)
        ability.AsyFC = 1;
    else
        ability.FC = 1;
    if (state->an_enabled) {
        ability.Half_10 = 1;
        ability.Half_100 = 1;
        if (state->duplex) {
            ability.Full_10 = 1;
            ability.Full_100 = 1;
            ability.Full_1000 = 1;
            ability.Full_2P5G = 1;
        }

        rtk_port_phyAutoNegoAbility_set(port, &ability);
    } else {
        if (state->duplex) {
            switch (state->speed) {
                case SPEED_10:
                    ability.Full_10 = 1;
                    break;
                case SPEED_100:
                    ability.Full_100 = 1;
                    break;
                case SPEED_1000:
                    ability.Full_1000 = 1;
                    break;
                case SPEED_2500:
                    ability.Full_2P5G = 1;
                    break;
                default:
                    ability.Full_1000 = 1;
            }
        } else {
            switch (state->speed) {
                case SPEED_10:
                    ability.Half_10 = 1;
                    break;
                default:
                    ability.Half_100 = 1;
            }
        }

        rtk_port_phyForceModeAbility_set(port, &ability);
    }
}



static void rtl8366ub_sw_phylink_mac_link_down(struct dsa_switch *ds, int port,
        unsigned int mode,
        phy_interface_t interface)
{
    rtksw_port_mac_ability_t mac_cfg;

    if (port >= RTL8366UB_NUM_PORTS)
        return;

    mac_cfg.link = RTKSW_PORT_LINKDOWN;
    rtk_port_macForceLink_set(port, &mac_cfg);
}


static void rtl8366ub_sw_phylink_mac_link_up(struct dsa_switch *ds, int port,
        unsigned int mode,
        phy_interface_t interface,
        struct phy_device *phydev,
        int speed, int duplex,
        bool tx_pause, bool rx_pause)
{
    rtksw_port_mac_ability_t mac_cfg;

    if (port >= RTL8366UB_NUM_PORTS)
        return;

    switch (speed) {
        case SPEED_10:
            mac_cfg.speed = RTKSW_PORT_SPEED_10M;
            break;
        case SPEED_100:
            mac_cfg.speed = RTKSW_PORT_SPEED_100M;
            break;
        case SPEED_1000:
            mac_cfg.speed = RTKSW_PORT_SPEED_1000M;
            break;
        case SPEED_2500:
            mac_cfg.speed = RTKSW_PORT_SPEED_2500M;
            break;
        default:
            mac_cfg.speed = RTKSW_PORT_SPEED_2500M;
    }
    mac_cfg.forcemode = PORT_MAC_NORMAL;
    mac_cfg.duplex = duplex;
    mac_cfg.link = RTKSW_PORT_LINKUP;
    mac_cfg.nway = RTKSW_DISABLED;
    mac_cfg.txpause = tx_pause;
    mac_cfg.rxpause = rx_pause;

    rtk_port_macForceLink_set(port, &mac_cfg);
}

static const struct dsa_switch_ops rtl8366ub_switch_ops = {
    .get_tag_protocol = rtl8366ub_sw_get_tag_protocol,
    .setup = rtl8366ub_sw_setup,
    .get_strings = rtl8366ub_sw_get_strings,
    .phy_read = rtl8366ub_sw_phy_read,
    .phy_write = rtl8366ub_sw_phy_write,
    .get_ethtool_stats = rtl8366ub_sw_get_ethtool_stats,
    .get_sset_count = rtl8366ub_get_sset_count,
    .port_enable = rtl8366ub_sw_port_enable,
    .port_disable = rtl8366ub_sw_port_disable,
    .port_stp_state_set = rtl8366ub_sw_stp_state_set,
    .port_bridge_join = rtl8366ub_sw_port_bridge_join,
    .port_bridge_leave = rtl8366ub_sw_port_bridge_leave,
    .port_vlan_add = rtl8366ub_sw_port_vlan_add,
    .port_vlan_del = rtl8366ub_sw_port_vlan_del,
    .port_vlan_prepare = rtl8366ub_sw_port_vlan_prepare,
    .phylink_validate = rtl8366ub_sw_phylink_validate,
    .phylink_mac_link_state = rtl8366ub_sw_phylink_mac_link_state,
    .phylink_mac_config = rtl8366ub_sw_phylink_mac_config,
    .phylink_mac_link_down = rtl8366ub_sw_phylink_mac_link_down,
    .phylink_mac_link_up = rtl8366ub_sw_phylink_mac_link_up,
};

static int port_isolation_show(struct seq_file *s, void *v)
{
    rtksw_portmask_t mask;
    int i;

    for (i = 0; i < RTL8366UB_NUM_PORTS; i++) {
        rtk_port_isolation_get(i, &mask);
        seq_printf(s, "port%d: 0x%x\n", i, mask.bits[0]);
    }

    return 0;
}
DEFINE_SHOW_ATTRIBUTE(port_isolation);

static ssize_t phy_reg_read(struct file *file,
                            const char __user *user_buf,
                            size_t count, loff_t *ppos)
{
    char buf[256] = "";
    int phy, reg, val;

    copy_from_user(buf, user_buf, count);

    sscanf(buf, "%d %x\n", &phy, (unsigned int *)&reg);

    rtk_port_phyReg_get(phy, reg, &val);

    pr_info("phy: %d, reg: 0x%x = 0x%x\n", phy, reg, val);

    return count;
}

static const struct file_operations phy_reg_fops = {
    .open = simple_open,
    .write = phy_reg_read,
};

static int rtl8366ub_mdio_probe(struct mdio_device *mdiodev)
{
    struct rtl8366ub_priv *priv;
    struct device_node *switch_node = NULL;
    struct device_node *dn;
    int ret;

    dn = mdiodev->dev.of_node;

    priv = devm_kzalloc(&mdiodev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    priv->ds = dsa_switch_alloc(&mdiodev->dev, EXT_PORT1 + 1);
    if (!priv->ds)
        return -ENOMEM;

    dev_info(&mdiodev->dev, "RTL8366UB DSA driver, version %s, mdio addr %d\n",
             DRIVER_VERSION,  mdiodev->addr);

    priv->reset = devm_gpiod_get_optional(&mdiodev->dev, "reset",
                                          GPIOD_OUT_LOW);
    if (IS_ERR(priv->reset)) {
        dev_err(&mdiodev->dev, "Couldn't get our reset line\n");
        return PTR_ERR(priv->reset);
    }

    switch_node = of_find_node_by_name(NULL, "switch0");
    if (switch_node) {
        priv->base = of_iomap(switch_node, 0);
        if (priv->base == NULL) {
            dev_err(&mdiodev->dev, "of_iomap failed\n");
            return -ENOMEM;
        }
    }

    priv->bus = mdiodev->bus;
    priv->dev = &mdiodev->dev;
    priv->ds->priv = priv;
    priv->ds->ops = &rtl8366ub_switch_ops;
    mutex_init(&priv->reg_mutex);
    dev_set_drvdata(&mdiodev->dev, priv);

    priv->mdio_addr = mdiodev->addr;

    rtk_set_mdc_mdio(priv->bus, priv->mdio_addr);

    ret = dsa_register_switch(priv->ds);
    if (ret) {
        if (ret == -EPROBE_DEFER)
            dev_err(priv->dev, "unable to register switch, deferred\n");
        else
            dev_err(priv->dev, "unable to register switch ret = %d\n", ret);
        return ret;
    }

    priv->dbgfs = debugfs_create_dir("rtl8366ub", NULL);

    debugfs_create_file("port_isolation", S_IRUSR, priv->dbgfs, NULL, &port_isolation_fops);
    debugfs_create_file("phy_reg", S_IWUSR, priv->dbgfs, NULL, &phy_reg_fops);

    return 0;
}

static void rtl8366ub_mdio_remove(struct mdio_device *mdiodev)
{
    struct rtl8366ub_priv *priv = dev_get_drvdata(&mdiodev->dev);

    debugfs_remove_recursive(priv->dbgfs);

    dsa_unregister_switch(priv->ds);
}

static const struct of_device_id rtl8366ub_mdio_of_match[] = {
    { .compatible = "realtek,rtl8366ub" },
    { /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, rtl8366ub_mdio_of_match);

static struct mdio_driver rtl8366ub_mdio_driver = {
    .mdiodrv.driver = {
        .name = "rtl8366ub-mdio",
        .of_match_table = rtl8366ub_mdio_of_match,
    },
    .probe  = rtl8366ub_mdio_probe,
    .remove = rtl8366ub_mdio_remove,
};

mdio_module_driver(rtl8366ub_mdio_driver);

MODULE_AUTHOR("Jianhui Zhao <jianhui.zhao@gl-inet.com>");
MODULE_DESCRIPTION("Driver for RTL8372 ethernet switch");
MODULE_LICENSE("GPL");
