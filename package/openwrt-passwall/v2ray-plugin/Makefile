# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (C) 2020 SharerMax
# Copyright (C) 2021 ImmortalWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=v2ray-plugin
PKG_VERSION:=5.13.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/teddysun/v2ray-plugin/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=23b3a66516cb3b722839754913a4f0c8ac4bbb01631a4ad9251252fd9b401100

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=madeye <max.c.lv@gmail.com>

PKG_BUILD_DEPENDS:=golang/host
PKG_BUILD_PARALLEL:=1
PKG_USE_MIPS16:=0
PKG_BUILD_FLAGS:=no-mips16

GO_PKG:=github.com/shadowsocks/v2ray-plugin
GO_PKG_LDFLAGS_X:=main.VERSION=v$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(TOPDIR)/feeds/packages/lang/golang/golang-package.mk

define Package/v2ray-plugin
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=SIP003 plugin for shadowsocks, based on v2ray
  URL:=https://github.com/teddysun/v2ray-plugin
  DEPENDS:=$(GO_ARCH_DEPENDS) +ca-bundle
endef

$(eval $(call GoBinPackage,v2ray-plugin))
$(eval $(call BuildPackage,v2ray-plugin))
