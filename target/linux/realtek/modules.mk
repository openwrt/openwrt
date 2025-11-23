define KernelPackage/rtl83xx-phy
  SUBMENU:=Realtek modules
  TITLE:=Realtek SoC PHYs
  KCONFIG:= \
    CONFIG_REALTEK_SOC_PHY
  DEPENDS:=@TARGET_realtek
  FILES:=$(LINUX_DIR)/drivers/net/phy/rtl83xx-phy.ko
  AUTOLOAD:=$(call AutoLoad,18,rtl83xx-phy)
endef

define KernelPackage/rtl83xx-phy/description
  Supports the PHYs found in combination with Realtek Switch SoCs
endef

$(eval $(call KernelPackage,rtl83xx-phy))

define KernelPackage/mdio-realtek-otto-aux
  SUBMENU:=Realtek modules
  TITLE:=Realtek RTL838x/RTL839x switch support
  KCONFIG:= \
    CONFIG_MDIO_REALTEK_OTTO_AUX
  DEPENDS:=@TARGET_realtek
  FILES:=$(LINUX_DIR)/drivers/net/mdio/mdio-realtek-otto-aux.ko
  AUTOLOAD:=$(call AutoLoad,20,mdio-realtek-otto-aux,1)
endef

define KernelPackage/mdio-realtek-otto-aux/description
  This driver adds support for Realtek RTL83xx series switching.
endef

$(eval $(call KernelPackage,mdio-realtek-otto-aux))

define KernelPackage/mdio-realtek-otto-serdes
  SUBMENU:=Realtek modules
  TITLE:=Realtek RTL838x/RTL839x switch support
  KCONFIG:= \
    CONFIG_MDIO_REALTEK_OTTO_SERDES
  DEPENDS:=@TARGET_realtek
  FILES:=$(LINUX_DIR)/drivers/net/mdio/mdio-realtek-otto-serdes.ko
  AUTOLOAD:=$(call AutoLoad,21,mdio-realtek-otto-serdes,1)
endef

define KernelPackage/mdio-realtek-otto-serdes/description
  This driver adds support for Realtek RTL83xx series switching.
endef

$(eval $(call KernelPackage,mdio-realtek-otto-serdes))

define KernelPackage/mdio-realtek-otto
  SUBMENU:=Realtek modules
  TITLE:=Realtek RTL838x/RTL839x switch support
  KCONFIG:= \
    CONFIG_MDIO_REALTEK_OTTO
  DEPENDS:=@TARGET_realtek
  FILES:=$(LINUX_DIR)/drivers/net/mdio/mdio-realtek-otto.ko
  AUTOLOAD:=$(call AutoLoad,22,mdio-realtek-otto,1)
endef
define KernelPackage/mdio-realtek-otto/description
  This driver adds support for Realtek RTL83xx series switching.
endef

$(eval $(call KernelPackage,mdio-realtek-otto))

define KernelPackage/pcs-rtl-otto
  SUBMENU:=Realtek modules
  TITLE:=Realtek RTL838x/RTL839x switch support
  KCONFIG:= \
    CONFIG_PCS_RTL_OTTO
  DEPENDS:=@TARGET_realtek +kmod-mdio-realtek-otto-serdes
  FILES:=$(LINUX_DIR)/drivers/net/pcs/pcs-rtl-otto.ko
  AUTOLOAD:=$(call AutoLoad,23,pcs-rtl-otto,1)
endef

define KernelPackage/pcs-rtl-otto/description
  This driver adds support for Realtek RTL83xx series switching.
endef

$(eval $(call KernelPackage,pcs-rtl-otto))

define KernelPackage/rtl838x_eth
  SUBMENU:=Realtek modules
  TITLE:=Realtek RTL838x/RTL839x switch support
  KCONFIG:= \
    CONFIG_NET_DSA=y \
    CONFIG_NET_RTL838X
  DEPENDS:=@TARGET_realtek
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/rtl838x_eth.ko
  AUTOLOAD:=$(call AutoLoad,24,rtl838x_eth,1)
endef

define KernelPackage/rtl838x_eth/description
  This driver adds support for Realtek RTL83xx series switching.
endef

$(eval $(call KernelPackage,rtl838x_eth))

define KernelPackage/dsa_rtl83xx
  SUBMENU:=Realtek modules
  TITLE:=Realtek RTL838x/RTL839x switch support
  KCONFIG:= \
    CONFIG_NET_DSA=y \
    CONFIG_NET_DSA_TAG_TRAILER=y \
    CONFIG_NET_DSA_RTL83XX
  # CONFIG_NET_DSA_RTL83XX_RTL930X_L3_OFFLOAD is not set
  DEPENDS:=@TARGET_realtek +kmod-rtl838x_eth +kmod-pcs-rtl-otto +kmod-mdio-realtek-otto
  FILES:=$(LINUX_DIR)/drivers/net/dsa/rtl83xx/dsa_rtl83xx.ko
  AUTOLOAD:=$(call AutoLoad,25,dsa_rtl83xx,1)
endef

define KernelPackage/dsa_rtl83xx/description
  This driver adds support for Realtek RTL83xx series switching.
endef

$(eval $(call KernelPackage,dsa_rtl83xx))