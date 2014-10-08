#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=comgt
PKG_VERSION:=0.32
PKG_RELEASE:=23

PKG_SOURCE:=$(PKG_NAME).$(PKG_VERSION).tgz
PKG_SOURCE_URL:=@SF/comgt
PKG_MD5SUM:=db2452680c3d953631299e331daf49ef

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME).$(PKG_VERSION)
PKG_CHECK_FORMAT_SECURITY:=0

include $(INCLUDE_DIR)/package.mk

define Package/comgt/Default
  SECTION:=utils
  CATEGORY:=Utilities
endef

define Package/comgt
$(call Package/comgt/Default)
  TITLE:=Option/Vodafone 3G/GPRS control tool
  DEPENDS:=+chat
  URL:=http://www.pharscape.org/comgt.html
endef

define Package/comgt-directip
$(call Package/comgt/Default)
  TITLE:=Sierra Wireless Direct-IP support
  DEPENDS:=+comgt +kmod-usb-serial +kmod-usb-serial-sierrawireless +kmod-usb-net +kmod-usb-net-sierrawireless
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
		LDFLAGS="" \
		comgt
endef

define Package/comgt/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/comgt $(1)/usr/bin/
	ln -s comgt $(1)/usr/bin/gcom
	$(INSTALL_DIR) $(1)/etc/chatscripts
	$(INSTALL_DATA) ./files/3g.chat $(1)/etc/chatscripts/3g.chat
	$(INSTALL_DATA) ./files/evdo.chat $(1)/etc/chatscripts/evdo.chat
	$(INSTALL_DIR) $(1)/etc/gcom
	$(INSTALL_DATA) ./files/setpin.gcom $(1)/etc/gcom/setpin.gcom
	$(INSTALL_DATA) ./files/setmode.gcom $(1)/etc/gcom/setmode.gcom
	$(INSTALL_DATA) ./files/getcardinfo.gcom $(1)/etc/gcom/getcardinfo.gcom
	$(INSTALL_DATA) ./files/getstrength.gcom $(1)/etc/gcom/getstrength.gcom
	$(INSTALL_DATA) ./files/getcarrier.gcom $(1)/etc/gcom/getcarrier.gcom
	$(INSTALL_DATA) ./files/getcnum.gcom $(1)/etc/gcom/getcnum.gcom
	$(INSTALL_DATA) ./files/getimsi.gcom $(1)/etc/gcom/getimsi.gcom
	$(INSTALL_DIR) $(1)/etc/hotplug.d/tty
	$(INSTALL_DATA) ./files/3g.usb $(1)/etc/hotplug.d/tty/30-3g
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/3g.sh $(1)/lib/netifd/proto/3g.sh
endef

define Package/comgt-directip/install
	$(INSTALL_DIR) $(1)/etc/gcom
	$(INSTALL_DATA) ./files/directip.gcom $(1)/etc/gcom/directip.gcom
	$(INSTALL_DATA) ./files/directip-stop.gcom $(1)/etc/gcom/directip-stop.gcom
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/directip.sh $(1)/lib/netifd/proto/directip.sh
endef

$(eval $(call BuildPackage,comgt))
$(eval $(call BuildPackage,comgt-directip))
