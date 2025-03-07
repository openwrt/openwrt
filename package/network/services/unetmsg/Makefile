#
# Copyright (C) 2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=unetmsg
PKG_RELEASE:=$(AUTORELEASE)

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk

define Package/unetmsg
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=unet network pub/sub message broker
  DEPENDS:=+ucode +ucode-mod-socket \
	+ucode-mod-ubus +ucode-mod-uloop \
	+ucode-mod-fs
endef

define Build/Compile
	:
endef

define Package/unetmsg/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,unetmsg))
