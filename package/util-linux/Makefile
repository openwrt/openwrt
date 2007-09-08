# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.12r
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@KERNEL/linux/utils/$(PKG_NAME)/
PKG_MD5SUM:=c261230b27fc0fbcc287c76884caf2d3

include $(INCLUDE_DIR)/package.mk

define Package/util-linux/Default
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@!TARGET_etrax
  URL:=http://www.kernel.org/pub/linux/utils/util-linux/
endef

define Package/fdisk
$(call Package/util-linux/Default)
  TITLE:=Partition table manipulation utility
endef

define Package/fdisk/description
 This package contains a utility for managing disk partition tables.
endef

define Package/cfdisk
$(call Package/util-linux/Default)
  TITLE:=Partition table manipulation utility
  DEPENDS:= +libncurses
endef

define Package/cfdisk/description
 This package contains a utility for managing disk partition tables.
endef

define Package/losetup
$(call Package/util-linux/Default)
  TITLE:=Loopback devices setup and control utility
endef

define Package/losetup/description
 This package contains a utility for managing loopback devices.
endef

define Package/swap-utils
$(call Package/util-linux/Default)
  TITLE:=Swap space management utilities
endef

define Package/swap-utils/description
 This package contains a collection of tools for managing swap space: 
 - mkswap
 - swapon
 - swapoff
endef

define Package/hwclock
$(call Package/util-linux/Default)
  TITLE:=Utilities for managing the hardware clock
endef

define Package/hwclock/description
 This package contains a utility for managing the hardware clock.
endef

define Build/Configure
	# this is NOT GNU configure!
	( cd $(PKG_BUILD_DIR); \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		OPT="$(TARGET_CFLAGS)" \
		INSTALLSUID="install -m 4755" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/Template
	@if [ \! -f "$(PKG_INSTALL_DIR)/$(1)" ]; then \
		rm -f $(PKG_BUILD_DIR)/.built; \
		$(MAKE) $(PKG_BUILD_DIR)/.built; \
	fi
	$(INSTALL_DIR) $(2)
	$(CP) $(PKG_INSTALL_DIR)/$(1) $(2)/
endef

define Package/fdisk/install
	$(call Package/Template,sbin/fdisk,$(1)/usr/sbin)
endef

define Package/cfdisk/install
	$(call Package/Template,sbin/cfdisk,$(1)/usr/sbin)
endef

define Package/losetup/install
	$(call Package/Template,sbin/losetup,$(1)/usr/sbin)
endef

define Package/swap-utils/install
	$(call Package/Template,sbin/mkswap,$(1)/usr/sbin)
	$(call Package/Template,sbin/swapon,$(1)/usr/sbin)
	$(call Package/Template,sbin/swapoff,$(1)/usr/sbin)
endef

define Package/hwclock/install
	$(call Package/Template,sbin/hwclock,$(1)/usr/sbin)
endef

$(eval $(call BuildPackage,fdisk))
$(eval $(call BuildPackage,cfdisk))
$(eval $(call BuildPackage,losetup))
$(eval $(call BuildPackage,swap-utils))
$(eval $(call BuildPackage,hwclock))
