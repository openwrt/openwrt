#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kobs-ng
PKG_VERSION:=3.0.35-4.0.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://repository.timesys.com/buildsources/k/kobs-ng/kobs-ng-$(PKG_VERSION)/
PKG_MD5SUM:=26104c577f59a6b81782a5bd16aadd82

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk

define Package/kobs-ng
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Application for writing bootstreams to NAND flash
  DEPENDS:=@TARGET_imx6
endef

define Package/kobs-ng/description
 The kobs-ng application writes a bootstream to NAND flash with the proper
 FCB/DBBT headers and replicated streams.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	echo "const char* git_sha = \"$(PKG_VERSION)\";" > $(PKG_BUILD_DIR)/autoversion.h
endef

define Package/kobs-ng/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/kobs-ng $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,kobs-ng))
