#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/reset.h>
#include <linux/hrtimer.h>
#include <linux/mii.h>
#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/of_net.h>
#include <linux/of_irq.h>
#include <linux/phy.h>
#include <linux/version.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/skbuff.h>
#include <linux/switch.h>
#include <linux/debugfs.h>
#include <net/rtnetlink.h>

#include "rtk_switch.h"
#include "port.h"
#include "vlan.h"
#include "cpu.h"
#include "dal/smi.h"
#include "rtl8366ub_mdio.h"
#include "stat.h"
#include "interrupt.h"

#define RTL_CHIP_NAME                  "rtl8366ub"
#define RTL_PROC_DIR                   "rtl_sw"
#define RTL_PROC_NODE_DEVICE           "device"

//RTL8367C_PHY_PORT_NUM + ext0 + ext1
#define RTL8366UB_NUM_PORTS 4
#define RTL8366UB_NUM_VIDS  4096

static DEFINE_MUTEX(rtl8366ub_devs_lock);
struct proc_dir_entry *proc_rtl8366ub_gsw_dir;

static int rtl8366ub_hw_reset(struct gsw_rtl8366ub *gsw);
static void rtl8366ub_sgmii_config(rtksw_port_t port);
static int rtl8366ub_swconfig_init(struct gsw_rtl8366ub *gsw);
static void rtl8366ub_swconfig_destroy(struct gsw_rtl8366ub *gsw);
static void rtl8366ub_wan_status_update(int port_id, rtksw_port_linkStatus_t links);
static void rtl8366ub_irq_worker(struct work_struct *work);

static inline int rtl8366ub_get_duplex(const struct device_node *np)
{
    return of_property_read_bool(np, "full-duplex");
}

static int rtl8366ub_hw_reset(struct gsw_rtl8366ub *gsw)
{
    struct device_node *np = gsw->dev->of_node;
    int ret;

    gsw->reset_pin = of_get_named_gpio(np, "reset-gpios", 0);
    if (gsw->reset_pin < 0) {
        dev_info(gsw->dev, "No reset pin of switch\n");
        return 0;
    }

    ret = devm_gpio_request(gsw->dev, gsw->reset_pin, "rtl8366ub-reset");
    if (ret) {
        dev_info(gsw->dev, "Failed to request gpio %d\n",
                 gsw->reset_pin);
        return ret;
    }

    gpio_direction_output(gsw->reset_pin, 0);
    gpio_set_value(gsw->reset_pin, 0);
    usleep_range(150000, 200000);
    gpio_set_value(gsw->reset_pin, 1);
    usleep_range(1500000, 2000000);

    return 0;
}

static int rtl8366ub_proc_device_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "%s\n", RTL_CHIP_NAME);

    return 0;
}

static int rtl8366ub_proc_device_open(struct inode *inode, struct file *file)
{
    return single_open(file, rtl8366ub_proc_device_read, 0);
}

#if (KERNEL_VERSION(5, 6, 0) <= LINUX_VERSION_CODE)
static const struct proc_ops rtl8366ub_proc_device_fops = {
    .proc_open    = rtl8366ub_proc_device_open,
    .proc_read    = seq_read,
    .proc_lseek    = seq_lseek,
    .proc_release    = single_release,
};
#else
static const struct file_operations rtl8366ub_proc_device_fops = {
    .owner    = THIS_MODULE,
    .open    = rtl8366ub_proc_device_open,
    .read    = seq_read,
    .llseek    = seq_lseek,
    .release    = single_release,
};
#endif

/*common rtl8366ub swconfig entry API*/
static int rtl8366ub_sw_set_vlan_enable(struct switch_dev *dev,
                                        const struct switch_attr *attr,
                                        struct switch_val *val)
{
    struct gsw_rtl8366ub *gsw = container_of(dev, struct gsw_rtl8366ub, swdev);

    gsw->global_vlan_enable = val->value.i != 0;

    return 0;
}

static int rtl8366ub_sw_get_vlan_enable(struct switch_dev *dev,
                                        const struct switch_attr *attr,
                                        struct switch_val *val)
{
    struct gsw_rtl8366ub *gsw = container_of(dev, struct gsw_rtl8366ub, swdev);

    val->value.i = gsw->global_vlan_enable;

    return 0;
}

static int rtl8366ub_sw_reset_mibs(struct switch_dev *dev,
                                   const struct switch_attr *attr,
                                   struct switch_val *val)
{
    return rtk_stat_global_reset();

}

static int rtl8366ub_sw_reset_port_mibs(struct switch_dev *dev,
                                        const struct switch_attr *attr,
                                        struct switch_val *val)
{
    int port;

    port = val->port_vlan;
    if (port >= RTL8366UB_NUM_PORTS)
        return -EINVAL;

    return rtk_stat_port_reset(port);
}

struct rtl8366ub_mib_counter {
    const char *name;
};

static  struct rtl8366ub_mib_counter rtl8366ub_mibs[] = {
    {"ifInOctets"},
    {"dot3StatsFCSErrors"},
    {"dot3StatsSymbolErrors"},
    {"dot3InPauseFrames"},
    {"dot3ControlInUnknownOpcodes"},
    {"etherStatsFragments"},
    {"etherStatsJabbers"},
    {"ifInUcastPkts"},
    {"etherStatsDropEvents"},
    {"etherStatsOctets"},
    {"etherStatsUndersizePkts"},
    {"etherStatsOversizePkts"},
    {"etherStatsPkts64Octets"},
    {"etherStatsPkts65to127Octets"},
    {"etherStatsPkts128to255Octets"},
    {"etherStatsPkts256to511Octets"},
    {"etherStatsPkts512to1023Octets"},
    {"etherStatsPkts1024toMaxOctets"},
    {"etherStatsMcastPkts"},
    {"etherStatsBcastPkts"},
    {"ifOutOctets"},
    {"dot3StatsSingleCollisionFrames"},
    {"dot3StatsMultipleCollisionFrames"},
    {"dot3StatsDeferredTransmissions"},
    {"dot3StatsLateCollisions"},
    {"etherStatsCollisions"},
    {"dot3StatsExcessiveCollisions"},
    {"dot3OutPauseFrames"},
    {"dot1dBasePortDelayExceededDiscards"},
    {"dot1dTpPortInDiscards"},
    {"ifOutUcastPkts"},
    {"ifOutMulticastPkts"},
    {"ifOutBrocastPkts"},
    {"outOampduPkts"},
    {"inOampduPkts"},
    {"pktgenPkts"},
    {"inMldChecksumError"},
    {"inIgmpChecksumError"},
    {"inMldSpecificQuery"},
    {"inMldGeneralQuery"},
    {"inIgmpSpecificQuery"},
    {"inIgmpGeneralQuery"},
    {"inMldLeaves"},
    {"inIgmpLeaves"},
    {"inIgmpJoinsSuccess"},
    {"inIgmpJoinsFail"},
    {"inMldJoinsSuccess"},
    {"inMldJoinsFail"},
    {"inReportSuppressionDrop"},
    {"inLeaveSuppressionDrop"},
    {"outIgmpReports"},
    {"outIgmpLeaves"},
    {"outIgmpGeneralQuery"},
    {"outIgmpSpecificQuery"},
    {"outMldReports"},
    {"outMldLeaves"},
    {"outMldGeneralQuery"},
    {"outMldSpecificQuery"},
    {"inKnownMulticastPkts"},
    {"ifInMulticastPkts"},
    {"ifInBroadcastPkts"},
    {"ifOutDiscards"},
};

static int rtl83666ub_get_mibs_num(void)
{
    return ARRAY_SIZE(rtl8366ub_mibs);
}

static const char *rtl8366ub_get_mib_name(int idx)
{
    return rtl8366ub_mibs[idx].name;
}

static int rtl8366ub_sw_get_port_mib(struct switch_dev *dev,
                                     const struct switch_attr *attr,
                                     struct switch_val *val)
{
    int i, len = 0;
    static char mib_buf[4096];
    rtk_stat_counter_t counters;

    if (val->port_vlan >= RTL8366UB_NUM_PORTS) {
        if ((val->port_vlan != EXT_PORT0) && (val->port_vlan != EXT_PORT1)) {
            return -EINVAL;
        }
    }

    len += snprintf(mib_buf + len, sizeof(mib_buf) - len,
                    "Port %d MIB counters\n", val->port_vlan);

    for (i = 0; i < rtl83666ub_get_mibs_num(); ++i) {
        counters = 0;
        rtk_stat_port_get(val->port_vlan, i, &counters);
        len += snprintf(mib_buf + len, sizeof(mib_buf) - len,
                        "%-36s: ", rtl8366ub_get_mib_name(i));
        len += snprintf(mib_buf + len, sizeof(mib_buf) - len, "%llu\n",
                        counters);
    }

    val->value.s = mib_buf;
    val->len = len;
    return 0;
}

static int rtl8366ub_sw_get_vlan_info(struct switch_dev *dev,
                                      const struct switch_attr *attr,
                                      struct switch_val *val)
{
    int i;
    u32 len = 0;
    rtksw_vlan_cfg_t vlan;
    static char vlan_buf[256];
    int err;

    if (0 >= val->port_vlan || RTL8366UB_NUM_VIDS <= val->port_vlan) {
        return -EINVAL;
    }

    memset(vlan_buf, '\0', sizeof(vlan_buf));

    err = rtk_vlan_get(val->port_vlan, &vlan);
    if (err) {
        return err;
    }

    len += snprintf(vlan_buf + len, sizeof(vlan_buf) - len,
                    "VLAN %d: Ports: '", val->port_vlan);

    for (i = 0; i < RTL8366UB_NUM_PORTS; i++) {
        if (!(RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, i))) {
            continue;
        }

        len += snprintf(vlan_buf + len, sizeof(vlan_buf) - len, "%d%s", i,
                        (RTKSW_PORTMASK_IS_PORT_SET(vlan.untag, i)) ? "" : "t");
    }

    if (RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, EXT_PORT0)) {
        len += snprintf(vlan_buf + len, sizeof(vlan_buf) - len, "%d%s", EXT_PORT0,
                        (RTKSW_PORTMASK_IS_PORT_SET(vlan.untag, EXT_PORT0)) ? "" : "t");
    }

    if (RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, EXT_PORT1)) {
        len += snprintf(vlan_buf + len, sizeof(vlan_buf) - len, "%d%s", EXT_PORT1,
                        (RTKSW_PORTMASK_IS_PORT_SET(vlan.untag, EXT_PORT1)) ? "" : "t");
    }

    len += snprintf(vlan_buf + len, sizeof(vlan_buf) - len,
                    "', members=%04x, untag=%04x, fid=%u",
                    vlan.mbr.bits[0], vlan.untag.bits[0], vlan.fid_msti);

    val->value.s = vlan_buf;
    val->len = len;

    return 0;
}

static int rtl8366ub_sw_get_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
    struct switch_port *port;
    rtksw_vlan_cfg_t vlan;
    int port_id;

    if (0 >= val->port_vlan || RTL8366UB_NUM_VIDS <= val->port_vlan)
        return -EINVAL;

    if (rtk_vlan_get(val->port_vlan, &vlan))
        return -EINVAL;

    port = &val->value.ports[0];
    val->len = 0;
    for (port_id = 0; port_id < RTL8366UB_NUM_PORTS ; port_id++) {
        if (!(RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, port_id)))
            continue;

        port->id = port_id;
        port->flags = (RTKSW_PORTMASK_IS_PORT_SET(vlan.untag, port_id)) ?
                      0 : BIT(SWITCH_PORT_FLAG_TAGGED);
        val->len++;
        port++;
    }

    if (RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, EXT_PORT0)) {
        port->id = EXT_PORT0;
        port->flags = (RTKSW_PORTMASK_IS_PORT_SET(vlan.untag, EXT_PORT0)) ?
                      0 : BIT(SWITCH_PORT_FLAG_TAGGED);
        val->len++;
        port++;
    }

    if (RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, EXT_PORT1)) {
        port->id = EXT_PORT1;
        port->flags = (RTKSW_PORTMASK_IS_PORT_SET(vlan.untag, EXT_PORT1)) ?
                      0 : BIT(SWITCH_PORT_FLAG_TAGGED);
        val->len++;
        port++;
    }

    return 0;
}

static int rtl8366ub_sw_set_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
    struct switch_port *port;
    rtksw_vlan_cfg_t vlan = {0};
    int err, i, pvid = 0;
    rtksw_pri_t port_Pri;

    if (0 >= val->port_vlan || RTL8366UB_NUM_VIDS <= val->port_vlan)
        return -EINVAL;

    //RTK_PORTMASK_PORT_SET(vlan.mbr, EXT_PORT1);
    port = &val->value.ports[0];
    for (i = 0; i < val->len; i++, port++) {
        RTK_PORTMASK_PORT_SET(vlan.mbr, port->id);

        if (!(port->flags & BIT(SWITCH_PORT_FLAG_TAGGED)))
            RTK_PORTMASK_PORT_SET(vlan.untag, port->id);

        if (EXT_PORT0 == port->id || EXT_PORT1 == port->id) {
            continue;
        }
        /*
         * To ensure that we have a valid MC entry for this VLAN,
         * initialize the port VLAN ID here.
         */
        err = rtk_vlan_portPvid_get(port->id, &pvid, &port_Pri);
        if (err < 0)
            return err;

        if (pvid == 0) {
            err = rtk_vlan_portPvid_set(port->id, val->port_vlan, port_Pri);
            if (err < 0)
                return err;
        }
    }
    vlan.ivl_en = 1;

    pr_info("[%s] vid=%d , mem=%x,untag=%x\n", __func__, val->port_vlan,
            vlan.mbr.bits[0], vlan.untag.bits[0]);

    return rtk_vlan_set(val->port_vlan, &vlan);
}

static int rtl8366ub_sw_get_port_pvid(struct switch_dev *dev, int port, int *val)
{
    rtksw_pri_t port_Pri;

    return rtk_vlan_portPvid_get(port, val, &port_Pri);
}

static int rtl8366ub_sw_set_port_pvid(struct switch_dev *dev, int port, int val)
{
    return rtk_vlan_portPvid_set(port, val, 0);
}

static int rtl8366ub_sw_reset_switch(struct switch_dev *dev)
{
    struct gsw_rtl8366ub *gsw = container_of(dev, struct gsw_rtl8366ub, swdev);

    gpio_direction_output(gsw->reset_pin, 0);
    gpio_set_value(gsw->reset_pin, 0);
    usleep_range(100000, 150000);
    gpio_set_value(gsw->reset_pin, 1);
    usleep_range(1000000, 1500000);

    rtk_switch_init();
    rtk_vlan_init();
    rtl8366ub_sgmii_config(EXT_PORT1);
    rtl8366ub_irq_enable(gsw);

    return 0;
}

static int rtl8366ub_sw_get_port_link(struct switch_dev *dev, int port,
                                      struct switch_port_link *link)
{
    rtksw_port_linkStatus_t LinkStatus;
    rtksw_port_speed_t speed;
    rtksw_port_duplex_t duplex;

    if (port >= RTL8366UB_NUM_PORTS)
        return -EINVAL;

    if (rtk_port_phyStatus_get(port, &LinkStatus, &speed, &duplex))
        return -EINVAL;

    link->link = LinkStatus ? 1 : 0;
    link->duplex = duplex ? 1 : 0;
    if (!link->link)
        return 0;

    switch (speed) {
        case RTK_PORT_SPEED_10M:
            link->speed = SWITCH_PORT_SPEED_10;
            break;
        case RTK_PORT_SPEED_100M:
            link->speed = SWITCH_PORT_SPEED_100;
            break;
        case RTK_PORT_SPEED_1000M:
            link->speed = SWITCH_PORT_SPEED_1000;
            break;
        case RTK_PORT_SPEED_2500M:
            link->speed = SWITCH_PORT_SPEED_2500;
            break;
        default:
            link->speed = SWITCH_PORT_SPEED_UNKNOWN;
            break;
    }

    return 0;
}

static int rtl8366ub_sw_set_port_link(struct switch_dev *dev, int port,
                                      struct switch_port_link *link)
{
    rtksw_port_phy_ability_t ability = {0};

    if (link->tx_flow || link->rx_flow)
        ability.AsyFC = 1;
    else
        ability.FC = 1;

    if (link->aneg) {
        ability.Half_10 = 1;
        ability.Half_100 = 1;
        if (link->duplex) {
            ability.Full_10 = 1;
            ability.Full_100 = 1;
            ability.Full_1000 = 1;
            ability.Full_2P5G = 1;
        }

        rtk_port_phyAutoNegoAbility_set(port, &ability);
    } else {
        if (link->duplex) {
            switch (link->speed) {
                case SWITCH_PORT_SPEED_10:
                    ability.Full_10 = 1;
                    break;
                case SWITCH_PORT_SPEED_100:
                    ability.Full_100 = 1;
                    break;
                case SWITCH_PORT_SPEED_1000:
                    ability.Full_1000 = 1;
                    break;
                case SWITCH_PORT_SPEED_2500:
                    ability.Full_2P5G = 1;
                    break;
                default:
                    ability.Full_1000 = 1;
            }
        } else {
            switch (link->speed) {
                case SWITCH_PORT_SPEED_10:
                    ability.Half_10 = 1;
                    break;
                default:
                    ability.Half_100 = 1;
            }
        }

        rtk_port_phyForceModeAbility_set(port, &ability);
    }

    return 0;
}

static int rtl8366ub_port_get_disable(struct switch_dev *dev,
                                      const struct switch_attr *attr,
                                      struct switch_val *val)
{
    int port = val->port_vlan;
    rtksw_port_phy_data_t reg_val;

    if (port < 0 || port > RTL8366UB_NUM_PORTS)
        return -EINVAL;

    rtk_port_get_disable(port, &reg_val);
    val->value.i = (reg_val & BIT(11)) > 0;

    if (val->value.i) {
        val->value.i = 1;
    } else {
        val->value.i = 0;
    }

    return 0;
}

static int rtl8366ub_port_set_disable(struct switch_dev *dev,
                                      const struct switch_attr *attr,
                                      struct switch_val *val)
{
    int port = val->port_vlan;
    rtksw_port_phy_data_t reg_val;

    if (port < 0 || port > RTL8366UB_NUM_PORTS)
        return -EINVAL;

    rtk_port_get_disable(port, &reg_val);
    if (val->value.i) {
        reg_val |= (0x1 << 11);
    } else {
        reg_val &= ~(0x1 << 11);
    }

    rtk_port_set_disable(port, reg_val);

    return 0;
}

static struct switch_attr rtl8366ub_globals[] = {
    {
        .type = SWITCH_TYPE_INT,
        .name = "enable_vlan",
        .description = "Enable VLAN mode",
        .set = rtl8366ub_sw_set_vlan_enable,
        .get = rtl8366ub_sw_get_vlan_enable,
        .max = 1,
    }, {
        .type = SWITCH_TYPE_NOVAL,
        .name = "reset_mibs",
        .description = "Reset all MIB counters",
        .set = rtl8366ub_sw_reset_mibs,
    }
};

static struct switch_attr rtl8366ub_port[] = {
    {
        .type = SWITCH_TYPE_NOVAL,
        .name = "reset_mib",
        .description = "Reset single port MIB counters",
        .set = rtl8366ub_sw_reset_port_mibs,
    }, {
        .type = SWITCH_TYPE_STRING,
        .name = "mib",
        .description = "Get MIB counters for port",
        //.max = 33,
        .set = NULL,
        .get = rtl8366ub_sw_get_port_mib,
    }, {
        .type = SWITCH_TYPE_INT,
        .name = "disable",
        .description = "Set disable",
        .get = rtl8366ub_port_get_disable,
        .set = rtl8366ub_port_set_disable,
    },
};

static struct switch_attr rtl8366ub_vlan[] = {
    {
        .type = SWITCH_TYPE_STRING,
        .name = "info",
        .description = "Get vlan information",
        .max = 1,
        .set = NULL,
        .get = rtl8366ub_sw_get_vlan_info,
    },
};

static const struct switch_dev_ops rtl8366ub_sw_ops = {
    .attr_global = {
        .attr = rtl8366ub_globals,
        .n_attr = ARRAY_SIZE(rtl8366ub_globals),
    },
    .attr_port = {
        .attr = rtl8366ub_port,
        .n_attr = ARRAY_SIZE(rtl8366ub_port),
    },
    .attr_vlan = {
        .attr = rtl8366ub_vlan,
        .n_attr = ARRAY_SIZE(rtl8366ub_vlan),
    },

    .get_vlan_ports = rtl8366ub_sw_get_vlan_ports,
    .set_vlan_ports = rtl8366ub_sw_set_vlan_ports,
    .get_port_pvid = rtl8366ub_sw_get_port_pvid,
    .set_port_pvid = rtl8366ub_sw_set_port_pvid,
    .reset_switch = rtl8366ub_sw_reset_switch,
    .get_port_link = rtl8366ub_sw_get_port_link,
    .set_port_link = rtl8366ub_sw_set_port_link,
};

static int rtl8366ub_swconfig_init(struct gsw_rtl8366ub *gsw)
{
    struct switch_dev *dev = &gsw->swdev;
    int err = 0;

    dev->name = "RTL8366UB";
    dev->cpu_port = EXT_PORT1;
    dev->ports = (EXT_PORT1 + 1);
    dev->vlans = RTL8366UB_NUM_VIDS;
    dev->ops = &rtl8366ub_sw_ops;
    dev->alias = "RTL8366UB";

    err = register_switch(dev, NULL);
    if (err) {
        if (err == -EPROBE_DEFER)
            dev_err(gsw->dev, "unable to register switch, deferred\n");
        else
            dev_err(gsw->dev, "unable to register switch ret = %d\n", err);
        return err;
    }

    pr_info("[%s]\n", __func__);

    return err;
}

static void rtl8366ub_swconfig_destroy(struct gsw_rtl8366ub *gsw)
{
    unregister_switch(&gsw->swdev);
}

static int rtl8366ub_proc_device_init(struct gsw_rtl8366ub *gsw)
{
    if (!proc_rtl8366ub_gsw_dir)
        proc_rtl8366ub_gsw_dir = proc_mkdir(RTL_PROC_DIR, 0);

    proc_create(RTL_PROC_NODE_DEVICE, 0400, proc_rtl8366ub_gsw_dir,
                &rtl8366ub_proc_device_fops);

    return 0;
}

static void rtl8366ub_proc_device_exit(void)
{
    remove_proc_entry(RTL_PROC_NODE_DEVICE, 0);
}

static int rtl8366ub_vlan_config(const char *pm)
{
    rtk_vlan_cfg_t vlan1, vlan2;

    /* Set LAN/WAN VLAN partition */
    memset(&vlan1, 0x00, sizeof(rtk_vlan_cfg_t));
    memset(&vlan2, 0x00, sizeof(rtk_vlan_cfg_t));
    vlan1.ivl_en = 1;
    vlan2.ivl_en = 1;

    RTK_PORTMASK_ALLPORT_SET(0, vlan1.mbr);
    RTK_PORTMASK_ALLPORT_SET(0, vlan1.untag);
    RTK_PORTMASK_PORT_CLEAR(vlan1.untag, EXT_PORT1);
    RTK_PORTMASK_PORT_SET(vlan2.mbr, EXT_PORT1);

    rtk_vlan_portPvid_set(UTP_PORT0, 1, 0);
    rtk_vlan_portPvid_set(UTP_PORT1, 1, 0);
    rtk_vlan_portPvid_set(UTP_PORT2, 1, 0);
    rtk_vlan_portPvid_set(UTP_PORT3, 1, 0);

    if (!strcasecmp(pm, "wlll")) {
        RTK_PORTMASK_PORT_CLEAR(vlan1.mbr, UTP_PORT0);
        RTK_PORTMASK_PORT_CLEAR(vlan1.untag, UTP_PORT0);
        RTK_PORTMASK_PORT_SET(vlan2.mbr, UTP_PORT0);
        RTK_PORTMASK_PORT_SET(vlan2.untag, UTP_PORT0);
        rtk_vlan_portPvid_set(UTP_PORT0, 2, 0);
    } else if (!strcasecmp(pm, "lwll")) {
        RTK_PORTMASK_PORT_CLEAR(vlan1.mbr, UTP_PORT1);
        RTK_PORTMASK_PORT_CLEAR(vlan1.untag, UTP_PORT1);
        RTK_PORTMASK_PORT_SET(vlan2.mbr, UTP_PORT1);
        RTK_PORTMASK_PORT_SET(vlan2.untag, UTP_PORT1);
        rtk_vlan_portPvid_set(UTP_PORT1, 2, 0);
    } else if (!strcasecmp(pm, "llwl")) {
        RTK_PORTMASK_PORT_CLEAR(vlan1.mbr, UTP_PORT2);
        RTK_PORTMASK_PORT_CLEAR(vlan1.untag, UTP_PORT2);
        RTK_PORTMASK_PORT_SET(vlan2.mbr, UTP_PORT2);
        RTK_PORTMASK_PORT_SET(vlan2.untag, UTP_PORT2);
        rtk_vlan_portPvid_set(UTP_PORT2, 2, 0);
    } else if (!strcasecmp(pm, "lllw")) {
        RTK_PORTMASK_PORT_CLEAR(vlan1.mbr, UTP_PORT3);
        RTK_PORTMASK_PORT_CLEAR(vlan1.untag, UTP_PORT3);
        RTK_PORTMASK_PORT_SET(vlan2.mbr, UTP_PORT3);
        RTK_PORTMASK_PORT_SET(vlan2.untag, UTP_PORT3);
        rtk_vlan_portPvid_set(UTP_PORT3, 2, 0);
    } else {
        rtk_vlan_set(1, &vlan1);
        return 0;
    }

    rtk_vlan_set(1, &vlan1);
    rtk_vlan_set(2, &vlan2);

    return 0;
}

static void rtl8366ub_sgmii_config(rtksw_port_t port)
{
    rtksw_port_mac_ability_t mac_cfg;
    rtksw_mode_ext_t mode_ext;
    int ret = 0;

    mode_ext = RTKSW_MODE_EXT_HSGMII;
    mac_cfg.forcemode = PORT_MAC_FORCE;
    mac_cfg.speed = RTKSW_PORT_SPEED_2500M;
    mac_cfg.duplex = RTKSW_PORT_FULL_DUPLEX;
    mac_cfg.link = RTKSW_PORT_LINKUP;
    mac_cfg.nway = RTKSW_DISABLED;
    mac_cfg.txpause = RTKSW_ENABLED;
    mac_cfg.rxpause = RTKSW_ENABLED;
    mac_cfg.speed = RTKSW_PORT_SPEED_2500M;

    ret = rtk_port_macForceLinkExt_set(port, mode_ext, &mac_cfg);
    if (ret == RT_ERR_INPUT || ret == RT_ERR_PORT_ID) {
        rtk_port_macForceLinkExt_set(port, mode_ext, &mac_cfg);
    }
    ret = rtk_port_sgmiiNway_set(port, RTKSW_DISABLED);
    if (ret == RT_ERR_INPUT || ret == RT_ERR_PORT_ID) {
        rtk_port_sgmiiNway_set(port, RTKSW_DISABLED);
    }
}

void rtl8366ub_irq_enable(struct gsw_rtl8366ub *gsw)
{
    struct device *dev = gsw->dev;
    struct device_node *np = dev->of_node;
    struct of_phandle_args oirq;
    rtksw_int_polarity_t int_mode = 0;
    rtksw_int_status_t statusmask;
    int i, irq_type, ret;

    ret = of_irq_parse_one(np, 0, &oirq);
    if (ret) {
        dev_err(dev, "Failed to parse interrupt property: %d\n", ret);
        return;
    }

    irq_type = oirq.args[1];

    switch (irq_type) {
        case IRQ_TYPE_LEVEL_HIGH:
            int_mode = INT_POLAR_HIGH;
            dev_info(gsw->dev, "Setting level-high interrupt mode\n");
            break;
        case IRQ_TYPE_LEVEL_LOW:
            int_mode = INT_POLAR_LOW;
            dev_info(gsw->dev, "Setting level-low interrupt mode\n");
            break;
        default:
            dev_warn(gsw->dev, "Unknown interrupt type 0x%x, using default\n", irq_type);
            int_mode = INT_POLAR_HIGH; // 默认值
            break;
    }

    rtk_int_polarity_set(int_mode);
    rtk_int_control_set(INT_TYPE_LINK_STATUS, RTKSW_ENABLED);
    for (i = INT_TYPE_METER_EXCEED; i < INT_TYPE_END; i++) {
        rtk_int_control_set(i, RTKSW_DISABLED);
    }

    statusmask.value[0] = (0x0001 << INT_TYPE_LINK_STATUS);
    rtk_int_status_set(&statusmask);
}

static irqreturn_t rtl8366ub_irq_handler(int irq, void *dev)
{
    struct gsw_rtl8366ub *gsw = dev;

    disable_irq_nosync(gsw->irq);

    schedule_work(&gsw->irq_worker);

    return IRQ_HANDLED;
}

static void rtl8366ub_wan_status_update(int port_id, rtksw_port_linkStatus_t links)
{
    char ifname[IFNAMSIZ + 12] = {'\0'};
    rtksw_vlan_t pvid;
    rtksw_pri_t Priority;
    rtksw_vlan_cfg_t vlan;
    int i, port_num = 0;
    struct net_device *dev_wan = NULL;

    if (RT_ERR_OK != rtk_vlan_portPvid_get(port_id, &pvid, &Priority)) {
        return;
    }

    if (rtk_vlan_get(pvid, &vlan)) {
        return;
    }

    if (!(RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, port_id) &&
            (RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, EXT_PORT0) || \
             RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, EXT_PORT1)))) {
        return;
    }

    for (i = 0; i < RTL8366UB_NUM_PORTS; i++) {
        if (RTKSW_PORTMASK_IS_PORT_SET(vlan.mbr, i)) {
            port_num++;
        }
    }

    if (1 < port_num) {
        return;
    }

    sprintf(ifname, "eth0.%d", pvid);
    rcu_read_lock();
    if (ifname[0] != '\0') {
        dev_wan = dev_get_by_name_rcu(&init_net, ifname);
        if (dev_wan)
            dev_hold(dev_wan);
    }
    rcu_read_unlock();

    if (links) {
        if (dev_wan) {
            rtnl_lock();
            netif_carrier_on(dev_wan);
            rtnl_unlock();
            dev_put(dev_wan);
        }
    } else {
        if (dev_wan) {
            rtnl_lock();
            if (netif_carrier_ok(dev_wan)) {
                netif_carrier_off(dev_wan);
            }
            rtnl_unlock();
            dev_put(dev_wan);
        }
    }

    return;
}

static void rtl8366ub_irq_worker(struct work_struct *work)
{
    rtksw_int_status_t statusmask;
    rtksw_int_info_t info;
    rtksw_port_linkStatus_t LinkStatus;
    rtksw_port_speed_t speed;
    rtksw_port_duplex_t duplex;

    struct gsw_rtl8366ub *gsw;
    int port_id;
    char *speeds[] = {"10Mbps", "100Mbps", "1Gbps", "500Mbps", "2.5Gbps", "5Gbps", "10Gbps"};

    gsw = container_of(work, struct gsw_rtl8366ub, irq_worker);
    if (!gsw) {
        pr_err("gsw is NULL in irq_worker\n");
        return;
    }

    rtk_int_status_get(&statusmask);
    if (statusmask.value[0] & (0x0001 << INT_TYPE_LINK_STATUS)) {
        /* Clear status*/
        statusmask.value[0] = (0x0001 << INT_TYPE_LINK_STATUS);
        rtk_int_status_set(&statusmask);

        /* Get advanced information */
        memset(&info, 0, sizeof(rtksw_int_info_t));
        rtk_int_advanceInfo_get(ADV_PORT_LINKUP_PORT_MASK, &info);

        if (0 != info.portMask.bits[0]) {
            for (port_id = 0; port_id < 4; port_id++) {
                if (RTKSW_PORTMASK_IS_PORT_SET(info.portMask, port_id)) {
                    rtk_port_phyStatus_get(port_id, &LinkStatus, &speed, &duplex);
                    dev_info(gsw->dev, "Port %d Link is Up - %s/%s\n", port_id, speeds[speed], duplex ? "Full" : "Half");
                    rtl8366ub_wan_status_update(port_id, LinkStatus);
                }
            }
        } else {
            memset(&info, 0, sizeof(rtksw_int_info_t));
            rtk_int_advanceInfo_get(ADV_PORT_LINKDOWN_PORT_MASK, &info);
            for (port_id = 0; port_id < 4; port_id++) {
                if (RTKSW_PORTMASK_IS_PORT_SET(info.portMask, port_id)) {
                    dev_info(gsw->dev, "Port %d Link is Down\n", port_id);
                    rtl8366ub_wan_status_update(port_id, 0);
                }
            }
        }
    }

    enable_irq(gsw->irq);
}

static int rtl8366ub_probe(struct platform_device *pdev)
{
    struct gsw_rtl8366ub *gsw;
    struct device_node *np = pdev->dev.of_node;
    struct device_node *mdio;
    struct mii_bus *mdio_bus;
    int ret = -EINVAL;
    const char *pm;
    u32 val;

    mdio = of_parse_phandle(np, "realtek,mdio", 0);
    if (!mdio)
        return -EINVAL;

    mdio_bus = of_mdio_find_bus(mdio);
    if (!mdio_bus)
        return -EPROBE_DEFER;

    gsw = devm_kzalloc(&pdev->dev, sizeof(struct gsw_rtl8366ub), GFP_KERNEL);
    if (!gsw)
        return -ENOMEM;

    gsw->host_bus = mdio_bus;
    gsw->dev = &pdev->dev;
    mutex_init(&gsw->mii_lock);

    dev_info(gsw->dev, "RTL8366UB Driver Version=%s\n",
             DRIVER_VERSION);

    /* Switch hard reset */
    if (rtl8366ub_hw_reset(gsw)) {
        dev_info(&pdev->dev, "reset switch fail.\n");
        goto fail;
    }

    rtk_set_mdc_mdio(gsw->host_bus, 0);

    /* Detect device */
    ret = reg_mdcmdio_read(0x4, &val);
    if (ret) {
        dev_err(gsw->dev, "can't get chip ID (%d)\n", ret);
        goto fail;
    }

    switch (val) {
        case 0x8366:
            dev_info(gsw->dev, "found an RTL8366UB switch\n");
            break;
        default:
            dev_err(gsw->dev, "found an Unknown Realtek switch (id=0x%04x)\n",
                    val);
            goto fail;
    }

    rtk_switch_init();
    rtk_vlan_init();
    rtl8366ub_sgmii_config(EXT_PORT1);

    //init default vlan or init swocnfig
    if (!of_property_read_string(pdev->dev.of_node,
                                 "realtek,portmap", &pm)) {
        rtl8366ub_vlan_config(pm);
    }

    gsw->irq = platform_get_irq(pdev, 0);
    if (gsw->irq >= 0) {
        INIT_WORK(&gsw->irq_worker, rtl8366ub_irq_worker);

        ret = devm_request_irq(gsw->dev, gsw->irq, rtl8366ub_irq_handler,
                               0, dev_name(gsw->dev), gsw);
        if (ret) {
            dev_err(gsw->dev, "Failed to request irq %d\n",
                    gsw->irq);
            goto fail;
        }
    }

    platform_set_drvdata(pdev, gsw);

    rtl8366ub_proc_device_init(gsw);

    rtl8366ub_swconfig_init(gsw);

    if (gsw->irq >= 0) {
        rtl8366ub_irq_enable(gsw);
    }

    return 0;

fail:
    devm_kfree(&pdev->dev, gsw);

    return ret;
}

static void rtl8366ub_remove(struct platform_device *pdev)
{
    struct gsw_rtl8366ub *gsw = platform_get_drvdata(pdev);

#if IS_ENABLED(CONFIG_DEBUG_FS)
    debugfs_remove_recursive(gsw->debugfs_dir);
#endif

    rtl8366ub_swconfig_destroy(gsw);

    rtl8366ub_proc_device_exit();

    platform_set_drvdata(pdev, NULL);
}

static const struct of_device_id rtl8366ub_match[] = {
    {.compatible = "realtek,rtl8366ub"},
    {},
};
MODULE_DEVICE_TABLE(of, rtl8366ub_match);

static struct platform_driver rtl8366ub_driver = {
    .probe = rtl8366ub_probe,
    .remove = rtl8366ub_remove,
    .driver = {
        .name = "rtl8366ub",
        .of_match_table = rtl8366ub_match,
    },
};

static int __init rtl8366ub_mdio_module_init(void)
{
	return platform_driver_register(&rtl8366ub_driver);
}
module_init(rtl8366ub_mdio_module_init);

static void __exit rtl8366ub_mdio_module_exit(void)
{
	platform_driver_unregister(&rtl8366ub_driver);
}

module_exit(rtl8366ub_mdio_module_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Driver for realtek RTL8366UB 2.5 Gigabit Switch");
