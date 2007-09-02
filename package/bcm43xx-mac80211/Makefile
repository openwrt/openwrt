#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
# 
# $Id: Makefile 7440 2007-06-02 02:22:01Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=bcm43xx-mac80211
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
  include $(LINUX_DIR)/.config
endif

define KernelPackage/bcm43xx-mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=Broadcom 43xx wireless support
  DESCRIPTION:=Kernel module for Broadcom 43xx wireless support (mac80211)
  DEPENDS:=@LINUX_2_6_BRCM47XX +kmod-mac80211
  KCONFIG:=CONFIG_MAC80211
  FILES:=$(PKG_BUILD_DIR)/bcm43xx-mac80211.$(LINUX_KMOD_SUFFIX)
#  AUTOLOAD:=$(call AutoLoad,30,bcm43xx-mac80211)
endef

ifneq ($(CONFIG_MAC80211),)

  PKG_EXTRA_KCONFIG:= \
	CONFIG_BCM43XX_MAC80211=m \
	CONFIG_BCM43XX_MAC80211_PCI=y \
	CONFIG_BCM43XX_MAC80211_DEBUG=y \
	CONFIG_BCM43XX_MAC80211_DMA=y \
	CONFIG_BCM43XX_MAC80211_PIO=y \
	CONFIG_BCM43XX_MAC80211_DMA_AND_PIO_MODE=y \

  PKG_EXTRA_CFLAGS:= \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

  define Build/Compile/it
	$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS)" \
		V="$(V)" \
		modules
  endef

endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/bcm43xx/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
$(call Build/Compile/it)
endef

$(eval $(call KernelPackage,bcm43xx-mac80211))
