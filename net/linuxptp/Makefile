#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=linuxptp
PKG_VERSION:=20151118
PKG_RELEASE:=1
PKG_REV:=999c86f4a9da4bf4508b3a69289f58166ed18a55

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://git.code.sf.net/p/linuxptp/code
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=git
PKG_MD5SUM:=

include $(INCLUDE_DIR)/package.mk

define Package/linuxptp
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Time Synchronization
  TITLE:=Linux Precision Time Protocol (PTP) daemon
  URL:=http://linuxptp.sourceforge.net/
  DEPENDS:=@!USE_UCLIBC +librt
endef

define Package/linuxptp/description
 The PTP daemon (PTPd) implements version 2 of the Precision Time Protocol (PTP)
 as defined by the IEEE 1588-2008 standard.
 PTP was developed to provide very precise time coordination of LAN connected
 computers.
endef

define Package/linuxptp/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hwstamp_ctl $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/phc2sys $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/phc_ctl $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pmc $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ptp4l $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/timemaster $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,linuxptp))
