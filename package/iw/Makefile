#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=iw
PKG_REV:=4d3a72da3f21761bf4f2866a6a3400d4ecf67365
PKG_VERSION:=20080414_$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://git.sipsolutions.net/iw.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_BUILD_DEPENDS:=mac80211 libnl

include $(INCLUDE_DIR)/package.mk

define Package/iw
  SECTION:=net
  CATEGORY:=Network
  TITLE:=cfg80211 interface configuration utility
  URL:=http://git.sipsolutions.net/?p=iw.git
  DEPENDS:=@!TARGET_avr32 @!TARGET_etrax +libnl
endef

define Build/Configure
	$(CP) $(PKG_BUILD_DIR)/defconfig $(PKG_BUILD_DIR)/.config
endef

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/mac80211 -I$(STAGING_DIR)/usr/include/libnl -I$(STAGING_DIR)/usr/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib -lnl"

ifneq ($(CONFIG_LINUX_2_6),)
  define Package/iw/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iw $(1)/usr/sbin/
  endef
endif

$(eval $(call BuildPackage,iw))
