#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=comgt
PKG_VERSION:=0.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.pharscape.org/3G/comgt
PKG_MD5SUM:=cd32663025ed94a0ee623a689989f3c3

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_UNPACK:=mkdir -p $(PKG_BUILD_DIR); zcat $(DL_DIR)/$(PKG_SOURCE) | $(TAR) -C $(PKG_BUILD_DIR) $(TAR_OPTIONS)

include $(INCLUDE_DIR)/package.mk


define Package/comgt
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Option/Vodafone 3G/GPRS control tool
  DEPENDS:=+chat
  DESCRIPTION:=\
	comgt is a scripting language interpreter useful for establishing communications \\\
	on serial lines and through PCMCIA modems as well as GPRS and 3G datacards.
  URL:= http://www.pharscape.org/content/view/46/70/
endef


define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		gcom
endef
	
define Package/comgt/install
	install -d -m0755 $(1)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/gcom $(1)/usr/bin/
	install -d -m0755 $(1)/etc/ppp
	install -m0755 ./files/3g.connect $(1)/etc/ppp/3g.connect
	install -d -m0755 $(1)/etc/chatscripts
	install -m0644 ./files/3g.chat $(1)/etc/chatscripts/3g.chat
	install -d -m0755 $(1)/lib/network
	install -m0644 ./files/3g.sh $(1)/lib/network/3g.sh
	install -d -m0755 $(1)/etc/hotplug.d/button
	install -m0644 ./files/3g.button $(1)/etc/hotplug.d/button/05-3g
	install -d -m0755 $(1)/etc/hotplug.d/iface
	install -m0644 ./files/3g.iface $(1)/etc/hotplug.d/iface/05-3g
	install -d -m0755 $(1)/etc/gcom
	install -m0644 ./files/setpin.gcom $(1)/etc/gcom/setpin.gcom
	install -m0644 ./files/setmode.gcom $(1)/etc/gcom/setmode.gcom
	install -m0644 ./files/getcardinfo.gcom $(1)/etc/gcom/getcardinfo.gcom
	install -m0644 ./files/getstrength.gcom $(1)/etc/gcom/getstrength.gcom
endef

$(eval $(call BuildPackage,comgt))

