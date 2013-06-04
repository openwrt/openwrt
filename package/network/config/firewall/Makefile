#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=firewall
PKG_VERSION:=2013-06-04
PKG_RELEASE:=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/firewall3.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=5ee2129eaa23a28bfef6d20c273cafc0be559b3d
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/firewall
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=OpenWrt C Firewall
  DEPENDS:=+libubox +libubus +libuci +libip4tc +IPV6:libip6tc +libxtables
endef

define Package/firewall/description
 This package provides a config-compatible C implementation of the UCI firewall.
endef

define Package/firewall/conffiles
/etc/config/firewall
/etc/firewall.user
endef

define Build/Configure
	$(foreach file,$(lastword $(wildcard $(KERNEL_BUILD_DIR)/iptables-*/extensions/libext.a)),$(CP) $(file) $(PKG_BUILD_DIR)/libext.a)
	$(foreach file,$(lastword $(wildcard $(KERNEL_BUILD_DIR)/iptables-*/extensions/libext4.a)),$(CP) $(file) $(PKG_BUILD_DIR)/libext4.a)
	$(foreach file,$(lastword $(wildcard $(KERNEL_BUILD_DIR)/iptables-*/extensions/libext6.a)),$(CP) $(file) $(PKG_BUILD_DIR)/libext6.a)
	$(call Build/Configure/Default)
endef

TARGET_CFLAGS += -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections
CMAKE_OPTIONS += $(if $(CONFIG_IPV6),,-DDISABLE_IPV6=1)

define Package/firewall/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firewall3 $(1)/sbin/fw3
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/firewall.init $(1)/etc/init.d/firewall
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/firewall.hotplug $(1)/etc/hotplug.d/iface/20-firewall
	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_DATA) ./files/firewall.config $(1)/etc/config/firewall
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_DATA) ./files/firewall.user $(1)/etc/firewall.user
endef

$(eval $(call BuildPackage,firewall))
