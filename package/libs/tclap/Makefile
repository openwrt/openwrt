#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tclap
PKG_VERSION:=1.2.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_HASH:=f5013be7fcaafc69ba0ce2d1710f693f61e9c336b6292ae4f57554f59fde5837

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk

define Package/tclap
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Templatized C++ Command Line Parser Library
  URL:=http://tclap.sourceforge.net
endef

define Package/tclap/description
  TCLAP is a small, flexible library that provides a simple
  interface for defining and accessing command line arguments.
endef

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/tclap/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,tclap))
