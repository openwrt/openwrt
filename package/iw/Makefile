#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iw
PKG_VERSION:=0.9.12
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://wireless.kernel.org/download/iw/
PKG_MD5SUM:=2aa919ff927dd9def151a87badfa111a
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
	echo "#define IW_VERSION \"$(PKG_VERSION)\"" > $(PKG_BUILD_DIR)/version.h
endef

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/mac80211 -I$(STAGING_DIR)/usr/include/libnl $(TARGET_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS) -lnl"

ifneq ($(CONFIG_LINUX_2_6),)
  define Package/iw/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iw $(1)/usr/sbin/
  endef
endif

$(eval $(call BuildPackage,iw))
