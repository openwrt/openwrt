#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lldpd
PKG_VERSION:=0.6.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://media.luffy.cx/files/lldpd
PKG_MD5SUM:=77279577e3b6d85a33dc0afe7c960b27

PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/lldpd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  TITLE:=Link Layer Discovery Protocol daemon
  URL:=https://github.com/vincentbernat/lldpd/wiki
  DEPENDS:=+libevent2
endef

define Package/lldpd/description
	LLDP (Link Layer Discovery Protocol) is an industry standard protocol designed
	to supplant proprietary Link-Layer protocols such as
	Extreme's EDP (Extreme Discovery Protocol) and
	CDP (Cisco Discovery Protocol).
	The goal of LLDP is to provide an inter-vendor compatible mechanism to deliver
	Link-Layer notifications to adjacent network devices.
endef

define Package/lldpd/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/lldp{ctl,d} $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/lldpd.init $(1)/etc/init.d/lldpd
	$(INSTALL_DATA) ./files/lldpd.config $(1)/etc/config/lldpd
endef

define Package/lldpd/conffiles
/etc/config/lldpd
endef

$(eval $(call BuildPackage,lldpd))
