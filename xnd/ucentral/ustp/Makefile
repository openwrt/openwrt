#
# Copyright (C) 2014-2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ustp
PKG_RELEASE:=1

PKG_SOURCE_URL=$(PROJECT_GIT)/project/ustp.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-08-27
PKG_SOURCE_VERSION:=25555611be916da7fe8cb53f85c741a51ddffa1c
PKG_MIRROR_HASH:=d84d05b1a05279f2efe4aa6a27ac9590381636091cb1ec2aba72a604f58cddae

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name
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
