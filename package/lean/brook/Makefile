#
# Copyright (C) 2015-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v3.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=brook
PKG_VERSION:=v20190601
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=brook is a cross-platform proxy software
	DEPENDS:=
	URL:=https://github.com/txthinking/brook/releases
endef

define Package/$(PKG_NAME)/description
brook is a cross-platform proxy software
endef

ifeq ($(ARCH),x86_64)
	BROOK_NAME:=brook
endif
ifeq ($(ARCH),i386)
	BROOK_NAME:=brook_linux_386
endif
ifeq ($(ARCH),mipsel)
	BROOK_NAME:=brook_linux_mipsle
endif
ifeq ($(ARCH),mips)
	BROOK_NAME:=brook_linux_mips
endif
ifeq ($(ARCH),arm)
	BROOK_NAME:=brook_linux_arm7
endif
ifeq ($(ARCH),aarch64)
	BROOK_NAME:=brook_linux_arm64
endif

define Build/Prepare
	[ ! -f $(PKG_BUILD_DIR)/brook ] && wget -O $(PKG_BUILD_DIR)/brook https://github.com/txthinking/brook/releases/download/$(PKG_VERSION)/$(BROOK_NAME)
endef

define Build/Configure
endef

define Build/Compile
	#chmod +x $(PKG_BUILD_DIR)/brook && upx -k --best --lzma $(PKG_BUILD_DIR)/brook
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/brook $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
