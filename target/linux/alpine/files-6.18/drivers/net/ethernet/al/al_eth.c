/*
 * al_eth.c: AnnapurnaLabs Unified 1GbE and 10GbE ethernet driver.
 *
 * Copyright (C) 2014 Annapurna Labs Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/cache.h>
#include <linux/cpu_rmap.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/errno.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/i2c.h>
#include <linux/if.h>
#include <linux/if_vlan.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/mdio.h>
#include <linux/mii.h>
#include <linux/netdevice.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/prefetch.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/stringify.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <net/checksum.h>
#include <net/ip.h>
#include <net/tcp.h>

#include "al_eth.h"
#include "al_eth_sysfs.h"
#include "al_hal_eth.h"
#include "al_init_eth_lm.h"
#include <mach/al_hal_udma_config.h>
#include <mach/al_hal_udma_debug.h>
#include <mach/al_hal_udma_iofic.h>
#include <mach/alpine_machine.h>

#define DRV_MODULE_NAME "al_eth"
#define DRV_MODULE_VERSION "0.2"
#define DRV_MODULE_RELDATE "Feb 18, 2013"

static char version[] =
    "AnnapurnaLabs unified 1GbE and 10GbE Ethernet Driver " DRV_MODULE_NAME
    " v" DRV_MODULE_VERSION " (" DRV_MODULE_RELDATE ")\n";

MODULE_AUTHOR("Saeed Bishara <saeed@annapurnaLabs.com>");
MODULE_DESCRIPTION("AnnapurnaLabs unified 1GbE and 10GbE Ethernet driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_MODULE_VERSION);

/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT (5 * HZ)

/* Time in mSec to keep trying to read / write from MDIO in case of error */
#define MDIO_TIMEOUT_MSEC 100

static int disable_msi;

module_param(disable_msi, int, 0);
MODULE_PARM_DESC(disable_msi, "Disable Message Signaled Interrupt (MSI)");

#define DEFAULT_MSG_ENABLE (NETIF_MSG_DRV | NETIF_MSG_PROBE | NETIF_MSG_LINK)
static int debug = -1;
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level (0=none,...,16=all)");

#define AL_ETH_BOARD_PARAMS_REG1_OFFSET 0x4
#define AL_ETH_BOARD_PARAMS_REG2_OFFSET 0x404

/* board params register 1 */
#define AL_ETH_BP_MEDIA_TYPE_MASK 0x0000000f
#define AL_ETH_BP_MEDIA_TYPE_SHIFT 0
#define AL_ETH_BP_EXT_PHY_BIT BIT(4)
#define AL_ETH_BP_PHY_ADDR_MASK 0x000003e0
#define AL_ETH_BP_PHY_ADDR_SHIFT 5
#define AL_ETH_BP_SFP_EXIST_BIT BIT(10)
#define AL_ETH_BP_AN_ENABLE_BIT BIT(11)
#define AL_ETH_BP_KR_LT_ENABLE_BIT BIT(12)
#define AL_ETH_BP_KR_FEC_ENABLE_BIT BIT(13)
#define AL_ETH_BP_MDIO_FREQ_MASK 0x0000c000
#define AL_ETH_BP_MDIO_FREQ_SHIFT 14
#define AL_ETH_BP_I2C_ADAPTER_ID_MASK 0x000f0000
#define AL_ETH_BP_I2C_ADAPTER_ID_SHIFT 16
#define AL_ETH_BP_EXT_PHY_IF_MASK 0x00300000
#define AL_ETH_BP_EXT_PHY_IF_SHIFT 20
#define AL_ETH_BP_AN_MODE_BIT BIT(22)
#define AL_ETH_BP_SERDES_GRP_MASK 0x06000000
#define AL_ETH_BP_SERDES_GRP_SHIFT 25
#define AL_ETH_BP_SERDES_LANE_MASK 0x18000000
#define AL_ETH_BP_SERDES_LANE_SHIFT 27
#define AL_ETH_BP_REF_CLK_FREQ_MASK 0xe0000000
#define AL_ETH_BP_REF_CLK_FREQ_SHIFT 29

/* board params register 2 */
#define AL_ETH_BP_DONT_OVERRIDE_SERDES_BIT BIT(0)
#define AL_ETH_BP_FORCE_1000_BASE_X_BIT BIT(1)
#define AL_ETH_BP_1G_AN_DISABLE_BIT BIT(2)
#define AL_ETH_BP_1G_SPEED_MASK 0x00000018
#define AL_ETH_BP_1G_SPEED_SHIFT 3
#define AL_ETH_BP_1G_HALF_DUPLEX_BIT BIT(5)
#define AL_ETH_BP_1G_FC_DISABLE_BIT BIT(6)
#define AL_ETH_BP_RETIMER_EXIST_BIT BIT(7)
#define AL_ETH_BP_RETIMER_BUS_ID_MASK 0x00000f00
#define AL_ETH_BP_RETIMER_BUS_ID_SHIFT 8
#define AL_ETH_BP_RETIMER_I2C_ADDR_MASK 0x0007f000
#define AL_ETH_BP_RETIMER_I2C_ADDR_SHIFT 12
#define AL_ETH_BP_RETIMER_CHANNEL_BIT BIT(19)
#define AL_ETH_BP_DAC_LENGTH_MASK 0x00f00000
#define AL_ETH_BP_DAC_LENGTH_SHIFT 20
#define AL_ETH_BP_DAC_BIT BIT(24)

#define AL_ETH_BP_FIELD(_reg, _mask, _shift) (((_reg) & (_mask)) >> (_shift))

static int al_eth_board_params_from_dt(struct al_eth_adapter *adapter,
                                       struct al_eth_board_params *params) {
  struct device_node *np = adapter->pdev->dev.of_node;
  u32 val;

  if (!np)
    return -ENODEV;

  memset(params, 0, sizeof(*params));

  if (of_property_read_u32(np, "al,media-type", &val))
    return -EINVAL;
  params->media_type = val;

  if (!of_property_read_u32(np, "al,serdes-grp", &val))
    params->serdes_grp = val;

  if (!of_property_read_u32(np, "al,serdes-lane", &val))
    params->serdes_lane = val;

  if (!of_property_read_u32(np, "al,mdio-freq", &val))
    params->mdio_freq = val;
  else
    params->mdio_freq = AL_ETH_BOARD_MDIO_FREQ_1_MHZ;

  params->an_disable = of_property_read_bool(np, "al,an-disable") ? AL_TRUE : AL_FALSE;
  params->dont_override_serdes =
      of_property_read_bool(np, "al,dont-override-serdes") ? AL_TRUE : AL_FALSE;

  /* Fixed-link SerDes ports (no external copper PHY) */
  params->phy_exist = AL_FALSE;
  params->phy_mdio_addr = 0;
  params->phy_if = AL_ETH_BOARD_PHY_IF_MDIO;
  params->autoneg_enable = AL_FALSE;
  params->an_mode = AL_ETH_BOARD_AUTONEG_OUT_OF_BAND;
  params->kr_lt_enable = AL_FALSE;
  params->kr_fec_enable = AL_FALSE;
  params->sfp_plus_module_exist = AL_FALSE;
  params->ref_clk_freq = AL_ETH_REF_FREQ_375_MHZ;
  params->speed = AL_ETH_BOARD_1G_SPEED_1000M;
  params->half_duplex = AL_FALSE;
  params->fc_disable = AL_FALSE;
  params->retimer_exist = AL_FALSE;
  params->retimer_bus_id = 0;
  params->retimer_i2c_addr = 0;
  params->retimer_channel = AL_ETH_RETIMER_CHANNEL_A;
  params->dac = AL_FALSE;
  params->dac_len = 0;

  return 0;
}

/* indexed by board_t */
static struct {
  char *name;
  unsigned int bar; /* needed for FPGA/NIC modes */
} board_info[] = {
    {.name = "AnnapurnaLabs unified 1Gbe/10Gbe"},
    {
        .name = "AnnapurnaLabs unified 1Gbe/10Gbe pcie NIC",
        .bar = 5,
    },
    {
        .name = "AnnapurnaLabs unified 1Gbe/10Gbe pcie FPGA NIC",
        .bar = 0,
    },
};

static const struct pci_device_id al_eth_pci_tbl[] = {
    {PCI_VENDOR_ID_AMAZON_ANNAPURNA_LABS,
     PCI_DEVICE_ID_AMAZON_ANNAPURNA_LABS_ETH, PCI_ANY_ID, PCI_ANY_ID, 0, 0,
     ALPINE_INTEGRATED},
    {PCI_VENDOR_ID_AMAZON_ANNAPURNA_LABS,
     PCI_DEVICE_ID_AMAZON_ANNAPURNA_LABS_ETH_ADVANCED, PCI_ANY_ID, PCI_ANY_ID,
     0, 0, ALPINE_INTEGRATED},
    {PCI_VENDOR_ID_AMAZON_ANNAPURNA_LABS,
     PCI_DEVICE_ID_AMAZON_ANNAPURNA_LABS_ETH_NIC, PCI_ANY_ID, PCI_ANY_ID, 0, 0,
     ALPINE_NIC},
    {PCI_VENDOR_ID_AMAZON_ANNAPURNA_LABS,
     PCI_DEVICE_ID_AMAZON_ANNAPURNA_LABS_ETH_FPGA_NIC, PCI_ANY_ID, PCI_ANY_ID,
     0, 0, ALPINE_FPGA_NIC},
    {
        0,
    }};

MODULE_DEVICE_TABLE(pci, al_eth_pci_tbl);

#ifdef CONFIG_AL_ETH_ALLOC_SKB
static DEFINE_PER_CPU(struct sk_buff_head, rx_recycle_cache);
#endif

#ifndef MII_ADDR_C45
#define MII_ADDR_C45 (1 << 30)
#endif

struct al_udma *al_eth_udma_get(struct al_eth_adapter *adapter, int tx) {
  if (tx)
    return &adapter->hal_adapter.tx_udma;
  return &adapter->hal_adapter.rx_udma;
}

/* MDIO */
#define AL_ETH_MDIO_DEV_ID 1
#define AL_ETH_MDIO_C45_DEV_MASK 0x1f0000
#define AL_ETH_MDIO_C45_DEV_SHIFT 16
#define AL_ETH_MDIO_C45_REG_MASK 0xffff

static int al_mdio_read(struct mii_bus *bp, int mii_id, int reg) {
  struct al_eth_adapter *adapter = bp->priv;
  u16 value = 0;
  int rc;
  int timeout = MDIO_TIMEOUT_MSEC;

  adapter->phy_addr = mii_id;

  while (timeout > 0) {
    if (reg & MII_ADDR_C45) {
      al_dbg("%s [c45]: dev %x reg %x val %x\n", __func__,
             ((reg & AL_ETH_MDIO_C45_DEV_MASK) >> AL_ETH_MDIO_C45_DEV_SHIFT),
             (reg & AL_ETH_MDIO_C45_REG_MASK), value);
      rc = al_eth_mdio_read(
          &adapter->hal_adapter, adapter->phy_addr,
          ((reg & AL_ETH_MDIO_C45_DEV_MASK) >> AL_ETH_MDIO_C45_DEV_SHIFT),
          (reg & AL_ETH_MDIO_C45_REG_MASK), &value);
    } else if ((adapter->phy_if == AL_ETH_BOARD_PHY_IF_XMDIO) &&
               (mii_id && 0x8000)) {
      /* There is only 16bit variable from user-space,
          so we used bit 32 of PHY ID to mark the C45 access */
      al_dbg("%s [c45]: dev %x reg %x val %x\n", __func__, (mii_id & 0x1f), reg,
             value);
      rc = al_eth_mdio_read(&adapter->hal_adapter, adapter->phy_addr,
                            (mii_id & 0x1f), reg, &value);
    } else {
      rc = al_eth_mdio_read(&adapter->hal_adapter, adapter->phy_addr,
                            MDIO_DEVAD_NONE, reg, &value);
    }

    if (rc == 0)
      return value;

    netdev_dbg(adapter->netdev, "mdio read failed. try again in 10 msec\n");

    timeout -= 10;
    msleep(10);
  }

  if (rc)
    netdev_err(adapter->netdev, "MDIO read failed on timeout\n");

  return value;
}

static int al_mdio_write(struct mii_bus *bp, int mii_id, int reg, u16 val) {
  struct al_eth_adapter *adapter = bp->priv;
  int rc;
  int timeout = MDIO_TIMEOUT_MSEC;

  adapter->phy_addr = mii_id;

  while (timeout > 0) {
    if (reg & MII_ADDR_C45) {
      al_dbg("%s [c45]: device %x reg %x val %x\n", __func__,
             ((reg & AL_ETH_MDIO_C45_DEV_MASK) >> AL_ETH_MDIO_C45_DEV_SHIFT),
             (reg & AL_ETH_MDIO_C45_REG_MASK), val);
      rc = al_eth_mdio_write(
          &adapter->hal_adapter, adapter->phy_addr,
          ((reg & AL_ETH_MDIO_C45_DEV_MASK) >> AL_ETH_MDIO_C45_DEV_SHIFT),
          (reg & AL_ETH_MDIO_C45_REG_MASK), val);
    } else if ((adapter->phy_if == AL_ETH_BOARD_PHY_IF_XMDIO) &&
               (mii_id && 0x8000)) {
      /* There is only 16bit variable from user-space,
          so we used bit 32 of PHY ID to mark the C45 access */
      al_dbg("%s [c45]: dev %x reg %x val %x\n", __func__, (mii_id & 0x1f), reg,
             val);
      rc = al_eth_mdio_write(&adapter->hal_adapter, adapter->phy_addr,
                             (mii_id & 0x1f), reg, val);
    } else {
      rc = al_eth_mdio_write(&adapter->hal_adapter, adapter->phy_addr,
                             MDIO_DEVAD_NONE, reg, val);
    }

    if (rc == 0)
      return 0;

    netdev_err(adapter->netdev, "mdio write failed. try again in 10 msec\n");

    timeout -= 10;
    msleep(10);
  }

  if (rc)
    netdev_err(adapter->netdev, "MDIO write failed on timeout\n");

  return rc;
}

static int al_eth_sw_mdio_probe(struct al_eth_adapter *adapter) {
  struct device *dev = &adapter->pdev->dev;
  struct device_node *np;
  struct mii_bus *bus;
  int ret;

  np = of_get_child_by_name(dev->of_node, "mdio");
  if (!np) {
    dev_info(dev, "MDIO child node missing\n");
    return 0;
  }

  bus = mdiobus_alloc();
  if (bus == NULL) {
    ret = -ENOMEM;
    goto fail_free_node;
  }

  bus->name = "mdio-al-eth";
  snprintf(bus->id, MII_BUS_ID_SIZE, "%s-0", bus->name);
  bus->priv = adapter;
  bus->parent = dev;
  bus->read = &al_mdio_read;
  bus->write = &al_mdio_write;

  ret = of_mdiobus_register(bus, np);
  if (ret) {
    dev_err(dev, "cannot register MDIO bus\n");
    goto fail_free_bus;
  }

  of_node_put(np);

  adapter->sw_mdio_bus = bus;

  return 0;

fail_free_bus:
  mdiobus_free(bus);

fail_free_node:
  of_node_put(np);

  return ret;
}

static void al_eth_sw_mdio_remove(struct al_eth_adapter *adapter) {
  if (adapter->sw_mdio_bus == NULL)
    return;

  mdiobus_unregister(adapter->sw_mdio_bus);
  mdiobus_free(adapter->sw_mdio_bus);
  adapter->sw_mdio_bus = NULL;
}

static int al_eth_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct mii_ioctl_data *mdio = if_mii(ifr);
  struct phy_device *phydev;

  al_dbg("ioctl: phy id 0x%x, reg 0x%x, val_in 0x%x\n", mdio->phy_id,
         mdio->reg_num, mdio->val_in);

  if (adapter->mdio_bus) {
    phydev = mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);
    if (phydev)
      return phy_mii_ioctl(phydev, ifr, cmd);
  }

  return -EOPNOTSUPP;
}
static int al_eth_flow_ctrl_config(struct al_eth_adapter *adapter);
static uint8_t al_eth_flow_ctrl_mutual_cap_get(struct al_eth_adapter *adapter);
static void al_eth_down(struct al_eth_adapter *adapter);
static int al_eth_up(struct al_eth_adapter *adapter);
static void al_eth_serdes_mode_set(struct al_eth_adapter *adapter);

static void al_eth_adjust_link(struct net_device *dev) {
  struct al_eth_adapter *adapter = netdev_priv(dev);
  struct al_eth_link_config *link_config = &adapter->link_config;
  struct phy_device *phydev = adapter->phydev;
  enum al_eth_mac_mode mac_mode_needed = AL_ETH_MAC_MODE_RGMII;
  int new_state = 0;
  int force_1000_base_x = false;

  if (phydev->link) {
    if (phydev->duplex != link_config->active_duplex) {
      new_state = 1;
      link_config->active_duplex = phydev->duplex;
    }

    if (phydev->speed != link_config->active_speed) {
      new_state = 1;
      switch (phydev->speed) {
      case SPEED_1000:
      case SPEED_100:
      case SPEED_10:
        mac_mode_needed =
            (adapter->mac_mode == AL_ETH_MAC_MODE_RGMII)
                ? AL_ETH_MAC_MODE_RGMII
                : AL_ETH_MAC_MODE_10GbE_Serial; // AL_ETH_MAC_MODE_SGMII;
        break;
      case SPEED_10000:
      case SPEED_5000:
      case SPEED_2500:
        mac_mode_needed = AL_ETH_MAC_MODE_10GbE_Serial;
        break;
      default:
        if (netif_msg_link(adapter))
          netdev_warn(adapter->netdev, "Ack!  Speed (%d) is not 10/100/1000!",
                      phydev->speed);
        break;
      }
      link_config->active_speed = phydev->speed;
    }

    if (!link_config->old_link) {
      new_state = 1;
      link_config->old_link = 1;
    }

    if (new_state) {
      int rc;

      if (adapter->mac_mode != mac_mode_needed) {
        al_eth_down(adapter);
        adapter->mac_mode = mac_mode_needed;
        if (link_config->active_speed > 1000) {
          al_eth_serdes_mode_set(adapter);
        } else {
          al_eth_serdes_mode_set(adapter);
          force_1000_base_x = true;
        }
        al_eth_up(adapter);
      }

      if (adapter->mac_mode != AL_ETH_MAC_MODE_10GbE_Serial) {
        /* change the MAC link configuration */
        rc = al_eth_mac_link_config(
            &adapter->hal_adapter, force_1000_base_x, link_config->autoneg,
            link_config->active_speed,
            link_config->active_duplex ? AL_TRUE : AL_FALSE);
        if (rc) {
          netdev_warn(adapter->netdev,
                      "Failed to config the mac with the new link settings!");
        }
      }
    }

    if (link_config->flow_ctrl_supported & AL_ETH_FLOW_CTRL_AUTONEG) {
      uint8_t new_flow_ctrl = al_eth_flow_ctrl_mutual_cap_get(adapter);

      if (new_flow_ctrl != link_config->flow_ctrl_active) {
        link_config->flow_ctrl_active = new_flow_ctrl;
        al_eth_flow_ctrl_config(adapter);
      }
    }
  } else if (adapter->link_config.old_link) {
    new_state = 1;
    link_config->old_link = 0;
    link_config->active_duplex = DUPLEX_UNKNOWN;
    link_config->active_speed = SPEED_UNKNOWN;
  }

  if (new_state && netif_msg_link(adapter))
    phy_print_status(phydev);
}

static int al_eth_phy_init(struct al_eth_adapter *adapter) {
  struct phy_device *phydev = phydev =
      mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);
  u32 features, supported, advertising;

  adapter->link_config.old_link = 0;
  adapter->link_config.active_duplex = DUPLEX_UNKNOWN;
  adapter->link_config.active_speed = SPEED_UNKNOWN;

  /* Attach the MAC to the PHY. */
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 9, 0)
  phydev = phy_connect(adapter->netdev, phydev_name(phydev), al_eth_adjust_link,
                       PHY_INTERFACE_MODE_RGMII);
#else
  phydev = phy_connect(adapter->netdev, dev_name(&phydev->dev),
                       al_eth_adjust_link, 0, PHY_INTERFACE_MODE_RGMII);
#endif
  if (IS_ERR(phydev)) {
    netdev_err(adapter->netdev, "Could not attach to PHY\n");
    return PTR_ERR(phydev);
  }

  phy_attached_info(phydev);

  ethtool_convert_link_mode_to_legacy_u32(&features, PHY_GBIT_FEATURES);
  ethtool_convert_link_mode_to_legacy_u32(&supported, phydev->supported);
  ethtool_convert_link_mode_to_legacy_u32(&advertising, phydev->advertising);

  /* Mask with MAC supported features. */
  supported &= (features | SUPPORTED_Pause | SUPPORTED_Asym_Pause);

  advertising = supported;

  ethtool_convert_legacy_u32_to_link_mode(phydev->supported, supported);
  ethtool_convert_legacy_u32_to_link_mode(phydev->advertising, advertising);

  adapter->phydev = phydev;
  /* Bring the PHY up */
  phy_start(adapter->phydev);

  return 0;
}

/**
 * al_eth_mdiobus_setup - initialize mdiobus and register to kernel
 *
 *
 **/
static int al_eth_mdiobus_setup(struct al_eth_adapter *adapter) {
  struct phy_device *phydev;
  int i;
  int ret = 0;

  adapter->mdio_bus = mdiobus_alloc();
  if (adapter->mdio_bus == NULL)
    return -ENOMEM;

  adapter->mdio_bus->name = "al mdio bus";
  snprintf(adapter->mdio_bus->id, MII_BUS_ID_SIZE, "%x",
           (adapter->pdev->bus->number << 8) | adapter->pdev->devfn);
  adapter->mdio_bus->priv = adapter;
  adapter->mdio_bus->parent = &adapter->pdev->dev;
  adapter->mdio_bus->read = &al_mdio_read;
  adapter->mdio_bus->write = &al_mdio_write;
  adapter->mdio_bus->phy_mask = ~(1 << adapter->phy_addr);

  for (i = 0; i < PHY_MAX_ADDR; i++)
    adapter->mdio_bus->irq[i] = PHY_POLL;

  printk("phy_if=%d\n", adapter->phy_if);

  if (adapter->phy_if != AL_ETH_BOARD_PHY_IF_XMDIO) {

    i = mdiobus_register(adapter->mdio_bus);
    if (i) {
      netdev_warn(adapter->netdev, "mdiobus_reg failed (0x%x)\n", i);
      mdiobus_free(adapter->mdio_bus);
      return i;
    }
    phydev = mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);
  } else {
    adapter->mdio_bus->phy_mask = 0xffffffff;
    i = mdiobus_register(adapter->mdio_bus);
    if (i) {
      netdev_warn(adapter->netdev, "mdiobus_reg failed (0x%x)\n", i);
      mdiobus_free(adapter->mdio_bus);
      return i;
    }

    phydev = get_phy_device(adapter->mdio_bus, adapter->phy_addr, true);
    if (!phydev) {
      pr_err("phy device get failed\n");
      goto error;
    }

    ret = phy_device_register(phydev);
    if (ret) {
      pr_err("phy device register failed\n");
      goto error;
    }
  }

  if (!phydev || !phydev->drv) {
    if (phydev)
      return 0;
    printk("phydev null!\n");
    goto error;
  }

  return 0;

error:
  netdev_warn(adapter->netdev, "No PHY devices\n");
  mdiobus_unregister(adapter->mdio_bus);
  mdiobus_free(adapter->mdio_bus);
  return -ENODEV;
}

/**
 * al_eth_mdiobus_teardown - mdiobus unregister
 *
 *
 **/
static void al_eth_mdiobus_teardown(struct al_eth_adapter *adapter) {
  if (!adapter->mdio_bus)
    return;

  mdiobus_unregister(adapter->mdio_bus);
  mdiobus_free(adapter->mdio_bus);
  phy_device_free(adapter->phydev);
}

static void al_eth_tx_timeout(struct net_device *dev, unsigned int txqueue) {
  struct al_eth_adapter *adapter = netdev_priv(dev);

  if (netif_msg_tx_err(adapter))
    netdev_err(dev, "transmit timed out!!!!\n");
}

static int al_eth_change_mtu(struct net_device *dev, int new_mtu) {
  struct al_eth_adapter *adapter = netdev_priv(dev);
  int max_frame = new_mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN;

  if ((new_mtu < AL_ETH_MIN_FRAME_LEN) || (new_mtu > AL_ETH_MAX_MTU) ||
      (max_frame > AL_ETH_MAX_FRAME_LEN)) {
    netdev_err(dev, "Invalid MTU setting\n");
    return -EINVAL;
  }

  netdev_dbg(adapter->netdev, "set MTU to %d\n", new_mtu);
  al_eth_rx_pkt_limit_config(&adapter->hal_adapter, AL_ETH_MIN_FRAME_LEN,
                             max_frame);

  dev->mtu = new_mtu;

  al_eth_tso_mss_config(&adapter->hal_adapter, 0, dev->mtu - 100);

  return 0;
}

static int al_init_rx_cpu_rmap(struct al_eth_adapter *adapter);

static int al_eth_fpga_read_pci_config(void *handle, int where, uint32_t *val) {
  /* handle is the base address of the adapter */
  *val = readl(handle + where);
  return 0;
}

static int al_eth_fpga_write_pci_config(void *handle, int where, uint32_t val) {
  /* handle is the base address of the adapter */
  // al_eth_hal_reg_write32(handle + where, val);
  writel(val, handle + where);
  return 0;
}

static int al_eth_read_pci_config(void *handle, int where, uint32_t *val) {
  /* handle is a pointer to the pci_dev */
  pci_read_config_dword((struct pci_dev *)handle, where, val);
  return 0;
}

static int al_eth_write_pci_config(void *handle, int where, uint32_t val) {
  /* handle is a pointer to the pci_dev */
  pci_write_config_dword((struct pci_dev *)handle, where, val);
  return 0;
}

static int al_eth_function_reset(struct al_eth_adapter *adapter) {
  struct al_eth_board_params params;
  int rc;

  /* save board params so we restore it after reset */
  al_eth_board_params_get(adapter->mac_base, &params);
  al_eth_mac_addr_read(adapter->ec_base, 0, adapter->mac_addr);
  if (adapter->board_type == ALPINE_INTEGRATED)
    rc = al_eth_flr_rmn(&al_eth_read_pci_config, &al_eth_write_pci_config,
                        adapter->pdev, adapter->mac_base);
  else
    rc = al_eth_flr_rmn(&al_eth_fpga_read_pci_config,
                        &al_eth_fpga_write_pci_config,
                        adapter->internal_pcie_base, adapter->mac_base);

  /* restore params */
  al_eth_board_params_set(adapter->mac_base, &params);
  al_eth_mac_addr_store(adapter->ec_base, 0, adapter->mac_addr);
  return rc;
}

static int al_eth_setup_int_mode(struct al_eth_adapter *adapter, int dis_msi);
static int al_eth_board_params_init(struct al_eth_adapter *adapter) {
  if (adapter->board_type == ALPINE_NIC) {
    adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;
    adapter->sfp_detection_needed = false;
    adapter->phy_exist = false;
    adapter->an_en = false;
    adapter->lt_en = false;
    adapter->ref_clk_freq = AL_ETH_REF_FREQ_375_MHZ;
    adapter->mdio_freq = AL_ETH_DEFAULT_MDIO_FREQ_KHZ;
  } else if (adapter->board_type == ALPINE_FPGA_NIC) {
    adapter->mac_mode = AL_ETH_MAC_MODE_SGMII;
    adapter->sfp_detection_needed = false;
    adapter->phy_exist = false;
    adapter->an_en = false;
    adapter->lt_en = false;
    adapter->ref_clk_freq = AL_ETH_REF_FREQ_375_MHZ;
    adapter->mdio_freq = AL_ETH_DEFAULT_MDIO_FREQ_KHZ;
  } else {
    struct al_eth_board_params params;
    int rc;

    adapter->auto_speed = false;

    rc = al_eth_board_params_get(adapter->mac_base, &params);
    if (rc) {
      int dt_rc;

      dt_rc = al_eth_board_params_from_dt(adapter, &params);
      if (dt_rc) {
        dev_err(&adapter->pdev->dev,
                "board info not available and DT fallback parse failed (%d)\n",
                dt_rc);
        return -1;
      }

      dev_warn(&adapter->pdev->dev,
               "board info missing in bootloader scratch registers, using DT al,* properties\n");
      al_eth_board_params_set(adapter->mac_base, &params);

      rc = al_eth_board_params_get(adapter->mac_base, &params);
      if (rc) {
        dev_err(&adapter->pdev->dev,
                "failed to program/read board params from DT fallback\n");
        return -1;
      }
    }

    adapter->phy_exist = params.phy_exist == AL_TRUE;
    adapter->phy_addr = params.phy_mdio_addr;
    adapter->an_en = params.autoneg_enable;
    adapter->lt_en = params.kr_lt_enable;
    adapter->serdes_grp = params.serdes_grp;
    adapter->serdes_lane = params.serdes_lane;
    adapter->sfp_detection_needed = params.sfp_plus_module_exist;
    adapter->i2c_adapter_id = params.i2c_adapter_id;
    adapter->ref_clk_freq = params.ref_clk_freq;
    adapter->dont_override_serdes = params.dont_override_serdes;
    adapter->link_config.active_duplex = !params.half_duplex;
    adapter->link_config.autoneg =
        (adapter->phy_exist) ? (params.an_mode == AL_ETH_BOARD_AUTONEG_IN_BAND)
                             : (!params.an_disable);
    adapter->link_config.force_1000_base_x = params.force_1000_base_x;
    adapter->retimer.exist = params.retimer_exist;
    adapter->retimer.bus_id = params.retimer_bus_id;
    adapter->retimer.i2c_addr = params.retimer_i2c_addr;
    adapter->retimer.channel = params.retimer_channel;
    adapter->phy_if = params.phy_if;

    switch (params.speed) {
    case AL_ETH_BOARD_1G_SPEED_1000M:
      adapter->link_config.active_speed = 1000;
      break;
    case AL_ETH_BOARD_1G_SPEED_100M:
      adapter->link_config.active_speed = 100;
      break;
    case AL_ETH_BOARD_1G_SPEED_10M:
      adapter->link_config.active_speed = 10;
      break;
    default:
      dev_warn(&adapter->pdev->dev, "%s: invalid speed (%d)\n", __func__,
               params.speed);
      adapter->link_config.active_speed = 1000;
    }

    switch (params.mdio_freq) {
    case AL_ETH_BOARD_MDIO_FREQ_2_5_MHZ:
      adapter->mdio_freq = 2500;
      break;
    case AL_ETH_BOARD_MDIO_FREQ_1_MHZ:
      adapter->mdio_freq = 1000;
      break;
    default:
      dev_warn(&adapter->pdev->dev, "%s: invalid mdio freq (%d)\n", __func__,
               params.mdio_freq);
      adapter->mdio_freq = 2500;
    }

    switch (params.media_type) {
    case AL_ETH_BOARD_MEDIA_TYPE_RGMII:
      if (params.sfp_plus_module_exist == AL_TRUE)
        /* Backward compatibility */
        adapter->mac_mode = AL_ETH_MAC_MODE_SGMII;
      else
        adapter->mac_mode = AL_ETH_MAC_MODE_RGMII;

      adapter->use_lm = false;
      break;
    case AL_ETH_BOARD_MEDIA_TYPE_SGMII:
      adapter->mac_mode = AL_ETH_MAC_MODE_SGMII;
      adapter->use_lm = true;
      break;
    case AL_ETH_BOARD_MEDIA_TYPE_SGMII_2_5G:
      adapter->mac_mode = AL_ETH_MAC_MODE_SGMII_2_5G;
      adapter->use_lm = false;
      break;
    case AL_ETH_BOARD_MEDIA_TYPE_10GBASE_SR:
      adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;
      adapter->use_lm = true;
      break;
    case AL_ETH_BOARD_MEDIA_TYPE_AUTO_DETECT:
      adapter->sfp_detection_needed = AL_TRUE;
      adapter->auto_speed = false;
      adapter->use_lm = true;
      break;
    case AL_ETH_BOARD_MEDIA_TYPE_AUTO_DETECT_AUTO_SPEED:
      adapter->sfp_detection_needed = AL_TRUE;
      adapter->auto_speed = true;
      adapter->mac_mode_set = false;
      adapter->use_lm = true;
      break;
    case AL_ETH_BOARD_MEDIA_TYPE_NBASE_T:
      adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;
      adapter->phy_fixup_needed = true;
      break;
    default:
      dev_err(&adapter->pdev->dev, "%s: unsupported media type %d\n", __func__,
              params.media_type);
      return -1;
    }
    dev_info(&adapter->pdev->dev,
             "Board info: board type %d ext phy exists %s, ext phy addr %x, "
             "mdio freq %u Khz, SFP connected %s, media %d\n",
             adapter->board_type, params.phy_exist == AL_TRUE ? "Yes" : "No",
             params.phy_mdio_addr, adapter->mdio_freq,
             params.sfp_plus_module_exist == AL_TRUE ? "Yes" : "No",
             params.media_type);
  }

  al_eth_mac_addr_read(adapter->ec_base, 0, adapter->mac_addr);

  return 0;
}

static inline void al_eth_flow_ctrl_init(struct al_eth_adapter *adapter) {
  uint8_t default_flow_ctrl;

  default_flow_ctrl = AL_ETH_FLOW_CTRL_TX_PAUSE;
  default_flow_ctrl |= AL_ETH_FLOW_CTRL_RX_PAUSE;

  adapter->link_config.flow_ctrl_supported = default_flow_ctrl;
}

static uint8_t al_eth_flow_ctrl_mutual_cap_get(struct al_eth_adapter *adapter) {
  struct phy_device *phydev =
      mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);
  struct al_eth_link_config *link_config = &adapter->link_config;
  uint8_t peer_flow_ctrl = AL_ETH_FLOW_CTRL_AUTONEG;
  uint8_t new_flow_ctrl = AL_ETH_FLOW_CTRL_AUTONEG;

  if (phydev->pause)
    peer_flow_ctrl |= (AL_ETH_FLOW_CTRL_TX_PAUSE | AL_ETH_FLOW_CTRL_RX_PAUSE);
  if (phydev->asym_pause)
    peer_flow_ctrl ^= (AL_ETH_FLOW_CTRL_RX_PAUSE);

  /*
   * in autoneg mode, supported flow ctrl is also
   * the current advertising
   */
  if ((peer_flow_ctrl & AL_ETH_FLOW_CTRL_TX_PAUSE) ==
      (link_config->flow_ctrl_supported & AL_ETH_FLOW_CTRL_TX_PAUSE))
    new_flow_ctrl |= AL_ETH_FLOW_CTRL_TX_PAUSE;
  if ((peer_flow_ctrl & AL_ETH_FLOW_CTRL_RX_PAUSE) ==
      (link_config->flow_ctrl_supported & AL_ETH_FLOW_CTRL_RX_PAUSE))
    new_flow_ctrl |= AL_ETH_FLOW_CTRL_RX_PAUSE;

  return new_flow_ctrl;
}

static int al_eth_flow_ctrl_config(struct al_eth_adapter *adapter) {
  struct al_eth_flow_control_params *flow_ctrl_params;
  uint8_t active = adapter->link_config.flow_ctrl_active;
  int i;

  flow_ctrl_params = &adapter->flow_ctrl_params;

  flow_ctrl_params->type = AL_ETH_FLOW_CONTROL_TYPE_LINK_PAUSE;
  flow_ctrl_params->obay_enable = ((active & AL_ETH_FLOW_CTRL_RX_PAUSE) != 0);
  flow_ctrl_params->gen_enable = ((active & AL_ETH_FLOW_CTRL_TX_PAUSE) != 0);

  flow_ctrl_params->rx_fifo_th_high = AL_ETH_FLOW_CTRL_RX_FIFO_TH_HIGH;
  flow_ctrl_params->rx_fifo_th_low = AL_ETH_FLOW_CTRL_RX_FIFO_TH_LOW;
  flow_ctrl_params->quanta = AL_ETH_FLOW_CTRL_QUANTA;
  flow_ctrl_params->quanta_th = AL_ETH_FLOW_CTRL_QUANTA_TH;

  /* map priority to queue index, queue id = priority/2 */
  for (i = 0; i < AL_ETH_FWD_PRIO_TABLE_NUM; i++)
    flow_ctrl_params->prio_q_map[0][i] = 1 << (i >> 1);

  al_eth_flow_control_config(&adapter->hal_adapter, flow_ctrl_params);

  return 0;
}

static void al_eth_flow_ctrl_enable(struct al_eth_adapter *adapter) {
  /* change the active configuration to the default / force by ethtool
   * and call to configure */
  adapter->link_config.flow_ctrl_active =
      adapter->link_config.flow_ctrl_supported;

  al_eth_flow_ctrl_config(adapter);
}

static void al_eth_flow_ctrl_disable(struct al_eth_adapter *adapter) {
  adapter->link_config.flow_ctrl_active = 0;
  al_eth_flow_ctrl_config(adapter);
}

static int al_eth_hw_init_adapter(struct al_eth_adapter *adapter) {
  struct al_eth_adapter_params *params = &adapter->eth_hal_params;
  int rc;

  params->dev_id = adapter->dev_id;
  params->rev_id = adapter->rev_id;
  params->udma_id = 0;
  params->enable_rx_parser = 1;                /* enable rx epe parser*/
  params->udma_regs_base = adapter->udma_base; /* UDMA register base address */
  params->ec_regs_base =
      adapter->ec_base; /* Ethernet controller registers base address */
  params->mac_regs_base =
      adapter->mac_base; /* Ethernet MAC registers base address */
  params->name = adapter->name;

  rc = al_eth_adapter_init(&adapter->hal_adapter, params);
  if (rc)
    dev_err(&adapter->pdev->dev, "%s failed at hal init!\n", __func__);

  if ((adapter->board_type == ALPINE_NIC) ||
      (adapter->board_type == ALPINE_FPGA_NIC)) {
    /* in pcie NIC mode, force eth UDMA to access PCIE0 using the vmid */
    if (adapter->rev_id > AL_ETH_REV_ID_0) {
      struct al_udma_gen_vmid_conf conf;
      int i;
      for (i = 0; i < DMA_MAX_Q; i++) {
        conf.tx_q_conf[i].queue_en = AL_TRUE;
        conf.tx_q_conf[i].desc_en = AL_FALSE;
        conf.tx_q_conf[i].vmid = 0x100; /* for access from PCIE0 */
        conf.rx_q_conf[i].queue_en = AL_TRUE;
        conf.rx_q_conf[i].desc_en = AL_FALSE;
        conf.rx_q_conf[i].vmid = 0x100; /* for access from PCIE0 */
      }
      al_udma_gen_vmid_conf_set(adapter->udma_base, &conf);
    }
  }
  return rc;
}

static int al_eth_hw_init(struct al_eth_adapter *adapter) {
  int rc;

  rc = al_eth_hw_init_adapter(adapter);
  if (rc)
    return rc;

  rc = al_eth_mac_config(&adapter->hal_adapter, adapter->mac_mode);
  if (rc < 0) {
    dev_err(&adapter->pdev->dev, "%s failed to configure mac!\n", __func__);
    return rc;
  }

  if ((adapter->mac_mode == AL_ETH_MAC_MODE_SGMII) ||
      (adapter->mac_mode == AL_ETH_MAC_MODE_RGMII &&
       adapter->phy_exist == AL_FALSE)) {
    rc = al_eth_mac_link_config(
        &adapter->hal_adapter, adapter->link_config.force_1000_base_x,
        adapter->link_config.autoneg, adapter->link_config.active_speed,
        adapter->link_config.active_duplex);
    if (rc) {
      dev_err(&adapter->pdev->dev, "%s failed to configure link parameters!\n",
              __func__);
      return rc;
    }
  }

  rc = al_eth_mdio_config(&adapter->hal_adapter,
                          (adapter->phy_if == AL_ETH_BOARD_PHY_IF_XMDIO)
                              ? AL_ETH_MDIO_TYPE_CLAUSE_45
                              : AL_ETH_MDIO_TYPE_CLAUSE_22,
                          AL_TRUE /*shared_mdio_if*/, adapter->ref_clk_freq,
                          adapter->mdio_freq);
  if (rc) {
    dev_err(&adapter->pdev->dev, "%s failed at mdio config!\n", __func__);
    return rc;
  }

  al_eth_flow_ctrl_init(adapter);

  return rc;
}

static int al_eth_udma_queues_disable_all(struct al_eth_adapter *adapter);

static int al_eth_hw_stop(struct al_eth_adapter *adapter) {
  al_eth_mac_stop(&adapter->hal_adapter);

  /* wait till pending rx packets written and UDMA becomes idle,
   * the MAC has ~10KB fifo, 10us should be enought time for the
   * UDMA to write to the memory
   */
  udelay(10);

  /* disable hw queues */
  al_eth_udma_queues_disable_all(adapter);
  al_eth_adapter_stop(&adapter->hal_adapter);

  /* disable flow ctrl to avoid pause packets*/
  al_eth_flow_ctrl_disable(adapter);

  return 0;
}

static int al_eth_udma_queue_enable(struct al_eth_adapter *adapter,
                                    enum al_udma_type type, int qid) {
  int rc = 0;
  char *name = (type == UDMA_TX) ? "Tx" : "Rx";
  struct al_udma_q_params *q_params;

  if (type == UDMA_TX)
    q_params = &adapter->tx_ring[qid].q_params;
  else
    q_params = &adapter->rx_ring[qid].q_params;

  rc = al_eth_queue_config(&adapter->hal_adapter, type, qid, q_params);
  if (rc < 0) {
    netdev_err(adapter->netdev, "config %s queue %u failed\n", name, qid);
    return rc;
  }

  rc = al_eth_queue_enable(&adapter->hal_adapter, type, qid);

  if (rc < 0)
    netdev_err(adapter->netdev, "enable %s queue %u failed\n", name, qid);

  return rc;
}

static int al_eth_udma_queues_enable_all(struct al_eth_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_tx_queues; i++)
    al_eth_udma_queue_enable(adapter, UDMA_TX, i);

  for (i = 0; i < adapter->num_rx_queues; i++)
    al_eth_udma_queue_enable(adapter, UDMA_RX, i);
  return 0;
}

static int al_eth_udma_queue_disable(struct al_eth_adapter *adapter,
                                     enum al_udma_type type, int qid) {
  int rc = 0;
  char *name = (type == UDMA_TX) ? "Tx" : "Rx";

  rc = al_eth_queue_disable(&adapter->hal_adapter, type, qid);

  if (rc < 0)
    netdev_err(adapter->netdev, "disable %s queue %u failed\n", name, qid);

  return rc;
}

static int al_eth_udma_queues_disable_all(struct al_eth_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_tx_queues; i++)
    al_eth_udma_queue_disable(adapter, UDMA_TX, i);

  for (i = 0; i < adapter->num_rx_queues; i++)
    al_eth_udma_queue_disable(adapter, UDMA_RX, i);
  return 0;
}

static void al_eth_init_rings(struct al_eth_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_tx_queues; i++) {
    struct al_eth_ring *ring = &adapter->tx_ring[i];

    ring->dev = &adapter->pdev->dev;
    ring->netdev = adapter->netdev;
    al_udma_q_handle_get(&adapter->hal_adapter.tx_udma, i, &ring->dma_q);
    ring->sw_count = adapter->tx_ring_count;
    ring->hw_count = adapter->tx_descs_count;
    ring->unmask_reg_offset = al_udma_iofic_unmask_offset_get(
        (struct unit_regs *)adapter->udma_base, AL_UDMA_IOFIC_LEVEL_PRIMARY,
        AL_INT_GROUP_C);
    ring->unmask_val = ~(1 << i);
  }

  for (i = 0; i < adapter->num_rx_queues; i++) {
    struct al_eth_ring *ring = &adapter->rx_ring[i];

    ring->dev = &adapter->pdev->dev;
    ring->netdev = adapter->netdev;
    ring->napi = &adapter->al_napi[AL_ETH_RXQ_NAPI_IDX(adapter, i)].napi;
    al_udma_q_handle_get(&adapter->hal_adapter.rx_udma, i, &ring->dma_q);
    ring->sw_count = adapter->rx_ring_count;
    ring->hw_count = adapter->rx_descs_count;
    ring->unmask_reg_offset = al_udma_iofic_unmask_offset_get(
        (struct unit_regs *)adapter->udma_base, AL_UDMA_IOFIC_LEVEL_PRIMARY,
        AL_INT_GROUP_B);
    ring->unmask_val = ~(1 << i);
  }
}

/**
 * al_eth_setup_tx_resources - allocate Tx resources (Descriptors)
 * @adapter: network interface device structure
 * @qid: queue index
 *
 * Return 0 on success, negative on failure
 **/
static int al_eth_setup_tx_resources(struct al_eth_adapter *adapter, int qid) {
  struct al_eth_ring *tx_ring = &adapter->tx_ring[qid];
  struct device *dev = tx_ring->dev;
  struct al_udma_q_params *q_params = &tx_ring->q_params;
  int size;

  size = sizeof(struct al_eth_tx_buffer) * tx_ring->sw_count;

  tx_ring->tx_buffer_info = kzalloc(size, GFP_KERNEL);
  if (!tx_ring->tx_buffer_info)
    return -ENOMEM;

  /* TODO: consider ALIGN to page size */
  tx_ring->descs_size = tx_ring->hw_count * sizeof(union al_udma_desc);
  q_params->size = tx_ring->hw_count;

  q_params->desc_base = dma_alloc_coherent(
      dev, tx_ring->descs_size, &q_params->desc_phy_base, GFP_KERNEL);

  if (!q_params->desc_base)
    return -ENOMEM;

  q_params->cdesc_base = NULL; /* completion queue not used for tx */
  q_params->cdesc_size = 8;
  tx_ring->next_to_use = 0;
  tx_ring->next_to_clean = 0;
  return 0;
}

/**
 * al_eth_free_tx_resources - Free Tx Resources per Queue
 * @adapter: network interface device structure
 * @qid: queue index
 *
 * Free all transmit software resources
 **/
static void al_eth_free_tx_resources(struct al_eth_adapter *adapter, int qid) {
  struct al_eth_ring *tx_ring = &adapter->tx_ring[qid];
  struct al_udma_q_params *q_params = &tx_ring->q_params;

  netdev_dbg(adapter->netdev, "%s qid %d\n", __func__, qid);

  kfree(tx_ring->tx_buffer_info);
  tx_ring->tx_buffer_info = NULL;

  /* if not set, then don't free */
  if (!q_params->desc_base)
    return;

  dma_free_coherent(tx_ring->dev, tx_ring->descs_size, q_params->desc_base,
                    q_params->desc_phy_base);

  q_params->desc_base = NULL;
}

/**
 * al_eth_setup_all_tx_resources - allocate all queues Tx resources
 * @adapter: private structure
 *
 * Return 0 on success, negative on failure
 **/
static int al_eth_setup_all_tx_resources(struct al_eth_adapter *adapter) {
  int i, rc = 0;

  for (i = 0; i < adapter->num_tx_queues; i++) {
    rc = al_eth_setup_tx_resources(adapter, i);
    if (!rc)
      continue;

    netdev_err(adapter->netdev, "Allocation for Tx Queue %u failed\n", i);
    goto err_setup_tx;
  }

  return 0;
err_setup_tx:
  /* rewind the index freeing the rings as we go */
  while (i--)
    al_eth_free_tx_resources(adapter, i);
  return rc;
}

/**
 * al_eth_free_all_tx_resources - Free Tx Resources for All Queues
 * @adapter: board private structure
 *
 * Free all transmit software resources
 **/
static void al_eth_free_all_tx_resources(struct al_eth_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_tx_queues; i++)
    if (adapter->tx_ring[i].q_params.desc_base)
      al_eth_free_tx_resources(adapter, i);
}

/**
 * al_eth_setup_rx_resources - allocate Rx resources (Descriptors)
 * @adapter: network interface device structure
 * @qid: queue index
 *
 * Returns 0 on success, negative on failure
 **/
static int al_eth_setup_rx_resources(struct al_eth_adapter *adapter,
                                     unsigned int qid) {
  struct al_eth_ring *rx_ring = &adapter->rx_ring[qid];
  struct device *dev = rx_ring->dev;
  struct al_udma_q_params *q_params = &rx_ring->q_params;
  int size;

  size = sizeof(struct al_eth_rx_buffer) * rx_ring->sw_count;

  /* alloc extra element so in rx path we can always prefetch rx_info + 1*/
  size += 1;

  rx_ring->rx_buffer_info = kzalloc(size, GFP_KERNEL);
  if (!rx_ring->rx_buffer_info)
    return -ENOMEM;

  /* TODO: consider Round up to nearest 4K */
  rx_ring->descs_size = rx_ring->hw_count * sizeof(union al_udma_desc);
  q_params->size = rx_ring->hw_count;

  q_params->desc_base = dma_alloc_coherent(
      dev, rx_ring->descs_size, &q_params->desc_phy_base, GFP_KERNEL);
  if (!q_params->desc_base)
    return -ENOMEM;

  q_params->cdesc_size = 16;
  rx_ring->cdescs_size = rx_ring->hw_count * q_params->cdesc_size;
  q_params->cdesc_base = dma_alloc_coherent(
      dev, rx_ring->cdescs_size, &q_params->cdesc_phy_base, GFP_KERNEL);
  if (!q_params->cdesc_base)
    return -ENOMEM;

  /* Zero out the descriptor ring */
  memset(q_params->cdesc_base, 0, rx_ring->cdescs_size);

  rx_ring->next_to_clean = 0;
  rx_ring->next_to_use = 0;

  return 0;
}

/**
 * al_eth_free_rx_resources - Free Rx Resources
 * @adapter: network interface device structure
 * @qid: queue index
 *
 * Free all receive software resources
 **/
static void al_eth_free_rx_resources(struct al_eth_adapter *adapter,
                                     unsigned int qid) {
  struct al_eth_ring *rx_ring = &adapter->rx_ring[qid];
  struct al_udma_q_params *q_params = &rx_ring->q_params;

  kfree(rx_ring->rx_buffer_info);
  rx_ring->rx_buffer_info = NULL;

  /* if not set, then don't free */
  if (!q_params->desc_base)
    return;

  dma_free_coherent(rx_ring->dev, rx_ring->descs_size, q_params->desc_base,
                    q_params->desc_phy_base);

  q_params->desc_base = NULL;

  /* if not set, then don't free */
  if (!q_params->cdesc_base)
    return;

  dma_free_coherent(rx_ring->dev, rx_ring->cdescs_size, q_params->cdesc_base,
                    q_params->cdesc_phy_base);

  q_params->cdesc_phy_base = 0;
}

/**
 * al_eth_setup_all_rx_resources - allocate all queues Rx resources
 * @adapter: board private structure
 *
 * Return 0 on success, negative on failure
 **/
static int al_eth_setup_all_rx_resources(struct al_eth_adapter *adapter) {
  int i, rc = 0;

  for (i = 0; i < adapter->num_rx_queues; i++) {
    rc = al_eth_setup_rx_resources(adapter, i);
    if (!rc)
      continue;

    netdev_err(adapter->netdev, "Allocation for Rx Queue %u failed\n", i);
    goto err_setup_rx;
  }
  return 0;

err_setup_rx:
  /* rewind the index freeing the rings as we go */
  while (i--)
    al_eth_free_rx_resources(adapter, i);
  return rc;
}

/**
 * al_eth_free_all_rx_resources - Free Rx Resources for All Queues
 * @adapter: board private structure
 *
 * Free all receive software resources
 **/
static void al_eth_free_all_rx_resources(struct al_eth_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_rx_queues; i++)
    if (adapter->rx_ring[i].q_params.desc_base)
      al_eth_free_rx_resources(adapter, i);
}

#ifdef CONFIG_AL_ETH_ALLOC_PAGE
static inline int al_eth_alloc_rx_page(struct al_eth_adapter *adapter,
                                       struct al_eth_rx_buffer *rx_info,
                                       gfp_t gfp) {
  struct al_buf *al_buf;
  struct page *page;
  dma_addr_t dma;

  /* if previous allocated page is not used */
  if (rx_info->page != NULL)
    return 0;

  page = alloc_page(gfp);
  if (unlikely(!page))
    return -ENOMEM;

  dma = dma_map_page(&adapter->pdev->dev, page, 0, PAGE_SIZE, DMA_FROM_DEVICE);
  if (unlikely(dma_mapping_error(&adapter->pdev->dev, dma))) {
    __free_page(page);
    return -EIO;
  }
  dev_dbg(&adapter->pdev->dev, "alloc page %p, rx_info %p\n", page, rx_info);

  rx_info->page = page;
  rx_info->page_offset = 0;
  al_buf = &rx_info->al_buf;
  dma_unmap_addr_set(al_buf, addr, dma);
  dma_unmap_addr_set(rx_info, dma, dma);
  dma_unmap_len_set(al_buf, len, PAGE_SIZE);
  return 0;
}

static void al_eth_free_rx_page(struct al_eth_adapter *adapter,
                                struct al_eth_rx_buffer *rx_info) {
  struct page *page = rx_info->page;
  struct al_buf *al_buf = &rx_info->al_buf;

  if (!page)
    return;

  dma_unmap_page(&adapter->pdev->dev, dma_unmap_addr(al_buf, addr), PAGE_SIZE,
                 DMA_FROM_DEVICE);

  __free_page(page);
  rx_info->page = NULL;
}

#elif defined(CONFIG_AL_ETH_ALLOC_FRAG)

static inline int al_eth_alloc_rx_frag(struct al_eth_adapter *adapter,
                                       struct al_eth_ring *rx_ring,
                                       struct al_eth_rx_buffer *rx_info) {
  struct al_buf *al_buf;
  dma_addr_t dma;
  u8 *data;

  /* if previous allocated frag is not used */
  if (rx_info->data != NULL)
    return 0;

  rx_info->data_size = min_t(
      unsigned int, (rx_ring->netdev->mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN),
      adapter->max_rx_buff_alloc_size);

  rx_info->data_size = max_t(unsigned int, rx_info->data_size,
                             AL_ETH_DEFAULT_MIN_RX_BUFF_ALLOC_SIZE);

  rx_info->frag_size = SKB_DATA_ALIGN(rx_info->data_size + AL_ETH_RX_OFFSET) +
                       SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
  data = netdev_alloc_frag(rx_info->frag_size);

  if (!data)
    return -ENOMEM;

  dma = dma_map_single(rx_ring->dev, data + AL_ETH_RX_OFFSET,
                       rx_info->data_size, DMA_FROM_DEVICE);
  if (unlikely(dma_mapping_error(rx_ring->dev, dma))) {
    put_page(virt_to_head_page(data));
    return -EIO;
  }
  netdev_dbg(rx_ring->netdev, "alloc frag %p, rx_info %p len %x skb size %x\n",
             data, rx_info, rx_info->data_size, rx_info->frag_size);

  rx_info->data = data;

  BUG_ON(!virt_addr_valid(rx_info->data));
  rx_info->page = virt_to_head_page(rx_info->data);
  rx_info->page_offset =
      (uintptr_t)rx_info->data - (uintptr_t)page_address(rx_info->page);
  al_buf = &rx_info->al_buf;
  dma_unmap_addr_set(al_buf, addr, dma);
  dma_unmap_addr_set(rx_info, dma, dma);
  dma_unmap_len_set(al_buf, len, rx_info->data_size);
  return 0;
}

static void al_eth_free_rx_frag(struct al_eth_adapter *adapter,
                                struct al_eth_rx_buffer *rx_info) {
  u8 *data = rx_info->data;
  struct al_buf *al_buf = &rx_info->al_buf;

  if (!data)
    return;

  dma_unmap_single(&adapter->pdev->dev, dma_unmap_addr(al_buf, addr),
                   rx_info->data_size, DMA_FROM_DEVICE);

  put_page(virt_to_head_page(data));
  rx_info->data = NULL;
}

#elif defined(CONFIG_AL_ETH_ALLOC_SKB)

static inline int al_eth_alloc_rx_skb(struct al_eth_adapter *adapter,
                                      struct al_eth_ring *rx_ring,
                                      struct al_eth_rx_buffer *rx_info) {
  struct sk_buff *skb;
  struct al_buf *al_buf;
  dma_addr_t dma;
  struct sk_buff_head *rx_rc = &__get_cpu_var(rx_recycle_cache);

  if (rx_info->skb)
    return 0;

  rx_info->data_size =
      rx_ring->netdev->mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN;

  rx_info->data_size = max_t(unsigned int, rx_info->data_size,
                             AL_ETH_DEFAULT_MIN_RX_BUFF_ALLOC_SIZE);

  skb = __skb_dequeue(rx_rc);
  if (skb == NULL)
    skb = __netdev_alloc_skb_ip_align(rx_ring->netdev, rx_info->data_size,
                                      GFP_DMA);

  if (!skb)
    return -ENOMEM;

  dma = dma_map_single(rx_ring->dev, skb->data + AL_ETH_RX_OFFSET,
                       rx_info->data_size, DMA_FROM_DEVICE);
  if (unlikely(dma_mapping_error(rx_ring->dev, dma)))
    return -EIO;

  rx_info->data = skb->data;
  rx_info->skb = skb;

  BUG_ON(!virt_addr_valid(rx_info->data));
  al_buf = &rx_info->al_buf;
  dma_unmap_addr_set(al_buf, addr, dma);
  dma_unmap_addr_set(rx_info, dma, dma);
  dma_unmap_len_set(al_buf, len, rx_info->data_size);
  return 0;
}

static void al_eth_free_rx_skb(struct al_eth_adapter *adapter,
                               struct al_eth_rx_buffer *rx_info) {
  struct al_buf *al_buf = &rx_info->al_buf;

  if (!rx_info->skb)
    return;

  dma_unmap_single(&adapter->pdev->dev, dma_unmap_addr(al_buf, addr),
                   rx_info->data_size, DMA_FROM_DEVICE);
  dev_kfree_skb_any(rx_info->skb);
  rx_info->skb = NULL;
}
#endif

static int al_eth_refill_rx_bufs(struct al_eth_adapter *adapter,
                                 unsigned int qid, unsigned int num) {
  struct al_eth_ring *rx_ring = &adapter->rx_ring[qid];
  u16 next_to_use;
  unsigned int i;

  next_to_use = rx_ring->next_to_use;

  for (i = 0; i < num; i++) {
    int rc;
    struct al_eth_rx_buffer *rx_info = &rx_ring->rx_buffer_info[next_to_use];

#ifdef CONFIG_AL_ETH_ALLOC_PAGE
    if (unlikely(al_eth_alloc_rx_page(adapter, rx_info,
                                      __GFP_COLD | GFP_ATOMIC | __GFP_COMP) <
                 0)) {
#elif defined(CONFIG_AL_ETH_ALLOC_FRAG)
    if (unlikely(al_eth_alloc_rx_frag(adapter, rx_ring, rx_info) < 0)) {
#elif defined(CONFIG_AL_ETH_ALLOC_SKB)
    if (unlikely(al_eth_alloc_rx_skb(adapter, rx_ring, rx_info) < 0)) {
#endif
      netdev_warn(adapter->netdev, "failed to alloc buffer for rx queue %d\n",
                  qid);
      break;
    }
    rc = al_eth_rx_buffer_add(rx_ring->dma_q, &rx_info->al_buf,
                              AL_ETH_RX_FLAGS_INT, NULL);
    if (unlikely(rc)) {
      netdev_warn(adapter->netdev, "failed to add buffer for rx queue %d\n",
                  qid);
      break;
    }
    next_to_use = AL_ETH_RX_RING_IDX_NEXT(rx_ring, next_to_use);
  }

  if (unlikely(i < num)) {
    netdev_warn(adapter->netdev,
                "refilled rx queue %d with %d pages only - available %d\n", qid,
                i, al_udma_available_get(rx_ring->dma_q));
  }

  if (likely(i))
    al_eth_rx_buffer_action(rx_ring->dma_q, i);

  rx_ring->next_to_use = next_to_use;

  return i;
}

static void al_eth_free_rx_bufs(struct al_eth_adapter *adapter,
                                unsigned int qid) {
  struct al_eth_ring *rx_ring = &adapter->rx_ring[qid];
  unsigned int i;

  for (i = 0; i < AL_ETH_DEFAULT_RX_DESCS; i++) {
    struct al_eth_rx_buffer *rx_info = &rx_ring->rx_buffer_info[i];

#ifdef CONFIG_AL_ETH_ALLOC_PAGE
    if (rx_info->page)
      al_eth_free_rx_page(adapter, rx_info);
#elif defined(CONFIG_AL_ETH_ALLOC_FRAG)
    if (rx_info->data)
      al_eth_free_rx_frag(adapter, rx_info);
#elif defined(CONFIG_AL_ETH_ALLOC_SKB)
    if (rx_info->skb)
      al_eth_free_rx_skb(adapter, rx_info);
#endif
  }
}

/**
 * al_eth_refill_all_rx_bufs - allocate all queues Rx buffers
 * @adapter: board private structure
 *
 **/
static void al_eth_refill_all_rx_bufs(struct al_eth_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_rx_queues; i++)
    al_eth_refill_rx_bufs(adapter, i, AL_ETH_DEFAULT_RX_DESCS - 1);
}

static void al_eth_free_all_rx_bufs(struct al_eth_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_rx_queues; i++)
    al_eth_free_rx_bufs(adapter, i);
}

/**
 * al_eth_free_tx_bufs - Free Tx Buffers per Queue
 * @adapter: network interface device structure
 * @qid: queue index
 **/
static void al_eth_free_tx_bufs(struct al_eth_adapter *adapter,
                                unsigned int qid) {
  struct al_eth_ring *tx_ring = &adapter->tx_ring[qid];
  unsigned int i;
  bool udma_debug_printed = 0;

  for (i = 0; i < AL_ETH_DEFAULT_TX_SW_DESCS; i++) {
    struct al_eth_tx_buffer *tx_info = &tx_ring->tx_buffer_info[i];
    struct al_buf *al_buf;
    int nr_frags;
    int j;

    if (tx_info->skb == NULL)
      continue;

    if (!udma_debug_printed) {
      al_udma_regs_print(tx_ring->dma_q->udma, AL_UDMA_DEBUG_QUEUE(qid));
      al_udma_q_struct_print(tx_ring->dma_q->udma, qid);
      udma_debug_printed = 1;
    }
    netdev_warn(adapter->netdev, "free uncompleted tx skb qid %d idx 0x%x\n",
                qid, i);

    al_buf = tx_info->hal_pkt.bufs;
    dma_unmap_single(&adapter->pdev->dev, dma_unmap_addr(al_buf, addr),
                     dma_unmap_len(al_buf, len), DMA_TO_DEVICE);

    /* unmap remaining mapped pages */
    nr_frags = tx_info->hal_pkt.num_of_bufs - 1;
    for (j = 0; j < nr_frags; j++) {
      al_buf++;
      dma_unmap_page(&adapter->pdev->dev, dma_unmap_addr(al_buf, addr),
                     dma_unmap_len(al_buf, len), DMA_TO_DEVICE);
    }

    dev_kfree_skb_any(tx_info->skb);
  }
  netdev_tx_reset_queue(netdev_get_tx_queue(adapter->netdev, qid));
}

static void al_eth_free_all_tx_bufs(struct al_eth_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_rx_queues; i++)
    al_eth_free_tx_bufs(adapter, i);
}

#ifdef CONFIG_AL_ETH_ALLOC_SKB
/* the following 3 functions taken from old kernels */
static bool skb_is_recycleable(const struct sk_buff *skb, int skb_size) {
  if (irqs_disabled())
    return false;

  if (skb_shinfo(skb)->tx_flags & SKBTX_DEV_ZEROCOPY)
    return false;

  if (skb_is_nonlinear(skb) || skb->fclone != SKB_FCLONE_UNAVAILABLE)
    return false;

  skb_size = SKB_DATA_ALIGN(skb_size + NET_SKB_PAD);
  if (skb_end_offset(skb) < skb_size)
    return false;

  if (skb_shared(skb) || skb_cloned(skb))
    return false;

  return true;
}

/**
 *     skb_recycle - clean up an skb for reuse
 *     @skb: buffer
 *
 *     Recycles the skb to be reused as a receive buffer. This
 *     function does any necessary reference count dropping, and
 *     cleans up the skbuff as if it just came from __alloc_skb().
 */
void skb_recycle(struct sk_buff *skb) {
  struct skb_shared_info *shinfo;

  skb_release_head_state(skb);
  shinfo = skb_shinfo(skb);
  memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
  atomic_set(&shinfo->dataref, 1);

  memset(skb, 0, offsetof(struct sk_buff, tail));
  skb->data = skb->head + NET_SKB_PAD;
  skb_reset_tail_pointer(skb);
}

/**
 *     skb_recycle_check - check if skb can be reused for receive
 *     @skb: buffer
 *     @skb_size: minimum receive buffer size
 *
 *     Checks that the skb passed in is not shared or cloned, and
 *     that it is linear and its head portion at least as large as
 *     skb_size so that it can be recycled as a receive buffer.
 *     If these conditions are met, this function does any necessary
 *     reference count dropping and cleans up the skbuff as if it
 *     just came from __alloc_skb().
 */
bool skb_recycle_check(struct sk_buff *skb, int skb_size) {
  if (!skb_is_recycleable(skb, skb_size))
    return false;

  skb_recycle(skb);

  return true;
}
#endif

/**
 * al_eth_tx_poll - NAPI Tx polling callback
 * @napi: structure for representing this polling device
 * @budget: how many packets driver is allowed to clean
 *
 * This function is used for legacy and MSI, NAPI mode
 **/
static int al_eth_tx_poll(struct napi_struct *napi, int budget) {
  struct al_eth_napi *al_napi = container_of(napi, struct al_eth_napi, napi);
  struct al_eth_adapter *adapter = al_napi->adapter;
  unsigned int qid = al_napi->qid;
  struct al_eth_ring *tx_ring = &adapter->tx_ring[qid];
  struct netdev_queue *txq;
  unsigned int tx_bytes = 0;
  unsigned int total_done;
  u16 next_to_clean;
  int tx_pkt = 0;
#ifdef CONFIG_AL_ETH_ALLOC_SKB
  struct sk_buff_head *rx_rc = &__get_cpu_var(rx_recycle_cache);
#endif
  total_done = al_eth_comp_tx_get(tx_ring->dma_q);
  dev_dbg(&adapter->pdev->dev, "tx_poll: q %d total completed descs %x\n", qid,
          total_done);
  next_to_clean = tx_ring->next_to_clean;
  txq = netdev_get_tx_queue(adapter->netdev, qid);

  while (total_done) {
    struct al_eth_tx_buffer *tx_info;
    struct sk_buff *skb;
    struct al_buf *al_buf;
    int i, nr_frags;

    tx_info = &tx_ring->tx_buffer_info[next_to_clean];
    /* stop if not all descriptors of the packet are completed */
    if (tx_info->tx_descs > total_done)
      break;

    skb = tx_info->skb;

    /* prefetch skb_end_pointer() to speedup skb_shinfo(skb) */
    prefetch(&skb->end);

    tx_info->skb = NULL;
    al_buf = tx_info->hal_pkt.bufs;
    dma_unmap_single(tx_ring->dev, dma_unmap_addr(al_buf, addr),
                     dma_unmap_len(al_buf, len), DMA_TO_DEVICE);

    /* unmap remaining mapped pages */
    nr_frags = tx_info->hal_pkt.num_of_bufs - 1;
    for (i = 0; i < nr_frags; i++) {
      al_buf++;
      dma_unmap_page(tx_ring->dev, dma_unmap_addr(al_buf, addr),
                     dma_unmap_len(al_buf, len), DMA_TO_DEVICE);
    }

    tx_bytes += skb->len;
    dev_dbg(&adapter->pdev->dev, "tx_poll: q %d skb %p completed\n", qid, skb);
#ifdef CONFIG_AL_ETH_ALLOC_SKB
    if ((skb_queue_len(rx_rc) < AL_ETH_DEFAULT_RX_DESCS) &&
        skb_recycle_check(skb, tx_ring->netdev->mtu + ETH_HLEN + ETH_FCS_LEN +
                                   VLAN_HLEN))
      __skb_queue_head(rx_rc, skb);
    else
#endif
      dev_kfree_skb(skb);
    tx_pkt++;
    total_done -= tx_info->tx_descs;
    next_to_clean = AL_ETH_TX_RING_IDX_NEXT(tx_ring, next_to_clean);
  }

  netdev_tx_completed_queue(txq, tx_pkt, tx_bytes);

  tx_ring->next_to_clean = next_to_clean;

  dev_dbg(&adapter->pdev->dev, "tx_poll: q %d done next to clean %x\n", qid,
          next_to_clean);

  /* need to make the rings circular update visible to
   * al_eth_start_xmit() before checking for netif_queue_stopped().
   */
  smp_mb();

  if (unlikely(
          netif_tx_queue_stopped(txq) &&
          (al_udma_available_get(tx_ring->dma_q) > AL_ETH_TX_WAKEUP_THRESH))) {
    __netif_tx_lock(txq, smp_processor_id());
    if (netif_tx_queue_stopped(txq) &&
        (al_udma_available_get(tx_ring->dma_q) > AL_ETH_TX_WAKEUP_THRESH))
      netif_tx_wake_queue(txq);
    __netif_tx_unlock(txq);
  }

  /* all work done, exit the polling mode */
  napi_complete(napi);
  al_reg_write32_relaxed(tx_ring->unmask_reg_offset, tx_ring->unmask_val);
  return 0;
}
#ifdef CONFIG_AL_ETH_ALLOC_PAGE
static struct sk_buff *al_eth_rx_skb(struct al_eth_adapter *adapter,
                                     struct al_eth_ring *rx_ring,
                                     struct al_eth_pkt *hal_pkt,
                                     unsigned int descs, u16 *next_to_clean) {
  struct sk_buff *skb;
  struct al_eth_rx_buffer *rx_info = &rx_ring->rx_buffer_info[*next_to_clean];
  struct page *page = rx_info->page;
  unsigned int len;
  unsigned int buf = 0;
  void *va;

  skb = netdev_alloc_skb_ip_align(adapter->netdev, adapter->small_copy_len);
  if (!skb) {
    /*rx_ring->rx_stats.alloc_rx_buff_failed++;*/
    netdev_dbg(adapter->netdev, "Failed allocating skb\n");
    return NULL;
  }

  netdev_dbg(adapter->netdev, "rx skb allocated. len %d. data_len %d\n",
             skb->len, skb->data_len);

  len = hal_pkt->bufs[0].len;
  dev_dbg(&adapter->pdev->dev, "rx_info %p page %p\n", rx_info, rx_info->page);

  page = rx_info->page;
  /* save virt address of first buffer */
  va = page_address(rx_info->page) + rx_info->page_offset;
  prefetch(va + AL_ETH_RX_OFFSET);

  if (len <= adapter->small_copy_len) {
    netdev_dbg(adapter->netdev, "rx small packet. len %d\n", len);
    /* sync this buffer for CPU use */
    dma_sync_single_for_cpu(rx_ring->dev, rx_info->dma, len, DMA_FROM_DEVICE);
    skb_copy_to_linear_data(skb, va, len);
    dma_sync_single_for_device(rx_ring->dev, rx_info->dma, len,
                               DMA_FROM_DEVICE);

    skb_put(skb, len);
    skb->protocol = eth_type_trans(skb, adapter->netdev);
    *next_to_clean = AL_ETH_RX_RING_IDX_ADD(rx_ring, *next_to_clean, descs);
    return skb;
  }

  do {
    dma_unmap_page(rx_ring->dev, dma_unmap_addr(rx_info, dma), PAGE_SIZE,
                   DMA_FROM_DEVICE);

    skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags, rx_info->page,
                    rx_info->page_offset, len, PAGE_SIZE);

    netdev_dbg(adapter->netdev, "rx skb updated. len %d. data_len %d\n",
               skb->len, skb->data_len);

    rx_info->page = NULL;
    *next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);
    if (likely(--descs == 0))
      break;
    rx_info = &rx_ring->rx_buffer_info[*next_to_clean];
    len = hal_pkt->bufs[++buf].len;
  } while (1);

  /* Copy headers into the skb linear buffer */
  skb_copy_to_linear_data(skb, va, AL_ETH_HEADER_COPY_SIZE);
  skb->tail += AL_ETH_HEADER_COPY_SIZE;

  /* Skip headers in first fragment */
  skb_shinfo(skb)->frags[0].page_offset += AL_ETH_HEADER_COPY_SIZE;

  /* Adjust size of first fragment */
  skb_frag_size_sub(&skb_shinfo(skb)->frags[0], AL_ETH_HEADER_COPY_SIZE);
  skb->data_len -= AL_ETH_HEADER_COPY_SIZE;
  skb->protocol = eth_type_trans(skb, adapter->netdev);
  return skb;
}
#elif defined(CONFIG_AL_ETH_ALLOC_FRAG)
static struct sk_buff *al_eth_rx_skb(struct al_eth_adapter *adapter,
                                     struct al_eth_ring *rx_ring,
                                     struct al_eth_pkt *hal_pkt,
                                     unsigned int descs, u16 *next_to_clean) {
  struct sk_buff *skb;
  struct al_eth_rx_buffer *rx_info = &rx_ring->rx_buffer_info[*next_to_clean];
  unsigned int len;
  unsigned int buf = 0;

  len = hal_pkt->bufs[0].len;
  netdev_dbg(adapter->netdev, "rx_info %p data %p\n", rx_info, rx_info->data);

  prefetch(rx_info->data + AL_ETH_RX_OFFSET);

  if (len <= adapter->small_copy_len) {
    netdev_dbg(adapter->netdev, "rx small packet. len %d\n", len);

    skb = netdev_alloc_skb_ip_align(adapter->netdev, adapter->small_copy_len);
    if (unlikely(!skb))
      return NULL;

    dma_sync_single_for_cpu(&adapter->pdev->dev, rx_info->dma, len,
                            DMA_FROM_DEVICE);
    skb_copy_to_linear_data(skb, rx_info->data + AL_ETH_RX_OFFSET, len);
    dma_sync_single_for_device(&adapter->pdev->dev, rx_info->dma, len,
                               DMA_FROM_DEVICE);
    skb_put(skb, len);
    skb->protocol = eth_type_trans(skb, adapter->netdev);
    *next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);
    return skb;
  }

  dma_unmap_single(rx_ring->dev, dma_unmap_addr(rx_info, dma),
                   rx_info->data_size, DMA_FROM_DEVICE);
#if 0
	skb = build_skb(rx_info->data, rx_ring->frag_size);
	if (unlikely(!skb))
		return NULL;
#else
  skb = napi_get_frags(rx_ring->napi);
  if (unlikely(!skb))
    return NULL;

  skb_fill_page_desc(skb, skb_shinfo(skb)->nr_frags, rx_info->page,
                     rx_info->page_offset + AL_ETH_RX_OFFSET, len);

  skb->len += len;
  skb->data_len += len;
  skb->truesize += len;
#endif
#if 0
	skb_reserve(skb, AL_ETH_RX_OFFSET);
	skb_put(skb, len);
#endif
  netdev_dbg(adapter->netdev, "rx skb updated. len %d. data_len %d\n", skb->len,
             skb->data_len);

  rx_info->data = NULL;
  *next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);

  while (--descs) {
    rx_info = &rx_ring->rx_buffer_info[*next_to_clean];
    len = hal_pkt->bufs[++buf].len;

    dma_unmap_single(rx_ring->dev, dma_unmap_addr(rx_info, dma),
                     rx_info->data_size, DMA_FROM_DEVICE);

    skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags, rx_info->page,
                    rx_info->page_offset + AL_ETH_RX_OFFSET, len,
                    rx_info->data_size);

    netdev_dbg(adapter->netdev,
               "rx skb updated. len %d. "
               "data_len %d\n",
               skb->len, skb->data_len);

    rx_info->data = NULL;

    *next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);
  }

  return skb;
}
#elif defined(CONFIG_AL_ETH_ALLOC_SKB)
static struct sk_buff *al_eth_rx_skb(struct al_eth_adapter *adapter,
                                     struct al_eth_ring *rx_ring,
                                     struct al_eth_pkt *hal_pkt,
                                     unsigned int descs, u16 *next_to_clean) {
  struct sk_buff *skb;
  struct al_eth_rx_buffer *rx_info = &rx_ring->rx_buffer_info[*next_to_clean];
  unsigned int len;

  prefetch(rx_info->data + AL_ETH_RX_OFFSET);
  skb = rx_info->skb;
  prefetch(skb);
  prefetch(&skb->end);
  prefetch(&skb->dev);

  len = hal_pkt->bufs[0].len;

  dma_unmap_single(rx_ring->dev, dma_unmap_addr(rx_info, dma),
                   rx_info->data_size, DMA_FROM_DEVICE);

  skb_reserve(skb, AL_ETH_RX_OFFSET);
  skb_put(skb, len);

  skb->protocol = eth_type_trans(skb, adapter->netdev);
  rx_info->skb = NULL;
  *next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);
  /* prefetch next packet */
  prefetch((rx_info + 1)->data + AL_ETH_RX_OFFSET);
  prefetch((rx_info + 1)->skb);

  return skb;
}
#endif

/**
 * al_eth_rx_checksum - indicate in skb if hw indicated a good cksum
 * @adapter: structure containing adapter specific data
 * @hal_pkt: HAL structure for the packet
 * @skb: skb currently being received and modified
 **/
static inline void al_eth_rx_checksum(struct al_eth_adapter *adapter,
                                      struct al_eth_pkt *hal_pkt,
                                      struct sk_buff *skb) {
  skb_checksum_none_assert(skb);

  /* Rx csum disabled */
  if (unlikely(!(adapter->netdev->features & NETIF_F_RXCSUM))) {
    netdev_dbg(adapter->netdev, "hw checksum offloading disabled\n");
    return;
  }

  /* if IP and error */
  if (unlikely((hal_pkt->l3_proto_idx == AL_ETH_PROTO_ID_IPv4) &&
               (hal_pkt->flags & AL_ETH_RX_FLAGS_L3_CSUM_ERR))) {
    /* ipv4 checksum error */
    netdev_dbg(adapter->netdev, "rx ipv4 header checksum error\n");
    return;
  }

  /* if TCP/UDP */
  if (likely((hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_TCP) ||
             (hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_UDP))) {
    /* TODO: check if we need the test above for TCP/UDP */
    if (unlikely(hal_pkt->flags & AL_ETH_RX_FLAGS_L4_CSUM_ERR)) {
      /* TCP/UDP checksum error */
      netdev_dbg(adapter->netdev, "rx L4 checksum error\n");
      return;
    } else {
      netdev_dbg(adapter->netdev, "rx checksum correct\n");
      skb->ip_summed = CHECKSUM_UNNECESSARY;
    }
  }
}

/**
 * al_eth_rx_poll - NAPI Rx polling callback
 * @napi: structure for representing this polling device
 * @budget: how many packets driver is allowed to clean
 *
 * This function is used for legacy and MSI, NAPI mode
 **/
static int al_eth_rx_poll(struct napi_struct *napi, int budget) {
  struct al_eth_napi *al_napi = container_of(napi, struct al_eth_napi, napi);
  struct al_eth_adapter *adapter = al_napi->adapter;
  unsigned int qid = al_napi->qid;
  struct al_eth_ring *rx_ring = &adapter->rx_ring[qid];
  struct al_eth_pkt *hal_pkt = &rx_ring->hal_pkt;
  int work_done = 0;
  u16 next_to_clean = rx_ring->next_to_clean;
  int refill_required;
  int refill_actual;

  netdev_dbg(adapter->netdev, "%s qid %d\n", __func__, qid);

  do {
    struct sk_buff *skb;
    unsigned int descs;

    descs = al_eth_pkt_rx(rx_ring->dma_q, hal_pkt);
    if (unlikely(descs == 0))
      break;

    netdev_dbg(adapter->netdev, "rx_poll: q %d got packet from hal. descs %d\n",
               qid, descs);
    netdev_dbg(adapter->netdev,
               "rx_poll: q %d flags %x. l3 proto %d l4 proto %d\n", qid,
               hal_pkt->flags, hal_pkt->l3_proto_idx, hal_pkt->l4_proto_idx);

    /* ignore if detected dma or eth controller errors */
    if (hal_pkt->flags & (AL_ETH_RX_ERROR | AL_UDMA_CDESC_ERROR)) {
      netdev_dbg(adapter->netdev, "receive packet with error. flags = 0x%x\n",
                 hal_pkt->flags);
      next_to_clean = AL_ETH_RX_RING_IDX_ADD(rx_ring, next_to_clean, descs);
      goto next;
    }

    /* allocate skb and fill it */
    skb = al_eth_rx_skb(adapter, rx_ring, hal_pkt, descs, &next_to_clean);

    /* exit if we failed to retrieve a buffer */
    if (unlikely(!skb)) {
      next_to_clean = AL_ETH_RX_RING_IDX_ADD(rx_ring, next_to_clean, descs);
      break;
    }

    al_eth_rx_checksum(adapter, hal_pkt, skb);
    if (likely(adapter->netdev->features & NETIF_F_RXHASH)) {
      skb->hash = hal_pkt->rxhash;
      if (likely((hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_TCP) ||
                 (hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_UDP)))
        skb->l4_hash = 1;
    }

    skb_record_rx_queue(skb, qid);

#ifdef CONFIG_AL_ETH_ALLOC_SKB
    netif_receive_skb(skb);
#else
    if (hal_pkt->bufs[0].len <= adapter->small_copy_len)
      napi_gro_receive(napi, skb);
    else
      napi_gro_frags(napi);
#endif

  next:
    budget--;
    work_done++;
  } while (likely(budget));

  rx_ring->next_to_clean = next_to_clean;

  refill_required = al_udma_available_get(rx_ring->dma_q);
  refill_actual = al_eth_refill_rx_bufs(adapter, qid, refill_required);

  if (unlikely(refill_actual < refill_required)) {
    netdev_warn(adapter->netdev, "%s: rescheduling rx queue %d\n", __func__,
                qid);
    napi_schedule(napi);
  } else if (budget > 0) {
    dev_dbg(&adapter->pdev->dev, "rx_poll: q %d done next to clean %x\n", qid,
            next_to_clean);
    napi_complete(napi);
    al_reg_write32_relaxed(rx_ring->unmask_reg_offset, rx_ring->unmask_val);
  }

  return work_done;
}

/**
 * al_eth_intr_intx_all - Legacy Interrupt Handler for all interrupts
 * @irq: interrupt number
 * @data: pointer to a network interface device structure
 **/
static irqreturn_t al_eth_intr_intx_all(int irq, void *data) {
  struct al_eth_adapter *adapter = data;
  struct unit_regs __iomem *regs_base =
      (struct unit_regs __iomem *)adapter->udma_base;
  uint32_t reg;

  reg = al_udma_iofic_read_cause(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY,
                                 AL_INT_GROUP_A);
  if (likely(reg))
    pr_debug("%s group A cause %x\n", __func__, reg);

  if (unlikely(reg & AL_INT_GROUP_A_GROUP_D_SUM)) {
    struct al_iofic_grp_ctrl __iomem *sec_ints_base;
    uint32_t cause_d = al_udma_iofic_read_cause(
        regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_D);

    sec_ints_base = &regs_base->gen.interrupt_regs.secondary_iofic_ctrl[0];
    if (cause_d) {
      pr_debug("got interrupt from group D. cause %x\n", cause_d);

      cause_d = al_iofic_read_cause(sec_ints_base, AL_INT_GROUP_A);
      pr_debug("secondary A cause %x\n", cause_d);

      cause_d = al_iofic_read_cause(sec_ints_base, AL_INT_GROUP_B);

      pr_debug("secondary B cause %x\n", cause_d);
    }
  }
  if (reg & AL_INT_GROUP_A_GROUP_B_SUM) {
    uint32_t cause_b = al_udma_iofic_read_cause(
        regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_B);
    int qid;
    for (qid = 0; qid < adapter->num_rx_queues; qid++) {
      if (cause_b & (1 << qid)) {
        /* mask */
        al_udma_iofic_mask((struct unit_regs __iomem *)adapter->udma_base,
                           AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_B,
                           1 << qid);

        napi_schedule(
            &adapter->al_napi[AL_ETH_RXQ_NAPI_IDX(adapter, qid)].napi);
      }
    }
  }
  if (reg & AL_INT_GROUP_A_GROUP_C_SUM) {
    uint32_t cause_c = al_udma_iofic_read_cause(
        regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_C);
    int qid;
    for (qid = 0; qid < adapter->num_tx_queues; qid++) {
      if (cause_c & (1 << qid)) {
        /* mask */
        al_udma_iofic_mask((struct unit_regs __iomem *)adapter->udma_base,
                           AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_C,
                           1 << qid);

        napi_schedule(
            &adapter->al_napi[AL_ETH_TXQ_NAPI_IDX(adapter, qid)].napi);
      }
    }
  }

  return IRQ_HANDLED;
}

/**
 * al_eth_intr_msix_all - MSIX Interrupt Handler for all interrupts
 * @irq: interrupt number
 * @data: pointer to a network interface device structure
 **/
static irqreturn_t al_eth_intr_msix_all(int irq, void *data) {
  return IRQ_HANDLED;
}

/**
 * al_eth_intr_msix_mgmt - MSIX Interrupt Handler for Management interrupts
 * @irq: interrupt number
 * @data: pointer to a network interface device structure
 **/
static irqreturn_t al_eth_intr_msix_mgmt(int irq, void *data) {
  pr_info("got mgmt interrupt\n");
  return IRQ_HANDLED;
}

/**
 * al_eth_intr_msix_tx - MSIX Interrupt Handler for Tx
 * @irq: interrupt number
 * @data: pointer to a network interface private napi device structure
 **/
static irqreturn_t al_eth_intr_msix_tx(int irq, void *data) {
  struct al_eth_napi *al_napi = data;

  pr_debug("%s\n", __func__);
  napi_schedule(&al_napi->napi);

  return IRQ_HANDLED;
}

/**
 * al_eth_intr_msix_rx - MSIX Interrupt Handler for Rx
 * @irq: interrupt number
 * @data: pointer to a network interface private napi device structure
 **/
static irqreturn_t al_eth_intr_msix_rx(int irq, void *data) {
  struct al_eth_napi *al_napi = data;

  pr_debug("%s\n", __func__);
  napi_schedule(&al_napi->napi);
  return IRQ_HANDLED;
}

static void al_eth_enable_msix(struct al_eth_adapter *adapter) {
  int i, msix_vecs, rc;

  msix_vecs = 1 + adapter->num_rx_queues + adapter->num_tx_queues;

  dev_dbg(&adapter->pdev->dev, "Try to enable MSIX, vectors %d\n", msix_vecs);

  adapter->msix_entries =
      kcalloc(msix_vecs, sizeof(struct msix_entry), GFP_KERNEL);

  if (!adapter->msix_entries) {
    dev_err(&adapter->pdev->dev,
            "failed to allocate msix_entries, vectors %d\n", msix_vecs);

    return;
  }

  /* management vector (GROUP_A) @2*/
  adapter->msix_entries[AL_ETH_MGMT_IRQ_IDX].entry = 2;
  adapter->msix_entries[AL_ETH_MGMT_IRQ_IDX].vector = 0;

  /* rx queues start @3 */
  for (i = 0; i < adapter->num_rx_queues; i++) {
    int irq_idx = AL_ETH_RXQ_IRQ_IDX(adapter, i);

    adapter->msix_entries[irq_idx].entry = 3 + i;
    adapter->msix_entries[irq_idx].vector = 0;
  }
  /* tx queues start @7 */
  for (i = 0; i < adapter->num_tx_queues; i++) {
    int irq_idx = AL_ETH_TXQ_IRQ_IDX(adapter, i);

    adapter->msix_entries[irq_idx].entry = 3 + AL_ETH_MAX_HW_QUEUES + i;
    adapter->msix_entries[irq_idx].vector = 0;
  }

  rc = -ENOSPC;
  while (msix_vecs >= 1) {
    rc = pci_enable_msix_exact(adapter->pdev, adapter->msix_entries, msix_vecs);
    if (rc <= 0)
      break;
    if (rc > 0)
      msix_vecs = 1; /* if we can't allocate all, then try only 1; */
  }

  if (rc != 0) {
    dev_dbg(&adapter->pdev->dev, "failed to enable MSIX, vectors %d\n",
            msix_vecs);
    adapter->msix_vecs = 0;
    kfree(adapter->msix_entries);
    adapter->msix_entries = NULL;
    dev_dbg(&adapter->pdev->dev, "%s %d\n", __func__, __LINE__);

    return;
  }
  dev_dbg(&adapter->pdev->dev, "enable MSIX, vectors %d\n", msix_vecs);

  /* enable MSIX in the msix capability of the eth controller
   * as the pci_enable_msix_exact enables it in the pcie unit capability
   */
  if ((adapter->board_type == ALPINE_NIC) ||
      (adapter->board_type == ALPINE_FPGA_NIC))
    writew(PCI_MSIX_FLAGS_ENABLE, adapter->internal_pcie_base + 0x92);

  if (msix_vecs >= 1) {
    if (al_init_rx_cpu_rmap(adapter))
      dev_warn(&adapter->pdev->dev, "failed to map irqs to cpus\n");
  }

  adapter->msix_vecs = msix_vecs;
  adapter->flags |= AL_ETH_FLAG_MSIX_ENABLED;
}

static int al_eth_setup_int_mode(struct al_eth_adapter *adapter, int dis_msi) {
  int i;
  unsigned int cpu;

  if (!dis_msi)
    al_eth_enable_msix(adapter);

  adapter->irq_vecs = max(1, adapter->msix_vecs);

  /* single INTX mode */
  if (adapter->msix_vecs == 0) {
    snprintf(adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].name, AL_ETH_IRQNAME_SIZE,
             "al-eth-intx-all@pci:%s", pci_name(adapter->pdev));
    adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].handler = al_eth_intr_intx_all;
    adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].vector = adapter->pdev->irq;
    adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].data = adapter;
    cpu = cpumask_first(cpu_online_mask);
    cpumask_set_cpu(cpu,
                    &adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].affinity_hint_mask);

    return 0;
  }

  /* single MSI-X mode */
  if (adapter->msix_vecs == 1) {
    snprintf(adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].name, AL_ETH_IRQNAME_SIZE,
             "al-eth-msix-all@pci:%s", pci_name(adapter->pdev));
    adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].handler = al_eth_intr_msix_all;
    adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].vector =
        adapter->msix_entries[AL_ETH_MGMT_IRQ_IDX].vector;
    adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].data = adapter;

    cpu = cpumask_first(cpu_online_mask);
    cpumask_set_cpu(cpu,
                    &adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].affinity_hint_mask);

    return 0;
  }
  /* MSI-X per queue*/
  snprintf(adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].name, AL_ETH_IRQNAME_SIZE,
           "al-eth-msix-mgmt@pci:%s", pci_name(adapter->pdev));
  adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].handler = al_eth_intr_msix_mgmt;

  adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].data = adapter;
  adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].vector =
      adapter->msix_entries[AL_ETH_MGMT_IRQ_IDX].vector;
  cpu = cpumask_first(cpu_online_mask);
  cpumask_set_cpu(cpu,
                  &adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].affinity_hint_mask);

  for (i = 0; i < adapter->num_rx_queues; i++) {
    int irq_idx = AL_ETH_RXQ_IRQ_IDX(adapter, i);
    int napi_idx = AL_ETH_RXQ_NAPI_IDX(adapter, i);

    snprintf(adapter->irq_tbl[irq_idx].name, AL_ETH_IRQNAME_SIZE,
             "al-eth-rx-comp-%d@pci:%s", i, pci_name(adapter->pdev));
    adapter->irq_tbl[irq_idx].handler = al_eth_intr_msix_rx;
    adapter->irq_tbl[irq_idx].data = &adapter->al_napi[napi_idx];
    adapter->irq_tbl[irq_idx].vector = adapter->msix_entries[irq_idx].vector;

    cpu = cpumask_next((i % num_online_cpus() - 1), cpu_online_mask);
    cpumask_set_cpu(cpu, &adapter->irq_tbl[irq_idx].affinity_hint_mask);
  }

  for (i = 0; i < adapter->num_tx_queues; i++) {
    int irq_idx = AL_ETH_TXQ_IRQ_IDX(adapter, i);
    int napi_idx = AL_ETH_TXQ_NAPI_IDX(adapter, i);

    snprintf(adapter->irq_tbl[irq_idx].name, AL_ETH_IRQNAME_SIZE,
             "al-eth-tx-comp-%d@pci:%s", i, pci_name(adapter->pdev));
    adapter->irq_tbl[irq_idx].handler = al_eth_intr_msix_tx;
    adapter->irq_tbl[irq_idx].data = &adapter->al_napi[napi_idx];
    adapter->irq_tbl[irq_idx].vector = adapter->msix_entries[irq_idx].vector;

    cpu = cpumask_next((i % num_online_cpus() - 1), cpu_online_mask);
    cpumask_set_cpu(cpu, &adapter->irq_tbl[irq_idx].affinity_hint_mask);
  }

  return 0;
}

static int al_eth_configure_int_mode(struct al_eth_adapter *adapter) {
  enum al_iofic_mode int_mode;
  uint32_t m2s_errors_disable = 0x480;
  uint32_t m2s_aborts_disable = 0x480;
  uint32_t s2m_errors_disable = 0x1E0;
  uint32_t s2m_aborts_disable = 0x1E0;

  /* single INTX mode */
  if (adapter->msix_vecs == 0)
    int_mode = AL_IOFIC_MODE_LEGACY;
  else if (adapter->msix_vecs > 1)
    int_mode = AL_IOFIC_MODE_MSIX_PER_Q;
  else {
    netdev_err(adapter->netdev,
               "udma doesn't support single MSI-X mode yet.\n");
    return -EIO;
  }

  if (adapter->board_type != ALPINE_INTEGRATED) {
    m2s_errors_disable |= 0x3f << 25;
    m2s_errors_disable |= 0x3f << 25;
    s2m_aborts_disable |= 0x3f << 25;
    s2m_aborts_disable |= 0x3f << 25;
  }

  if (al_udma_iofic_config((struct unit_regs __iomem *)adapter->udma_base,
                           int_mode, m2s_errors_disable, m2s_aborts_disable,
                           s2m_errors_disable, s2m_aborts_disable)) {
    netdev_err(adapter->netdev, "al_udma_unit_int_config failed!.\n");
    return -EIO;
  }
  adapter->int_mode = int_mode;
  netdev_info(adapter->netdev, "using %s interrupt mode",
              int_mode == AL_IOFIC_MODE_LEGACY       ? "INTx"
              : int_mode == AL_IOFIC_MODE_MSIX_PER_Q ? "MSI-X per Queue"
                                                     : "Unknown");
  /* set interrupt moderation resolution to 15us */
  al_iofic_moder_res_config(&((struct unit_regs *)(adapter->udma_base))
                                 ->gen.interrupt_regs.main_iofic,
                            AL_INT_GROUP_B, 15);
  al_iofic_moder_res_config(&((struct unit_regs *)(adapter->udma_base))
                                 ->gen.interrupt_regs.main_iofic,
                            AL_INT_GROUP_C, 15);

  return 0;
}

static int al_eth_request_irq(struct al_eth_adapter *adapter) {
  unsigned long flags;
  struct al_eth_irq *irq;
  int rc = 0, i;

  if (adapter->flags & AL_ETH_FLAG_MSIX_ENABLED)
    flags = 0;
  else
    flags = IRQF_SHARED;

  for (i = 0; i < adapter->irq_vecs; i++) {
    irq = &adapter->irq_tbl[i];
    rc = request_irq(irq->vector, irq->handler, flags, irq->name, irq->data);
    if (rc) {
      netdev_err(adapter->netdev, "failed to request irq %d. index %d rc %d\n",
                 irq->vector, i, rc);
      break;
    }
    irq->requested = 1;

    netdev_dbg(adapter->netdev,
               "set affinity hint of irq. index %d"
               " to 0x%lx (irq vector: %d)\n",
               i, irq->affinity_hint_mask.bits[0], irq->vector);

    irq_set_affinity_hint(irq->vector, &irq->affinity_hint_mask);
  }
  return rc;
}

static void __al_eth_free_irq(struct al_eth_adapter *adapter) {
  struct al_eth_irq *irq;
  int i;

  for (i = 0; i < adapter->irq_vecs; i++) {
    irq = &adapter->irq_tbl[i];
    if (irq->requested) {
      irq_set_affinity_hint(irq->vector, NULL);
      free_irq(irq->vector, irq->data);
    }
    irq->requested = 0;
  }
}

static void al_eth_free_irq(struct al_eth_adapter *adapter) {
#ifdef CONFIG_RFS_ACCEL
  if (adapter->msix_vecs >= 1) {
    free_irq_cpu_rmap(adapter->netdev->rx_cpu_rmap);
    adapter->netdev->rx_cpu_rmap = NULL;
  }
#endif

  __al_eth_free_irq(adapter);
  if (adapter->flags & AL_ETH_FLAG_MSIX_ENABLED)
    pci_disable_msix(adapter->pdev);

  adapter->flags &= ~AL_ETH_FLAG_MSIX_ENABLED;

  kfree(adapter->msix_entries);
  adapter->msix_entries = NULL;
}

static void al_eth_interrupts_mask(struct al_eth_adapter *adapter);

static void al_eth_disable_int_sync(struct al_eth_adapter *adapter) {
  int i;

  if (!netif_running(adapter->netdev))
    return;

  /* disable forwarding interrupts from eth through pci end point*/
  if ((adapter->board_type == ALPINE_FPGA_NIC) ||
      (adapter->board_type == ALPINE_NIC))
    writel(0, adapter->internal_pcie_base + 0x1800000 + 0x1210);

  /* mask hw interrupts */
  al_eth_interrupts_mask(adapter);

  for (i = 0; i < adapter->irq_vecs; i++)
    synchronize_irq(adapter->irq_tbl[i].vector);
}

static void al_eth_interrupts_unmask(struct al_eth_adapter *adapter) {
  u32 group_a_mask = AL_INT_GROUP_A_GROUP_D_SUM; /* enable group D summery */
  u32 group_b_mask = (1 << adapter->num_rx_queues) - 1; /* bit per Rx q*/
  u32 group_c_mask = (1 << adapter->num_tx_queues) - 1; /* bit per Tx q*/
  u32 group_d_mask = 3 << 8;
  struct unit_regs __iomem *regs_base =
      (struct unit_regs __iomem *)adapter->udma_base;

  if (adapter->int_mode == AL_IOFIC_MODE_LEGACY)
    group_a_mask |= AL_INT_GROUP_A_GROUP_B_SUM | AL_INT_GROUP_A_GROUP_C_SUM |
                    AL_INT_GROUP_A_GROUP_D_SUM;

  al_udma_iofic_unmask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_A,
                       group_a_mask);
  al_udma_iofic_unmask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_B,
                       group_b_mask);
  al_udma_iofic_unmask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_C,
                       group_c_mask);
  al_udma_iofic_unmask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_D,
                       group_d_mask);
}

static void al_eth_interrupts_mask(struct al_eth_adapter *adapter) {
  struct unit_regs __iomem *regs_base =
      (struct unit_regs __iomem *)adapter->udma_base;

  /* mask all interrupts */
  al_udma_iofic_mask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_A,
                     0x7);
  al_udma_iofic_mask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_B,
                     0xF);
  al_udma_iofic_mask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_C,
                     0xF);
  al_udma_iofic_mask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_D,
                     0xFFFFFFFF);
}

static int al_init_rx_cpu_rmap(struct al_eth_adapter *adapter) {
#ifdef CONFIG_RFS_ACCEL
  unsigned int i;
  int rc;

  adapter->netdev->rx_cpu_rmap = alloc_irq_cpu_rmap(adapter->num_rx_queues);
  if (!adapter->netdev->rx_cpu_rmap)
    return -ENOMEM;
  for (i = 0; i < adapter->num_rx_queues; i++) {
    int irq_idx = AL_ETH_RXQ_IRQ_IDX(adapter, i);

    rc = irq_cpu_rmap_add(adapter->netdev->rx_cpu_rmap,
                          adapter->msix_entries[irq_idx].vector);
    if (rc) {
      free_irq_cpu_rmap(adapter->netdev->rx_cpu_rmap);
      adapter->netdev->rx_cpu_rmap = NULL;
      return rc;
    }
  }
#endif
  return 0;
}

static void al_eth_del_napi(struct al_eth_adapter *adapter) {
  int i;
  int napi_num = adapter->num_rx_queues + adapter->num_tx_queues;

  for (i = 0; i < napi_num; i++)
    netif_napi_del(&adapter->al_napi[i].napi);
}

static void al_eth_init_napi(struct al_eth_adapter *adapter) {
  int i;
  int napi_num = adapter->num_rx_queues + adapter->num_tx_queues;

  for (i = 0; i < napi_num; i++) {
    struct al_eth_napi *napi = &adapter->al_napi[i];
    int (*poll)(struct napi_struct *, int);

    if (i < adapter->num_rx_queues) {
      poll = al_eth_rx_poll;
      napi->qid = i;
      netif_napi_add(adapter->netdev, &adapter->al_napi[i].napi, poll);
    } else {
      poll = al_eth_tx_poll;
      napi->qid = i - adapter->num_rx_queues;
      netif_napi_add(adapter->netdev, &adapter->al_napi[i].napi, poll);
    }
    napi->adapter = adapter;
  }
}

static void al_eth_napi_disable_all(struct al_eth_adapter *adapter) {
  int i;
  int napi_num = adapter->num_rx_queues + adapter->num_tx_queues;

  for (i = 0; i < napi_num; i++)
    napi_disable(&adapter->al_napi[i].napi);
}

static void al_eth_napi_enable_all(struct al_eth_adapter *adapter)

{
  int i;
  int napi_num = adapter->num_rx_queues + adapter->num_tx_queues;

  for (i = 0; i < napi_num; i++)
    napi_enable(&adapter->al_napi[i].napi);
}

/* init FSM, no tunneling supported yet, if packet is tcp/udp over ipv4/ipv6,
 * use 4 tuple hash */
static void al_eth_fsm_table_init(struct al_eth_adapter *adapter) {
  uint32_t val;
  int i;

  for (i = 0; i < AL_ETH_RX_FSM_TABLE_SIZE; i++) {
    uint8_t outer_type = AL_ETH_FSM_ENTRY_OUTER(i);
    switch (outer_type) {
    case AL_ETH_FSM_ENTRY_IPV4_TCP:
    case AL_ETH_FSM_ENTRY_IPV4_UDP:
    case AL_ETH_FSM_ENTRY_IPV6_TCP:
    case AL_ETH_FSM_ENTRY_IPV6_UDP:
      val = AL_ETH_FSM_DATA_OUTER_4_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
      break;
    case AL_ETH_FSM_ENTRY_IPV6_NO_UDP_TCP:
    case AL_ETH_FSM_ENTRY_IPV4_NO_UDP_TCP:
      val = AL_ETH_FSM_DATA_OUTER_2_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
      break;
    case AL_ETH_FSM_ENTRY_NOT_IP:
      if (AL_ETH_FSM_ENTRY_TUNNELED(i) && !AL_ETH_FSM_ENTRY_INNER(i)) /*PPPoE*/
        val = AL_ETH_FSM_DATA_INNER_4_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
      else
        val = (0 << AL_ETH_FSM_DATA_DEFAULT_Q_SHIFT |
               ((1 << 0) << AL_ETH_FSM_DATA_DEFAULT_UDMA_SHIFT));
      break;
    default:
      val = (0 << AL_ETH_FSM_DATA_DEFAULT_Q_SHIFT |
             ((1 << 0) << AL_ETH_FSM_DATA_DEFAULT_UDMA_SHIFT));
    }
    al_eth_fsm_table_set(&adapter->hal_adapter, i, val);
  }
}

/* the following defines should be moved to hal */
#define AL_ETH_CTRL_TABLE_PRIO_SEL_SHIFT 0
#define AL_ETH_CTRL_TABLE_PRIO_SEL_MASK                                        \
  (0xF << AL_ETH_CTRL_TABLE_PRIO_SEL_SHIFT)
#define AL_ETH_CTRL_TABLE_PRIO_SEL_0 (12 << AL_ETH_CTRL_TABLE_PRIO_SEL_SHIFT)

#define AL_ETH_CTRL_TABLE_Q_SEL_SHIFT 4
#define AL_ETH_CTRL_TABLE_Q_SEL_MASK (0xF << AL_ETH_CTRL_TABLE_Q_SEL_SHIFT)
#define AL_ETH_CTRL_TABLE_Q_SEL_THASH (1 << AL_ETH_CTRL_TABLE_Q_SEL_SHIFT)

#define AL_ETH_CTRL_TABLE_Q_PRIO_SEL_SHIFT 8
#define AL_ETH_CTRL_TABLE_Q_PRIO_SEL_MASK                                      \
  (0x3 << AL_ETH_CTRL_TABLE_Q_PRIO_SEL_SHIFT)
/* selected queue is hash output table */
#define AL_ETH_CTRL_TABLE_Q_PRIO_SEL_Q (3 << AL_ETH_CTRL_TABLE_Q_PRIO_SEL_SHIFT)

#define AL_ETH_CTRL_TABLE_UDMA_SEL_SHIFT 10
#define AL_ETH_CTRL_TABLE_UDMA_SEL_MASK                                        \
  (0xF << AL_ETH_CTRL_TABLE_UDMA_SEL_SHIFT)
/* select UDMA from rfw_default opt1 register */
#define AL_ETH_CTRL_TABLE_UDMA_SEL_DEF_1 (7 << AL_ETH_CTRL_TABLE_UDMA_SEL_SHIFT)
#define AL_ETH_CTRL_TABLE_UDMA_SEL_0 (15 << AL_ETH_CTRL_TABLE_UDMA_SEL_SHIFT)

#define AL_ETH_CTRL_TABLE_UDMA_SEL_MASK_INPUT (1 << 14)

#define AL_ETH_CTRL_TABLE_USE_TABLE (1 << 20)

#define AL_ETH_MAC_TABLE_UNICAST_IDX_BASE 0
#define AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT 4
#define AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX                                     \
  (AL_ETH_MAC_TABLE_UNICAST_IDX_BASE + AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT)

#define AL_ETH_MAC_TABLE_DROP_IDX (AL_ETH_FWD_MAC_NUM - 1)
#define AL_ETH_MAC_TABLE_BROADCAST_IDX (AL_ETH_MAC_TABLE_DROP_IDX - 1)

#define MAC_ADDR_STR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ADDR(addr) addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

static void al_eth_mac_table_unicast_add(struct al_eth_adapter *adapter,
                                         uint8_t idx, uint8_t *addr,
                                         uint8_t udma_mask) {
  struct al_eth_fwd_mac_table_entry entry = {{0}};

  memcpy(entry.addr, addr, sizeof(entry.addr));

  memset(entry.mask, 0xff, sizeof(entry.mask));
  entry.rx_valid = true;
  entry.tx_valid = false;
  entry.udma_mask = udma_mask;
  entry.filter = false;

  netdev_dbg(adapter->netdev,
             "%s: [%d]: addr " MAC_ADDR_STR " mask " MAC_ADDR_STR "\n",
             __func__, idx, MAC_ADDR(entry.addr), MAC_ADDR(entry.mask));

  al_eth_fwd_mac_table_set(&adapter->hal_adapter, idx, &entry);
}

static void al_eth_mac_table_all_multicast_add(struct al_eth_adapter *adapter,
                                               uint8_t idx, uint8_t udma_mask) {
  struct al_eth_fwd_mac_table_entry entry = {{0}};

  memset(entry.addr, 0x00, sizeof(entry.addr));
  memset(entry.mask, 0x00, sizeof(entry.mask));
  entry.mask[0] |= BIT(0);
  entry.addr[0] |= BIT(0);

  entry.rx_valid = true;
  entry.tx_valid = false;
  entry.udma_mask = udma_mask;
  entry.filter = false;

  netdev_dbg(adapter->netdev,
             "%s: [%d]: addr " MAC_ADDR_STR " mask " MAC_ADDR_STR "\n",
             __func__, idx, MAC_ADDR(entry.addr), MAC_ADDR(entry.mask));

  al_eth_fwd_mac_table_set(&adapter->hal_adapter, idx, &entry);
}

static void al_eth_mac_table_broadcast_add(struct al_eth_adapter *adapter,
                                           uint8_t idx, uint8_t udma_mask) {
  struct al_eth_fwd_mac_table_entry entry = {{0}};

  memset(entry.addr, 0xff, sizeof(entry.addr));
  memset(entry.mask, 0xff, sizeof(entry.mask));

  entry.rx_valid = true;
  entry.tx_valid = false;
  entry.udma_mask = udma_mask;
  entry.filter = false;

  netdev_dbg(adapter->netdev,
             "%s: [%d]: addr " MAC_ADDR_STR " mask " MAC_ADDR_STR "\n",
             __func__, idx, MAC_ADDR(entry.addr), MAC_ADDR(entry.mask));

  al_eth_fwd_mac_table_set(&adapter->hal_adapter, idx, &entry);
}

static void al_eth_mac_table_promiscuous_set(struct al_eth_adapter *adapter,
                                             al_bool promiscuous) {
  struct al_eth_fwd_mac_table_entry entry = {{0}};

  memset(entry.addr, 0x00, sizeof(entry.addr));
  memset(entry.mask, 0x00, sizeof(entry.mask));

  entry.rx_valid = true;
  entry.tx_valid = false;
  entry.udma_mask = (promiscuous) ? 1 : 0;
  entry.filter = (promiscuous) ? false : true;

  netdev_dbg(adapter->netdev, "%s: %s promiscuous mode\n", __func__,
             (promiscuous) ? "enter" : "exit");

  al_eth_fwd_mac_table_set(&adapter->hal_adapter, AL_ETH_MAC_TABLE_DROP_IDX,
                           &entry);
}

static void al_eth_mac_table_entry_clear(struct al_eth_adapter *adapter,
                                         uint8_t idx) {
  struct al_eth_fwd_mac_table_entry entry = {{0}};

  netdev_dbg(adapter->netdev, "%s: clear entry %d\n", __func__, idx);

  al_eth_fwd_mac_table_set(&adapter->hal_adapter, idx, &entry);
}

/* configure the RX forwarding (UDMA/QUEUE.. selection)
 * currently we don't use the full control table, we use only the default
 * configuration
 */

static void al_eth_config_rx_fwd(struct al_eth_adapter *adapter) {
  struct al_eth_fwd_ctrl_table_entry entry;
  int i;

  /* let priority be equal to pbits */
  for (i = 0; i < AL_ETH_FWD_PBITS_TABLE_NUM; i++)
    al_eth_fwd_pbits_table_set(&adapter->hal_adapter, i, i);

  /* map priority to queue index, queue id = priority/2 */
  for (i = 0; i < AL_ETH_FWD_PRIO_TABLE_NUM; i++)
    al_eth_fwd_priority_table_set(&adapter->hal_adapter, i, i >> 1);

  entry.prio_sel = AL_ETH_CTRL_TABLE_PRIO_SEL_VAL_0;
  entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_THASH_TABLE;
  entry.queue_sel_2 = AL_ETH_CTRL_TABLE_QUEUE_SEL_2_NO_PRIO;
  entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_MAC_TABLE;
  entry.filter = AL_FALSE;

  al_eth_ctrl_table_def_set(&adapter->hal_adapter, AL_FALSE, &entry);

  /*
   * By default set the mac table to forward all unicast packets to our
   * MAC address and all broadcast. all the rest will be dropped.
   */
  al_eth_mac_table_unicast_add(adapter, AL_ETH_MAC_TABLE_UNICAST_IDX_BASE,
                               adapter->mac_addr, 1);
  al_eth_mac_table_broadcast_add(adapter, AL_ETH_MAC_TABLE_BROADCAST_IDX, 1);
  al_eth_mac_table_promiscuous_set(adapter, false);

  /* set toeplitz hash keys */
  get_random_bytes(adapter->toeplitz_hash_key,
                   sizeof(adapter->toeplitz_hash_key));

  for (i = 0; i < AL_ETH_RX_HASH_KEY_NUM; i++)
    al_eth_hash_key_set(&adapter->hal_adapter, i,
                        htonl(adapter->toeplitz_hash_key[i]));

  for (i = 0; i < AL_ETH_RX_RSS_TABLE_SIZE; i++)
    al_eth_thash_table_set(&adapter->hal_adapter, i, 0,
                           adapter->rss_ind_tbl[i]);

  al_eth_fsm_table_init(adapter);
}

static void al_eth_set_coalesce(struct al_eth_adapter *adapter,
                                unsigned int tx_usecs, unsigned int rx_usecs);

static void al_eth_restore_ethtool_params(struct al_eth_adapter *adapter) {
  int i;
  unsigned int tx_usecs = adapter->tx_usecs;
  unsigned int rx_usecs = adapter->rx_usecs;

  adapter->tx_usecs = 0;
  adapter->rx_usecs = 0;

  al_eth_set_coalesce(adapter, tx_usecs, rx_usecs);

  for (i = 0; i < AL_ETH_RX_RSS_TABLE_SIZE; i++)
    al_eth_thash_table_set(&adapter->hal_adapter, i, 0,
                           adapter->rss_ind_tbl[i]);
}

static void al_eth_up_complete(struct al_eth_adapter *adapter) {
  al_eth_configure_int_mode(adapter);

  /*config rx fwd*/
  al_eth_config_rx_fwd(adapter);

  al_eth_init_napi(adapter);
  al_eth_napi_enable_all(adapter);

  al_eth_change_mtu(adapter->netdev, adapter->netdev->mtu);
  /* enable hw queues */
  al_eth_udma_queues_enable_all(adapter);

  al_eth_refill_all_rx_bufs(adapter);

  al_eth_interrupts_unmask(adapter);

  /* enable forwarding interrupts from eth through pci end point*/
  if ((adapter->board_type == ALPINE_FPGA_NIC) ||
      (adapter->board_type == ALPINE_NIC))
    writel(0x1FFFF, adapter->internal_pcie_base + 0x1800000 + 0x1210);

  /* enable transmits */
  netif_tx_start_all_queues(adapter->netdev);

  /* enable flow control */
  al_eth_flow_ctrl_enable(adapter);

  al_eth_restore_ethtool_params(adapter);

  /* enable the mac tx and rx paths */
  al_eth_mac_start(&adapter->hal_adapter);
}

static int al_eth_up(struct al_eth_adapter *adapter) {
  int rc;

  netdev_info(adapter->netdev, "%s\n", __func__);

  rc = al_eth_hw_init(adapter);
  if (rc)
    goto err_hw_init_open;

  rc = al_eth_setup_int_mode(adapter, disable_msi);
  if (rc) {
    dev_err(&adapter->pdev->dev, "%s failed at setup interrupt mode!\n",
            __func__);
    goto err_setup_int;
  }

  /* allocate transmit descriptors */
  rc = al_eth_setup_all_tx_resources(adapter);
  if (rc)
    goto err_setup_tx;

  /* allocate receive descriptors */
  rc = al_eth_setup_all_rx_resources(adapter);
  if (rc)
    goto err_setup_rx;

  rc = al_eth_request_irq(adapter);
  if (rc)
    goto err_req_irq;

  al_eth_up_complete(adapter);

  adapter->up = true;

  return rc;

err_req_irq:
  al_eth_free_all_rx_resources(adapter);
err_setup_rx:
  al_eth_free_all_tx_resources(adapter);
err_setup_tx:
  al_eth_free_irq(adapter);
err_setup_int:
  al_eth_hw_stop(adapter);
err_hw_init_open:
  al_eth_function_reset(adapter);

  return rc;
}

#ifdef CONFIG_RFS_ACCEL
static int al_eth_flow_steer(struct net_device *netdev,
                             const struct sk_buff *skb, u16 rxq_index,
                             u32 flow_id) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  int rc = 0;

  if ((skb->protocol != htons(ETH_P_IP)) &&
      (skb->protocol != htons(ETH_P_IPV6)))
    return -EPROTONOSUPPORT;

  if (skb->protocol == htons(ETH_P_IP)) {
    if (ip_is_fragment(ip_hdr(skb)))
      return -EPROTONOSUPPORT;
    if ((ip_hdr(skb)->protocol != IPPROTO_TCP) &&
        (ip_hdr(skb)->protocol != IPPROTO_UDP))
      return -EPROTONOSUPPORT;
  }

  if (skb->protocol == htons(ETH_P_IPV6)) {
    /* ipv6 with extension not supported yet */
    if ((ipv6_hdr(skb)->nexthdr != IPPROTO_TCP) &&
        (ipv6_hdr(skb)->nexthdr != IPPROTO_UDP))
      return -EPROTONOSUPPORT;
  }
  rc = flow_id & (AL_ETH_RX_THASH_TABLE_SIZE - 1);

  adapter->rss_ind_tbl[rc] = rxq_index;
  al_eth_thash_table_set(&adapter->hal_adapter, rc, 0, rxq_index);
  if (skb->protocol == htons(ETH_P_IP)) {
    int nhoff = skb_network_offset(skb);
    const struct iphdr *ip = (const struct iphdr *)(skb->data + nhoff);
    const __be16 *ports = (const __be16 *)(skb->data + nhoff + 4 * ip->ihl);

    netdev_info(adapter->netdev,
                "steering %s %pI4:%u:%pI4:%u to queue %u [flow %u filter %d]\n",
                (ip->protocol == IPPROTO_TCP) ? "TCP" : "UDP", &ip->saddr,
                ntohs(ports[0]), &ip->daddr, ntohs(ports[1]), rxq_index,
                flow_id, rc);
  } else {
    struct ipv6hdr *ip6h = ipv6_hdr(skb);
    const __be16 *ports = (const __be16 *)skb_transport_header(skb);

    netdev_info(
        adapter->netdev,
        "steering %s %pI6c:%u:%pI6c:%u to queue %u [flow %u filter %d]\n",
        (ipv6_hdr(skb)->nexthdr == IPPROTO_TCP) ? "TCP" : "UDP", &ip6h->saddr,
        ntohs(ports[0]), &ip6h->daddr, ntohs(ports[1]), rxq_index, flow_id, rc);
  }

  return rc;
}
#endif

static int al_set_features(struct net_device *dev, netdev_features_t features) {
#if defined(NETIF_F_MQ_TX_LOCK_OPT)
  if (((features ^ dev->features) & NETIF_F_MQ_TX_LOCK_OPT) &&
      netif_running(dev)) {
    netdev_warn(dev,
                "Can't toggle NETIF_F_MQ_TX_LOCK_OPT : device is running!  \n");
    return -EINVAL;
  }
#endif
  return 0;
}

/************************ Link management ************************/
#define SFP_I2C_ADDR 0x50

static int al_eth_i2c_byte_read(void *context, uint8_t bus_id, uint8_t i2c_addr,
                                uint8_t reg_addr, uint8_t *val) {
  struct i2c_adapter *i2c_adapter;
  struct al_eth_adapter *adapter = context;

  struct i2c_msg msgs[] = {{
                               .addr = i2c_addr,
                               .flags = 0,
                               .len = 1,
                               .buf = &reg_addr,
                           },
                           {
                               .addr = i2c_addr,
                               .flags = I2C_M_RD,
                               .len = 1,
                               .buf = val,
                           }};

  i2c_adapter = i2c_get_adapter(bus_id);

  if (i2c_adapter == NULL) {
    netdev_err(adapter->netdev,
               "Failed to get i2c adapter. "
               "probably caused by wrong i2c bus id in the device tree, "
               "wrong i2c mux implementation, or the port is configured "
               "wrongly as SFP+\n");
    return -EINVAL;
  }

  if (i2c_transfer(i2c_adapter, msgs, 2) != 2) {
    netdev_dbg(adapter->netdev, "Failed to read sfp+ parameters\n");
    return -ETIMEDOUT;
  }

  return 0;
}

static int al_eth_i2c_byte_write(void *context, uint8_t bus_id,
                                 uint8_t i2c_addr, uint8_t reg_addr,
                                 uint8_t val) {
  struct i2c_adapter *i2c_adapter;
  struct al_eth_adapter *adapter = context;

  struct i2c_msg msgs[] = {{
                               .addr = i2c_addr,
                               .flags = 0,
                               .len = 1,
                               .buf = &reg_addr,
                           },
                           {
                               .addr = i2c_addr,
                               .flags = 0,
                               .len = 1,
                               .buf = &val,
                           }};

  i2c_adapter = i2c_get_adapter(bus_id);

  if (i2c_adapter == NULL) {
    netdev_err(adapter->netdev,
               "Failed to get i2c adapter. "
               "probably caused by wrong i2c bus id in the device tree, "
               "wrong i2c mux implementation, or the port is configured "
               "wrongly as SFP+\n");
    return -EINVAL;
  }

  if (i2c_transfer(i2c_adapter, msgs, 2) != 2) {
    netdev_dbg(adapter->netdev, "Failed to read sfp+ parameters\n");
    return -ETIMEDOUT;
  }

  return 0;
}

static uint8_t al_eth_get_rand_byte(void) {
  uint8_t byte;
  get_random_bytes(&byte, 1);
  return byte;
}

static void al_eth_serdes_mode_set(struct al_eth_adapter *adapter) {
#ifdef CONFIG_ARCH_ALPINE
  enum alpine_serdes_eth_mode mode =
      (adapter->mac_mode == AL_ETH_MAC_MODE_SGMII)
          ? ALPINE_SERDES_ETH_MODE_SGMII
          : ALPINE_SERDES_ETH_MODE_KR;

  if ((adapter->mac_mode != AL_ETH_MAC_MODE_SGMII) &&
      (adapter->mac_mode != AL_ETH_MAC_MODE_10GbE_Serial)) {
    netdev_err(adapter->netdev, "%s: mac_mode not supported\n", __func__);
    return;
  }

  if (alpine_serdes_eth_mode_set(adapter->serdes_grp, mode))
    netdev_err(adapter->netdev,
               "%s: alpine_serdes_eth_mode_set(%d, %d) failed!\n", __func__,
               adapter->serdes_grp, mode);

  al_udelay(1000);
#endif
}

static void al_eth_lm_mode_apply(struct al_eth_adapter *adapter,
                                 enum al_eth_lm_link_mode new_mode) {
  enum al_eth_mac_mode last_mac_mode = adapter->mac_mode;

  if (new_mode == AL_ETH_LM_MODE_DISCONNECTED)
    return;

  if (new_mode == AL_ETH_LM_MODE_1G) {
    adapter->mac_mode = AL_ETH_MAC_MODE_SGMII;
    adapter->link_config.active_speed = SPEED_1000;
  } else {
    adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;
    adapter->link_config.active_speed = SPEED_10000;
  }

  adapter->link_config.active_duplex = DUPLEX_FULL;

  if ((adapter->auto_speed) && (last_mac_mode != adapter->mac_mode))
    al_eth_serdes_mode_set(adapter);
}

static void al_eth_serdes_init(struct al_eth_adapter *adapter) {
#ifdef CONFIG_ARCH_ALPINE
  void __iomem *serdes_base;

  adapter->serdes_init = false;

  /*
   * always call with group A to get the base address of
   * all groups.
   */
  serdes_base = alpine_serdes_resource_get(AL_SRDS_GRP_A);

  if (!serdes_base) {
    netdev_err(adapter->netdev, "serdes_base get failed!\n");
    return;
  }

  al_serdes_handle_init(serdes_base, &adapter->serdes_obj);

  adapter->serdes_init = true;
#endif
}

static void al_eth_down(struct al_eth_adapter *adapter) {
  netdev_info(adapter->netdev, "%s\n", __func__);

  BUG_ON(!adapter->up);

  adapter->up = false;

  netif_carrier_off(adapter->netdev);
  al_eth_disable_int_sync(adapter);
  al_eth_napi_disable_all(adapter);
  netif_tx_disable(adapter->netdev);
  al_eth_free_irq(adapter);
  al_eth_hw_stop(adapter);
  al_eth_del_napi(adapter);

  al_eth_free_all_tx_bufs(adapter);
  al_eth_free_all_rx_bufs(adapter);
  al_eth_free_all_tx_resources(adapter);
  al_eth_free_all_rx_resources(adapter);
}

static void al_eth_link_status_task(struct work_struct *work) {
  struct al_eth_adapter *adapter = container_of(
      to_delayed_work(work), struct al_eth_adapter, link_status_task);
  enum al_eth_lm_link_mode old_mode = AL_ETH_LM_MODE_DISCONNECTED;
  enum al_eth_lm_link_mode new_mode = AL_ETH_LM_MODE_DISCONNECTED;
  al_bool fault = false;
  al_bool link_up = false;
  int rc;

  rc = al_eth_lm_link_detection(&adapter->lm_context, &fault, &old_mode,
                                &new_mode);
  if (rc)
    goto reschedule;

  /* The link is still up. continue */
  if (fault == false)
    goto reschedule;

  if (new_mode == old_mode) {
    if (new_mode == AL_ETH_LM_MODE_DISCONNECTED)
      goto reschedule;
  } else {
    if (old_mode != AL_ETH_LM_MODE_DISCONNECTED) {
      netdev_info(adapter->netdev, "%s link down\n", __func__);
      adapter->last_link = false;
      al_eth_down(adapter);
    }

    al_eth_lm_mode_apply(adapter, new_mode);

    if (new_mode != AL_ETH_LM_MODE_DISCONNECTED) {
      if (al_eth_up(adapter))
        goto reschedule;
    } else {
      goto reschedule;
    }
  }

#ifdef CONFIG_ARCH_ALPINE
  alpine_serdes_eth_group_lock(adapter->serdes_grp);
#endif
  rc = al_eth_lm_link_establish(&adapter->lm_context, &link_up);
#ifdef CONFIG_ARCH_ALPINE
  alpine_serdes_eth_group_unlock(adapter->serdes_grp);
#endif

  if ((rc == 0) && (link_up == true)) {
    netdev_info(adapter->netdev, "%s link up\n", __func__);
    adapter->last_establish_failed = false;

    netif_carrier_on(adapter->netdev);
  } else {
    if (adapter->last_link != link_up) {
      netdev_info(adapter->netdev, "%s link down\n", __func__);
      adapter->last_establish_failed = false;
    } else if ((rc != 0) && (!adapter->last_establish_failed)) {
      netdev_info(adapter->netdev, "%s failed to establish link\n", __func__);
      adapter->last_establish_failed = true;
    }

    if (adapter->last_link == true)
      netif_carrier_off(adapter->netdev);
  }

  adapter->last_link = link_up;

reschedule:
  /* setting link status delay to 0 (through sysfs) will stop the task */
  if (adapter->link_poll_interval != 0) {
    unsigned long delay;

    delay = msecs_to_jiffies(adapter->link_poll_interval);

    schedule_delayed_work(&adapter->link_status_task, delay);
  }
}

static void al_eth_lm_config(struct al_eth_adapter *adapter) {
  struct al_eth_lm_init_params params;

  params.adapter = &adapter->hal_adapter;
  params.serdes_obj = &adapter->serdes_obj;
  params.grp = adapter->serdes_grp;
  params.lane = adapter->serdes_lane;
  params.sfp_detection = adapter->sfp_detection_needed;
  if (adapter->sfp_detection_needed) {
    params.sfp_bus_id = adapter->i2c_adapter_id;
    params.sfp_i2c_addr = SFP_I2C_ADDR;
  }

  if (adapter->sfp_detection_needed == false) {
    switch (adapter->mac_mode) {
    case AL_ETH_MAC_MODE_10GbE_Serial:
      if (adapter->lt_en && adapter->an_en)
        params.default_mode = AL_ETH_LM_MODE_10G_DA;
      else
        params.default_mode = AL_ETH_LM_MODE_10G_OPTIC;
      break;
    case AL_ETH_MAC_MODE_SGMII:
    case AL_ETH_MAC_MODE_SGMII_2_5G:
      params.default_mode = AL_ETH_LM_MODE_1G;
      break;
    default:
      netdev_err(adapter->netdev, "mac mode not supported!\n");
      params.default_mode = AL_ETH_LM_MODE_10G_DA;
    }
  } else {
    params.default_mode = AL_ETH_LM_MODE_10G_DA;
  }

  params.link_training = adapter->lt_en;
  params.rx_equal = true;
  params.static_values = !adapter->dont_override_serdes;
  params.i2c_read = &al_eth_i2c_byte_read;
  params.i2c_write = &al_eth_i2c_byte_write;
  params.i2c_context = adapter;
  params.get_random_byte = &al_eth_get_rand_byte;
  params.kr_fec_enable = false;

  params.retimer_exist = adapter->retimer.exist;
  params.retimer_bus_id = adapter->retimer.bus_id;
  params.retimer_i2c_addr = adapter->retimer.i2c_addr;
  params.retimer_channel = adapter->retimer.channel;

  al_eth_lm_init(&adapter->lm_context, &params);
}

#define AQUANTIA_AQR105_ID 0x3a1b4a2

static int al_eth_aq_phy_fixup(struct phy_device *phydev) {
  int temp = 0;

  temp = phy_read(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0x20));
  temp &= ~(1 << 12);

  phy_write(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0x20), temp);

  temp = phy_read(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0xc400));
  temp |= ((1 << 15) | (1 << 11) | (1 << 10));
  phy_write(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0xc400), temp);

  temp = phy_read(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0));
  temp |= (1 << 9);
  temp &= ~(1 << 15);

  phy_write(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0), temp);

  return 0;
}

/**
 * al_eth_open - Called when a network interface is made active
 * @netdev: network interface device structure
 *
 * Returns 0 on success, negative value on failure
 *
 * The open entry point is called when a network interface is made
 * active by the system (IFF_UP).  At this point all resources needed
 * for transmit and receive operations are allocated, the interrupt
 * handler is registered with the OS, the watchdog timer is started,
 * and the stack is notified that the interface is ready.
 **/
static int al_eth_open(struct net_device *netdev) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  int rc;
  enum al_eth_lm_link_mode old_mode;
  enum al_eth_lm_link_mode new_mode;

  netdev_info(adapter->netdev, "%s\n", __func__);

  netif_carrier_off(netdev);

  /* Notify the stack of the actual queue counts. */
  rc = netif_set_real_num_tx_queues(netdev, adapter->num_tx_queues);
  if (rc)
    return rc;

  rc = netif_set_real_num_rx_queues(netdev, adapter->num_rx_queues);
  if (rc)
    return rc;

  al_eth_serdes_init(adapter);

  adapter->last_establish_failed = false;

  if (adapter->phy_exist == false) {
    netdev_info(adapter->netdev, "%s: no phy, use_lm %d\n", __func__,
                adapter->use_lm);

    if (adapter->use_lm) {
      al_eth_lm_config(adapter);

      rc = al_eth_lm_link_detection(&adapter->lm_context, NULL, &old_mode,
                                    &new_mode);
      if (rc)
        return rc;

      al_eth_lm_mode_apply(adapter, new_mode);

      if (new_mode != AL_ETH_LM_MODE_DISCONNECTED) {
        rc = al_eth_up(adapter);
        if (rc)
          return rc;
      }
    } else {
      rc = al_eth_up(adapter);
      if (rc)
        return rc;
    }
  } else {
    rc = al_eth_up(adapter);
    if (rc)
      return rc;

    if (adapter->phy_fixup_needed) {
      rc = phy_register_fixup_for_uid(AQUANTIA_AQR105_ID, 0xffffffff,
                                      al_eth_aq_phy_fixup);
      if (rc)
        netdev_warn(adapter->netdev, "failed to register PHY fixup\n");
    }

    rc = al_eth_mdiobus_setup(adapter);
    printk("al eth midobus setup 0x%x\n", rc);
    if (rc) {
      netdev_err(netdev, "failed at mdiobus setup!\n");
      goto err_mdiobus_setup;
    }
  }

  if (adapter->mdio_bus) {
    rc = al_eth_phy_init(adapter);
  } else {
    if ((adapter->board_type == ALPINE_INTEGRATED) && (adapter->use_lm)) {
      unsigned long delay;

      delay = msecs_to_jiffies(AL_ETH_FIRST_LINK_POLL_INTERVAL);

      INIT_DELAYED_WORK(&adapter->link_status_task, al_eth_link_status_task);
      schedule_delayed_work(&adapter->link_status_task, delay);
    } else {
      netif_carrier_on(adapter->netdev);
    }
  }

  return rc;

err_mdiobus_setup:
  al_eth_down(adapter);

  return rc;
}

/**
 * al_eth_close - Disables a network interface
 * @netdev: network interface device structure
 *
 * Returns 0, this is not allowed to fail
 *
 * The close entry point is called when an interface is de-activated
 * by the OS.  The hardware is still under the drivers control, but
 * needs to be disabled.  A global MAC reset is issued to stop the
 * hardware, and all transmit and receive resources are freed.
 */
static int al_eth_close(struct net_device *netdev) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);

  netdev_dbg(adapter->netdev, "%s\n", __func__);

  if ((adapter->board_type == ALPINE_INTEGRATED) && (adapter->use_lm)) {
    cancel_delayed_work_sync(&adapter->link_status_task);
  }

  if (adapter->phydev) {
    phy_stop(adapter->phydev);
    phy_disconnect(adapter->phydev);
    al_eth_mdiobus_teardown(adapter);
  }

  if (adapter->up)
    al_eth_down(adapter);

  /*al_eth_release_hw_control(adapter);*/

  return 0;
}

static int al_eth_get_link_ksettings(struct net_device *netdev,
                                     struct ethtool_link_ksettings *ecmd) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct phy_device *phydev = adapter->phydev;

  if (phydev) {
    phy_ethtool_ksettings_get(phydev, ecmd);
    return 0;
  }

  ecmd->base.speed = adapter->link_config.active_speed;
  ecmd->base.duplex = adapter->link_config.active_duplex;
  ecmd->base.autoneg = adapter->link_config.autoneg;

  return 0;
}

static int
al_eth_set_link_ksettings(struct net_device *netdev,
                          const struct ethtool_link_ksettings *ecmd) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct phy_device *phydev = adapter->phydev;
  int rc = 0;

  if (phydev)
    return phy_ethtool_ksettings_set(phydev, ecmd);

  /* in case no phy exist set only mac parameters */
  adapter->link_config.active_speed = ecmd->base.speed;
  adapter->link_config.active_duplex = ecmd->base.duplex;
  adapter->link_config.autoneg = ecmd->base.autoneg;

  if (adapter->up)
    dev_warn(&adapter->pdev->dev,
             "%s this action will take place in the next activation (up)\n",
             __func__);

  return rc;
}

static int al_eth_get_coalesce(struct net_device *net_dev,
                               struct ethtool_coalesce *coalesce,
                               struct kernel_ethtool_coalesce *,
                               struct netlink_ext_ack *) {
  struct al_eth_adapter *adapter = netdev_priv(net_dev);

  coalesce->tx_coalesce_usecs = adapter->tx_usecs;
  coalesce->tx_coalesce_usecs_irq = adapter->tx_usecs;
  coalesce->rx_coalesce_usecs = adapter->rx_usecs;
  coalesce->rx_coalesce_usecs_irq = adapter->rx_usecs;
  coalesce->use_adaptive_rx_coalesce = false;

  return 0;
}

static void al_eth_set_coalesce(struct al_eth_adapter *adapter,
                                unsigned int tx_usecs, unsigned int rx_usecs) {
  struct unit_regs *udma_base = (struct unit_regs *)(adapter->udma_base);

  if (adapter->tx_usecs != tx_usecs) {
    int qid;
    uint interval = (tx_usecs + 15) / 16;
    BUG_ON(interval > 255);
    adapter->tx_usecs = interval * 16;
    for (qid = 0; qid < adapter->num_tx_queues; qid++)
      al_iofic_msix_moder_interval_config(
          &udma_base->gen.interrupt_regs.main_iofic, AL_INT_GROUP_C, qid,
          interval);
  }
  if (adapter->rx_usecs != rx_usecs) {
    int qid;
    uint interval = (rx_usecs + 15) / 16;
    BUG_ON(interval > 255);
    adapter->rx_usecs = interval * 16;
    for (qid = 0; qid < adapter->num_rx_queues; qid++)
      al_iofic_msix_moder_interval_config(
          &udma_base->gen.interrupt_regs.main_iofic, AL_INT_GROUP_B, qid,
          interval);
  }
}

static int al_eth_ethtool_set_coalesce(struct net_device *net_dev,
                                       struct ethtool_coalesce *coalesce,
                                       struct kernel_ethtool_coalesce *,
                                       struct netlink_ext_ack *) {
  struct al_eth_adapter *adapter = netdev_priv(net_dev);
  unsigned int tx_usecs = adapter->tx_usecs;
  unsigned int rx_usecs = adapter->rx_usecs;

  if (coalesce->use_adaptive_tx_coalesce)
    return -EINVAL;

  if (coalesce->rx_coalesce_usecs != rx_usecs)
    rx_usecs = coalesce->rx_coalesce_usecs;
  else
    rx_usecs = coalesce->rx_coalesce_usecs_irq;

  if (coalesce->tx_coalesce_usecs != tx_usecs)
    tx_usecs = coalesce->tx_coalesce_usecs;
  else
    tx_usecs = coalesce->tx_coalesce_usecs_irq;

  if (tx_usecs > (255 * 16))
    return -EINVAL;
  if (rx_usecs > (255 * 16))
    return -EINVAL;

  al_eth_set_coalesce(adapter, tx_usecs, rx_usecs);

  return 0;
}

static int al_eth_nway_reset(struct net_device *netdev) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct phy_device *phydev = adapter->phydev;

  if (!phydev)
    return -ENODEV;

  return phy_start_aneg(phydev);
}

static u32 al_eth_get_msglevel(struct net_device *netdev) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  return adapter->msg_enable;
}

static void al_eth_set_msglevel(struct net_device *netdev, u32 value) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  adapter->msg_enable = value;
}

static void al_eth_get_stats64(struct net_device *netdev,
                               struct rtnl_link_stats64 *stats) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct al_eth_mac_stats *mac_stats = &adapter->mac_stats;

  if (!adapter->up)
    return;

  al_eth_mac_stats_get(&adapter->hal_adapter, mac_stats);

  stats->rx_packets = mac_stats->aFramesReceivedOK; /* including pause frames */
  stats->tx_packets =
      mac_stats->aFramesTransmittedOK; /* including pause frames */
  stats->rx_bytes = mac_stats->aOctetsReceivedOK;
  stats->tx_bytes = mac_stats->aOctetsTransmittedOK;
  stats->rx_dropped = 0;
  stats->multicast = mac_stats->ifInMulticastPkts;
  stats->collisions = 0;

  stats->rx_length_errors =
      (mac_stats->etherStatsUndersizePkts + /* good but short */
       mac_stats->etherStatsFragments +     /* short and bad*/
       mac_stats->etherStatsJabbers +       /* with crc errors */
       mac_stats->etherStatsOversizePkts);
  stats->rx_crc_errors = mac_stats->aFrameCheckSequenceErrors;
  stats->rx_frame_errors = mac_stats->aAlignmentErrors;
  stats->rx_fifo_errors = mac_stats->etherStatsDropEvents;
  stats->rx_missed_errors = 0;
  stats->tx_window_errors = 0;

  stats->rx_errors = mac_stats->ifInErrors;
  stats->tx_errors = mac_stats->ifOutErrors;
}

static void al_eth_get_drvinfo(struct net_device *dev,
                               struct ethtool_drvinfo *info) {
  struct al_eth_adapter *adapter = netdev_priv(dev);

  strscpy(info->driver, DRV_MODULE_NAME, sizeof(info->driver));
  strscpy(info->version, DRV_MODULE_VERSION, sizeof(info->version));
  strscpy(info->bus_info, pci_name(adapter->pdev), sizeof(info->bus_info));
}

static void al_eth_get_ringparam(struct net_device *netdev,
                                 struct ethtool_ringparam *ring,
                                 struct kernel_ethtool_ringparam *,
                                 struct netlink_ext_ack *) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct al_eth_ring *tx_ring = &adapter->tx_ring[0];
  struct al_eth_ring *rx_ring = &adapter->rx_ring[0];

  ring->rx_max_pending = AL_ETH_DEFAULT_RX_DESCS;
  ring->tx_max_pending = AL_ETH_DEFAULT_TX_SW_DESCS;
  ring->rx_pending = rx_ring->sw_count;
  ring->tx_pending = tx_ring->sw_count;
}

static void al_eth_get_pauseparam(struct net_device *netdev,
                                  struct ethtool_pauseparam *pause) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct al_eth_link_config *link_config = &adapter->link_config;

  pause->autoneg =
      ((link_config->flow_ctrl_active & AL_ETH_FLOW_CTRL_AUTONEG) != 0);
  pause->rx_pause =
      ((link_config->flow_ctrl_active & AL_ETH_FLOW_CTRL_RX_PAUSE) != 0);
  pause->tx_pause =
      ((link_config->flow_ctrl_active & AL_ETH_FLOW_CTRL_TX_PAUSE) != 0);
}

static int al_eth_set_pauseparam(struct net_device *netdev,
                                 struct ethtool_pauseparam *pause) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct al_eth_link_config *link_config = &adapter->link_config;
  uint32_t newadv;

  /* auto negotiation and receive pause are currently not supported */
  if (pause->autoneg == AUTONEG_ENABLE)
    return -EINVAL;

  link_config->flow_ctrl_supported = 0;

  if (pause->rx_pause) {
    link_config->flow_ctrl_supported |= AL_ETH_FLOW_CTRL_RX_PAUSE;

    if (pause->tx_pause) {
      link_config->flow_ctrl_supported |= AL_ETH_FLOW_CTRL_TX_PAUSE;
      newadv = ADVERTISED_Pause;
    } else
      newadv = ADVERTISED_Pause | ADVERTISED_Asym_Pause;
  } else if (pause->tx_pause) {
    link_config->flow_ctrl_supported |= AL_ETH_FLOW_CTRL_TX_PAUSE;
    newadv = ADVERTISED_Asym_Pause;
  } else {
    newadv = 0;
  }

  if (pause->autoneg) {
    struct phy_device *phydev;
    uint32_t oldadv;
    u32 advertising;

    phydev = mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);

    ethtool_convert_link_mode_to_legacy_u32(&advertising, phydev->advertising);

    oldadv = advertising & (ADVERTISED_Pause | ADVERTISED_Asym_Pause);
    link_config->flow_ctrl_supported |= AL_ETH_FLOW_CTRL_AUTONEG;

    if (oldadv != newadv) {
      advertising &= ~(ADVERTISED_Pause | ADVERTISED_Asym_Pause);
      advertising |= newadv;

      ethtool_convert_legacy_u32_to_link_mode(phydev->advertising, advertising);

      if (phydev->autoneg)
        return phy_start_aneg(phydev);
    }
  } else {
    link_config->flow_ctrl_active = link_config->flow_ctrl_supported;
    al_eth_flow_ctrl_config(adapter);
  }

  return 0;
}

static int al_eth_get_rxnfc(struct net_device *netdev,
                            struct ethtool_rxnfc *info,
                            u32 *rules __always_unused) {
  /*struct al_eth_adapter *adapter = netdev_priv(netdev);*/

  switch (info->cmd) {
  case ETHTOOL_GRXRINGS:
    info->data = AL_ETH_NUM_QUEUES;
    return 0;
    /*	case ETHTOOL_GRXFH:
                    return bnx2x_get_rss_flags(bp, info);
    */
  default:
    netdev_err(netdev, "Command parameters not supported\n");
    return -EOPNOTSUPP;
  }
}

static u32 al_eth_get_rxfh_indir_size(struct net_device *netdev) {
  return AL_ETH_RX_RSS_TABLE_SIZE;
}

static int al_eth_get_rxfh(struct net_device *netdev,
                           struct ethtool_rxfh_param *rxfh) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  int i;
  if (rxfh->hfunc != ETH_RSS_HASH_NO_CHANGE)
    rxfh->hfunc = ETH_RSS_HASH_TOP;

  if (rxfh->indir) {
    for (i = 0; i < AL_ETH_RX_RSS_TABLE_SIZE; i++)
      rxfh->indir[i] = adapter->rss_ind_tbl[i];
  }

  return 0;
}

static int al_eth_set_rxfh(struct net_device *netdev,
                           struct ethtool_rxfh_param *rxfh,
                           struct netlink_ext_ack *extack) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  size_t i;

  if (rxfh->key || (rxfh->hfunc != ETH_RSS_HASH_NO_CHANGE &&
                    rxfh->hfunc != ETH_RSS_HASH_TOP))
    return -EOPNOTSUPP;

  if (rxfh->indir) {
    for (i = 0; i < AL_ETH_RX_RSS_TABLE_SIZE; i++) {
      adapter->rss_ind_tbl[i] = rxfh->indir[i];
      al_eth_thash_table_set(&adapter->hal_adapter, i, 0, rxfh->indir[i]);
    }
  }

  return 0;
}

static void al_eth_get_channels(struct net_device *netdev,
                                struct ethtool_channels *channels) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);

  channels->max_rx = AL_ETH_NUM_QUEUES;
  channels->max_tx = AL_ETH_NUM_QUEUES;
  channels->max_other = 0;
  channels->max_combined = 0;
  channels->rx_count = adapter->num_rx_queues;
  channels->tx_count = adapter->num_tx_queues;
  channels->other_count = 0;
  channels->combined_count = 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0)
static int al_eth_get_eee(struct net_device *netdev,
                          struct ethtool_keee *edata) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct al_eth_eee_params params;

  if (!adapter->phy_exist)
    return -EOPNOTSUPP;

  al_eth_eee_get(&adapter->hal_adapter, &params);

  edata->eee_enabled = params.enable;
  edata->tx_lpi_timer = params.tx_eee_timer;

  return phy_ethtool_get_eee(adapter->phydev, edata);
}

static int al_eth_set_eee(struct net_device *netdev,
                          struct ethtool_keee *edata) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct al_eth_eee_params params;

  struct phy_device *phydev;

  if (!adapter->phy_exist)
    return -EOPNOTSUPP;

  phydev = mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);

  phy_init_eee(phydev, 1);

  params.enable = edata->eee_enabled;
  params.tx_eee_timer = edata->tx_lpi_timer;
  params.min_interval = 10;

  al_eth_eee_config(&adapter->hal_adapter, &params);

  return phy_ethtool_set_eee(phydev, edata);
}
#endif

static void al_eth_get_wol(struct net_device *netdev,
                           struct ethtool_wolinfo *wol) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct phy_device *phydev;

  wol->wolopts = adapter->wol;

  if ((adapter) && (adapter->phy_exist) && (adapter->mdio_bus)) {
    phydev = mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);
    if (phydev) {
      phy_ethtool_get_wol(phydev, wol);
      wol->supported |= WAKE_PHY;
      return;
    }
  }

  wol->supported |= WAKE_UCAST | WAKE_MCAST | WAKE_BCAST;
}

static int al_eth_set_wol(struct net_device *netdev,
                          struct ethtool_wolinfo *wol) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct phy_device *phydev;

  if (wol->wolopts & (WAKE_ARP | WAKE_MAGICSECURE))
    return -EOPNOTSUPP;

  adapter->wol = wol->wolopts;

  if ((adapter) && (adapter->phy_exist) && (adapter->mdio_bus)) {
    phydev = mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);
    if (phydev)
      return phy_ethtool_set_wol(phydev, wol);
  }

  device_set_wakeup_enable(&adapter->pdev->dev, adapter->wol);

  return 0;
}

static struct {
  const char str[ETH_GSTRING_LEN];
} al_ethtool_stats_keys[] = {
    {"mac_aOctetsReceivedOK"},
    {"mac_aOctetsTransmittedOK"},
    {"mac_etherStatsPkts"},
    {"mac_ifInUcastPkts"},
    {"mac_ifInMulticastPkts"},
    {"mac_ifInBroadcastPkts"},
    {"mac_ifInErrors"},
    {"mac_ifOutUcastPkts"},
    {"mac_ifOutMulticastPkts"},
    {"mac_ifOutBroadcastPkts"},
    {"mac_ifOutErrors"},
    {"mac_aFramesReceivedOK"},
    {"mac_aFramesTransmittedOK"},
    {"mac_etherStatsUndersizePkts"},
    {"mac_etherStatsFragments"},
    {"mac_etherStatsJabbers"},
    {"mac_etherStatsOversizePkts"},
    {"mac_aFrameCheckSequenceErrors"},
    {"mac_aAlignmentErrors"},
    {"mac_etherStatsDropEvents"},
    {"mac_aPAUSEMACCtrlTxFrames"},
    {"mac_aPAUSEMACCtrlRxFrames"},
    {"mac_aFrameTooLongErrors"},
    {"mac_aInRangeLengthErrors"},
    {"mac_VLANTransmittedOK"},
    {"mac_VLANReceivedOK"},
    {"mac_etherStatsOctets"},
    {"mac_etherStatsPkts64"},
    {"mac_etherStatsPkts65to127"},
    {"mac_etherStatsPkts128to255"},
    {"mac_etherStatsPkts256to511"},
    {"mac_etherStatsPkts512to1023"},
    {"mac_etherStatsPkts1024to1518"},
    {"mac_etherStatsPkts1519toX"},
    {"mac_eee_in"},
    {"mac_eee_out"},

    {"ec_faf_in_rx_pkt"},
    {"ec_faf_in_rx_short"},
    {"ec_faf_in_rx_long"},
    {"ec_faf_out_rx_pkt"},
    {"ec_faf_out_rx_short"},
    {"ec_faf_out_rx_long"},
    {"ec_faf_out_drop"},
    {"ec_rxf_in_rx_pkt"},
    {"ec_rxf_in_fifo_err"},
    {"ec_lbf_in_rx_pkt"},
    {"ec_lbf_in_fifo_err"},
    {"ec_rxf_out_rx_1_pkt"},
    {"ec_rxf_out_rx_2_pkt"},
    {"ec_rxf_out_drop_1_pkt"},
    {"ec_rxf_out_drop_2_pkt"},
    {"ec_rpe_1_in_rx_pkt"},
    {"ec_rpe_1_out_rx_pkt"},
    {"ec_rpe_2_in_rx_pkt"},
    {"ec_rpe_2_out_rx_pkt"},
    {"ec_rpe_3_in_rx_pkt"},
    {"ec_rpe_3_out_rx_pkt"},
    {"ec_tpe_in_tx_pkt"},
    {"ec_tpe_out_tx_pkt"},
    {"ec_tpm_tx_pkt"},
    {"ec_tfw_in_tx_pkt"},
    {"ec_tfw_out_tx_pkt"},
    {"ec_rfw_in_rx_pkt"},
    {"ec_rfw_in_vlan_drop"},
    {"ec_rfw_in_parse_drop"},
    {"ec_rfw_in_mc"},
    {"ec_rfw_in_bc"},
    {"ec_rfw_in_vlan_exist"},
    {"ec_rfw_in_vlan_nexist"},
    {"ec_rfw_in_mac_drop"},
    {"ec_rfw_in_mac_ndet_drop"},
    {"ec_rfw_in_ctrl_drop"},
    {"ec_rfw_in_prot_i_drop"},
    {"ec_eee_in"},
};

static void al_eth_get_strings(struct net_device *netdev, u32 stringset,
                               u8 *data) {
  memcpy(data, al_ethtool_stats_keys, sizeof(al_ethtool_stats_keys));
}

static int al_eth_get_sset_count(struct net_device *netdev, int sset) {
  switch (sset) {
  case ETH_SS_STATS:
    return ARRAY_SIZE(al_ethtool_stats_keys);
  default:
    return -EOPNOTSUPP;
  }
}

static void al_eth_get_ethtool_stats(struct net_device *netdev,
                                     struct ethtool_stats *stats, u64 *data) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);
  struct al_eth_mac_stats *mac_stats = &adapter->mac_stats;
  struct al_eth_ec_stats *ec_stats = &adapter->ec_stats;
  int i = 0;

  al_eth_mac_stats_get(&adapter->hal_adapter, mac_stats);
  al_eth_ec_stats_get(&adapter->hal_adapter, ec_stats);

  data[i++] = mac_stats->aOctetsReceivedOK;
  data[i++] = mac_stats->aOctetsTransmittedOK;
  data[i++] = mac_stats->etherStatsPkts;
  data[i++] = mac_stats->ifInUcastPkts;
  data[i++] = mac_stats->ifInMulticastPkts;
  data[i++] = mac_stats->ifInBroadcastPkts;
  data[i++] = mac_stats->ifInErrors;
  data[i++] = mac_stats->ifOutUcastPkts;
  data[i++] = mac_stats->ifOutMulticastPkts;
  data[i++] = mac_stats->ifOutBroadcastPkts;
  data[i++] = mac_stats->ifOutErrors;
  data[i++] = mac_stats->aFramesReceivedOK;
  data[i++] = mac_stats->aFramesTransmittedOK;
  data[i++] = mac_stats->etherStatsUndersizePkts;
  data[i++] = mac_stats->etherStatsFragments;
  data[i++] = mac_stats->etherStatsJabbers;
  data[i++] = mac_stats->etherStatsOversizePkts;
  data[i++] = mac_stats->aFrameCheckSequenceErrors;
  data[i++] = mac_stats->aAlignmentErrors;
  data[i++] = mac_stats->etherStatsDropEvents;
  data[i++] = mac_stats->aPAUSEMACCtrlFramesTransmitted;
  data[i++] = mac_stats->aPAUSEMACCtrlFramesReceived;
  data[i++] = mac_stats->aFrameTooLongErrors;
  data[i++] = mac_stats->aInRangeLengthErrors;
  data[i++] = mac_stats->VLANTransmittedOK;
  data[i++] = mac_stats->VLANReceivedOK;
  data[i++] = mac_stats->etherStatsOctets;
  data[i++] = mac_stats->etherStatsPkts64Octets;
  data[i++] = mac_stats->etherStatsPkts65to127Octets;
  data[i++] = mac_stats->etherStatsPkts128to255Octets;
  data[i++] = mac_stats->etherStatsPkts256to511Octets;
  data[i++] = mac_stats->etherStatsPkts512to1023Octets;
  data[i++] = mac_stats->etherStatsPkts1024to1518Octets;
  data[i++] = mac_stats->etherStatsPkts1519toX;
  data[i++] = mac_stats->eee_in;
  data[i++] = mac_stats->eee_out;

  data[i++] = ec_stats->faf_in_rx_pkt;
  data[i++] = ec_stats->faf_in_rx_short;
  data[i++] = ec_stats->faf_in_rx_long;
  data[i++] = ec_stats->faf_out_rx_pkt;
  data[i++] = ec_stats->faf_out_rx_short;
  data[i++] = ec_stats->faf_out_rx_long;
  data[i++] = ec_stats->faf_out_drop;
  data[i++] = ec_stats->rxf_in_rx_pkt;
  data[i++] = ec_stats->rxf_in_fifo_err;
  data[i++] = ec_stats->lbf_in_rx_pkt;
  data[i++] = ec_stats->lbf_in_fifo_err;
  data[i++] = ec_stats->rxf_out_rx_1_pkt;
  data[i++] = ec_stats->rxf_out_rx_2_pkt;
  data[i++] = ec_stats->rxf_out_drop_1_pkt;
  data[i++] = ec_stats->rxf_out_drop_2_pkt;
  data[i++] = ec_stats->rpe_1_in_rx_pkt;
  data[i++] = ec_stats->rpe_1_out_rx_pkt;
  data[i++] = ec_stats->rpe_2_in_rx_pkt;
  data[i++] = ec_stats->rpe_2_out_rx_pkt;
  data[i++] = ec_stats->rpe_3_in_rx_pkt;
  data[i++] = ec_stats->rpe_3_out_rx_pkt;
  data[i++] = ec_stats->tpe_in_tx_pkt;
  data[i++] = ec_stats->tpe_out_tx_pkt;
  data[i++] = ec_stats->tpm_tx_pkt;
  data[i++] = ec_stats->tfw_in_tx_pkt;
  data[i++] = ec_stats->tfw_out_tx_pkt;
  data[i++] = ec_stats->rfw_in_rx_pkt;
  data[i++] = ec_stats->rfw_in_vlan_drop;
  data[i++] = ec_stats->rfw_in_parse_drop;
  data[i++] = ec_stats->rfw_in_mc;
  data[i++] = ec_stats->rfw_in_bc;
  data[i++] = ec_stats->rfw_in_vlan_exist;
  data[i++] = ec_stats->rfw_in_vlan_nexist;
  data[i++] = ec_stats->rfw_in_mac_drop;
  data[i++] = ec_stats->rfw_in_mac_ndet_drop;
  data[i++] = ec_stats->rfw_in_ctrl_drop;
  data[i++] = ec_stats->rfw_in_prot_i_drop;
  data[i++] = ec_stats->eee_in;
}

static const struct ethtool_ops al_eth_ethtool_ops = {
    .supported_coalesce_params =
        ETHTOOL_COALESCE_USECS | ETHTOOL_COALESCE_MAX_FRAMES,
    .get_link_ksettings = al_eth_get_link_ksettings,
    .set_link_ksettings = al_eth_set_link_ksettings,
    .get_drvinfo = al_eth_get_drvinfo,
    .get_wol = al_eth_get_wol,
    .set_wol = al_eth_set_wol,
    .get_msglevel = al_eth_get_msglevel,
    .set_msglevel = al_eth_set_msglevel,
    .nway_reset = al_eth_nway_reset,
    .get_link = ethtool_op_get_link,
    .get_coalesce = al_eth_get_coalesce,
    .set_coalesce = al_eth_ethtool_set_coalesce,
    .get_ringparam = al_eth_get_ringparam,
    .get_pauseparam = al_eth_get_pauseparam,
    .set_pauseparam = al_eth_set_pauseparam,
    .get_strings = al_eth_get_strings,
    .get_ethtool_stats = al_eth_get_ethtool_stats,
    .get_rxnfc = al_eth_get_rxnfc,
    .get_sset_count = al_eth_get_sset_count,
    .get_rxfh_indir_size = al_eth_get_rxfh_indir_size,
    .get_rxfh = al_eth_get_rxfh,
    .set_rxfh = al_eth_set_rxfh,
    .get_channels = al_eth_get_channels,

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0)
    .get_eee = al_eth_get_eee,
    .set_eee = al_eth_set_eee,
#endif

};

static void al_eth_tx_csum(struct al_eth_ring *tx_ring,
                           struct al_eth_tx_buffer *tx_info,
                           struct al_eth_pkt *hal_pkt, struct sk_buff *skb) {
  u32 mss = skb_shinfo(skb)->gso_size;
  if ((skb->ip_summed == CHECKSUM_PARTIAL) || mss) {
    struct al_eth_meta_data *meta = &tx_ring->hal_meta;
    if (mss)
      hal_pkt->flags |= AL_ETH_TX_FLAGS_TSO | AL_ETH_TX_FLAGS_L4_CSUM;
    else
      hal_pkt->flags |=
          AL_ETH_TX_FLAGS_L4_CSUM | AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM;

    switch (skb->protocol) {
    case __constant_htons(ETH_P_IP):
      hal_pkt->l3_proto_idx = AL_ETH_PROTO_ID_IPv4;
      if (mss)
        hal_pkt->flags |= AL_ETH_TX_FLAGS_IPV4_L3_CSUM;
      if (ip_hdr(skb)->protocol == IPPROTO_TCP)
        hal_pkt->l4_proto_idx = AL_ETH_PROTO_ID_TCP;
      else
        hal_pkt->l4_proto_idx = AL_ETH_PROTO_ID_UDP;
      break;
    case __constant_htons(ETH_P_IPV6):
      hal_pkt->l3_proto_idx = AL_ETH_PROTO_ID_IPv6;
      /* TODO: add support for csum offloading for ipv6 with options */
      if (ipv6_hdr(skb)->nexthdr == IPPROTO_TCP)
        hal_pkt->l4_proto_idx = AL_ETH_PROTO_ID_TCP;
      else
        hal_pkt->l4_proto_idx = AL_ETH_PROTO_ID_UDP;
      break;
    default:
      break;
    }

    meta->words_valid = 4;
    meta->l3_header_len = skb_network_header_len(skb);
    meta->l3_header_offset = skb_network_offset(skb);
    meta->l4_header_len = tcp_hdr(skb)->doff; /* this param needed only
    for TSO */
    meta->mss_idx_sel = 0; /* TODO: check how to select MSS */
    meta->mss_val = skb_shinfo(skb)->gso_size;
    hal_pkt->meta = meta;
  } else
    hal_pkt->meta = NULL;
}

/* Called with netif_tx_lock.
 */
static netdev_tx_t al_eth_start_xmit(struct sk_buff *skb,
                                     struct net_device *dev) {
  struct al_eth_adapter *adapter = netdev_priv(dev);
  dma_addr_t dma;
  struct al_eth_tx_buffer *tx_info;
  struct al_eth_pkt *hal_pkt;
  struct al_buf *al_buf;
  u32 len, last_frag;
  u16 next_to_use;
  int i, qid;
  struct al_eth_ring *tx_ring;
  struct netdev_queue *txq;

  netdev_dbg(adapter->netdev, "%s skb %p\n", __func__, skb);
  /*  Determine which tx ring we will be placed on */
  qid = skb_get_queue_mapping(skb);
  tx_ring = &adapter->tx_ring[qid];
  txq = netdev_get_tx_queue(dev, qid);

  len = skb_headlen(skb);

  dma = dma_map_single(tx_ring->dev, skb->data, len, DMA_TO_DEVICE);
  if (dma_mapping_error(tx_ring->dev, dma)) {
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
  }

  next_to_use = tx_ring->next_to_use;
  tx_info = &tx_ring->tx_buffer_info[next_to_use];
  tx_info->skb = skb;
  hal_pkt = &tx_info->hal_pkt;

  /* set flags and meta data */
  hal_pkt->flags = AL_ETH_TX_FLAGS_INT;
  al_eth_tx_csum(tx_ring, tx_info, hal_pkt, skb);

  al_buf = hal_pkt->bufs;

  dma_unmap_addr_set(al_buf, addr, dma);
  dma_unmap_len_set(al_buf, len, len);

  last_frag = skb_shinfo(skb)->nr_frags;

  for (i = 0; i < last_frag; i++) {
    const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

    al_buf++;

    len = skb_frag_size(frag);
    dma = skb_frag_dma_map(tx_ring->dev, frag, 0, len, DMA_TO_DEVICE);
    if (dma_mapping_error(tx_ring->dev, dma))
      goto dma_error;
    dma_unmap_addr_set(al_buf, addr, dma);
    dma_unmap_len_set(al_buf, len, len);
  }

  hal_pkt->num_of_bufs = 1 + last_frag;
  if (unlikely(last_frag > (AL_ETH_PKT_MAX_BUFS - 2))) {
    int i;
    netdev_err(adapter->netdev, "too much descriptors. last_frag %d!\n",
               last_frag);
    for (i = 0; i <= last_frag; i++)
      netdev_err(adapter->netdev, "frag[%d]: addr:0x%llx, len 0x%x\n", i,
                 (unsigned long long)hal_pkt->bufs[i].addr,
                 hal_pkt->bufs[i].len);
    BUG();
  }
  netdev_tx_sent_queue(txq, skb->len);

  /*smp_wmb();*/ /* commit the item before incrementing the head */
  tx_ring->next_to_use = AL_ETH_TX_RING_IDX_NEXT(tx_ring, next_to_use);

  /* prepare the packet's descriptors to dma engine */
  tx_info->tx_descs = al_eth_tx_pkt_prepare(tx_ring->dma_q, hal_pkt);

  /* stop the queue when no more space available, the packet can have up
   * to MAX_SKB_FRAGS + 1 buffers and a meta descriptor */
  if (unlikely(al_udma_available_get(tx_ring->dma_q) < (MAX_SKB_FRAGS + 2))) {
    dev_dbg(&adapter->pdev->dev, "%s stop queue %d\n", __func__, qid);
    netif_tx_stop_queue(txq);
  }

  /* trigger the dma engine */
  al_eth_tx_dma_action(tx_ring->dma_q, tx_info->tx_descs);

  return NETDEV_TX_OK;

dma_error:
  /* save value of frag that failed */
  last_frag = i;

  /* start back at beginning and unmap skb */
  tx_info->skb = NULL;
  al_buf = hal_pkt->bufs;
  dma_unmap_single(tx_ring->dev, dma_unmap_addr(al_buf, addr),
                   dma_unmap_len(al_buf, len), DMA_TO_DEVICE);

  /* unmap remaining mapped pages */
  for (i = 0; i < last_frag; i++) {
    al_buf++;
    dma_unmap_page(tx_ring->dev, dma_unmap_addr(al_buf, addr),
                   dma_unmap_len(al_buf, len), DMA_TO_DEVICE);
  }

  dev_kfree_skb(skb);
  return NETDEV_TX_OK;
}

/* Return subqueue id on this core (one per core). */
static u16 al_eth_select_queue(struct net_device *dev, struct sk_buff *skb,
                               struct net_device *sb_dev) {
#ifdef CONFIG_ARCH_ALPINE
  return smp_processor_id();
#else
  return smp_processor_id() % AL_ETH_NUM_QUEUES;
#endif
}

static int al_eth_set_mac_addr(struct net_device *dev, void *p) {
  struct al_eth_adapter *adapter = netdev_priv(dev);
  struct sockaddr *addr = p;
  int err = 0;

  if (!is_valid_ether_addr(addr->sa_data))
    return -EADDRNOTAVAIL;

  dev_addr_set(dev, addr->sa_data);
  memcpy(adapter->mac_addr, addr->sa_data, dev->addr_len);
  al_eth_mac_table_unicast_add(adapter, AL_ETH_MAC_TABLE_UNICAST_IDX_BASE,
                               adapter->mac_addr, 1);

  if (!netif_running(dev))
    return 0;

  return err;
}

/**
 *  Unicast, Multicast and Promiscuous mode set
 *  @netdev: network interface device structure
 *
 *  The set_rx_mode entry point is called whenever the unicast or multicast
 *  address lists or the network interface flags are updated.  This routine is
 *  responsible for configuring the hardware for proper unicast, multicast,
 *  promiscuous mode, and all-multi behavior.
 **/
static void al_eth_set_rx_mode(struct net_device *netdev) {
  struct al_eth_adapter *adapter = netdev_priv(netdev);

  if (netdev->flags & IFF_PROMISC) {
    al_eth_mac_table_promiscuous_set(adapter, true);
  } else {
    if (netdev->flags & IFF_ALLMULTI) {
      al_eth_mac_table_all_multicast_add(adapter,
                                         AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX, 1);
    } else {
      if (netdev_mc_empty(netdev))
        al_eth_mac_table_entry_clear(adapter,
                                     AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX);
      else
        al_eth_mac_table_all_multicast_add(
            adapter, AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX, 1);
    }

    if (!netdev_uc_empty(netdev)) {
      struct netdev_hw_addr *ha;
      uint8_t i = AL_ETH_MAC_TABLE_UNICAST_IDX_BASE + 1;

      if (netdev_uc_count(netdev) > AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT) {
        /* In this case there are more addresses then
         * entries in the mac table - set promiscuous */
        al_eth_mac_table_promiscuous_set(adapter, true);
        return;
      }

      /* clear the last configuration */
      while (i < (AL_ETH_MAC_TABLE_UNICAST_IDX_BASE + 1 +
                  AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT)) {
        al_eth_mac_table_entry_clear(adapter, i);
        i++;
      }

      /* set new addresses */
      i = AL_ETH_MAC_TABLE_UNICAST_IDX_BASE + 1;
      netdev_for_each_uc_addr(ha, netdev) {
        al_eth_mac_table_unicast_add(adapter, i, ha->addr, 1);
        i++;
      }
    }

    al_eth_mac_table_promiscuous_set(adapter, false);
  }
}

static const struct net_device_ops al_eth_netdev_ops = {
    .ndo_open = al_eth_open,
    .ndo_stop = al_eth_close,
    .ndo_start_xmit = al_eth_start_xmit,
    .ndo_select_queue = al_eth_select_queue,
    .ndo_get_stats64 = al_eth_get_stats64,
    .ndo_do_ioctl = al_eth_ioctl,
    .ndo_tx_timeout = al_eth_tx_timeout,
    .ndo_change_mtu = al_eth_change_mtu,
    .ndo_set_mac_address = al_eth_set_mac_addr,
    .ndo_set_rx_mode = al_eth_set_rx_mode,
#if 0
	.ndo_validate_addr	= eth_validate_addr,

#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= al_eth_netpoll,
#endif
#endif
#ifdef CONFIG_RFS_ACCEL
    .ndo_rx_flow_steer = al_eth_flow_steer,
#endif
    .ndo_set_features = al_set_features,
};

/**
 * al_eth_probe - Device Initialization Routine
 * @pdev: PCI device information struct
 * @ent: entry in al_eth_pci_tbl
 *
 * Returns 0 on success, negative on failure
 *
 * al_eth_probe initializes an adapter identified by a pci_dev structure.
 * The OS initialization, configuring of the adapter private structure,
 * and a hardware reset occur.
 **/
static int al_eth_probe(struct pci_dev *pdev, const struct pci_device_id *ent) {
  static int version_printed;
  struct net_device *netdev;
  struct al_eth_adapter *adapter;
  void __iomem *const *iomap;
  struct al_hal_eth_adapter *hal_adapter;
  static int adapters_found;
  u16 dev_id;
  u8 rev_id;
  int i;

  int rc;

  dev_dbg(&pdev->dev, "%s\n", __func__);

  if (version_printed++ == 0)
    pr_info("%s", version);

  rc = pcim_enable_device(pdev);
  if (rc) {
    dev_err(&pdev->dev, "pcim_enable_device failed!\n");
    return rc;
  }

  if (ent->driver_data == ALPINE_INTEGRATED)
    rc = pcim_iomap_regions(pdev, (1 << 0) | (1 << 2) | (1 << 4),
                            DRV_MODULE_NAME);
  else
    rc = pcim_iomap_regions(pdev, (1 << board_info[ent->driver_data].bar),
                            DRV_MODULE_NAME);

  if (rc) {
    dev_err(&pdev->dev, "pci_request_selected_regions failed 0x%x\n", rc);
    return rc;
  }

  iomap = pcim_iomap_table(pdev);
  if (!iomap) {
    dev_err(&pdev->dev, "pcim_iomap_table failed\n");
    return -ENOMEM;
  }

  rc = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(40));
  if (rc) {
    dev_err(&pdev->dev, "dma_set_mask_and_coherent failed 0x%x\n", rc);
    return rc;
  }

  pci_set_master(pdev);
  pci_save_state(pdev);

  /* dev zeroed in init_etherdev */
  netdev = alloc_etherdev_mq(sizeof(struct al_eth_adapter), AL_ETH_NUM_QUEUES);
  if (!netdev) {
    dev_err(&pdev->dev, "alloc_etherdev_mq failed\n");
    return -ENOMEM;
  }

  SET_NETDEV_DEV(netdev, &pdev->dev);

  adapter = netdev_priv(netdev);
  pci_set_drvdata(pdev, adapter);

  adapter->netdev = netdev;
  adapter->pdev = pdev;
  adapter->board_type = ent->driver_data;
  hal_adapter = &adapter->hal_adapter;
  adapter->msg_enable = netif_msg_init(debug, DEFAULT_MSG_ENABLE);

  if (adapter->board_type == ALPINE_INTEGRATED) {
    adapter->udma_base = iomap[AL_ETH_UDMA_BAR];
    adapter->ec_base = iomap[AL_ETH_EC_BAR];
    adapter->mac_base = iomap[AL_ETH_MAC_BAR];
  } else {
    u16 adapter_pci_cmd;

    /*
     * pci adapter configuration space: 0-4K
     * BAR0-ETH_CTL: 20K-36K (start offset 0x5000)
     * BAR1-MAC_CTL: 36K-40K (start offset 0x9000)
     * BAR2-UDMA: 128K-256K
     */
    adapter->internal_pcie_base = iomap[board_info[ent->driver_data].bar];
    adapter->udma_base = iomap[board_info[ent->driver_data].bar] + 128 * 0x400;
    adapter->ec_base = iomap[board_info[ent->driver_data].bar] + 20 * 0x400;
    adapter->mac_base = iomap[board_info[ent->driver_data].bar] + 36 * 0x400;
    /* enable master/slave in the adapter conf */
    adapter_pci_cmd = readw(adapter->internal_pcie_base + PCI_COMMAND);
    adapter_pci_cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
    adapter_pci_cmd &= ~PCI_COMMAND_INTX_DISABLE;

    writew(adapter_pci_cmd, adapter->internal_pcie_base + PCI_COMMAND);
  }

  if (adapter->board_type == ALPINE_INTEGRATED) {
    pci_read_config_word(pdev, PCI_DEVICE_ID, &dev_id);
    pci_read_config_byte(pdev, PCI_REVISION_ID, &rev_id);
  } else {
    dev_id = readw(adapter->internal_pcie_base + PCI_DEVICE_ID);
    rev_id = readb(adapter->internal_pcie_base + PCI_REVISION_ID);
  }

  adapter->rev_id = rev_id;
  adapter->dev_id = dev_id;
  adapter->id_number = adapters_found;

  /* set default ring sizes */
  adapter->tx_ring_count = AL_ETH_DEFAULT_TX_SW_DESCS;
  adapter->tx_descs_count = AL_ETH_DEFAULT_TX_HW_DESCS;
  adapter->rx_ring_count = AL_ETH_DEFAULT_RX_DESCS;
  adapter->rx_descs_count = AL_ETH_DEFAULT_RX_DESCS;

  adapter->num_tx_queues = AL_ETH_NUM_QUEUES;
  adapter->num_rx_queues = AL_ETH_NUM_QUEUES;

  adapter->small_copy_len = AL_ETH_DEFAULT_SMALL_PACKET_LEN;
  adapter->link_poll_interval = AL_ETH_DEFAULT_LINK_POLL_INTERVAL;
  adapter->max_rx_buff_alloc_size = AL_ETH_DEFAULT_MAX_RX_BUFF_ALLOC_SIZE;
  adapter->link_config.force_1000_base_x = AL_ETH_DEFAULT_FORCE_1000_BASEX;

  spin_lock_init(&adapter->serdes_config_lock);

  snprintf(adapter->name, AL_ETH_NAME_MAX_LEN, "al_eth_%d", adapter->id_number);
  rc = al_eth_board_params_init(adapter);
  if (rc)
    goto fail_free_netdev;

  al_eth_function_reset(adapter);

  rc = al_eth_hw_init(adapter);
  if (rc)
    goto fail_free_netdev;

  al_eth_init_rings(adapter);

  netdev->netdev_ops = &al_eth_netdev_ops;
  netdev->watchdog_timeo = TX_TIMEOUT;
  netdev->ethtool_ops = &al_eth_ethtool_ops;
  netdev->min_mtu = AL_ETH_MIN_FRAME_LEN;
  netdev->max_mtu = AL_ETH_MAX_MTU;

  if (!is_valid_ether_addr(adapter->mac_addr)) {
    eth_hw_addr_random(netdev);
    memcpy(adapter->mac_addr, netdev->dev_addr, ETH_ALEN);
  } else {
    dev_addr_set(netdev, adapter->mac_addr);
  }

  memcpy(adapter->netdev->perm_addr, adapter->mac_addr, netdev->addr_len);

  netdev->hw_features = NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
                        NETIF_F_TSO | NETIF_F_TSO_ECN | NETIF_F_TSO6 |
                        NETIF_F_RXCSUM | NETIF_F_NTUPLE | NETIF_F_RXHASH |
#ifdef CONFIG_NET_MQ_TX_LOCK_OPT
                        NETIF_F_MQ_TX_LOCK_OPT |
#endif
                        NETIF_F_HIGHDMA;

  netdev->features = netdev->hw_features;
#if defined(NETIF_F_MQ_TX_LOCK_OPT)
  netdev->features &= ~NETIF_F_MQ_TX_LOCK_OPT;
#endif
  netdev->priv_flags |= IFF_UNICAST_FLT;

  for (i = 0; i < AL_ETH_RX_RSS_TABLE_SIZE; i++)
    adapter->rss_ind_tbl[i] = ethtool_rxfh_indir_default(i, AL_ETH_NUM_QUEUES);

  rc = register_netdev(netdev);
  if (rc) {
    dev_err(&pdev->dev, "Cannot register net device\n");
    goto fail_unregister_netdev;
  }

  rc = al_eth_sysfs_init(&adapter->pdev->dev);
  if (rc)
    goto fail_unregister_netdev;

  rc = al_eth_sw_mdio_probe(adapter);
  if (rc)
    goto fail_unregister_netdev;

  netdev_info(netdev, "%s found at mem %lx, mac addr %pM\n",
              board_info[ent->driver_data].name,
              (long)pci_resource_start(pdev, 0), netdev->dev_addr);

  adapters_found++;

  return 0;

fail_unregister_netdev:
  unregister_netdev(netdev);

fail_free_netdev:
  free_netdev(netdev);

  return rc;
}

/**
 * al_eth_remove - Device Removal Routine
 * @pdev: PCI device information struct
 *
 * al_eth_remove is called by the PCI subsystem to alert the driver
 * that it should release a PCI device.
 **/
static void al_eth_remove(struct pci_dev *pdev) {
  struct al_eth_adapter *adapter = pci_get_drvdata(pdev);
  struct net_device *dev = adapter->netdev;

  al_eth_sw_mdio_remove(adapter);

  /* disable forwarding interrupts from eth through pci end point*/
  if (adapter->board_type == ALPINE_FPGA_NIC)
    writel(0, adapter->internal_pcie_base + 0x1800000 + 0x1210);

  unregister_netdev(dev);

  al_eth_sysfs_terminate(&pdev->dev);

  free_netdev(dev);

  pci_set_drvdata(pdev, NULL);
  pci_disable_device(pdev);
}

#ifdef CONFIG_PM
static int al_eth_resume(struct pci_dev *pdev) {
  struct al_eth_adapter *adapter = pci_get_drvdata(pdev);
  struct net_device *netdev = adapter->netdev;
  u32 err;

  pci_set_power_state(pdev, PCI_D0);
  pci_restore_state(pdev);
  /*
   * pci_restore_state clears dev->state_saved so call
   * pci_save_state to restore it.
   */
  pci_save_state(pdev);

  err = pci_enable_device_mem(pdev);
  if (err) {
    pr_err("Cannot enable PCI device from suspend\n");
    return err;
  }
  pci_set_master(pdev);

  pci_wake_from_d3(pdev, false);

  if (adapter->rev_id > AL_ETH_REV_ID_0) {
    al_eth_wol_disable(&adapter->hal_adapter);
  } else {

    al_eth_mac_table_unicast_add(adapter, AL_ETH_MAC_TABLE_UNICAST_IDX_BASE,
                                 adapter->mac_addr, 1);
    al_eth_mac_table_broadcast_add(adapter, AL_ETH_MAC_TABLE_BROADCAST_IDX, 1);
    al_eth_mac_table_promiscuous_set(adapter, false);
    al_eth_set_rx_mode(netdev);
  }

  netif_device_attach(netdev);

  return 0;
}

static int al_eth_wol_config(struct al_eth_adapter *adapter) {
  if (adapter->rev_id > AL_ETH_REV_ID_0) {
    struct al_eth_wol_params wol = {0};

    if (adapter->wol & WAKE_UCAST) {
      wol.int_mask = AL_ETH_WOL_INT_UNICAST;
      wol.forward_mask = AL_ETH_WOL_FWRD_UNICAST;
    }

    if (adapter->wol & WAKE_MCAST) {
      wol.int_mask = AL_ETH_WOL_INT_MULTICAST;
      wol.forward_mask = AL_ETH_WOL_FWRD_MULTICAST;
    }

    if (adapter->wol & WAKE_BCAST) {
      wol.int_mask = AL_ETH_WOL_INT_BROADCAST;
      wol.forward_mask = AL_ETH_WOL_FWRD_BROADCAST;
    }

    if (wol.int_mask != 0) {
      al_eth_wol_enable(&adapter->hal_adapter, &wol);
      return 1;
    }

    return 0;
  }

  if (!(adapter->wol & WAKE_UCAST)) {
    int i = AL_ETH_MAC_TABLE_UNICAST_IDX_BASE;
    /* clear all unicast addresses */
    while (i < (AL_ETH_MAC_TABLE_UNICAST_IDX_BASE + 1 +
                AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT)) {
      al_eth_mac_table_entry_clear(adapter, i);
      i++;
    }
  }

  if (!(adapter->wol & WAKE_MCAST))
    al_eth_mac_table_entry_clear(adapter, AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX);

  if (!(adapter->wol & WAKE_BCAST))
    al_eth_mac_table_entry_clear(adapter, AL_ETH_MAC_TABLE_BROADCAST_IDX);

  if (adapter->wol)
    return 1;

  return 0;
}

static int al_eth_suspend(struct pci_dev *pdev, pm_message_t state) {
  struct al_eth_adapter *adapter = pci_get_drvdata(pdev);

  if (al_eth_wol_config(adapter)) {
    pci_prepare_to_sleep(pdev);
  } else {
    pci_wake_from_d3(pdev, false);
    pci_set_power_state(pdev, PCI_D3hot);
  }

  return 0;
}
#endif /* CONFIG_PM */

static struct pci_driver al_eth_pci_driver = {
    .name = DRV_MODULE_NAME,
    .id_table = al_eth_pci_tbl,
    .probe = al_eth_probe,
    .remove = al_eth_remove,
#ifdef CONFIG_PM
    .suspend = al_eth_suspend,
    .resume = al_eth_resume,
#endif
};

static int __init al_eth_init(void) {
#ifdef CONFIG_AL_ETH_ALLOC_SKB
  struct sk_buff_head *rx_rc;
  int cpu;

  for_each_possible_cpu(cpu) {
    rx_rc = &per_cpu(rx_recycle_cache, cpu);
    skb_queue_head_init(rx_rc);
  }
#endif
  return pci_register_driver(&al_eth_pci_driver);
}

static void __exit al_eth_cleanup(void) {
#ifdef CONFIG_AL_ETH_ALLOC_SKB
  struct sk_buff_head *rx_rc;
  int cpu;

  for_each_possible_cpu(cpu) {
    rx_rc = &per_cpu(rx_recycle_cache, cpu);
    skb_queue_purge(rx_rc);
  }
#endif
  pci_unregister_driver(&al_eth_pci_driver);
}

module_init(al_eth_init);
module_exit(al_eth_cleanup);
