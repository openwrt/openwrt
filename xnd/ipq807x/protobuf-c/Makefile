#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libprotobuf-c
PKG_VERSION:=1.3.1
PKG_RELEASE:=2

PKG_SOURCE:=protobuf-c-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/protobuf-c/protobuf-c/releases/download/v$(PKG_VERSION)
PKG_HASH:=51472d3a191d6d7b425e32b612e477c06f73fe23e07f6a6a839b11808e9d2267
PKG_BUILD_DIR:=$(BUILD_DIR)/protobuf-c-$(PKG_VERSION)
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/protobuf-c-$(PKG_VERSION)

PKG_MAINTAINER:=Rosen Penev <rosenp@gmail.com>
PKG_LICENSE:=BSD-2c

PKG_BUILD_DEPENDS:=protobuf-c/host
HOST_BUILD_DEPENDS:=protobuf/host

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define Package/libprotobuf-c
  TITLE:=Protocol Buffers library
  SECTION:=libs
  CATEGORY:=Libraries
  URL:=https://github.com/protobuf-c/protobuf-c
endef

define Package/libprotobuf-c/description
  Runtime library to use Google Protocol Buffers from C applications.
  Protocol Buffers are a way of encoding structured data in an efficient yet
  extensible format. Google uses Protocol Buffers for almost all of its
  internal RPC protocols and file formats.
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-protoc

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprotobuf-c.{a,la,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* $(1)/usr/lib/pkgconfig/
endef

define Package/libprotobuf-c/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprotobuf-c.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libprotobuf-c))
$(eval $(call HostBuild))

