# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=robocfg
PKG_VERSION:=0.01
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/robocfg

include $(INCLUDE_DIR)/package.mk

define Package/robocfg
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=BCM5325E/536x switch configuration utility
endef

define Package/robocfg/description
 This package contains an utility for configuring the Broadcom BCM5325E/536x 
 based switches.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/robocfg/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/robocfg $(1)/sbin/
endef

$(eval $(call BuildPackage,robocfg))
