# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ebtables
PKG_VERSION:=2.0.6
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/ebtables-v$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ebtables
PKG_MD5SUM:=c4559af2366c764c6c42a3fdd40d60d3
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

define Package/ebtables
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
TITLE:=Ethernet bridge firewall administration utility
DESCRIPTION:=Ethernet Bridge Firewalling (ebtables)
URL:=http://ebtables.sourceforge.net/
endef

define Package/ebtables/install
	install -d -m0755 $(IDIR_DNSMASQ)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/ebtables $(IDIR_EBTABLES)/usr/sbin/ebtables
ended

$(eval $(call BuildPackage,ebtables))
