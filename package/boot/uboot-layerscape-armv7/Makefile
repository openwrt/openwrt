#
# Copyright (C) 2017 NXP semiconductors
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=uboot-layerscape-armv7
PKG_VERSION:=v1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_MIRROR_HASH:=462fea9bdf03ec4bc23f2f1bb3917c2ec57017eca1e11b8a609bf60cd51a94e7
PKG_SOURCE_URL:=https://github.com/openil/u-boot.git
PKG_SOURCE_VERSION:=4ebc5c0fea263da867e41abb3f410b7cc4392def

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=layerscape
  BUILD_SUBTARGET:=armv7
  BUILD_DEVICES:=$(1)
  UBOOT_IMAGE:=u-boot-dtb.bin
  HIDDEN:=y
endef

define U-Boot/ls1021aiot
  NAME:=NXP ls1021aiot Dev Board
  UBOOT_CONFIG:=ls1021aiot_sdcard
  UBOOT_IMAGE:=u-boot-with-spl-pbl.bin
endef

UBOOT_TARGETS := \
	ls1021aiot

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-armv7-uboot.bin
	$(PKG_BUILD_DIR)/tools/mkenvimage -s 131072 -o $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-uboot-env.bin uboot-env/$(BUILD_VARIANT)-uboot-env.txt
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
