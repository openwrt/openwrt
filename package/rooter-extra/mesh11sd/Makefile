# SPDX-License-Identifier: GPL-2.0-only
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
# Copyright (C) 2022 BlueWave Projects and Services  <licence@blue-wave.net>
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mesh11sd
PKG_VERSION:=1.2.0
PKG_RELEASE:=$(AUTORELEASE)

PKG_MAINTAINER:=Rob White <rob@blue-wave.net>
PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=LICENSE

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/opennds/mesh11sd/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=b719eaacf63eb3684d0cd6a026f4357a4f400f2339f5d5a6cf74ba3744fe30d8
PKG_BUILD_DIR:=$(BUILD_DIR)/mesh11sd-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/mesh11sd
  SUBMENU:=Captive Portals
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Dynamic 802.11s Mesh Configuration Daemon
  PKGARCH:=all
  URL:=https://github.com/opennds/mesh11sd
endef

define Package/mesh11sd/description
  Mesh11sd is a dynamic parameter configuration daemon for 802.11s mesh networks.
  It was originally designed to leverage 802.11s mesh networking at Captive Portal venues.
  This is the open source version and it enables easy and automated mesh network operation with multiple mesh nodes.
  It allows all mesh parameters supported by the wireless driver to be set in the uci config file.
  Settings take effect immediately without having to restart the wireless network.
  Default settings give rapid and reliable layer 2 mesh convergence.
  Without mesh11sd, many mesh parameters cannot be set in the uci wireless config file as the mesh interface must be up before the parameters can be set.
  Some of those that are supported, would fail to be implemented when the network is (re)started resulting in errors or dropped nodes.
  The mesh11sd daemon dynamically checks configured parameters and sets them as required.
  This version does not require a Captive Portal to be running.
endef

define Package/mesh11sd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mesh11sd $(1)/usr/sbin
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/linux_openwrt/mesh11sd/files/etc/config/mesh11sd $(1)/etc/config/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/linux_openwrt/mesh11sd/files/etc/init.d/mesh11sd $(1)/etc/init.d/
endef

define Package/mesh11sd/conffiles
/etc/config/mesh11sd
endef

define Build/Compile
endef

$(eval $(call BuildPackage,mesh11sd))
