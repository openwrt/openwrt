#
# Copyright (C) 2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=provision
PKG_RELEASE:=$(AUTORELEASE)

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk

define Package/provision
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Utility for managing device provisioning data
  DEPENDS:=+ucode +ucode-mod-fs +ucode-mod-struct
endef

define Build/Compile
	:
endef

define Package/provision/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,provision))
