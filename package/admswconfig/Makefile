# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=admswconfig
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@openwrt/
PKG_MD5SUM:=faafd4618f970119a665b11b21ac6a26

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/admswconfig
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ADM5120 Switch configuration tool
  DEPENDS:=@LINUX_2_6_ADM5120||@LINUX_2_6_ADM5120EB
  DESCRIPTION:=\
	A program to configure the internal ethernet switch of an ADM5120 processor. \\\
	You need the corresponding driver for the switch in the kernel. \\\
	with this program you can configure which ports of the switch belong \\\
	to the different ethernet devices.
  URL:=http://sharon.esrac.ele.tue.nl/users/pe1rxq/linux-adm/admswconfig/
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Os $(PKG_BUILD_DIR)/admswconfig.c -o $(PKG_BUILD_DIR)/$(PKG_NAME)
endef

define Package/admswconfig/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/admswconfig $(1)/sbin/
endef

$(eval $(call BuildPackage,admswconfig))
