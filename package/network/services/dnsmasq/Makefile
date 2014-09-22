#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dnsmasq
PKG_VERSION:=2.71
PKG_RELEASE:=5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://thekelleys.org.uk/dnsmasq
PKG_MD5SUM:=15a68f7f6cc0119e843f67d2f79598f1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=COPYING

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dnsmasq/Default
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=DNS and DHCP server
  URL:=http://www.thekelleys.org.uk/dnsmasq/
endef

define Package/dnsmasq
$(call Package/dnsmasq/Default)
  VARIANT:=nodhcpv6
endef

define Package/dnsmasq-dhcpv6
$(call Package/dnsmasq/Default)
  TITLE += (with DHCPv6 support)
  DEPENDS:=@IPV6 +kmod-ipv6
  VARIANT:=dhcpv6
endef

define Package/dnsmasq-full
$(call Package/dnsmasq/Default)
  TITLE += (with DNSSEC, DHCPv6, Auth DNS, IPSET)
  DEPENDS:=@IPV6 +kmod-ipv6 +libnettle
  VARIANT:=full
endef

define Package/dnsmasq/description
  It is intended to provide coupled DNS and DHCP service to a LAN.
endef

define Package/dnsmasq-dhcpv6/description
$(call Package/dnsmasq/description)

This is a variant with DHCPv6 support
endef

define Package/dnsmasq-full/description
$(call Package/dnsmasq/description)

This is a variant with DHCPv6, DNSSEC, Authroitative DNS and IPSET support
endef

define Package/dnsmasq/conffiles
/etc/config/dhcp
/etc/dnsmasq.conf
endef

Package/dnsmasq-dhcpv6/conffiles = $(Package/dnsmasq/conffiles)
Package/dnsmasq-full/conffiles = $(Package/dnsmasq/conffiles)

TARGET_CFLAGS += -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections

COPTS = $(if $(CONFIG_IPV6),,-DNO_IPV6)

ifeq ($(BUILD_VARIANT),nodhcpv6)
	COPTS += -DNO_DHCP6
endif

ifeq ($(BUILD_VARIANT),full)
	COPTS += -DHAVE_DNSSEC
	COPTS += $(if $(CONFIG_LIBNETTLE_MINI),-DNO_GMP,)
else
	COPTS += -DNO_AUTH -DNO_IPSET
endif

MAKE_FLAGS := \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	COPTS="$(COPTS)" \
	PREFIX="/usr"

define Package/dnsmasq/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/dnsmasq $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/dhcp.conf $(1)/etc/config/dhcp
	$(INSTALL_DATA) ./files/dnsmasq.conf $(1)/etc/dnsmasq.conf
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dnsmasq.init $(1)/etc/init.d/dnsmasq
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/dnsmasq.hotplug $(1)/etc/hotplug.d/iface/25-dnsmasq
endef

Package/dnsmasq-dhcpv6/install = $(Package/dnsmasq/install)

define Package/dnsmasq-full/install
$(call Package/dnsmasq/install,$(1))
	$(INSTALL_DIR) $(1)/usr/share/dnsmasq
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/trust-anchors.conf $(1)/usr/share/dnsmasq
endef

$(eval $(call BuildPackage,dnsmasq))
$(eval $(call BuildPackage,dnsmasq-dhcpv6))
$(eval $(call BuildPackage,dnsmasq-full))
