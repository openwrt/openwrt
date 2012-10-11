#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=thc-ipv6
PKG_VERSION:=0.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://freeworld.thc.org/releases/
PKG_MD5SUM:=01715cfa7f5d4a53fe8f142a10c38c78

PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/thc-ipv6/Default
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+libpcap
  URL:=http://freeworld.thc.org/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		OPT="$(TARGET_CFLAGS) -L$(STAGING_DIR)/usr/lib -I$(PKG_BUILD_DIR) -I$(STAGING_DIR)/usr/include" all
endef


define BuildTool

  PKG_CONFIG_DEPENDS+=CONFIG_PACKAGE_$(1)

  define Package/$(1)
  $$(call Package/thc-ipv6/Default)
    TITLE:=$(2)
    SUBMENU:=IPv6 attack and analyzing toolset (thc-ipv6)
  endef

  define Package/$(1)/description
    $(3)
  endef

  define Package/$(1)/install
	$(INSTALL_DIR) $$(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(1) $$(1)/usr/sbin/$(1)
  endef

  $$(eval $$(call BuildPackage,$(1)))

endef


#$(eval $(call BuildTool,NAME,TITLE,DESCRIPTION))
$(eval $(call BuildTool,parasite6,ARP spoofer for IPv6,Provides false answers to Neighbor Solitication requests))
$(eval $(call BuildTool,dos-new-ip6,DAD spoofer,Prevents new IPv6 interfaces to come up by spoofing DAD))
$(eval $(call BuildTool,detect-new-ip6,Host detector,Detects new IPv6 addresses joining the network))
$(eval $(call BuildTool,fake_router6,Router spoofer,Send Router Advertisements and try to become the default router))
$(eval $(call BuildTool,fake_advertise6,RA spoofer,Advertise arbitary IPv6 addresses))
$(eval $(call BuildTool,fake_mld6,Multicast membership spoofer,Advertise local host in arbitary multicast groups))
$(eval $(call BuildTool,fake_mipv6,MIPv6 update spoofer,Spoof MIPv6 updates to redirect packets directed at the home-address))
$(eval $(call BuildTool,redir6,Route injector,Inject IPv6 routes to redirect traffic to other routers))
$(eval $(call BuildTool,smurf6,Local smurfer,Smurf the target or all-nodes multicast group with ICMPv6 echo replies))
$(eval $(call BuildTool,alive6,Host presence scanner,Find active IPv6 hosts in the network segment))
$(eval $(call BuildTool,toobig6,MTU injector,Inject MTU values in a IPv6 host))
$(eval $(call BuildTool,rsmurf6,Remote smurfer,Smurf the local network of a target host by exploiting Linux implementation errors))
