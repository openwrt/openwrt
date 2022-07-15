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

define KernelPackage/fsl-enetc-net
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=:NXP ENETC (LS1028A) Ethernet
  DEPENDS:=@(TARGET_armvirt_64) +kmod-phylink +kmod-fsl-pcs-lynx
  KCONFIG:= \
    CONFIG_FSL_ENETC \
    CONFIG_FSL_ENETC_VF \
    CONFIG_FSL_ENETC_QOS
  FILES:= \
    $(LINUX_DIR)/drivers/net/ethernet/freescale/enetc/fsl-enetc.ko \
    $(LINUX_DIR)/drivers/net/ethernet/freescale/enetc/fsl-enetc-vf.ko \
    $(LINUX_DIR)/drivers/net/ethernet/freescale/enetc/fsl-enetc-mdio.ko \
    $(LINUX_DIR)/drivers/net/ethernet/freescale/enetc/fsl-enetc-ierb.ko
  AUTOLOAD=$(call AutoLoad,35,fsl-enetc)
endef

$(eval $(call KernelPackage,fsl-enetc-net))

define KernelPackage/fsl-dpaa1-net
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=NXP DPAA1 (LS1043/LS1046) Ethernet
  DEPENDS:=@(TARGET_armvirt_64) +kmod-fsl-xgmac-mdio +kmod-libphy +kmod-crypto-crc32
  KCONFIG:= \
    CONFIG_FSL_DPAA=y \
    CONFIG_FSL_DPAA_ETH \
    CONFIG_FSL_FMAN \
    CONFIG_FSL_DPAA_CHECKING=n \
    CONFIG_FSL_BMAN_TEST=n \
    CONFIG_FSL_QMAN_TEST=n
  MODULES:= \
    $(LINUX_DIR)/drivers/net/ethernet/freescale/dpaa/fsl_dpa.ko \
    $(LINUX_DIR)/drivers/net/ethernet/freescale/fman/fsl_dpaa_fman.ko \
    $(LINUX_DIR)/drivers/net/ethernet/freescale/fman/fsl_dpaa_mac.ko
  AUTOLOAD=$(call AutoLoad,35,fsl-dpa)
endef

$(eval $(call KernelPackage,fsl-dpaa1-net))

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

define KernelPackage/marvell-mdio
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Marvell Armada platform MDIO driver
  DEPENDS:=@(TARGET_armvirt_64) +kmod-libphy +kmod-of-mdio +kmod-acpi-mdio
  KCONFIG:=CONFIG_MVMDIO
  FILES=$(LINUX_DIR)/drivers/net/ethernet/marvell/mvmdio.ko
  AUTOLOAD=$(call AutoLoad,30,marvell-mdio)
endef

$(eval $(call KernelPackage,marvell-mdio))

define KernelPackage/phy-marvell-10g
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Marvell Alaska 10G PHY driver
  DEPENDS:=@(TARGET_armvirt_64) +kmod-libphy
  KCONFIG:=CONFIG_MARVELL_10G_PHY
  FILES=$(LINUX_DIR)/drivers/net/phy/marvell10g.ko
  AUTOLOAD=$(call AutoLoad,35,marvell10g)
endef

$(eval $(call KernelPackage,phy-marvell-10g))

define KernelPackage/mvneta
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Marvell Armada 370/38x/XP/37xx network driver
  DEPENDS:=@(TARGET_armvirt_64) +kmod-marvell-mdio +kmod-phylink
  KCONFIG:=CONFIG_MVNETA
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/marvell/mvneta.ko
  AUTOLOAD=$(call AutoLoad,40,mvneta)
endef

$(eval $(call KernelPackage,mvneta))

define KernelPackage/mvpp2
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Marvell Armada 375/7K/8K network driver
  DEPENDS:=@(TARGET_armvirt_64) +kmod-marvell-mdio +kmod-phylink
  KCONFIG:=CONFIG_MVPP2 \
    CONFIG_MVPP2_PTP=n
  FILES=$(LINUX_DIR)/drivers/net/ethernet/marvell/mvpp2/mvpp2.ko
  AUTOLOAD=$(call AutoLoad,40,mvpp2)
endef

$(eval $(call KernelPackage,mvpp2))

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

