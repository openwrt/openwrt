define KernelPackage/mvmdio
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  HIDDEN:=1
  TITLE:=Marvell MDIO interface support
  KCONFIG:=CONFIG_MVMDIO
  DEPENDS:=+kmod-libphy +kmod-of-mdio
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/marvell/mvmdio.ko
  AUTOLOAD:=$(call AutoProbe,mvmdio)
endef

define KernelPackage/mdio/description
 This driver is used by the MV643XX_ETH and MVNETA drivers.
endef

$(eval $(call KernelPackage,mvmdio))


define KernelPackage/mvneta
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Marvell Armada 370/XP network interface support
  DEPENDS:=@TARGET_mvebu +kmod-mvmdio +kmod-phylink
  KCONFIG:= \
    CONFIG_MVNETA \
    CONFIG_MVNETA_BM \
    CONFIG_MVNETA_BM_ENABLE=y
  FILES:= \
    $(LINUX_DIR)/drivers/net/ethernet/marvell/mvneta.ko \
    $(LINUX_DIR)/drivers/net/ethernet/marvell/mvneta_bm.ko
  AUTOLOAD:=$(call AutoProbe,mvneta mvneta_bm)
endef

define KernelPackage/mdio/description
 This driver supports the network interface units in the Marvell ARMADA XP and
 ARMADA 370 SoC family.
endef

$(eval $(call KernelPackage,mvneta))


define KernelPackage/phy-marvell
   SUBMENU:=$(NETWORK_DEVICES_MENU)
   TITLE:=Drivers for Marvell PHYs
   KCONFIG:=CONFIG_MARVELL_PHY
   DEPENDS:=+kmod-libphy
   FILES:=$(LINUX_DIR)/drivers/net/phy/marvell.ko
   AUTOLOAD:=$(call AutoProbe,marvell,1)
endef

define KernelPackage/phy-marvell/description
  Currently has a driver for the 88E1011S
endef

$(eval $(call KernelPackage,phy-marvell))


define KernelPackage/net-dsa-mv88e6x
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Marvell 88E6xxx Ethernet switch fabric support
  DEPENDS:=+kmod-net-dsa +kmod-net-dsa-tag +kmod-net-edsa-tag
  KCONFIG:= \
    CONFIG_NET_DSA_MV88E6XXX \
    CONFIG_NET_DSA_MV88E6XXX_GLOBAL2=y
  FILES:=$(LINUX_DIR)/drivers/net/dsa/mv88e6xxx/mv88e6xxx.ko
  AUTOLOAD:=$(call AutoProbe,mv88e6xxx)
endef

define KernelPackage/net-dsa-mv88e6x/description
  This driver adds support for most of the Marvell 88E6xxx models of Ethernet switch chips,
  except 88E6060.
endef

$(eval $(call KernelPackage,net-dsa-mv88e6x))


define KernelPackage/net-dsa-tag
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Tag driver for Marvell switches using DSA headers
  DEPENDS:=+kmod-net-dsa
  KCONFIG:= CONFIG_NET_DSA_TAG_DSA
  FILES:=$(LINUX_DIR)/net/dsa/tag_dsa.ko
  AUTOLOAD:=$(call AutoProbe,tag_dsa)
endef

define KernelPackage/net-dsa-tag/description
  enable support for tagging frames for the Marvell switches which use DSA headers.
endef

$(eval $(call KernelPackage,net-dsa-tag))


define KernelPackage/net-edsa-tag
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Tag driver for Marvell sw using EtherType DSA headers
  DEPENDS:=+kmod-net-dsa
  KCONFIG:= CONFIG_NET_DSA_TAG_EDSA
  FILES:=$(LINUX_DIR)/net/dsa/tag_edsa.ko
  AUTOLOAD:=$(call AutoProbe,tag_edsa)
endef

define KernelPackage/net-edsa-tag/description
  enable support for tagging frames for the Marvell switches which use EtherType DSA headers.
endef

$(eval $(call KernelPackage,net-edsa-tag))
