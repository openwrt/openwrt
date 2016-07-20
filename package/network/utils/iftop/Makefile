#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iftop
PKG_VERSION:=1.0pre4
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.ex-parrot.com/~pdw/iftop/download
PKG_MD5SUM:=7e6decb4958e8a4890cccac335239f24

PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/iftop
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap +libncurses +libpthread
  TITLE:=display bandwith usage on an interface
  URL:=http://www.ex-parrot.com/~pdw/iftop/
endef

define Package/iftop/description
	iftop does for network usage what top(1) does for CPU usage. It 
	listens to network traffic on a named interface and displays a 
	table of current bandwidth usage by pairs of hosts. Handy for 
	answering the question 'why is our ADSL link so slow?'.
endef

define Package/iftop/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iftop $(1)/usr/bin/
endef

$(eval $(call BuildPackage,iftop))
