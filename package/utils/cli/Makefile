#
# Copyright (C) 2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cli
PKG_RELEASE:=$(AUTORELEASE)

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk

define Package/cli
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt CLI
  DEPENDS:=+ucode +ucode-mod-uline \
	+ucode-mod-ubus +ucode-mod-uloop \
	+ucode-mod-fs +ucode-mod-rtnl
endef

define Build/Compile
	:
endef

define Package/cli/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,cli))
