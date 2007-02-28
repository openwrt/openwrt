# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=iptables
PKG_VERSION:=1.3.5
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.netfilter.org/projects/iptables/files \
	ftp://ftp.be.netfilter.org/pub/netfilter/iptables/ \
	ftp://ftp.de.netfilter.org/pub/netfilter/iptables/ \
	ftp://ftp.no.netfilter.org/pub/netfilter/iptables/
PKG_MD5SUM:=00fb916fa8040ca992a5ace56d905ea5
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
  -include $(LINUX_DIR)/.config
  include $(INCLUDE_DIR)/netfilter.mk
endif

define Package/iptables/Default
  SECTION:=net
  CATEGORY:=Base system
  URL:=http://netfilter.org/
endef

define Package/iptables
  $(call Package/iptables/Default)
  TITLE:=IPv4 firewall administration tool
  MENU:=1
endef

define Package/iptables/conffiles
/etc/config/firewall
/etc/firewall.user
endef

define Package/iptables-mod-conntrack
  $(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-conntrack
  TITLE:=connection tracking modules
  DESCRIPTION:=\
	iptables extensions for connection tracking.\\\
	\\\
	Includes: \\\
	- libipt_conntrack \\\
	- libipt_helper \\\
	- libipt_connmark/CONNMARK
endef

define Package/iptables-mod-filter
  $(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-filter
  TITLE:=filter modules
  DESCRIPTION:=\
	iptables extensions for packet content inspection.\\\
	\\\
	Includes: \\\
	- libipt_ipp2p \\\
	- libipt_layer7 
endef

define Package/iptables-mod-imq
  $(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-imq
  TITLE:=IMQ support
  DESCRIPTION:=\
	iptables extension for IMQ support.\\\
	\\\
	Includes: \\\
	- libipt_IMQ
endef

define Package/iptables-mod-ipopt
  $(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-ipopt
  TITLE:=IP/Packet option modules
  DESCRIPTION:=\
	iptables extensions for matching/changing IP packet options.\\\
	\\\
	Includes: \\\
	- libipt_CLASSIFY \\\
	- libipt_dscp/DSCP \\\
	- libipt_ecn/ECN \\\
	- libipt_length \\\
	- libipt_mac \\\
	- libipt_tos/TOS \\\
	- libipt_tcpmms \\\
	- libipt_ttl/TTL \\\
	- libipt_unclean 
endef

define Package/iptables-mod-ipsec
  $(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-ipsec
  TITLE:=IPSec extensions
  DESCRIPTION:=\
	iptables extensions for matching ipsec traffic.\\\
	\\\
	Includes: \\\
	- libipt_ah \\\
	- libipt_esp
endef

define Package/iptables-mod-nat
  $(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-nat
  TITLE:=extra NAT targets
  DESCRIPTION:=\
	iptables extensions for different NAT targets.\\\
	\\\
	Includes: \\\
	- libipt_REDIRECT
endef

define Package/iptables-mod-ulog
  $(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-ulog
  TITLE:=user-space packet logging
  DESCRIPTION:=\
	iptables extensions for user-space packet logging.\\\
	\\\
	Includes: \\\
	- libipt_ULOG
endef

define Package/iptables-mod-extra
  $(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-extra
  TITLE:=other extra iptables extensions
  DESCRIPTION:=\
	other extra iptables extensions.\\\
	\\\
	Includes: \\\
	- libipt_limit \\\
	- libipt_owner \\\
	- libipt_physdev \\\
	- libipt_pkttype \\\
	- libipt_recent
endef

define Package/iptables-utils
  $(call Package/iptables/Default)
  DEPENDS:=iptables
  TITLE:=iptables save and restore utilities
endef

define Package/ip6tables
  $(call Package/iptables/Default)
  CATEGORY:=Network
  TITLE:=IPv6 firewall administration tool
endef

define Build/Configure
endef

define Build/Compile
	chmod a+x $(PKG_BUILD_DIR)/extensions/.*-test*
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		COPT_FLAGS="$(TARGET_CFLAGS)" \
		KERNEL_DIR="$(LINUX_DIR)" PREFIX=/usr \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		COPT_FLAGS="$(TARGET_CFLAGS)" \
		KERNEL_DIR="$(LINUX_DIR)" PREFIX=/usr \
		DESTDIR="$(STAGING_DIR)" \
		install-devel
	$(CP) $(PKG_BUILD_DIR)/include $(STAGING_DIR)/usr
	$(CP) $(PKG_BUILD_DIR)/libiptc/libiptc.a $(STAGING_DIR)/usr/lib
endef

define Build/UninstallDev
	rm -rf $(STAGING_DIR)/usr/include/libipq.h
	rm -rf $(STAGING_DIR)/usr/include/libipulog
	rm -rf $(STAGING_DIR)/usr/include/libipq
	rm -rf $(STAGING_DIR)/usr/include/libiptc
	rm -rf $(STAGING_DIR)/usr/include/linux/netfilter*
	rm -rf $(STAGING_DIR)/usr/include/iptables*
	rm -rf $(STAGING_DIR)/usr/include/ip6tables*
	rm -rf $(STAGING_DIR)/usr/lib/libipq.a
	rm -rf $(STAGING_DIR)/usr/lib/libiptc.a
endef

define Package/iptables/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/firewall.config $(1)/etc/config/firewall
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/firewall.init $(1)/etc/init.d/firewall
	$(INSTALL_BIN) ./files/firewall.user $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) ./files/firewall.awk $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/iptables $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/iptables
	(cd $(PKG_INSTALL_DIR)/usr/lib/iptables ; \
		$(CP) $(patsubst %,lib%.so,$(IPT_BUILTIN:xt_%=ipt_%)) $(1)/usr/lib/iptables/ \
	)
endef

define Package/iptables-utils/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/iptables-{save,restore} $(1)/usr/sbin/
endef

define Package/ip6tables/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ip6tables $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/iptables
	(cd $(PKG_INSTALL_DIR)/usr/lib/iptables ; \
		$(CP) libip6t_*.so $(1)/usr/lib/iptables/ \
	)
endef

define BuildPlugin
  define Package/$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/iptables
	for m in $$(patsubst xt_%,ipt_%,$(2)); do \
		$(CP) $(PKG_INSTALL_DIR)/usr/lib/iptables/lib$$$$$$$${m}.so $$(1)/usr/lib/iptables/ ; \
	done
	$(3)
  endef

  $$(eval $$(call BuildPackage,$(1)))
endef

L7_INSTALL:=\
	$(INSTALL_DIR) $$(1)/etc/l7-protocols; \
	$(CP) files/l7/*.pat $$(1)/etc/l7-protocols/

$(eval $(call BuildPackage,iptables))
$(eval $(call BuildPackage,iptables-utils))
$(eval $(call BuildPlugin,iptables-mod-conntrack,$(IPT_CONNTRACK-m)))
$(eval $(call BuildPlugin,iptables-mod-extra,$(IPT_EXTRA-m)))
$(eval $(call BuildPlugin,iptables-mod-filter,$(IPT_FILTER-m),$(L7_INSTALL)))
$(eval $(call BuildPlugin,iptables-mod-imq,$(IPT_IMQ-m)))
$(eval $(call BuildPlugin,iptables-mod-ipopt,$(IPT_IPOPT-m)))
$(eval $(call BuildPlugin,iptables-mod-ipsec,$(IPT_IPSEC-m)))
$(eval $(call BuildPlugin,iptables-mod-nat,$(IPT_NAT-m)))
$(eval $(call BuildPlugin,iptables-mod-ulog,$(IPT_ULOG-m)))
$(eval $(call BuildPackage,ip6tables))
