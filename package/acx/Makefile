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
PKG_VERSION:=20080210
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/acx100
PKG_MD5SUM:=7d5ce3215708e4e9f95cf567a9ee3a12

PKG_FW_C16_NAME:=tiacx111c16
PKG_FW_C16_URL:=http://teknoraver.campuslife.it/software/acx-mac80211/
PKG_FW_C16_MD5SUM:=7026826460376f6b174f9225bd7781b9

PKG_FW_C19_NAME:=tiacx111c19
PKG_FW_C19_URL:=http://ipkg.k1k2.de/files/
PKG_FW_C19_MD5SUM:=a1fa9681e297b4e36e257090fc12265a

PKG_UNPACK:= \
	bzcat $(DL_DIR)/$(PKG_SOURCE) | \
	$(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS); \
	chmod -R u+w $(PKG_BUILD_DIR)

include $(INCLUDE_DIR)/package.mk

# XXX: remove @!TARGET_* later when we have PCI support properly detected on all targets
define KernelPackage/acx
  TITLE:=Driver for TI ACX1xx chipset
  DEPENDS:=@LINUX_2_6 +wireless-tools @!TARGET_atheros @!TARGET_avr32 @!TARGET_uml
  URL:=http://acx100.sourceforge.net/
  SUBMENU:=Wireless Drivers
  FILES:= $(PKG_BUILD_DIR)/acx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,acx)
endef

define KernelPackage/acx/description
 This package contains a driver for TI ACX1xx 802.11a/b/g chipsets.
endef

$(STAMP_BUILT): $(DL_DIR)/$(PKG_FW_C16_NAME) $(DL_DIR)/$(PKG_FW_C19_NAME)

$(DL_DIR)/$(PKG_FW_C16_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW_C16_NAME)" "$(PKG_FW_C16_MD5SUM)" $(PKG_FW_C16_URL)

$(DL_DIR)/$(PKG_FW_C19_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW_C19_NAME)" "$(PKG_FW_C19_MD5SUM)" $(PKG_FW_C19_URL)

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
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_FW_C16_NAME) $(1)/lib/firmware/
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_FW_C19_NAME) $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,acx))
