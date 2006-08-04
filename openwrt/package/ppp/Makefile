# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ppp
PKG_VERSION:=2.4.3
PKG_RELEASE:=7
PKG_BUILDDEP:=libpcap linux-atm

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.samba.org/pub/ppp/
PKG_MD5SUM:=848f6c3cafeb6074ffeb293c3af79b7c
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/ppp
SECTION:=net
CATEGORY:=Network
MENU:=1
DEFAULT:=y
DEPENDS:=+kmod-ppp
TITLE:=PPP daemon
DESCRIPTION:=PPP (Point-to-Point Protocol) daemon
URL:=http://ppp.samba.org/
endef

define Package/ppp-mod-pppoa
SECTION:=net
CATEGORY:=Network
DEPENDS:=ppp +linux-atm +kmod-pppoa
TITLE:=PPPoA plugin
DESCRIPTION:=PPPoA (PPP over ATM) plugin for ppp
endef

define Package/ppp-mod-pppoe
SECTION:=net
CATEGORY:=Network
DEFAULT:=y
DEPENDS:=ppp +kmod-pppoe
TITLE:=PPPoE plugin
DESCRIPTION:=PPPoE (PPP over Ethernet) plugin for ppp
endef

define Package/ppp-mod-radius
SECTION:=net
CATEGORY:=Network
DEPENDS:=ppp
TITLE:=RADIUS plugin
DESCRIPTION:=RADIUS (Remote Authentication Dial-In User Service) plugin for ppp
endef

define Package/chat
SECTION:=net
CATEGORY:=Network
DEPENDS:=ppp
TITLE:=Establish conversation with a modem
DESCRIPTION:=Utility to establish conversation with other PPP servers (via a modem)
endef

define Package/pppdump
SECTION:=net
CATEGORY:=Network
DEPENDS:=ppp
TITLE:=Read PPP record file
DESCRIPTION:=Utility to read PPP record file
endef

define Package/pppstats
SECTION:=net
CATEGORY:=Network
DEPENDS:=ppp
TITLE:=Report PPP statistics
DESCRIPTION:=Utility to report PPP statistics
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		COPTS="$(TARGET_CFLAGS)" \
		PRECOMPILED_FILTER=1 \
		STAGING_DIR="$(STAGING_DIR)" \
		DESTDIR="$(PKG_INSTALL_DIR)/usr" \
		all install
endef

define Package/ppp/install
	install -d -m0755 $(1)/lib/network
	install -m0755 ./files/ppp.sh $(1)/lib/network/
	install -d -m0755 $(1)/etc/ppp
	install -m0600 ./files/etc/ppp/chap-secrets $(1)/etc/ppp/
	install -m0644 ./files/etc/ppp/filter $(1)/etc/ppp/
	install -m0755 ./files/etc/ppp/ip-up $(1)/etc/ppp/
	install -d -m0755 $(1)/etc/ppp/ip-up.d
	install -m0755 ./files/etc/ppp/ip-down $(1)/etc/ppp/
	install -d -m0755 $(1)/etc/ppp/ip-down.d
	install -m0644 ./files/etc/ppp/options $(1)/etc/ppp/
	ln -sf /tmp/resolv.conf $(1)/etc/ppp/resolv.conf
	install -d -m0755 $(1)/usr/lib/pppd/$(PKG_VERSION)
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_INSTALL_DIR)/usr/sbin/pppd $(1)/usr/sbin/
endef
	
define Package/ppp-mod-pppoa/install
	install -d -m0755 $(1)/lib/network
	install -m0755 ./files/pppoa.sh $(1)/lib/network/
	install -d -m0755 $(1)/usr/lib/pppd/$(PKG_VERSION)
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/pppoatm.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
endef

define Package/ppp-mod-pppoe/install
	install -d -m0755 $(1)/lib/network
	install -m0755 ./files/pppoe.sh $(1)/lib/network/
	install -d -m0755 $(1)/usr/lib/pppd/$(PKG_VERSION)
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/rp-pppoe.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
endef

define Package/ppp-mod-radius/install
	install -d -m0755 $(1)/etc/ppp
	install -m644 ./files/etc/ppp/radius.conf $(1)/etc/ppp/
	install -d -m0755 $(1)/etc/ppp/radius
	install -m644 ./files/etc/ppp/radius/dictionary* \
		$(1)/etc/ppp/radius/
	install -m600 ./files/etc/ppp/radius/servers \
		$(1)/etc/ppp/radius/
	install -d -m0755 $(1)/usr/lib/pppd/$(PKG_VERSION)
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/radius.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
endef

define Package/chat/install
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_INSTALL_DIR)/usr/sbin/chat $(1)/usr/sbin/
endef

define Package/pppdump/install
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_INSTALL_DIR)/usr/sbin/pppdump $(1)/usr/sbin/
endef

define Package/pppstats/install
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_INSTALL_DIR)/usr/sbin/pppstats $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ppp))
$(eval $(call BuildPackage,ppp-mod-pppoa))
$(eval $(call BuildPackage,ppp-mod-pppoe))
$(eval $(call BuildPackage,ppp-mod-radius))
$(eval $(call BuildPackage,chat))
$(eval $(call BuildPackage,pppdump))
$(eval $(call BuildPackage,pppstats))
