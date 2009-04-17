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
  TITLE:=Support for FireWire
  DEPENDS:=@PCI_SUPPORT
  KCONFIG:=CONFIG_IEEE1394
  FILES:=$(LINUX_DIR)/drivers/ieee1394/ieee1394.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ieee1394)
endef

define KernelPackage/ieee1394/description
 Kernel support for FireWire
endef

$(eval $(call KernelPackage,ieee1394))


define KernelPackage/ohci1394
  $(call firewiredep,)
  TITLE:=Support for OHCI-1394 controllers
  KCONFIG:= CONFIG_IEEE1394_OHCI1394
  FILES:=$(LINUX_DIR)/drivers/ieee1394/ohci1394.$(LINUX_KMOD_SUFFIX)
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
  FILES:=$(LINUX_DIR)/drivers/ieee1394/sbp2.$(LINUX_KMOD_SUFFIX)
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
  FILES:=$(LINUX_DIR)/drivers/ieee1394/raw1394.$(LINUX_KMOD_SUFFIX)
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
  FILES:=$(LINUX_DIR)/drivers/ieee1394/video1394.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,video1394)
endef

define KernelPackage/video1394/description
 Kernel support for FireWire video
endef

$(eval $(call KernelPackage,video1394))

