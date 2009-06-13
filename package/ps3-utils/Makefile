#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ps3-utils
PKG_VERSION:=2.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/kernel/people/geoff/cell/ps3-utils
PKG_MD5SUM:=d0b24527a8582c9ff5be02c62bb2161d

include $(INCLUDE_DIR)/package.mk

define Package/ps3-utils
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=PS3 Linux Utilities
  DEPENDS:=@TARGET_ps3||TARGET_ps3chk
  URL:=http://kernel.org/pub/linux/kernel/people/geoff/cell/ps3-utils/
endef

define Package/ps3-utils/description
     The ps3-utils package is a set of system administration utilites for the
     PS3 game console.
endef

define Build/Configure
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" all install
endef

define Package/ps3-utils/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ps3-boot-game-os $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ps3-dump-bootloader $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ps3-flash-util $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ps3-video-mode $(1)/usr/bin

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libps3-utils.so.* $(1)/usr/lib
endef

$(eval $(call BuildPackage,ps3-utils))
