#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

NETWORK_DEVICES_MENU:=Network Devices

define KernelPackage/libphy
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=PHY library
  KCONFIG:=CONFIG_PHYLIB
  FILES:=$(LINUX_DIR)/drivers/net/phy/libphy.ko
  AUTOLOAD:=$(call AutoLoad,40,libphy)
endef

define KernelPackage/libphy/description
  PHY library
endef

$(eval $(call KernelPackage,libphy))

define KernelPackage/swconfig
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=switch configuration API
  DEPENDS:=+kmod-libphy
  KCONFIG:=CONFIG_SWCONFIG
  FILES:=$(LINUX_DIR)/drivers/net/phy/swconfig.ko
  AUTOLOAD:=$(call AutoLoad,41,swconfig)
endef

define KernelPackage/swconfig/description
  Switch configuration API module
endef

$(eval $(call KernelPackage,swconfig))

define KernelPackage/mvswitch
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Marvell 88E6060 switch support
  DEPENDS:=+kmod-swconfig
  KCONFIG:=CONFIG_MVSWITCH_PHY
  FILES:=$(LINUX_DIR)/drivers/net/phy/mvswitch.ko
  AUTOLOAD:=$(call AutoLoad,41,mvswitch)
endef

define KernelPackage/mvswitch/description
  Marvell 88E6060 switch support
endef

$(eval $(call KernelPackage,mvswitch))

define KernelPackage/switch-ip17xx
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=IC+ IP17XX switch support
  DEPENDS:=+kmod-swconfig
  KCONFIG:=CONFIG_IP17XX_PHY
  FILES:=$(LINUX_DIR)/drivers/net/phy/ip17xx.ko
  AUTOLOAD:=$(call AutoLoad,42,ip17xx)
endef

define KernelPackage/switch-ip17xx/description
  IC+ IP175C/IP178C switch support
endef

$(eval $(call KernelPackage,switch-ip17xx))

define KernelPackage/natsemi
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=National Semiconductor DP8381x series
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_NATSEMI
  FILES:=$(LINUX_DIR)/drivers/net/natsemi.ko
  AUTOLOAD:=$(call AutoLoad,20,natsemi)
endef

define KernelPackage/natsemi/description
 Kernel modules for National Semiconductor DP8381x series PCI Ethernet
 adapters.
endef

$(eval $(call KernelPackage,natsemi))


define KernelPackage/r6040
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=RDC Fast-Ethernet support
  DEPENDS:=@TARGET_rdc
  KCONFIG:=CONFIG_R6040 \
		CONFIG_R6040_NAPI=y
  FILES:=$(LINUX_DIR)/drivers/net/r6040.ko
  AUTOLOAD:=$(call AutoLoad,99,r6040)
endef

define KernelPackage/r6040/description
 Kernel modules for RDC Fast-Ethernet adapters.
endef

$(eval $(call KernelPackage,r6040))


define KernelPackage/sis900
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=SiS 900 Ethernet support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SIS900
  FILES:=$(LINUX_DIR)/drivers/net/sis900.ko
  AUTOLOAD:=$(call AutoLoad,50,sis900)
endef

define KernelPackage/sis900/description
 Kernel modules for Sis 900 Ethernet adapters.
endef

$(eval $(call KernelPackage,sis900))


define KernelPackage/sky2
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=SysKonnect Yukon2 support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SKY2
  FILES:=$(LINUX_DIR)/drivers/net/sky2.ko
  AUTOLOAD:=$(call AutoLoad,50,sky2)
endef

define KernelPackage/sky2/description
  This driver supports Gigabit Ethernet adapters based on the
  Marvell Yukon 2 chipset:
  Marvell 88E8021/88E8022/88E8035/88E8036/88E8038/88E8050/88E8052/
  88E8053/88E8055/88E8061/88E8062, SysKonnect SK-9E21D/SK-9S21

  There is companion driver for the older Marvell Yukon and
  Genesis based adapters: skge.
endef

$(eval $(call KernelPackage,sky2))


define KernelPackage/via-rhine
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Via Rhine ethernet support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_VIA_RHINE \
    CONFIG_VIA_RHINE_MMIO=y
  FILES:=$(LINUX_DIR)/drivers/net/via-rhine.ko
  AUTOLOAD:=$(call AutoLoad,50,via-rhine)
endef

define KernelPackage/via-rhine/description
 Kernel modules for Via Rhine Ethernet chipsets.
endef

$(eval $(call KernelPackage,via-rhine))


define KernelPackage/via-velocity
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=VIA Velocity Gigabit Ethernet Adapter kernel support
  DEPENDS:=@TARGET_ixp4xx||TARGET_mpc83xx||TARGET_x86
  KCONFIG:=CONFIG_VIA_VELOCITY
  FILES:=$(LINUX_DIR)/drivers/net/via-velocity.ko
  AUTOLOAD:=$(call AutoLoad,50,via-velocity)
endef

define KernelPackage/via-velocity/description
 Kernel modules for VIA Velocity Gigabit Ethernet chipsets.
endef

$(eval $(call KernelPackage,via-velocity))

define KernelPackage/ixp4xx-eth
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=IXP4xxt Ethernet Adapter kernel support
  DEPENDS:=@TARGET_ixp4xx
  KCONFIG:=CONFIG_IXP4XX_ETH
  FILES:=$(LINUX_DIR)/drivers/net/arm/ixp4xx_eth.ko
  AUTOLOAD:=$(call AutoLoad,50,ixp4xx_eth)
endef

define KernelPackage/ixp4xx-eth/description
 Kernel modules for Intel IXP4xx Ethernet chipsets.
endef

$(eval $(call KernelPackage,ixp4xx-eth))

define KernelPackage/8139too
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=RealTek RTL-8139 PCI Fast Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_8139TOO \
    CONFIG_8139TOO_PIO=y \
    CONFIG_8139TOO_TUNE_TWISTER=n \
    CONFIG_8139TOO_8129=n \
    CONFIG_8139_OLD_RX_RESET=n
  FILES:=$(LINUX_DIR)/drivers/net/8139too.ko
  AUTOLOAD:=$(call AutoLoad,50,8139too)
endef

define KernelPackage/8139too/description
 Kernel modules for RealTek RTL-8139 PCI Fast Ethernet adapters.
endef

$(eval $(call KernelPackage,8139too))


define KernelPackage/8139cp
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=RealTek RTL-8139C+ PCI Fast Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_8139CP
  FILES:=$(LINUX_DIR)/drivers/net/8139cp.ko
  AUTOLOAD:=$(call AutoLoad,50,8139cp)
endef

define KernelPackage/8139cp/description
 Kernel module for RealTek RTL-8139C+ PCI Fast Ethernet adapters.
endef

$(eval $(call KernelPackage,8139cp))


define KernelPackage/r8169
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=RealTek RTL-8169 PCI Gigabit Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_R8169 \
    CONFIG_R8169_NAPI=y \
    CONFIG_R8169_VLAN=n
  FILES:=$(LINUX_DIR)/drivers/net/r8169.ko
  AUTOLOAD:=$(call AutoLoad,50,r8169)
endef

define KernelPackage/r8169/description
 Kernel modules for RealTek RTL-8169 PCI Gigabit Ethernet adapters.
endef

$(eval $(call KernelPackage,r8169))


define KernelPackage/ne2k-pci
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=ne2k-pci Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_NE2K_PCI
  FILES:= \
	$(LINUX_DIR)/drivers/net/ne2k-pci.ko \
	$(LINUX_DIR)/drivers/net/8390.ko
  AUTOLOAD:=$(call AutoLoad,50,8390 ne2k-pci)
endef

define KernelPackage/ne2k-pci/description
 Kernel modules for NE2000 PCI Ethernet Adapter kernel.
endef

$(eval $(call KernelPackage,ne2k-pci))


define KernelPackage/e100
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Intel(R) PRO/100+ cards kernel support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_E100
  FILES:=$(LINUX_DIR)/drivers/net/e100.ko
  AUTOLOAD:=$(call AutoLoad,50,e100)
endef

define KernelPackage/e100/description
 Kernel modules for Intel(R) PRO/100+ Ethernet adapters.
endef

$(eval $(call KernelPackage,e100))


define KernelPackage/e1000
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Intel(R) PRO/1000 PCI cards kernel support
  DEPENDS:=@PCI_SUPPORT
  KCONFIG:=CONFIG_E1000 \
    CONFIG_E1000_DISABLE_PACKET_SPLIT=n \
    CONFIG_E1000_NAPI=y
  FILES:=$(LINUX_DIR)/drivers/net/e1000/e1000.ko
  AUTOLOAD:=$(call AutoLoad,35,e1000)
endef

define KernelPackage/e1000/description
 Kernel modules for Intel(R) PRO/1000 PCI Ethernet adapters.
endef

$(eval $(call KernelPackage,e1000))


define KernelPackage/e1000e
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Intel(R) PRO/1000 PCIe cards kernel support
  DEPENDS:=@PCIE_SUPPORT
  KCONFIG:=CONFIG_E1000E
  FILES:=$(LINUX_DIR)/drivers/net/e1000e/e1000e.ko
  AUTOLOAD:=$(call AutoLoad,50,e1000e)
endef

define KernelPackage/e1000e/description
 Kernel modules for Intel(R) PRO/1000 PCIe Ethernet adapters.
endef

$(eval $(call KernelPackage,e1000e))


define KernelPackage/b44
  TITLE:=Broadcom 44xx driver
  FILES:=$(LINUX_DIR)/drivers/net/b44.ko
  KCONFIG:=CONFIG_B44
  DEPENDS:=@TARGET_x86 +kmod-ssb
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  AUTOLOAD:=$(call AutoLoad,50,b44)
endef

define KernelPackage/b44/description
 Kernel modules for Broadcom 44xx Ethernet adapters.
endef

$(eval $(call KernelPackage,b44))


define KernelPackage/3c59x
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=3Com 3c590/3c900 series (592/595/597) Vortex/Boomerang
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_VORTEX
  FILES:=$(LINUX_DIR)/drivers/net/3c59x.ko
  AUTOLOAD:=$(call AutoLoad,50,3c59x)
endef

define KernelPackage/3c59x/description
 This option enables driver support for a large number of 10mbps and
 10/100mbps EISA, PCI and PCMCIA 3Com Ethernet adapters:
 - "Vortex"    (Fast EtherLink 3c590/3c592/3c595/3c597) EISA and PCI
 - "Boomerang" (EtherLink XL 3c900 or 3c905)            PCI
 - "Cyclone"   (3c540/3c900/3c905/3c980/3c575/3c656)    PCI and Cardbus
 - "Tornado"   (3c905)                                  PCI
 - "Hurricane" (3c555/3cSOHO)                           PCI
endef

$(eval $(call KernelPackage,3c59x))


define KernelPackage/pcnet32
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=AMD PCnet32 PCI support
  DEPENDS:=@(TARGET_x86||TARGET_malta)
  KCONFIG:=CONFIG_PCNET32
  FILES:=$(LINUX_DIR)/drivers/net/pcnet32.ko
  AUTOLOAD:=$(call AutoLoad,50,pcnet32)
endef

define KernelPackage/pcnet32/description
 Kernel modules for AMD PCnet32 Ethernet adapters.
endef

$(eval $(call KernelPackage,pcnet32))


define KernelPackage/tg3
  TITLE:=Broadcom Tigon3 Gigabit Ethernet
  FILES:=$(LINUX_DIR)/drivers/net/tg3.ko
  KCONFIG:=CONFIG_TIGON3
  DEPENDS:=+!TARGET_brcm47xx:kmod-libphy @!TARGET_ubicom32
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  AUTOLOAD:=$(call AutoLoad,50,tg3)
endef

define KernelPackage/tg3/description
 Kernel modules for Broadcom Tigon3 Gigabit Ethernet adapters.
endef

$(eval $(call KernelPackage,tg3))


define KernelPackage/ssb-gige
  TITLE:=Broadcom SSB Gigabit Ethernet
  KCONFIG:=CONFIG_SSB_DRIVER_GIGE=y
  DEPENDS:=@TARGET_brcm47xx +kmod-tg3
  SUBMENU:=$(NETWORK_DEVICES_MENU)
endef

define KernelPackage/ssb-gige/description
 Kernel modules for Broadcom SSB Gigabit Ethernet adapters.
endef

$(eval $(call KernelPackage,ssb-gige))

define KernelPackage/hfcmulti
  TITLE:=HFC multiport cards (HFC-4S/8S/E1)
  KCONFIG:=CONFIG_MISDN_HFCMULTI
  DEPENDS:=+kmod-misdn
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  FILES:=$(LINUX_DIR)/drivers/isdn/hardware/mISDN/hfcmulti.ko
  AUTOLOAD:=$(call AutoLoad,31,hfcmulti)
endef

define KernelPackage/hfcmulti/description
  HFC multiport cards (HFC-4S/8S/E1) support
endef

$(eval $(call KernelPackage,hfcmulti))


define KernelPackage/gigaset
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Siemens Gigaset support (isdn)
  DEPENDS:=@USB_SUPPORT +kmod-isdn4linux +kmod-crc-ccitt +kmod-usb-core
  URL:=http://gigaset307x.sourceforge.net/
  KCONFIG:= \
    CONFIG_ISDN_DRV_GIGASET \
    CONFIG_GIGASET_BASE \
    CONFIG_GIGASET_M101 \
    CONFIG_GIGASET_M105 \
    CONFIG_GIGASET_UNDOCREQ=y \
    CONFIG_GIGASET_I4L=y
  FILES:= \
    $(LINUX_DIR)/drivers/isdn/gigaset/gigaset.ko \
    $(LINUX_DIR)/drivers/isdn/gigaset/bas_gigaset.ko \
    $(LINUX_DIR)/drivers/isdn/gigaset/ser_gigaset.ko \
    $(LINUX_DIR)/drivers/isdn/gigaset/usb_gigaset.ko
  AUTOLOAD:=$(call AutoLoad,50,gigaset bas_gigaset ser_gigaset usb_gigaset)
endef

define KernelPackage/gigaset/description
 This driver supports the Siemens Gigaset SX205/255 family of
 ISDN DECT bases, including the predecessors Gigaset 3070/3075
 and 4170/4175 and their T-Com versions Sinus 45isdn and Sinus
 721X.
endef

$(eval $(call KernelPackage,gigaset))


define KernelPackage/macvlan
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=MAC-VLAN support
  KCONFIG:=CONFIG_MACVLAN
  FILES:=$(LINUX_DIR)/drivers/net/macvlan.ko
  AUTOLOAD:=$(call AutoLoad,50,macvlan)
endef

define KernelPackage/macvlan/description
 A kernel module which allows one to create virtual interfaces that
 map packets to or from specific MAC addresses to a particular interface.
endef

$(eval $(call KernelPackage,macvlan))

define KernelPackage/tulip
  TITLE:=Tulip family network device support
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  KCONFIG:= \
    CONFIG_NET_TULIP=y \
    CONFIG_DE2104X \
    CONFIG_DE2104X_DSL=0 \
    CONFIG_TULIP \
    CONFIG_TULIP_MWI=y \
    CONFIG_TULIP_MMIO=y \
    CONFIG_TULIP_NAPI=y \
    CONFIG_TULIP_NAPI_HW_MITIGATION=y \
    CONFIG_DE4X5 \
    CONFIG_WINBOND_840 \
    CONFIG_DM9102 \
    CONFIG_ULI526X 
  FILES:= \
    $(LINUX_DIR)/drivers/net/tulip/tulip.ko \
    $(LINUX_DIR)/drivers/net/tulip/de2104x.ko \
    $(LINUX_DIR)/drivers/net/tulip/de4x5.ko \
    $(LINUX_DIR)/drivers/net/tulip/dmfe.ko \
    $(LINUX_DIR)/drivers/net/tulip/uli526x.ko \
    $(LINUX_DIR)/drivers/net/tulip/winbond-840.ko
  AUTOLOAD:=$(call AutoLoad,50,tulip)
endef

define KernelPackage/tulip/description
 Kernel modules for the Tulip family of network cards,
 including DECchip Tulip, DIGITAL EtherWORKS, Winbond W89c840,
 Davicom DM910x/DM980x and ULi M526x controller support.
endef

$(eval $(call KernelPackage,tulip))


define KernelPackage/solos-pci
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Solos ADSL2+ multiport modem
  DEPENDS:=@TARGET_x86 +kmod-atm +soloscli
  KCONFIG:=CONFIG_ATM_SOLOS
  FILES:=$(LINUX_DIR)/drivers/atm/solos-pci.ko
  AUTOLOAD:=$(call AutoLoad,50,solos-pci)
endef

define KernelPackage/solos-pci/description
 Kernel module for Traverse Technologies' Solos PCI cards
 and Geos ADSL2+ x86 motherboard.
endef

$(eval $(call KernelPackage,solos-pci))

define KernelPackage/dummy
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Dummy network device
  KCONFIG:=CONFIG_DUMMY
  FILES:=$(LINUX_DIR)/drivers/net/dummy.ko
  AUTOLOAD:=$(call AutoLoad,34,dummy)
endef

define KernelPackage/dummy/description
  The dummy network device
endef

$(eval $(call KernelPackage,dummy))
