# 
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ipset
PKG_VERSION:=4.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://ipset.netfilter.org
PKG_MD5SUM:=d104007fdf5ea64fef774c22b1c0a97b

include $(INCLUDE_DIR)/package.mk

define Package/ipset/Default
  DEPENDS:= @LINUX_2_6 @(!(TARGET_ps3||TARGET_pxcab)||BROKEN)
endef

define Package/ipset
$(call Package/ipset/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS+= +iptables-mod-ipset +kmod-ipt-ipset
  TITLE:=IPset administration utility
  URL:=http://ipset.netfilter.org/
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

IPSET_MAKEOPTS:= -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	COPT_FLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="-rdynamic -static-libgcc" \
	NO_EXTRA_WARN_FLAGS=yes \
	KERNEL_DIR="$(LINUX_DIR)" \
	PREFIX=/usr \

IPSET_MODULES:= \
	ip_set \
	ip_set_iphash \
	ip_set_ipmap \
	ip_set_ipporthash \
	ip_set_ipportiphash \
	ip_set_ipportnethash \
	ip_set_iptree \
	ip_set_iptreemap \
	ip_set_macipmap \
	ip_set_nethash \
	ip_set_portmap \
	ip_set_setlist \
	ipt_set \
	ipt_SET \

define Build/Compile
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) $(IPSET_MAKEOPTS) binaries
	$(MAKE) $(KERNEL_MAKEOPTS) M="$(PKG_BUILD_DIR)/kernel" IP_NF_SET_MAX="256" IP_NF_SET_HASHSIZE="1024" modules
	$(MAKE) $(IPSET_MAKEOPTS) DESTDIR="$(PKG_INSTALL_DIR)" binaries_install
endef

define Package/ipset/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ipset $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/ipset
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/ipset/libipset*.so $(1)/usr/lib/ipset/
endef

define KernelPackage/ipt-ipset
$(call Package/ipset/Default)
  SUBMENU:=Netfilter Extensions
  TITLE:=IPset netfilter modules
  DEPENDS+= kmod-ipt-core
  FILES:=$(foreach mod,$(IPSET_MODULES),$(PKG_BUILD_DIR)/kernel/$(mod).ko)
  AUTOLOAD:=$(call AutoLoad,46,$(IPSET_MODULES))
endef

$(eval $(call BuildPackage,ipset))
$(eval $(call KernelPackage,ipt-ipset))
