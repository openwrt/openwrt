# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=acx
PKG_VERSION:=20080112
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/acx100
PKG_MD5SUM:=b376b5359ef15da589ef17387a6b37d3

PKG_FW_NAME:=tiacx111c16
PKG_FW_URL:=http://teknoraver.campuslife.it/software/acx-mac80211/
PKG_FW_MD5SUM:=7026826460376f6b174f9225bd7781b9

PKG_UNPACK:= \
	bzcat $(DL_DIR)/$(PKG_SOURCE) | \
	$(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS); \
	chmod -R u+w $(PKG_BUILD_DIR)

include $(INCLUDE_DIR)/package.mk

# XXX: remove @!TARGET_* later when we have PCI support properly detected on all targets
define KernelPackage/acx
  TITLE:=Driver for TI ACX1xx chipset
  DEPENDS:=@LINUX_2_6 +wireless-tools @!TARGET_atheros @!TARGET_avr32
  URL:=http://acx100.sourceforge.net/
  SUBMENU:=Wireless Drivers
  FILES:= $(PKG_BUILD_DIR)/acx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,acx)
endef

define KernelPackage/acx/description
 This package contains a driver for TI ACX1xx 802.11a/b/g chipsets.
endef

$(STAMP_BUILT): $(DL_DIR)/$(PKG_FW_NAME)

$(DL_DIR)/$(PKG_FW_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW_NAME)" "$(PKG_FW_MD5SUM)" $(PKG_FW_URL)

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CC="$(TARGET_CC)" \
		CPP="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SOURCE="$(LINUX_DIR)" \
		KDIR="$(LINUX_DIR)" \
		CONFIG_ACX=m \
		CONFIG_ACX_PCI=m \
		modules
endef

define KernelPackage/acx/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_FW_NAME) $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,acx))
