#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fitstrip
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=Weijie Gao <hackpascal@gmail.com>

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/fitstrip
  HIDDEN:=1
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=FIT image stripping tool
  DEPENDS:=+libfdt
endef

define Package/fitstrip/description
Remove trailing data from FIT image
endef

define Package/fitstrip/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fitstrip $(1)/usr/bin/
endef

$(eval $(call BuildPackage,fitstrip))
