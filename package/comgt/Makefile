#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=comgt
PKG_VERSION:=0.32
PKG_RELEASE:=8

PKG_SOURCE:=$(PKG_NAME).$(PKG_VERSION).tgz
PKG_SOURCE_URL:=@SF/comgt
PKG_MD5SUM:=db2452680c3d953631299e331daf49ef

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME).$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/comgt
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Option/Vodafone 3G/GPRS control tool
  DEPENDS:=+chat
  URL:=http://www.pharscape.org/comgt.html
endef

define Package/comgt/description
 comgt is a scripting language interpreter useful for establishing 
 communications on serial lines and through PCMCIA modems as well as GPRS 
 and 3G datacards.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		gcom
endef
	
define Package/comgt/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gcom $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_BIN) ./files/3g.connect $(1)/etc/ppp/3g.connect
	$(INSTALL_DIR) $(1)/etc/chatscripts
	$(INSTALL_DATA) ./files/3g.chat $(1)/etc/chatscripts/3g.chat
	$(INSTALL_DATA) ./files/evdo.chat $(1)/etc/chatscripts/evdo.chat
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/3g.sh $(1)/lib/network/3g.sh
	$(INSTALL_DIR) $(1)/etc/hotplug.d/button
	$(INSTALL_DATA) ./files/3g.button $(1)/etc/hotplug.d/button/05-3g
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/3g.iface $(1)/etc/hotplug.d/iface/05-3g
	$(INSTALL_DIR) $(1)/etc/hotplug.d/tty
	$(INSTALL_DATA) ./files/3g.usb $(1)/etc/hotplug.d/tty/30-3g
	$(INSTALL_DIR) $(1)/etc/gcom
	$(INSTALL_DATA) ./files/setpin.gcom $(1)/etc/gcom/setpin.gcom
	$(INSTALL_DATA) ./files/setmode.gcom $(1)/etc/gcom/setmode.gcom
	$(INSTALL_DATA) ./files/getcardinfo.gcom $(1)/etc/gcom/getcardinfo.gcom
	$(INSTALL_DATA) ./files/getstrength.gcom $(1)/etc/gcom/getstrength.gcom
endef

$(eval $(call BuildPackage,comgt))
