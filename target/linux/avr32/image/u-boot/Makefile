#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=u-boot
PKG_VERSION:=1.3.3
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=6ee26954bb548ad90392cd329ab5cc4c
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/package.mk

UBOOT_CONFIG=$(strip $(subst ",, $(CONFIG_AVR32_UBOOT_TARGET)))

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(UBOOT_CONFIG)_config
	export CROSS_COMPILE=$(TARGET_CROSS);
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Build/InstallDev
	mkdir -p $(BIN_DIR)
	dd if=$(PKG_BUILD_DIR)/u-boot.bin of=$(BIN_DIR)/u-boot-$(UBOOT_CONFIG).bin bs=64k conv=sync
endef

$(eval $(call Build/DefaultTargets))
