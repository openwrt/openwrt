#
# Copyright (C) 2012-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=brcm2708-gpu-fw
PKG_REV:=30fe8178d61c1ff9bc168edaafdbcb101aa6245e
PKG_VERSION:=20160304
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_REV).tar.gz
PKG_SOURCE_URL:=https://github.com/Hexxeh/rpi-firmware/archive/
PKG_MD5SUM:=5707f4d9ffd8d10018c68ecb8b363308

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)/rpi-firmware-$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define Package/brcm2708-gpu-fw
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_brcm2708
  TITLE:=brcm2708-gpu-fw
  DEFAULT:=y if TARGET_brcm2708
endef

define Package/brcm2708-gpu-fw/description
 GPU and kernel boot firmware for brcm2708. 
endef

define Build/Compile
	true
endef

define Package/brcm2708-gpu-fw/install
	true
endef

define Build/InstallDev
	$(CP) $(PKG_BUILD_DIR)/bootcode.bin $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/COPYING.linux $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/LICENCE.broadcom $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start_cd.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup_cd.dat $(KERNEL_BUILD_DIR)
endef

$(eval $(call BuildPackage,brcm2708-gpu-fw))
