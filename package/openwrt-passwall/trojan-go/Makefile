# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (C) 2021 ImmortalWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=trojan-go
PKG_VERSION:=0.10.6
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/p4gefau1t/trojan-go/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=925f02647dda944813f1eab0b71eac98b83eb5964ef5a6f63e89bc7eb4486c1f

PKG_LICENSE:=GPL-3.0-only
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Tianling Shen <cnsztl@immortalwrt.org>

PKG_BUILD_DEPENDS:=golang/host
PKG_BUILD_PARALLEL:=1
PKG_USE_MIPS16:=0
PKG_BUILD_FLAGS:=no-mips16

GO_PKG:=github.com/p4gefau1t/trojan-go
GO_PKG_BUILD_PKG:=$(GO_PKG)
GO_PKG_LDFLAGS_X:= \
	$(GO_PKG)/constant.Version=$(PKG_VERSION) \
	$(GO_PKG)/constant.Commit=v$(PKG_VERSION)
GO_PKG_TAGS:=full

include $(INCLUDE_DIR)/package.mk
include $(TOPDIR)/feeds/packages/lang/golang/golang-package.mk

define Package/trojan-go
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=A Trojan proxy written in Go
  DEPENDS:=$(GO_ARCH_DEPENDS) +ca-bundle
  URL:=https://p4gefau1t.github.io/trojan-go/
endef

define Package/trojan-go/description
  Trojan features multiple protocols over TLS to avoid both
  active/passive detections and ISP QoS limitations.
endef

$(eval $(call GoBinPackage,trojan-go))
$(eval $(call BuildPackage,trojan-go))
