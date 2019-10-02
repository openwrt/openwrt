#
# Copyright (C) 2015-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v3.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=clash
PKG_VERSION:=0.15.0
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=clash is a cross-platform proxy software
	DEPENDS:=
	URL:=https://github.com/Dreamacro/clash/releases
endef

define Package/$(PKG_NAME)/description
clash is a cross-platform proxy software
endef

ifeq ($(ARCH),x86_64)
	PKG_ARCH_CLASH:=linux-amd64
endif
ifeq ($(ARCH),i386)
	PKG_ARCH_CLASH:=linux-386
endif
ifeq ($(ARCH),mipsel)
	PKG_ARCH_CLASH:=linux-mipsle
endif
ifeq ($(ARCH),mips)
	PKG_ARCH_CLASH:=linux-mips64
endif
ifeq ($(ARCH),arm)
	PKG_ARCH_CLASH:=linux-armv6
endif
ifeq ($(ARCH),aarch64)
	PKG_ARCH_CLASH:=linux-armv8
endif

define Build/Prepare
	#[ ! -f $(PKG_BUILD_DIR)/clash-v$(PKG_VERSION)-$(PKG_ARCH_CLASH).tar.gz ] && wget  https://github.com/Dreamacro/clash/releases/download/v$(PKG_VERSION)/clash-$(PKG_ARCH_CLASH).tar.gz -O $(PKG_BUILD_DIR)/clash-v$(PKG_VERSION)-$(PKG_ARCH_CLASH).tar.gz
	#tar -xzvf $(PKG_BUILD_DIR)/clash-v$(PKG_VERSION)-$(PKG_ARCH_CLASH).tar.gz -C $(PKG_BUILD_DIR)
	[ ! -f $(PKG_BUILD_DIR)/clash-$(PKG_ARCH_CLASH).gz ] && wget  https://github.com/Dreamacro/clash/releases/download/v$(PKG_VERSION)/clash-$(PKG_ARCH_CLASH)-v$(PKG_VERSION).gz -O $(PKG_BUILD_DIR)/clash-$(PKG_ARCH_CLASH).gz
	gunzip $(PKG_BUILD_DIR)/clash-$(PKG_ARCH_CLASH).gz
endef

define Build/Configure
endef

define Build/Compile
	#chmod +x $(PKG_BUILD_DIR)/clash-$(PKG_ARCH_CLASH) && upx --ultra-brute $(PKG_BUILD_DIR)/clash-$(PKG_ARCH_CLASH)
	#echo "$(PKG_VERSION)" > $(PKG_BUILD_DIR)/clash_version
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/clash-$(PKG_ARCH_CLASH) $(1)/usr/bin/clash
	#$(INSTALL_DIR) $(1)/usr/share/clash
	#$(INSTALL_DATA) $(PKG_BUILD_DIR)/clash_version $(1)/usr/share/clash/clash_version
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
