#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=json-c
PKG_VERSION:=0.14
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-nodoc.tar.gz
PKG_SOURCE_URL:=https://s3.amazonaws.com/json-c_releases/releases/
PKG_HASH:=99914e644a25201d82ccefa20430f7515c110923360f9ef46755527c02412afa

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=COPYING
PKG_CPE_ID:=cpe:/a:json-c_project:json-c

CMAKE_INSTALL:=1
CMAKE_OPTIONS += -DCMAKE_INSTALL_INCLUDEDIR=$(STAGING_DIR)/usr/include

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

TARGET_CFLAGS += $(FPIC) -Wno-implicit-fallthrough
HOST_CFLAGS += -Wno-implicit-fallthrough
HOST_BUILD_PREFIX:=$(STAGING_DIR_HOST)

define Package/libjson-c
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=javascript object notation
  URL:=https://json-c.github.io/json-c/
  ABI_VERSION:=5
endef

define Package/libjson-c/description
 This package contains a library for javascript object notation backends.
endef

define Package/libjson-c/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libjson-c.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libjson-c))
$(eval $(call HostBuild))
