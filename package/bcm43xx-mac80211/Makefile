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

# This horrible, horrible hack is because I can't work out ow to add
# CONFIG_* definitions to a kernel config from an external module dir.
# It should come from a proper configuration based on the Kconfig in the src
# directory.
MAKE_CONFIGS=CONFIG_BCM43XX_MAC80211=m CONFIG_BCM43XX_MAC80211_PCI=y \
	CONFIG_BCM43XX_MAC80211_DEBUG=y CONFIG_BCM43XX_MAC80211_DMA=y \
	CONFIG_BCM43XX_MAC80211_PIO=y CONFIG_BCM43XX_MAC80211_DMA_AND_PIO_MODE=y

BUILDFLAGS+=$(patsubst CONFIG_%, -DCONFIG_%, $(MAKE_CONFIGS))

define KernelPackage/bcm43xx-mac80211
  TITLE:=Broadcom 43xx wireless support
  DESCRIPTION:=Kernel module for Broadcom 43xx wireless support (mac80211)
  VERSION:=$(PKG_RELEASE)+$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)
  DEPENDS:=@LINUX_2_6_BRCM47XX +kmod-mac80211
  SUBMENU:=Wireless Drivers
  ifneq ($(KERNEL_PATCHVER),2.6.21)
#  AUTOLOAD:=$(call AutoLoad,30,bcm43xx-mac80211)
   FILES:=$(PKG_BUILD_DIR)/bcm43xx-mac80211.$(LINUX_KMOD_SUFFIX)
  endif
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/bcm43xx/* $(PKG_BUILD_DIR)/
endef

ifneq ($(KERNEL_PATCHVER),2.6.21)
  define Build/Compile
	$(MAKE) $(MAKE_CONFIGS) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
  endef
endif

$(eval $(call KernelPackage,bcm43xx-mac80211))
