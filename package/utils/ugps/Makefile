#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ugps
PKG_VERSION:=2016-10-24
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL=$(LEDE_GIT)/project/ugps.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=32a6b2b702c3b9f8c425f3d9dc9f4273e276029c
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.xz
PKG_MIRROR_MD5SUM:=a353c722c3fe490dec1b6180c215e6a1d502e843c780dd97d556d1b346e8fa49

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=GPL-2.0+

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ugps
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt GPS Daemon
  DEPENDS:=+libubox +libubus
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

ifneq ($(CONFIG_USE_GLIBC),)
  TARGET_CFLAGS += -D_DEFAULT_SOURCE
endif

define Package/ugps/conffiles
/etc/config/gps
endef

define Package/ugps/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ugps $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/ugps.init $(1)/etc/init.d/ugps
	$(INSTALL_CONF) ./files/gps.config $(1)/etc/config/gps
endef

$(eval $(call BuildPackage,ugps))
