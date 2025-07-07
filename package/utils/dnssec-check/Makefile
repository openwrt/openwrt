#
# Copyright (C) 2025 Liu Yu <f78fk@live.com>
#
# This is free software, licensed under the GNU General Public License v2.0 or later.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dnssec-check
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0-or-later
PKG_MAINTAINER:=Liu Yu <f78fk@live.com>

include $(INCLUDE_DIR)/package.mk

define Package/dnssec-check
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Simple DNSSEC validation tool
  DEPENDS:=+libuci +base-files +bind-dig
endef

define Package/dnssec-check/description
Tool to check if the current DNS supports DNSSEC.
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) \
		-o $(PKG_BUILD_DIR)/dnssec-check \
		$(PKG_BUILD_DIR)/dnssec-check.c -luci
endef

define Package/dnssec-check/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dnssec-check $(1)/usr/bin/dnssec-check

	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/dnssec-check.defaults $(1)/etc/uci-defaults/99-dnssec-check
endef

$(eval $(call BuildPackage,dnssec-check))
