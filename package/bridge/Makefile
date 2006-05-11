# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bridge
PKG_VERSION:=1.0.6
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/bridge-utils-$(PKG_VERSION)
PKG_SOURCE:=bridge-utils-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/bridge
PKG_MD5SUM:=9b7dc52656f5cbec846a7ba3299f73bd
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

define Package/bridge
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
TITLE:=Ethernet bridging configuration utility
DESCRIPTION:=Ethernet bridging configuration utility\\\
Manage ethernet bridging; a way to connect networks together to\\\
form a larger network.
URL:=http://bridge.sourceforge.net/
endef

define Build/Configure
$(call Build/Configure/Default,--with-linux-headers=$(LINUX_DIR))
endef

define Package/bridge/install
	install -m0755 -d $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/brctl/brctl $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,bridge))
