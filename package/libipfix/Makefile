# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libipfix
PKG_VERSION:=r51
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_MD5SUM:=0e5b2871ea20ac48eda3f6006c5dba28
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME).$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/libipfix
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=IP Flow Information Export Library
  URL:=http://www.fokus.fraunhofer.de/de/net/more_about/download/ipfixlib.html
  BUILDONLY:=1
endef

TARGET_CFLAGS += \
	-ffunction-sections -fdata-sections

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CCOPT="$(TARGET_CFLAGS) -I$(BUILD_DIR)/linux/include" \
		prefix="$(PKG_INSTALL_DIR)/usr" \
		exec_prefix="$(PKG_INSTALL_DIR)/usr" \
		all install
	$(TARGET_CROSS)ranlib $(PKG_INSTALL_DIR)/usr/lib/libipfix.a
	$(TARGET_CROSS)ranlib $(PKG_INSTALL_DIR)/usr/lib/libmisc.a
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

$(eval $(call BuildPackage,libipfix))
