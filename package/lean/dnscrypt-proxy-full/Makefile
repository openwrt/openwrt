#
# Copyright (C) 2015-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v3.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dnscrypt-proxy-full
PKG_VERSION:=2.0.25
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=A flexible DNS proxy, with support for modern encrypted DNS protocols such as DNSCrypt v2 and DNS-over-HTTPS.
	DEPENDS:=
	URL:=https://github.com/jedisct1/dnscrypt-proxy/releases
endef

define Package/$(PKG_NAME)/description
A flexible DNS proxy, with support for modern encrypted DNS protocols such as DNSCrypt v2 and DNS-over-HTTPS.
endef

ifeq ($(ARCH),x86_64)
	PKG_ARCH_DNSCRYPT:=linux_x86_64
	PKG_BUILD_DNSCRYPT:=linux-x86_64
endif
ifeq ($(ARCH),i386)
	PKG_ARCH_DNSCRYPT:=linux_i386
	PKG_BUILD_DNSCRYPT:=linux-i386
endif
ifeq ($(ARCH),mipsel)
	PKG_ARCH_DNSCRYPT:=linux_mipsle
	PKG_BUILD_DNSCRYPT:=linux-mipsle
endif
ifeq ($(ARCH),mips)
	PKG_ARCH_DNSCRYPT:=linux_mips
	PKG_BUILD_DNSCRYPT:=linux-mips
endif
ifeq ($(ARCH),arm)
	PKG_ARCH_DNSCRYPT:=linux_armv6
	PKG_BUILD_DNSCRYPT:=linux-armv6
endif
ifeq ($(ARCH),aarch64)
	PKG_ARCH_DNSCRYPT:=linux_armv8
	PKG_BUILD_DNSCRYPT:=linux-armv8
endif

define Build/Prepare
	[ ! -f $(PKG_BUILD_DIR)/dnscrypt-proxy-$(PKG_ARCH_DNSCRYPT)-$(PKG_VERSION).tar.gz ] && wget https://github.com/jedisct1/dnscrypt-proxy/releases/download/$(PKG_VERSION)/dnscrypt-proxy-$(PKG_ARCH_DNSCRYPT)-$(PKG_VERSION).tar.gz -O $(PKG_BUILD_DIR)/dnscrypt-proxy-$(PKG_ARCH_DNSCRYPT)-$(PKG_VERSION).tar.gz
	tar -xzvf $(PKG_BUILD_DIR)/dnscrypt-proxy-$(PKG_ARCH_DNSCRYPT)-$(PKG_VERSION).tar.gz -C $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	chmod +x $(PKG_BUILD_DIR)/$(PKG_BUILD_DNSCRYPT)/dnscrypt-proxy && upx --lzma $(PKG_BUILD_DIR)/$(PKG_BUILD_DNSCRYPT)/dnscrypt-proxy
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_BUILD_DNSCRYPT)/dnscrypt-proxy $(1)/usr/bin/dnscrypt-proxy
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/dnscrypt-proxy $(1)/etc/init.d/dnscrypt-proxy
	$(INSTALL_DIR) $(1)/etc/dnscrypt-proxy
	$(CP) -r ./files/etc/dnscrypt-proxy $(1)/etc
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
