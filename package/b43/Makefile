#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
# 
# $Id: Makefile 7440 2007-06-02 02:22:01Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=b43
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/b43
  SUBMENU:=Wireless Drivers
  TITLE:=Broadcom 43xx wireless support
  DEPENDS:=@TARGET_brcm47xx +kmod-mac80211 @BROKEN
  FILES:=$(PKG_BUILD_DIR)/b43.$(LINUX_KMOD_SUFFIX)
#  AUTOLOAD:=$(call AutoLoad,30,b43)
endef

define KernelPackage/b43/description
Kernel module for Broadcom 43xx wireless support (mac80211)
endef

EXTRA_KCONFIG:= \
	CONFIG_B43=m \
	CONFIG_B43_DMA=y \
	$(if $(CONFIG_LEDS_TRIGGERS),CONFIG_B43_LEDS=y) \


EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	LINUXINCLUDE="-I$(STAGING_DIR)/usr/include/mac80211 -I$(LINUX_DIR)/include -include linux/autoconf.h" \
	$(EXTRA_KCONFIG)

ifneq ($(findstring 2.6.23,$(LINUX_VERSION)),)
  define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
  endef

  define Build/Configure
  endef

  define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
  endef
else
  override CONFIG_PACKAGE_kmod-b43=
endif

$(eval $(call KernelPackage,b43))
