#
# Copyright (C) 2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=vrx518_ep
PKG_VERSION:=2.1.0
PKG_RELEASE:=$(AUTORELEASE)
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

# TODO this driver depends on the vrx518 aca firmware, add this dependency if
# that ever gets a compatible license
define KernelPackage/vrx518_ep
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=VRX518 EP Support
  DEPENDS:=@TARGET_ipq40xx
  AUTOLOAD:=$(call AutoLoad,26,vrx518)
  FILES:=$(PKG_BUILD_DIR)/vrx518.ko
endef

define KernelPackage/vrx518_ep/description
  VRX518 endpoint driver
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/net/
	$(CP) $(PKG_BUILD_DIR)/include/net/dc_ep.h $(1)/usr/include/net/
endef

EXTRA_KCONFIG:= \
	CONFIG_VRX518=m
#	CONFIG_TEST=m
#	CONFIG_VRX518_PCIE_SWITCH_BONDING=y

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \
	-I$(PKG_BUILD_DIR)/include

define Build/Compile
	$(KERNEL_MAKE) \
		M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		$(EXTRA_KCONFIG) \
		modules
endef

$(eval $(call KernelPackage,vrx518_ep))
