#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ugps
PKG_RELEASE:=1

PKG_SOURCE_URL=$(PROJECT_GIT)/project/ugps.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2020-08-29
PKG_SOURCE_VERSION:=511a5b3c84fa715ef0305cf26c98619c12a4867a
PKG_MIRROR_HASH:=551d57b4c18da4c4695fa2f7a959c25a03be35d14cf872785e7db740f12366e2

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
