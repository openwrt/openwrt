 
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ipset
PKG_VERSION:=6.20.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://ipset.netfilter.org
PKG_MD5SUM:=8af741492752fbf24d3a28a9d1473d40

PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ipset/Default
  DEPENDS:= @(!(TARGET_ps3||TARGET_pxcab)||BROKEN)
endef

define Package/ipset
$(call Package/ipset/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS+= +kmod-ipt-ipset +libmnl
  TITLE:=IPset administration utility
  URL:=http://ipset.netfilter.org/
endef

CONFIGURE_ARGS += \
	--with-kbuild="$(LINUX_DIR)"

MAKE_FLAGS += \
	ARCH="$(LINUX_KARCH)" \
	SHELL="$(BASH)"

IPSET_MODULES:= \
	ipset/ip_set \
	ipset/ip_set_bitmap_ip \
	ipset/ip_set_bitmap_ipmac \
	ipset/ip_set_bitmap_port \
	ipset/ip_set_hash_ip \
	ipset/ip_set_hash_ipport \
	ipset/ip_set_hash_ipportip \
	ipset/ip_set_hash_ipportnet \
	ipset/ip_set_hash_net \
	ipset/ip_set_hash_netiface \
	ipset/ip_set_hash_netport \
	ipset/ip_set_list_set \
	xt_set \

define Build/Compile
	$(call Build/Compile/Default)
	$(call Build/Compile/Default,modules)
endef

define Package/ipset/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ipset $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libipset*.so* $(1)/usr/lib/
endef

define KernelPackage/ipt-ipset
$(call Package/ipset/Default)
  SUBMENU:=Netfilter Extensions
  TITLE:=IPset netfilter modules
  DEPENDS+= +kmod-ipt-core +kmod-nfnetlink
  FILES:=$(foreach mod,$(IPSET_MODULES),$(PKG_BUILD_DIR)/kernel/net/netfilter/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,49,$(notdir $(IPSET_MODULES)))
endef

$(eval $(call BuildPackage,ipset))
$(eval $(call KernelPackage,ipt-ipset))
