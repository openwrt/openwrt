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

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.samba.org/pub/ppp/
PKG_MD5SUM:=848f6c3cafeb6074ffeb293c3af79b7c
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

PKG_BUILDDEP:=libpcap linux-atm

include $(INCLUDE_DIR)/package.mk

define Package/ppp/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=http://ppp.samba.org/
endef

define Package/ppp
  $(call Package/ppp/Default)
  DEFAULT:=y
  DEPENDS:=+kmod-ppp
  TITLE:=PPP daemon
  DESCRIPTION:=\
	This package contains the PPP (Point-to-Point Protocol) daemon.
  MENU:=1
endef

define Package/ppp-mod-pppoa
  $(call Package/ppp/Default)
  DEPENDS:=ppp +linux-atm +kmod-pppoa
  TITLE:=PPPoA plugin
  DESCRIPTION:=\
	This package contains a PPPoA (PPP over ATM) plugin for ppp.
endef

define Package/ppp-mod-pppoe
  $(call Package/ppp/Default)
  DEFAULT:=y
  DEPENDS:=ppp +kmod-pppoe
  TITLE:=PPPoE plugin
  DESCRIPTION:=\
	This package contains a PPPoE (PPP over Ethernet) plugin for ppp.
endef

define Package/ppp-mod-radius
  $(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=RADIUS plugin
  DESCRIPTION:=\
	This package contains a RADIUS (Remote Authentication Dial-In User \\\
	Service) plugin for ppp.
endef

define Package/chat
  $(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Establish conversation with a modem
  DESCRIPTION:=\
	This package contains an utility to establish conversation with other \\\
	PPP servers (via a modem).
endef

define Package/pppdump
  $(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Read PPP record file
  DESCRIPTION:=\
	This package contains an utility to read PPP record file.
endef

define Package/pppstats
  $(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Report PPP statistics
  DESCRIPTION:=\
	This package contains an utility to report PPP statistics.
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
	install -d -m0755 $(1)/usr/lib/pppd/$(PKG_VERSION)
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_INSTALL_DIR)/usr/sbin/pppd $(1)/usr/sbin/
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
endef
	
define Package/ppp-mod-pppoa/install
	install -d -m0755 $(1)/usr/lib/pppd/$(PKG_VERSION)
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/pppoatm.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	install -d -m0755 $(1)/lib/network
	install -m0755 ./files/pppoa.sh $(1)/lib/network/
endef

define Package/ppp-mod-pppoe/install
	install -d -m0755 $(1)/usr/lib/pppd/$(PKG_VERSION)
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/rp-pppoe.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	install -d -m0755 $(1)/lib/network
	install -m0755 ./files/pppoe.sh $(1)/lib/network/
endef

define Package/ppp-mod-radius/install
	install -d -m0755 $(1)/usr/lib/pppd/$(PKG_VERSION)
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/radius.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	install -d -m0755 $(1)/etc/ppp
	install -m0644 ./files/etc/ppp/radius.conf $(1)/etc/ppp/
	install -d -m0755 $(1)/etc/ppp/radius
	install -m0644 ./files/etc/ppp/radius/dictionary* \
		$(1)/etc/ppp/radius/
	install -m0600 ./files/etc/ppp/radius/servers \
		$(1)/etc/ppp/radius/
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
