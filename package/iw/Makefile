#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=iw
PKG_VERSION:=0.9.5
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://wireless.kernel.org/download/iw/
PKG_BUILD_DEPENDS:=mac80211 libnl

include $(INCLUDE_DIR)/package.mk

define Package/iw
  SECTION:=net
  CATEGORY:=Network
  TITLE:=cfg80211 interface configuration utility
  URL:=http://wireless.kernel.org/en/users/Documentation/iw
  DEPENDS:=@!TARGET_avr32 @!TARGET_etrax +libnl
endef

define Build/Configure
	$(CP) $(PKG_BUILD_DIR)/defconfig $(PKG_BUILD_DIR)/.config
	echo "#define IW_VERSION \"$(VERSION)\"" > $(PKG_BUILD_DIR)/version.h
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
