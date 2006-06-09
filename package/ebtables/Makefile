# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ebtables
PKG_VERSION:=2.0.8-rc2
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/ebtables-v$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ebtables
PKG_MD5SUM:=f07111fcc1966be669278433c35dcc28
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

define Package/ebtables
SECTION:=base
CATEGORY:=Network
TITLE:=Ethernet bridge firewall administration utility
DESCRIPTION:=Ethernet Bridge Firewalling (ebtables)
URL:=http://ebtables.sourceforge.net/
endef

define Package/ebtables/install
	install -d -m0755 $(1)/etc
	install -m0644 $(PKG_BUILD_DIR)/ethertypes $(1)/etc/
	install -d -m0755 $(1)/usr/lib/ebtables
	install -m0755 $(PKG_BUILD_DIR)/lib*.so $(1)/usr/lib/
	install -m0755 $(PKG_BUILD_DIR)/extensions/*.so $(1)/usr/lib/ebtables/
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/ebtables $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/ebtables-save $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/ebtables-restore $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ebtables))
