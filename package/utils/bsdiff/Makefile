#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bsdiff
PKG_VERSION:=4.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.daemonology.net/bsdiff/
PKG_MD5SUM:=e6d812394f0e0ecc8d5df255aa1db22a
PKG_MAINTAINER:=Hauke Mehrtens <hauke@hauke-m.de>
HOST_BUILD_DEPENDS:=bzip2/host

PKG_LICENSE:=BSD-2-Clause

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/bsdiff
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libbz2
  TITLE:=Binary diff tool
  URL:=http://www.daemonology.net/bsdiff/
endef

define Package/bspatch
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libbz2
  TITLE:=Binary patch tool
  URL:=http://www.daemonology.net/bsdiff/
endef


define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		-o $(PKG_BUILD_DIR)/bsdiff \
		$(PKG_BUILD_DIR)/bsdiff.c -lbz2
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		-o $(PKG_BUILD_DIR)/bspatch \
		$(PKG_BUILD_DIR)/bspatch.c -lbz2
endef

define Package/bsdiff/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bsdiff $(1)/usr/bin/bsdiff
endef

define Package/bspatch/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bspatch $(1)/usr/bin/bspatch
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR)/host/bin/
	$(MAKE) -C $(HOST_BUILD_DIR) PREFIX=$(STAGING_DIR)/host/ install
endef

define Host/Compile
	$(HOSTCC) $(HOST_CFLAGS) $(HOST_LDFLAGS) \
		-o $(HOST_BUILD_DIR)/bsdiff \
		$(HOST_BUILD_DIR)/bsdiff.c -lbz2
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/bsdiff $(STAGING_DIR)/host/bin/
endef

$(eval $(call HostBuild))

$(eval $(call BuildPackage,bsdiff))
$(eval $(call BuildPackage,bspatch))
