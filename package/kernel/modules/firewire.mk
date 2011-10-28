#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

FIREWIRE_MENU:=FireWire support

define firewiredep
  SUBMENU:=$(FIREWIRE_MENU)
  DEPENDS:=kmod-ieee1394 $(1)
endef


define KernelPackage/ieee1394
  SUBMENU:=$(FIREWIRE_MENU)
  TITLE:=Support for FireWire (old stack)
  DEPENDS:=@PCI_SUPPORT @LINUX_2_6_30||LINUX_2_6_31||LINUX_2_6_32||LINUX_2_6_36
  KCONFIG:=CONFIG_IEEE1394
  FILES:=$(LINUX_DIR)/drivers/ieee1394/ieee1394.ko
  AUTOLOAD:=$(call AutoLoad,20,ieee1394)
endef

define KernelPackage/ieee1394/description
 Kernel support for FireWire (old stack)
endef

$(eval $(call KernelPackage,ieee1394))


define KernelPackage/ohci1394
  $(call firewiredep,)
  TITLE:=Support for OHCI-1394 controllers
  KCONFIG:= CONFIG_IEEE1394_OHCI1394
  FILES:=$(LINUX_DIR)/drivers/ieee1394/ohci1394.ko
  AUTOLOAD:=$(call AutoLoad,50,ohci1394)
endef

define KernelPackage/ohci1394/description
 Kernel support for FireWire OHCI-1394 controllers
endef

$(eval $(call KernelPackage,ohci1394))


define KernelPackage/sbp2
  $(call firewiredep,kmod-ohci1394)
  TITLE:=Support for SBP-2 devices over FireWire
  KCONFIG:= \
  	CONFIG_IEEE1394_SBP2 \
	CONFIG_IEEE1394_SBP2_PHYS_DMA=n
  FILES:=$(LINUX_DIR)/drivers/ieee1394/sbp2.ko
  AUTOLOAD:=$(call AutoLoad,50,sbp2)
endef

define KernelPackage/sbp2/description
 Kernel support for SBP-2 devices over FireWire
endef

$(eval $(call KernelPackage,sbp2))


define KernelPackage/raw1394
  $(call firewiredep,kmod-ohci1394)
  TITLE:=Support for Raw I/O for FireWire devices
  KCONFIG:=CONFIG_IEEE1394_RAWIO
  FILES:=$(LINUX_DIR)/drivers/ieee1394/raw1394.ko
  AUTOLOAD:=$(call AutoLoad,50,raw1394)
endef

define KernelPackage/raw1394/description
 Kernel support for FireWire Raw I/O
endef

$(eval $(call KernelPackage,raw1394))


define KernelPackage/video1394
  $(call firewiredep,kmod-ohci1394)
  TITLE:=Support for FireWire video
  KCONFIG:=CONFIG_IEEE1394_VIDEO1394
  FILES:=$(LINUX_DIR)/drivers/ieee1394/video1394.ko
  AUTOLOAD:=$(call AutoLoad,50,video1394)
endef

define KernelPackage/video1394/description
 Kernel support for FireWire video
endef

$(eval $(call KernelPackage,video1394))


define KernelPackage/firewire
  SUBMENU:=$(FIREWIRE_MENU)
  TITLE:=Support for FireWire (new stack)
  DEPENDS:=@PCI_SUPPORT
  KCONFIG:=CONFIG_FIREWIRE
  FILES:=$(LINUX_DIR)/drivers/firewire/firewire-core.ko
  AUTOLOAD:=$(call AutoLoad,20,firewire-core)
endef

define KernelPackage/firewire/description
  Kernel support for FireWire (new stack)
endef

$(eval $(call KernelPackage,firewire))


define KernelPackage/firewire-ohci
  SUBMENU:=$(FIREWIRE_MENU)
  TITLE:=Support for OHCI-1394 controllers
  DEPENDS:=kmod-firewire
  KCONFIG:= \
	CONFIG_FIREWIRE_OHCI \
	CONFIG_FIREWIRE_OHCI_DEBUG=n \
	CONFIG_FIREWIRE_OHCI_REMOTE_DMA=n
  FILES:=$(LINUX_DIR)/drivers/firewire/firewire-ohci.ko
  AUTOLOAD:=$(call AutoLoad,50,firewire-ohci)
endef


define KernelPackage/firewire-ohci/description
 Kernel support for FireWire OHCI-1394 controllers
endef

$(eval $(call KernelPackage,firewire-ohci))


define KernelPackage/firewire-sbp2
  SUBMENU:=$(FIREWIRE_MENU)
  TITLE:=Support for SBP-2 devices over FireWire
  DEPENDS:=kmod-firewire +kmod-scsi-core
  KCONFIG:=CONFIG_FIREWIRE_SBP2
  FILES:=$(LINUX_DIR)/drivers/firewire/firewire-sbp2.ko
  AUTOLOAD:=$(call AutoLoad,50,firewire-sbp2)
endef

define KernelPackage/firewire-sbp2/description
 Kernel support for SBP-2 devices over FireWire
endef

$(eval $(call KernelPackage,firewire-sbp2))


define KernelPackage/firewire-net
  SUBMENU:=$(FIREWIRE_MENU)
  TITLE:=Support for IP networking over FireWire
  DEPENDS:=kmod-firewire @!LINUX_2_6_30
  KCONFIG:=CONFIG_FIREWIRE_NET
  FILES:=$(LINUX_DIR)/drivers/firewire/firewire-net.ko
  AUTOLOAD:=$(call AutoLoad,50,firewire-net)
endef

define KernelPackage/firewire-net/description
 Kernel support for IPv4 over FireWire
endef

$(eval $(call KernelPackage,firewire-net))

