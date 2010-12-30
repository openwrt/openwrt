#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=relayd
PKG_VERSION:=2010-10-11
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://nbd.name/relayd.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=caf4d30c669ad8b64ef566b8bab4aec445b90b46

include $(INCLUDE_DIR)/package.mk

define Package/relayd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Transparent routing / relay daemon
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/relayd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/relayd $(1)/usr/sbin/relayd
endef

$(eval $(call BuildPackage,relayd))
