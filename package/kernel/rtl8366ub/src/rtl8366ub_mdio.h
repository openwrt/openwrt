#ifndef __RTL8366UB_MDIO_H__
#define __RTL8366UB_MDIO_H__

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/of_mdio.h>
#include <linux/workqueue.h>
#include <linux/gpio/consumer.h>
#include <linux/phy.h>

#ifdef CONFIG_SWCONFIG
#include <linux/switch.h>
#endif

/* RTL8366UB driver version */
#ifndef DRIVER_VERSION
#define DRIVER_VERSION "unknown"
#endif

struct rtl8366ub_wan_info {
    char wan_name[12];
    char secondwan_name[12];
    int wan_portid;
    int secondwan_portid;
};

struct gsw_rtl8366ub {
    u32 id;
    int reset_pin;
    u32 cpu_port;
    int global_vlan_enable;
    struct device *dev;

    struct mii_bus *host_bus;
    struct mutex mii_lock;    /* MII access lock */
    struct switch_dev swdev;
    struct list_head list;
#if IS_ENABLED(CONFIG_DEBUG_FS)
    struct dentry *debugfs_dir;
#endif

    int irq;
    struct work_struct irq_worker;
    struct rtl8366ub_wan_info wan_info;
};

void rtl8366ub_irq_enable(struct gsw_rtl8366ub *gsw);
#endif
