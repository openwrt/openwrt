#ifndef __RTL8366_DSA_H__
#define __RTL8366_DSA_H__

#define MDC_MDIO_CTRL_REG           21
#define MDC_MDIO_ADDR_REG           22
#define MDC_MDIO_DATA_LOW           23
#define MDC_MDIO_DATA_HIGH          24
#define MDC_MDIO_READ_CMD           0x1B
#define MDC_MDIO_WRITE_CMD          0x19

#define RTL8366UB_NUM_PORTS          4
#ifndef DRIVER_VERSION
#define DRIVER_VERSION "unknown"
#endif

#define PORTMATRIX_MATRIX(x)	((x) & 0x0f)

struct rtl8366ub_port {
    bool enable;
    u32 pm;
    u16 pvid;
};

struct rtl8366ub_priv {
    struct device *dev;
    struct regmap *map;
    struct mii_bus *bus;
    int mdio_addr;
    int cpu_port;
    struct dsa_switch *ds;
    struct gpio_desc *reset;
    struct dentry *dbgfs;
    unsigned int id;
    void __iomem *base;

    struct mutex reg_mutex;

    struct rtl8366ub_port ports[RTL8366UB_NUM_PORTS];
};

enum rtl8366ub_id {
    ID_RTL8366UB = 0,
};

struct rtl8366ub_dev_info {
    enum rtl8366ub_id id;

    int (*sw_setup)(struct dsa_switch *ds);
    int (*phy_read)(struct dsa_switch *ds, int port, int regnum);
    int (*phy_write)(struct dsa_switch *ds, int port, int regnum,
                     u16 val);
    int (*pad_setup)(struct dsa_switch *ds, phy_interface_t interface);
    int (*cpu_port_config)(struct dsa_switch *ds, int port);
    bool (*phy_mode_supported)(struct dsa_switch *ds, int port,
                               const struct phylink_link_state *state);
    void (*mac_port_validate)(struct dsa_switch *ds, int port,
                              unsigned long *supported);
    int (*mac_port_get_state)(struct dsa_switch *ds, int port,
                              struct phylink_link_state *state);
    int (*mac_port_config)(struct dsa_switch *ds, int port,
                           unsigned int mode, phy_interface_t interface);
    void (*mac_pcs_an_restart)(struct dsa_switch *ds, int port);
};

#endif