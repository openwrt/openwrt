#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ifenslave
PKG_VERSION:=$(LINUX_VERSION)
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ifenslave
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@LINUX_2_6 +kmod-bonding
  TITLE:=A utility to manipulate kernel ethernet bonding devices
  DESCRIPTION:=ifenslave is needed to add/remove interfaces to/from ethernet bond devices
  URL:=http://www.kernel.org
endef

define Build/Prepare
	$(CP) $(LINUX_DIR)/Documentation/networking/ifenslave.c  $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(PKG_BUILD_DIR)/ifenslave.c -o $(PKG_BUILD_DIR)/$(PKG_NAME)
endef

define Package/ifenslave/install
	mkdir -p $(1)/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/sbin
endef

$(eval $(call BuildPackage,ifenslave))
