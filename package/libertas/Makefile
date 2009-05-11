# 
# Copyright (C) 2007 - 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-libertas
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

# XXX: current version needs 'get_unaligned_le16' helper introduced in 2.6.26
define KernelPackage/libertas
  SUBMENU:=Other modules
  DEPENDS:=+kmod-ieee80211 @LINUX_2_6 @!LINUX_2_6_21||!LINUX_2_6_23||!LINUX_2_6_24||!LINUX_2_6_25
  TITLE:=Marvell 88W8015 Wireless Driver
  FILES:= \
	$(PKG_BUILD_DIR)/libertas.$(LINUX_KMOD_SUFFIX)  \
	$(PKG_BUILD_DIR)/usb8xxx.$(LINUX_KMOD_SUFFIX) 
  AUTOLOAD:=$(call AutoLoad,30,libertas usb8xxx)
endef

define Download/firmware
  URL:=http://dev.laptop.org/pub/firmware/libertas/
  FILE:=usb8388-5.110.22.p20.bin
  #MD5SUM=37cc814d5a475fcf8f8fbe89a9c5d546
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		CONFIG_LIBERTAS=m \
		CONFIG_LIBERTAS_USB=m \
		EXTRA_CFLAGS="-I$(PKG_BUILD_DIR) -DCONFIG_LIBERTAS_DEBUG -I$(STAGING_DIR)/usr/include/mac80211" \
		modules 
endef

define KernelPackage/libertas/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_BIN) $(DL_DIR)/usb8388-5.110.22.p20.bin $(1)/lib/firmware/usb8388.bin
	$(INSTALL_DATA) ./files/LICENSE $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,libertas))
$(eval $(call Download,firmware))
