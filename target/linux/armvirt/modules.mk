define KernelPackage/acpi-mdio
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=ACPI MDIO support
  DEPENDS:=@(TARGET_armvirt_64) +kmod-libphy +kmod-mdio-devres
  KCONFIG:=CONFIG_ACPI_MDIO
  FILES:=$(LINUX_DIR)/drivers/net/mdio/acpi_mdio.ko
  AUTOLOAD:=$(call AutoLoad,11,acpi_mdio)
endef

define KernelPackage/acpi-mdio/description
 Kernel driver for ACPI MDIO support
endef

$(eval $(call KernelPackage,acpi-mdio))

define KernelPackage/fsl-pcs-lynx
  SUBMENU=$(NETWORK_DEVICES_MENU)
  DEPENDS:=@(TARGET_armvirt_64) +kmod-libphy +kmod-of-mdio +kmod-phylink
  TITLE:=NXP (Freescale) Lynx PCS
  HIDDEN:=1
  KCONFIG:=CONFIG_PCS_LYNX
  FILES=$(LINUX_DIR)/drivers/net/pcs/pcs-lynx.ko
  AUTOLOAD=$(call AutoLoad,30,pcs-lynx)
endef

$(eval $(call KernelPackage,fsl-pcs-lynx))

define KernelPackage/fsl-xgmac-mdio
  SUBMENU=$(NETWORK_DEVICES_MENU)
  DEPENDS:=@(TARGET_armvirt_64) +kmod-libphy +kmod-of-mdio +kmod-acpi-mdio
  TITLE:=NXP (Freescale) MDIO bus
  KCONFIG:=CONFIG_FSL_XGMAC_MDIO
  FILES=$(LINUX_DIR)/drivers/net/ethernet/freescale/xgmac_mdio.ko
  AUTOLOAD=$(call AutoLoad,30,xgmac_mdio)
endef

$(eval $(call KernelPackage,fsl-xgmac-mdio))

define KernelPackage/fsl-mc-dpio
  SUBMENU:=$(OTHER_MENU)
  TITLE:=NXP DPAA2 DPIO (Data Path IO) driver
  HIDDEN:=1
  KCONFIG:=CONFIG_FSL_MC_BUS=y \
    CONFIG_FSL_MC_DPIO
  FILES:=$(LINUX_DIR)/drivers/soc/fsl/dpio/fsl-mc-dpio.ko
  AUTOLOAD=$(call AutoLoad,30,fsl-mc-dpio)
endef

$(eval $(call KernelPackage,fsl-mc-dpio))

define KernelPackage/fsl-dpaa2-net
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=NXP DPAA2 Ethernet
  DEPENDS:=@(TARGET_armvirt_64) +kmod-fsl-xgmac-mdio +kmod-phylink \
    +kmod-fsl-pcs-lynx +kmod-fsl-mc-dpio
  KCONFIG:= \
    CONFIG_FSL_MC_UAPI_SUPPORT=y \
    CONFIG_FSL_DPAA2_ETH
  FILES:= \
  $(LINUX_DIR)/drivers/net/ethernet/freescale/dpaa2/fsl-dpaa2-eth.ko
  AUTOLOAD=$(call AutoLoad,35,fsl-dpaa2-eth)
endef

$(eval $(call KernelPackage,fsl-dpaa2-net))

define KernelPackage/fsl-dpaa2-console
  SUBMENU:=$(OTHER_MENU)
  TITLE:=NXP DPAA2 Debug console
  DEPENDS:=@(TARGET_armvirt_64)
  KCONFIG:=CONFIG_DPAA2_CONSOLE
  FILES=$(LINUX_DIR)/drivers/soc/fsl/dpaa2-console.ko
  AUTOLOAD=$(call AutoLoad,40,dpaa2-console)
endef

define KernelPackage/fsl-dpaa2-console/description
  Kernel modules for the NXP DPAA2 debug consoles
  (Management Complex and AIOP).
endef

$(eval $(call KernelPackage,fsl-dpaa2-console))

define KernelPackage/wdt-sp805
  SUBMENU:=$(OTHER_MENU)
  TITLE:=ARM SP805 Watchdog
  KCONFIG:=CONFIG_ARM_SP805_WATCHDOG
  FILES=$(LINUX_DIR)/drivers/watchdog/sp805_wdt.ko
  AUTOLOAD=$(call AutoLoad,50,sp805_wdt)
endef

define KernelPackage/wdt-sp805/description
  Support for the ARM SP805 wathchdog module.
  This is present in the NXP Layerscape family,
  HiSilicon HI3660 among others.
endef

$(eval $(call KernelPackage,wdt-sp805))
