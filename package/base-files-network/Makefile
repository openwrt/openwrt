#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=base-files-network
PKG_RELEASE:=1

PKG_BUILD_DEPENDS:=opkg/host

include $(INCLUDE_DIR)/package.mk

define Package/base-files-network
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Network scripts for the OpenWrt base system
  URL:=http://openwrt.org/
  VERSION:=$(PKG_RELEASE)
endef

define Package/base-files-network/description
 This package contains the network system scripts for OpenWrt.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile

endef

define Package/base-files-network/install
	$(CP) ./files/* $(1)/
endef


$(eval $(call BuildPackage,base-files-network))

