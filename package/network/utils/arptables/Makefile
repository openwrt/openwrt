# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arptables
PKG_VERSION:=2015-05-20
PKG_RELEASE:=1

PKG_SOURCE_URL:=git://git.netfilter.org/arptables
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=f4ab8f63f11a72f14687a6646d04ae1bae3fa45f
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.xz

PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/arptables
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall
  TITLE:=ARP firewalling software
  DEPENDS:=+kmod-arptables
  URL:=https://git.netfilter.org/arptables/
endef

MAKE_FLAGS += \
	COPT_FLAGS="$(TARGET_CFLAGS) -D__OPTIMIZE__=1" \
	KERNEL_DIR="$(LINUX_DIR)"

define Package/arptables/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,arptables))
