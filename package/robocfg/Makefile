# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

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
  DESCRIPTION:=\
	This package contains an utility for configuring the Broadcom BCM5325E/536x \\\
	based switches.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/robocfg/install
	install -d -m0755 $(1)/sbin
	install -m0755 $(PKG_BUILD_DIR)/robocfg $(1)/sbin/
endef

$(eval $(call BuildPackage,robocfg))
