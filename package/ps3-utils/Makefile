#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ps3-utils
PKG_REV:=6488134e48cf2d6f2d6471ced8346ac8cb1b855a
PKG_VERSION:=20090320
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://git.kernel.org/pub/scm/linux/kernel/git/geoff/ps3-utils.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)

PKG_FIXUP:=autoreconf
PKG_INSTALL=1

include $(INCLUDE_DIR)/package.mk

define Package/ps3-utils
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=PS3 Linux Utilities
	URL:=http://kernel.org/pub/linux/kernel/people/geoff/cell/ps3-utils/
	MAINTAINER:=Geoff Levand <geoffrey.levand@am.sony.com>
	DEPENDS:=@TARGET_ps3||TARGET_ps3chk||TARGET_powerpc
endef

define Package/ps3-utils/description
     The ps3-utils package is a set of system administration utilites for the
     PS3 game console.
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR) && $(BASH) -x ./bootstrap)
	$(call Build/Configure/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/include/ps3*.h \
		$(1)/usr/include

	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/lib/libps3-utils.{la,a,so*} \
		$(1)/usr/lib/
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
