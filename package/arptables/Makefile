# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arptables
PKG_VERSION:=0.0.3-3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ebtables
PKG_MD5SUM:=3f4a8b62920a46d746ab892be7de088f

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-v$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/arptables
  SECTION:=net
  CATEGORY:=Network
  TITLE:=ARP firewalling software
  DEPENDS:=+kmod-arptables
  URL:=http://ebtables.sourceforge.net
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		COPT_FLAGS="$(TARGET_CFLAGS)" \
		KERNEL_DIR="./include/linux"
endef

define Package/arptables/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,arptables))
