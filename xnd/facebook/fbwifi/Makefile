# Copyright (c) Facebook, Inc. and its affiliates.
# All rights reserved.
#
# This source code is licensed under the license found in the
# LICENSE file in the root directory of this source tree.
#


include $(TOPDIR)/rules.mk

PKG_NAME:=fbwifi
PKG_VERSION:=2
PKG_RELEASE:=0
PKG_LICENSE:=GPL-2.0

PKG_MAINTAINER:=Simon Kinane <skinane@fb.com>

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/fbwifi
  SUBMENU:=Captive Portals
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+iptables +luasec +luasocket \
	+luci-base +libuci-lua +luaposix \
	+luci-mod-network +luci-mod-status +luci-theme-bootstrap \
	+lua-cjson +uhttpd
  TITLE:=Facebook Wi-Fi
  PKGARCH:=all
endef

define Package/fbwifi/description
  Facebook Wi-Fi, an AP authorisation solution
endef

define Package/fbwifi/conffiles
/etc/config/fbwifi
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/fbwifi/install
	$(INSTALL_DIR) $(1)
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,fbwifi))
