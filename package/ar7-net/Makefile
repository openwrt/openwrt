# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id:$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=avalanche-cpmac
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_MD5SUM:=ce46849a8f1055cef09c59c6bdb7f86a
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/kmod-avalanche-cpmac
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@LINUX_2_4_AR7 +kmod-atm
  DEFAULT:=y
  TITLE:=AR7 ADSL driver (Annex A)
  DESCRIPTION:=The AR7 ADSL driver for Annex A
  VERSION:=$(PKG_VERSION)+$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-DCONFIG_AVALANCHE_CPMAC_AUTO -DCONFIG_MIPS_CPMAC_INIT_BUF_MALLOC -DCONFIG_MIPS_CPMAC_PORTS=1" \
		modules
endef

define Package/kmod-avalanche-cpmac/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/avalanche_cpmac.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)
endef

$(eval $(call BuildPackage,kmod-avalanche-cpmac))
