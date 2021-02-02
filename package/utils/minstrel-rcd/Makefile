#
# Copyright (C) 2021 Felix Fietkau <nbd@nbd.name>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=minstrel-rcd
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_RELEASE)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/minstrel-rcd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Daemon for remote controlling mac80211 minstrel rate control
  DEPENDS:=+libubox
  MAINTAINER:=Felix Fietkau <nbd@nbd.name>
endef

define Package/minstrel-rcd/conffiles
/etc/config/minstrel-rcd
endef

define Package/minstrel-rcd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_DATA) ./files/minstrel-rcd.config $(1)/etc/config/minstrel-rcd
	$(INSTALL_BIN) ./files/minstrel-rcd.init $(1)/etc/init.d/minstrel-rcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/minstrel-rcd $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,minstrel-rcd))
