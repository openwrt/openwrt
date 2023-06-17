#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ppp
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/ppp-project/ppp
PKG_SOURCE_DATE:=2022-12-10
PKG_SOURCE_VERSION:=760ce18f82670eb81cc186fb792919339a2e2fbe
PKG_MIRROR_HASH:=5ba8f8e0517476d25e4f3e4c347cc8524ef46dd997f17651fdf5f76ef74e5e64
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_LICENSE:=BSD-4-Clause
PKG_CPE_ID:=cpe:/a:samba:ppp
PKG_FIXUP:=autoreconf

PKG_RELEASE_VERSION:=2.5.0
PKG_VERSION:=$(PKG_RELEASE_VERSION).git-$(PKG_SOURCE_DATE)
# pcap is statically linked in patch #310:
PKG_BUILD_DEPENDS:=libpcap

PKG_ASLR_PIE_REGULAR:=1
PKG_BUILD_FLAGS:=gc-sections lto
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
# enable_microsoft_extensions should be yes for mppe/mppc, but it lead to openssl in upstream:
# https://github.com/ppp-project/ppp/blob/master/pppd/crypto_ms.c#L125
CONFIGURE_VARS += \
    enable_microsoft_extensions=yes \
	enable_eaptls=no \
	enable_peap=no

CONFIGURE_ARGS += \
	with_pcap=no \
	with_static_pcap=yes \
	with_openssl=no

define Package/ppp/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=https://ppp.samba.org/
endef

define Package/ppp
$(call Package/ppp/Default)
  DEPENDS:=+kmod-ppp
  TITLE:=PPP daemon
  VARIANT:=default
endef

# implies tdb-trivial database:
define Package/ppp-multilink
$(call Package/ppp/Default)
  CONFIGURE_VARS += \
      enable_multilink=yes
  DEPENDS:=+kmod-ppp
  TITLE:=PPP daemon (with multilink support)
  VARIANT:=multilink
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
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink) +linux-atm +kmod-pppoa
  TITLE:=PPPoA plugin
endef

define Package/ppp-mod-pppoa/description
This package contains a PPPoA (PPP over ATM) plugin for ppp.
endef

define Package/ppp-mod-pppoe
$(call Package/ppp/Default)
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink) +kmod-pppoe
  TITLE:=PPPoE plugin
endef

define Package/ppp-mod-pppoe/description
This package contains a PPPoE (PPP over Ethernet) plugin for ppp.
endef

define Package/ppp-mod-radius
$(call Package/ppp/Default)
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink)
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

define Package/ppp-mod-pppol2tp
$(call Package/ppp/Default)
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink) +kmod-pppol2tp
  TITLE:=PPPoL2TP plugin
endef

define Package/ppp-mod-pppol2tp/description
This package contains a PPPoL2TP (PPP over L2TP) plugin for ppp.
endef

define Package/ppp-mod-pptp
$(call Package/ppp/Default)
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink) +kmod-pptp +kmod-mppe +resolveip
  TITLE:=PPtP plugin
endef

define Package/ppp-mod-pptp/description
This package contains a PPtP plugin for ppp.
endef

define Package/ppp-mod-passwordfd
$(call Package/ppp/Default)
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink)
  TITLE:=pap/chap secret from filedescriptor
endef

define Package/ppp-mod-passwordfd/description
This package allows to pass the PAP/CHAP secret from a filedescriptor.
Eliminates the need for a secrets file.
endef

define Package/chat
$(call Package/ppp/Default)
  TITLE:=Establish conversation with a modem
endef

define Package/chat/description
This package contains an utility to establish conversation with other PPP servers
(via a modem).
endef

define Package/pppdump
$(call Package/ppp/Default)
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink)
  TITLE:=Read PPP record file
endef

define Package/pppdump/description
This package contains an utility to read PPP record file.
endef

define Package/pppstats
$(call Package/ppp/Default)
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink)
  TITLE:=Report PPP statistics
endef

define Package/pppstats/description
This package contains an utility to report PPP statistics.
endef

define Package/pppoe-discovery
$(call Package/ppp/Default)
  DEPENDS:=@(PACKAGE_ppp||PACKAGE_ppp-multilink) +ppp-mod-pppoe
  TITLE:=Perform a PPPoE-discovery process
endef

define Package/pppoe-discovery/description
This tool performs the same discovery process as pppoe, but does
not initiate a session. Can be useful to debug pppoe.
endef


define Build/Configure
$(call Build/Configure/Default,, \
	UNAME_S="Linux" \
	UNAME_R="$(LINUX_VERSION)" \
	UNAME_M="$(ARCH)" \
)
	mkdir -p $(PKG_BUILD_DIR)/pppd/plugins/pppoatm/linux
	$(CP) \
		$(LINUX_DIR)/include/linux/compiler.h \
		$(LINUX_DIR)/include/$(LINUX_UAPI_DIR)linux/atm*.h \
		$(PKG_BUILD_DIR)/pppd/plugins/pppoatm/linux/

	# Kernel 4.14.9+ only, ignore the exit status of cp in case the file
	# doesn't exits
	-$(CP) $(LINUX_DIR)/include/linux/compiler_types.h \
		$(PKG_BUILD_DIR)/pppd/plugins/pppoatm/linux/
endef

MAKE_FLAGS += COPTS="$(TARGET_CFLAGS)" \
		PRECOMPILED_FILTER=1 \
		STAGING_DIR="$(STAGING_DIR)"

ifeq ($(BUILD_VARIANT),multilink)
  MAKE_FLAGS += HAVE_MULTILINK=y
else
  MAKE_FLAGS += HAVE_MULTILINK=
endif

ifdef CONFIG_USE_MUSL
  MAKE_FLAGS += USE_LIBUTIL=
endif

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/pppd $(1)/usr/include/
endef

define Package/ppp/script_install
endef

define Package/ppp/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/pppd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_CONF) ./files/etc/ppp/chap-secrets $(1)/etc/ppp/
	$(INSTALL_DATA) ./files/etc/ppp/filter $(1)/etc/ppp/
	$(INSTALL_DATA) ./files/etc/ppp/options $(1)/etc/ppp/
	$(LN) /tmp/resolv.conf.ppp $(1)/etc/ppp/resolv.conf
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/ppp.sh $(1)/lib/netifd/proto/
	$(INSTALL_BIN) ./files/lib/netifd/ppp-up $(1)/lib/netifd/
	$(INSTALL_BIN) ./files/lib/netifd/ppp6-up $(1)/lib/netifd/
	$(INSTALL_BIN) ./files/lib/netifd/ppp-down $(1)/lib/netifd/
endef
Package/ppp-multilink/install=$(Package/ppp/install)

define Package/ppp-mod-pppoa/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/pppoatm.so \
		$(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/
endef

define Package/ppp-mod-pppoe/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/pppoe.so \
		$(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/
endef

define Package/ppp-mod-radius/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/radius.so \
		$(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_DATA) ./files/etc/ppp/radius.conf $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/ppp/radius
	$(INSTALL_DATA) ./files/etc/ppp/radius/dictionary* \
		$(1)/etc/ppp/radius/
	$(INSTALL_CONF) ./files/etc/ppp/radius/servers \
		$(1)/etc/ppp/radius/
endef

define Package/ppp-mod-pppol2tp/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/pppol2tp.so \
		$(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/
endef

define Package/ppp-mod-pptp/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/pptp.so \
		$(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_DATA) ./files/etc/ppp/options.pptp $(1)/etc/ppp/
endef

define Package/ppp-mod-passwordfd/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/passwordfd.so \
		$(1)/usr/lib/pppd/$(PKG_RELEASE_VERSION)/
endef

define Package/chat/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/chat $(1)/usr/sbin/
endef

define Package/pppdump/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/pppdump $(1)/usr/sbin/
endef

define Package/pppstats/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/pppstats $(1)/usr/sbin/
endef

define Package/pppoe-discovery/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/pppoe-discovery $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ppp))
$(eval $(call BuildPackage,ppp-multilink))
$(eval $(call BuildPackage,ppp-mod-pppoa))
$(eval $(call BuildPackage,ppp-mod-pppoe))
$(eval $(call BuildPackage,ppp-mod-radius))
$(eval $(call BuildPackage,ppp-mod-pppol2tp))
$(eval $(call BuildPackage,ppp-mod-pptp))
$(eval $(call BuildPackage,ppp-mod-passwordfd))
$(eval $(call BuildPackage,chat))
$(eval $(call BuildPackage,pppdump))
$(eval $(call BuildPackage,pppstats))
$(eval $(call BuildPackage,pppoe-discovery))
