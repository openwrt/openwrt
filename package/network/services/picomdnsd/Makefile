#
# Copyright (C) 2018 Daniel Engberg <daniel.engberg.lists@pyret.net>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=picomdnsd
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/yegor-alexeyev/picomdnsd.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2017-03-01
PKG_SOURCE_VERSION:=3e5912cb7ea936d248f9e70b4f320e1918c720ce
PKG_MIRROR_HASH:=525091ab51303b4b70a6d6e330ac5c2cc867f366a1be1d237dc6bbb6e7a00190

PKG_LICENSE:=CUSTOM
PKG_LICENSE_FILES:=LICENSE.txt

include $(INCLUDE_DIR)/package.mk

define Package/picomdnsd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Tiny mDNS daemon
  URL:=https://github.com/yegor-alexeyev/picomdnsd
endef

define Package/picomdnsd/description
  picomdnsd is a tiny mDNS daemon used for announcing
  openwrt.local hostname
endef

TARGET_CFLAGS += -Wall -pedantic -std=gnu99 -DNDEBUG -flto

define Package/picomdnsd/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/picomdnsd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/picomdnsd.init $(1)/etc/init.d/picomdnsd
endef

$(eval $(call BuildPackage,picomdnsd))
