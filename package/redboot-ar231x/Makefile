#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=redboot-ar231x
PKG_VERSION:=2010-10-26
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_TARGETS:=bin

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/redboot-ar231x.git
PKG_SOURCE_VERSION:=327f02ce1645d3427f26cf8116353332c81564a9
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.bz2

include $(INCLUDE_DIR)/package.mk

export GCC_HONOUR_COPTS=s

define Package/redboot-ar231x
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_atheros @BROKEN
  TITLE:=Tiny redboot for AR231x/AR531x
endef

define Build/Configure
	mkdir -p $(PKG_BUILD_DIR)/host-build
	if [ \! -x $(PKG_BUILD_DIR)/host-install/bin/ecosconfig ]; then ( \
		cd $(PKG_BUILD_DIR)/host-build; \
		../ecos/host/configure \
			--prefix=$(PKG_BUILD_DIR)/host-install \
			--exec-prefix=$(PKG_BUILD_DIR)/host-install; \
		CFLAGS="$(HOST_CFLAGS)" $(MAKE) all install; \
	) fi
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) all
endef

define Package/redboot-ar231x/install
	$(CP) $(PKG_BUILD_DIR)/bin/* $(1)/
endef

$(eval $(call BuildPackage,redboot-ar231x))
