#
# Copyright (C) 2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ustp
PKG_RELEASE:=1

PKG_SOURCE_URL=$(PROJECT_GIT)/project/ustp.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-08-25
PKG_SOURCE_VERSION:=9622264cf92691f18ae9222b0a4c9db95af5d80d
PKG_MIRROR_HASH:=de4ed29eee21192b60e8683633d916d251bcccd5701bdac83b5ba435189297f1

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ustp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=OpenWrt STP/RSTP daemon
  DEPENDS:=+libubox +libubus
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include -flto
TARGET_LDFLAGS += -flto -fuse-linker-plugin

define Package/ustp/install
	$(INSTALL_DIR) $(1)/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/sbin/* $(1)/sbin/
	$(INSTALL_BIN) ./files/ustpd.init $(1)/etc/init.d/ustpd
endef

$(eval $(call BuildPackage,ustp))
