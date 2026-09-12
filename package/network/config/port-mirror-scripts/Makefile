#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=port-mirror-scripts
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

PKG_MAINTAINER:=Albrecht Lohofener <albrechtloh@gmx.de>

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/port-mirror-scripts
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=Port mirroring UCI service
  DEPENDS:=+tc-tiny
  PKGARCH:=all
endef

define Package/port-mirror-scripts/description
 A shell-based UCI service that manages persistent tc-based port mirroring
 rules for supported switch ports.
endef

define Package/port-mirror-scripts/conffiles
/etc/config/port-mirror
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/port-mirror-scripts/install
	$(INSTALL_DIR) $(1)
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,port-mirror-scripts))