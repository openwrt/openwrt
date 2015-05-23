#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=conntrack-tools
PKG_VERSION:=1.4.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://www.netfilter.org/projects/conntrack-tools/files \
	ftp://ftp.netfilter.org/pub/conntrack-tools \
	http://mirrors.evolva.ro/netfilter.org/conntrack-tools
PKG_MD5SUM:=b1f9d006e7bf000a77395ff7cd3fac16

PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>
PKG_LICENSE:=GPL-2.0

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

TAR_OPTIONS += --exclude='*.rej'

PKG_BUILD_DEPENDS:=librpc

include $(INCLUDE_DIR)/package.mk

define Package/conntrack-tools
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libnetfilter-conntrack +libnetfilter-cttimeout +libnetfilter-cthelper +libnetfilter-queue
  SUBMENU:=Firewall
  TITLE:=Connection tracking userspace tools
  URL:=http://conntrack-tools.netfilter.org/
endef

define Package/conntrack-tools/description
 The conntrack-tools are a set of free software userspace tools for Linux
 that allow system administrators interact with the Connection Tracking
 System, which is the module that provides stateful packet inspection for
 iptables. The conntrack-tools are the userspace daemon conntrackd and the
 command line interface conntrack.
endef

define Package/conntrack-tools/conffiles
/etc/conntrackd/conntrackd.conf
endef

TARGET_CFLAGS += -D_GNU_SOURCE=1

define Package/conntrack-tools/install
	$(INSTALL_DIR) $(1)/etc/conntrackd
	$(INSTALL_CONF) \
		$(PKG_BUILD_DIR)/doc/stats/conntrackd.conf \
		$(1)/etc/conntrackd/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/conntrack \
		$(PKG_INSTALL_DIR)/usr/sbin/conntrackd \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/conntrackd.init $(1)/etc/init.d/conntrackd
endef

$(eval $(call BuildPackage,conntrack-tools))
