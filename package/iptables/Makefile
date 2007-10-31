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
PKG_VERSION:=1.3.8
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.netfilter.org/projects/iptables/files \
	ftp://ftp.be.netfilter.org/pub/netfilter/iptables/ \
	ftp://ftp.de.netfilter.org/pub/netfilter/iptables/ \
	ftp://ftp.no.netfilter.org/pub/netfilter/iptables/
PKG_MD5SUM:=0a9209f928002e5eee9cdff8fef4d4b3

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
  -include $(LINUX_DIR)/.config
  include $(INCLUDE_DIR)/netfilter.mk
  STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell grep 'NETFILTER' $(LINUX_DIR)/.config | md5s)
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
endef

define Package/iptables-mod-conntrack/description
 iptables extensions for connection tracking.
 Includes:
 - libipt_conntrack
 - libipt_helper
 - libipt_connmark/CONNMARK
endef

define Package/iptables-mod-filter
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-filter
  TITLE:=filter modules
endef

define Package/iptables-mod-filter/description
 iptables extensions for packet content inspection.
 Includes: 
 - libipt_ipp2p
 - libipt_layer7 
endef

define Package/iptables-mod-imq
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-imq
  TITLE:=IMQ support
endef

define Package/iptables-mod-imq/description
 iptables extension for IMQ support.
 Includes: 
 - libipt_IMQ
endef

define Package/iptables-mod-ipopt
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-ipopt
  TITLE:=IP/Packet option modules
endef

define Package/iptables-mod-ipopt/description
 iptables extensions for matching/changing IP packet options.\\\
 Includes: 
 - libipt_CLASSIFY
 - libipt_dscp/DSCP
 - libipt_ecn/ECN
 - libipt_length
 - libipt_mac
 - libipt_mark/MARK
 - libipt_statistic
 - libipt_tcpmms
 - libipt_tos/TOS
 - libipt_ttl/TTL
 - libipt_unclean 
endef

define Package/iptables-mod-ipsec
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-ipsec
  TITLE:=IPSec extensions
endef

define Package/iptables-mod-ipsec/description
 iptables extensions for matching ipsec traffic.
 Includes: 
 - libipt_ah
 - libipt_esp
 - libipt_policy
endef

define Package/iptables-mod-nat
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-nat
  TITLE:=extra NAT targets
endef

define Package/iptables-mod-nat/description
 iptables extensions for different NAT targets.
 Includes: 
 - libipt_REDIRECT
endef

define Package/iptables-mod-ulog
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-ulog
  TITLE:=user-space packet logging
endef

define Package/iptables-mod-ulog/description
 iptables extensions for user-space packet logging.
 Includes: 
 - libipt_ULOG
endef

define Package/iptables-mod-iprange
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-iprange
  TITLE:=ip range module
endef

define Package/iptables-mod-iprange/description
 iptables extensions for matching ip ranges.
 Includes: 
 - libipt_iprange
endef

define Package/iptables-mod-extra
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-extra
  TITLE:=other extra iptables extensions
endef

define Package/iptables-mod-extra/description
 other extra iptables extensions.
 Includes: 
 - libipt_limit
 - libipt_owner
 - libipt_physdev
 - libipt_pkttype
 - libipt_recent
 - iptable_raw
 - libipt_NOTRACK
 - libipt_TARPIT
 - libipt_DELUDE
 - libipt_CHAOS
endef

define Package/iptables-mod-ipset
$(call Package/iptables/Default)
  DEPENDS:=iptables +kmod-ipt-ipset
  TITLE:=ipset iptables extension
endef

define Package/iptables-utils
$(call Package/iptables/Default)
  DEPENDS:=iptables
  TITLE:=iptables save and restore utilities
endef

define Package/ip6tables
$(call Package/iptables/Default)
  DEPENDS:=+kmod-ipv6
  CATEGORY:=Network
  TITLE:=IPv6 firewall administration tool
endef

define Package/ip6tables-utils
 $(call Package/iptables/Default)
 DEPENDS:=ip6tables
 CATEGORY:=Network
 TITLE:=ip6tables save and restore utilities
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	chmod a+x $(PKG_BUILD_DIR)/extensions/.*-test*
endef

define Build/Configure
	rm -f $(PKG_BUILD_DIR)/.configured*
	$(MAKE) -C $(PKG_BUILD_DIR) \
		clean
endef

define Build/Compile
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		COPT_FLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="-rdynamic -static-libgcc" \
		KERNEL_DIR="$(LINUX_DIR)" PREFIX=/usr \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all experimental install install-experimental
endef

define Build/InstallDev
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		COPT_FLAGS="$(TARGET_CFLAGS)" \
		KERNEL_DIR="$(LINUX_DIR)" PREFIX=/usr \
		DESTDIR="$(1)" \
		install-devel
	$(CP) $(PKG_BUILD_DIR)/include $(1)/usr
	$(CP) $(PKG_BUILD_DIR)/libiptc/libiptc.a $(1)/usr/lib
endef

define Build/UninstallDev
	rm -rf $(1)/usr/include/libipq.h
	rm -rf $(1)/usr/include/libipulog
	rm -rf $(1)/usr/include/libipq
	rm -rf $(1)/usr/include/libiptc
	rm -rf $(1)/usr/include/linux/netfilter*
	rm -rf $(1)/usr/include/iptables*
	rm -rf $(1)/usr/include/ip6tables*
	rm -rf $(1)/usr/lib/libipq.a
	rm -rf $(1)/usr/lib/libiptc.a
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

define Package/ip6tables-utils/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ip6tables-{save,restore} $(1)/usr/sbin/
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
$(eval $(call BuildPlugin,iptables-mod-ipset,$(IPT_IPSET-m)))
$(eval $(call BuildPlugin,iptables-mod-filter,$(IPT_FILTER-m),$(L7_INSTALL)))
$(eval $(call BuildPlugin,iptables-mod-imq,$(IPT_IMQ-m)))
$(eval $(call BuildPlugin,iptables-mod-ipopt,$(IPT_IPOPT-m)))
$(eval $(call BuildPlugin,iptables-mod-ipsec,$(IPT_IPSEC-m)))
$(eval $(call BuildPlugin,iptables-mod-nat,$(IPT_NAT-m)))
$(eval $(call BuildPlugin,iptables-mod-iprange,$(IPT_IPRANGE-m)))
$(eval $(call BuildPlugin,iptables-mod-ulog,$(IPT_ULOG-m)))
$(eval $(call BuildPackage,ip6tables))
$(eval $(call BuildPackage,ip6tables-utils))
