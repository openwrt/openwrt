#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=u-boot
PKG_VERSION:=1.3.2
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=78b1c2722d3907b5fae2cd219dbaf927
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/package.mk

UBOOT_CONFIG=$(strip $(subst ",, $(CONFIG_AVR32_UBOOT_TARGET)))

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(UBOOT_CONFIG)_config
	export CROSS_COMPILE=$(TARGET_CROSS);
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Build/InstallDev
	dd if=$(PKG_BUILD_DIR)/u-boot.bin of=$(BIN_DIR)/u-boot-$(CONFIG_AVR32_UBOOT_TARGET).bin bs=64k count=1 conv=sync
endef

$(eval $(call Build/DefaultTargets))
