#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=u-boot
PKG_VERSION:=2010.06
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/package.mk

UBOOT_CONFIG=$(call qstrip,$(CONFIG_UBOOT_TARGET))
LAN_IP=$(call qstrip,$(CONFIG_UBOOT_IPADDR))
LAN_SERVERIP=$(call qstrip,$(CONFIG_UBOOT_SERVERIP))

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(UBOOT_CONFIG)_config
	export CROSS_COMPILE=$(TARGET_CROSS); \
	export LAN_IP=$(LAN_IP); \
	export LAN_SERVERIP=$(LAN_SERVERIP); \
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/ubclient
	$(CP) ./ubclient/* $(PKG_BUILD_DIR)/ubclient
	export LAN_IP=$(LAN_IP); \
	export LAN_SERVERIP=$(LAN_SERVERIP); \
	#$(MAKE) -C $(PKG_BUILD_DIR)/ubclient \
	#	$(TARGET_CONFIGURE_OPTS) \
	#	CFLAGS="$(TARGET_CFLAGS) -Dtarget_$(BOARD)=1"
endef

define Build/InstallDev
	dd if=$(PKG_BUILD_DIR)/u-boot.bin of=$(PKG_BUILD_DIR)/u-boot.block bs=232k count=1 conv=sync
#	$(INSTALL_DIR) $(STAGING_DIR)/ubclient/sbin
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ubclient/ubpar $(STAGING_DIR)/ubclient/sbin/
#	$(CP) $(PKG_BUILD_DIR)/ubclient/ubpar ../../base-files/sbin
	mkdir -p $(BIN_DIR)/
	cp $(PKG_BUILD_DIR)/u-boot.bin $(BIN_DIR)/u-boot.$(PKG_VERSION).bin
endef

$(eval $(call Build/DefaultTargets))
