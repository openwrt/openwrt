# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id:$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-switch
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/switch
  SUBMENU:=Other modules
  DEPENDS:=@LINUX_2_6_BRCM||LINUX_2_4_BRCM
  TITLE:=Switch drivers
  DESCRIPTION:=\
	This package contains switch drivers for ADM6996L and BCM53XX RoboSwitch.
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:= \
	$(PKG_BUILD_DIR)/switch-core.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/switch-adm.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/switch-robo.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,switch-core switch-robo switch-adm)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-DBCMGPIO2" \
		modules
endef
	
define KernelPackage/switch/install
	$(INSTALL_DIR) $(1)/lib/network/
	$(INSTALL_BIN) ./files/switch.sh $(1)/lib/network/
endef

$(eval $(call KernelPackage,switch))
