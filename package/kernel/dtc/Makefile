#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dtc
PKG_VERSION:=1.4.1
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_URL:=git://git.kernel.org/pub/scm/utils/dtc/dtc.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=302fca9f4c283e1994cf0a5a9ce1cf43ca15e6d2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz

PKG_MAINTAINER:=Marek Vasut <marex@denx.de>
PKG_LICENSE:=GPL-2.0+

include $(INCLUDE_DIR)/package.mk

define Package/dtc
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Device Tree Compiler
endef

define Package/dtc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dtc $(1)/usr/bin/
endef

$(eval $(call BuildPackage,dtc))
