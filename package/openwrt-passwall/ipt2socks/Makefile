# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (C) 2021 ImmortalWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=ipt2socks
PKG_VERSION:=1.1.3
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/zfl9/ipt2socks/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=5279eb1cb7555cf9292423cc9f672dc43e6e214b3411a6df26a6a1cfa59d88b7

PKG_BUILD_PARALLEL:=1
PKG_USE_MIPS16:=0
PKG_BUILD_FLAGS:=no-mips16

PKG_LICENSE:=AGPL-3.0
PKG_LICENSE_FILE:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/ipt2socks
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Convert iptables to socks5
  URL:=https://github.com/zfl9/ipt2socks
  DEPENDS:=+libpthread
endef

define Package/ipt2socks/description
  Utility for converting iptables (redirect/tproxy) to socks5.
endef

TARGET_CFLAGS+= $(FPIC) -flto
MAKE_FLAGS+= \
	CFLAGS="-std=c99 -pthread $(TARGET_CFLAGS)" \
	EVCFLAGS="$(TARGET_CFLAGS)"

define Package/ipt2socks/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipt2socks $(1)/usr/bin
endef

$(eval $(call BuildPackage,ipt2socks))
