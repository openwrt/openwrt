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
PKG_SOURCE_DATE:=2023-05-29
PKG_SOURCE_VERSION:=a85a5bc83bde5b485319ca12b6e32c4b7f0b120f
PKG_MIRROR_HASH:=b907b91989320eb8916e719ced9bdce96b8c5db6abefcee35e25fb112ad5b27f

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_LICENSE:=GPL-2.0

PKG_BUILD_FLAGS:=lto

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ustp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=OpenWrt STP/RSTP daemon
  DEPENDS:=+libubox +libubus
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/ustp/install
	$(INSTALL_DIR) $(1)/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/sbin/* $(1)/sbin/
	$(INSTALL_BIN) ./files/ustpd.init $(1)/etc/init.d/ustpd
endef

$(eval $(call BuildPackage,ustp))
