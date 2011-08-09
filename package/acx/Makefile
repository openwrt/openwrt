# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=acx
PKG_VERSION:=20080210
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/acx100
PKG_MD5SUM:=7d5ce3215708e4e9f95cf567a9ee3a12

PKG_UNPACK:= \
	bzcat $(DL_DIR)/$(PKG_SOURCE) | \
	$(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS); \
	chmod -R u+w $(PKG_BUILD_DIR)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/acx
  TITLE:=Driver for TI ACX1xx chipset
  DEPENDS:=@PCI_SUPPORT||TARGET_ar7 +wireless-tools @!LINUX_3_1||BROKEN
  URL:=http://acx100.sourceforge.net/
  SUBMENU:=Wireless Drivers
  FILES:= $(PKG_BUILD_DIR)/acx.ko
  AUTOLOAD:=$(call AutoLoad,50,acx)
endef

define KernelPackage/acx/description
 This package contains a driver for TI ACX1xx 802.11a/b/g chipsets.
endef

C16_FW:=tiacx111c16
C19_FW:=tiacx111c19

define Download/tiacx111c16
  FILE:=$(C16_FW)
  URL:=http://teknoraver.net/software/acx-mac80211/
  MD5SUM:=7026826460376f6b174f9225bd7781b9
endef
$(eval $(call Download,tiacx111c16))

define Download/tiacx111c19
  FILE:=$(C19_FW)
  URL:=http://ipkg.k1k2.de/files/
  MD5SUM:=a1fa9681e297b4e36e257090fc12265a
endef
$(eval $(call Download,tiacx111c19))


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
	$(INSTALL_DATA) $(DL_DIR)/$(C16_FW) $(1)/lib/firmware/
	$(INSTALL_DATA) $(DL_DIR)/$(C19_FW) $(1)/lib/firmware/
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/lib/wifi/acx.sh $(1)/lib/wifi/
endef

$(eval $(call KernelPackage,acx))
