# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 4823 2006-09-23 13:45:49Z nico $

include $(TOPDIR)/rules.mk

PKG_NAME:=qos-scripts
PKG_VERSION:=0.9.3
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/qos-scripts
  SECTION:=utils
  CATEGORY:=Base system
  DEPENDS:=+tc +kmod-sched +iptables-mod-filter +iptables-mod-ipopt +iptables-mod-extra +iptables-mod-conntrack +iptables-mod-imq
  TITLE:=QoS scripts
  DESCRIPTION:=QoS scripts \\\
	 A set of scripts that abstract QoS configuration \\\
	 into a simple configuration file supporting stanzas that specify \\\
	 any number of QoS entries.
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/qos-scripts/install
	mkdir -p $(1)
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,qos-scripts))
