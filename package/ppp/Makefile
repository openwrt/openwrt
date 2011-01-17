#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ppp
PKG_VERSION:=2.4.4
PKG_RELEASE:=15

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.samba.org/pub/ppp/
PKG_MD5SUM:=183800762e266132218b204dfb428d29

PKG_BUILD_DEPENDS:=libpcap

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ppp/Default
  SECTION:=net
  CATEGORY:=Network
  MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
  URL:=http://ppp.samba.org/
endef

define Package/ppp
$(call Package/ppp/Default)
  DEPENDS:=+kmod-ppp
  TITLE:=PPP daemon
  MENU:=1
endef

define Package/ppp/description
This package contains the PPP (Point-to-Point Protocol) daemon.
endef

define Package/ppp/conffiles
/etc/ppp/chap-secrets
/etc/ppp/filter
/etc/ppp/ip-down
/etc/ppp/ip-up
/etc/ppp/ipv6-down
/etc/ppp/ipv6-up
/etc/ppp/options
endef

define Package/ppp-mod-pppoa
$(call Package/ppp/Default)
  DEPENDS:=ppp +linux-atm +kmod-pppoa
  TITLE:=PPPoA plugin
endef

define Package/ppp-mod-pppoa/description
This package contains a PPPoA (PPP over ATM) plugin for ppp.
endef

define Package/ppp-mod-pppoe
$(call Package/ppp/Default)
  DEPENDS:=ppp +kmod-pppoe
  TITLE:=PPPoE plugin
endef

define Package/ppp-mod-pppoe/description
This package contains a PPPoE (PPP over Ethernet) plugin for ppp.
endef

define Package/ppp-mod-radius
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=RADIUS plugin
endef

define Package/ppp-mod-radius/description
This package contains a RADIUS (Remote Authentication Dial-In User Service)
plugin for ppp.
endef

define Package/ppp-mod-radius/conffiles
/etc/ppp/radius.conf
/etc/ppp/radius/
endef

define Package/chat
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Establish conversation with a modem
endef

define Package/chat/description
This package contains an utility to establish conversation with other PPP servers
(via a modem).
endef

define Package/pppdump
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Read PPP record file
endef

define Package/pppdump/description
This package contains an utility to read PPP record file.
endef

define Package/pppstats
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Report PPP statistics
endef

define Package/pppstats/description
This package contains an utility to report PPP statistics.
endef


define Build/Configure
$(call Build/Configure/Default,, \
	UNAME_S="Linux" \
	UNAME_R="$(LINUX_VERSION)" \
	UNAME_M="$(ARCH)" \
)
	mkdir -p $(PKG_BUILD_DIR)/pppd/plugins/pppoatm/linux
	cp \
		$(LINUX_DIR)/include/linux/compiler.h \
		$(LINUX_DIR)/include/linux/atm*.h \
		$(PKG_BUILD_DIR)/pppd/plugins/pppoatm/linux/
endef

MAKE_FLAGS += COPTS="$(TARGET_CFLAGS)" \
		PRECOMPILED_FILTER=1 \
		STAGING_DIR="$(STAGING_DIR)"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/include/pppd $(1)/usr/include/
endef

define Package/ppp/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_VERSION)
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/pppd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/ppp.sh $(1)/lib/network/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_CONF) ./files/etc/ppp/chap-secrets $(1)/etc/ppp/
	$(INSTALL_DATA) ./files/etc/ppp/filter $(1)/etc/ppp/
	$(INSTALL_BIN) ./files/etc/ppp/ip-up $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/ppp/ip-up.d
	$(INSTALL_BIN) ./files/etc/ppp/ip-down $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/ppp/ip-down.d
	$(INSTALL_BIN) ./files/etc/ppp/ipv6-up $(1)/etc/ppp/
	$(INSTALL_BIN) ./files/etc/ppp/ipv6-down $(1)/etc/ppp/
	$(INSTALL_DATA) ./files/etc/ppp/options $(1)/etc/ppp/
	ln -sf /tmp/resolv.conf.ppp $(1)/etc/ppp/resolv.conf
endef

define Package/ppp-mod-pppoa/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/pppd/$(PKG_VERSION)/pppoatm.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/pppoa.sh $(1)/lib/network/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/atm
	$(INSTALL_DATA) ./files/etc/hotplug.d/atm/20-atm-modem $(1)/etc/hotplug.d/atm/
endef

define Package/ppp-mod-pppoe/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/pppd/$(PKG_VERSION)/rp-pppoe.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/pppoe.sh $(1)/lib/network/
endef

define Package/ppp-mod-radius/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/pppd/$(PKG_VERSION)/radius.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_DATA) ./files/etc/ppp/radius.conf $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/ppp/radius
	$(INSTALL_DATA) ./files/etc/ppp/radius/dictionary* \
		$(1)/etc/ppp/radius/
	$(INSTALL_CONF) ./files/etc/ppp/radius/servers \
		$(1)/etc/ppp/radius/
endef

define Package/chat/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/chat $(1)/usr/sbin/
endef

define Package/pppdump/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/pppdump $(1)/usr/sbin/
endef

define Package/pppstats/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/pppstats $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ppp))
$(eval $(call BuildPackage,ppp-mod-pppoa))
$(eval $(call BuildPackage,ppp-mod-pppoe))
$(eval $(call BuildPackage,ppp-mod-radius))
$(eval $(call BuildPackage,chat))
$(eval $(call BuildPackage,pppdump))
$(eval $(call BuildPackage,pppstats))
