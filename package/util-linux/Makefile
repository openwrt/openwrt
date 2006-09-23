# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 3822 2006-05-24 07:01:52Z nico $

include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.12r
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.kernel.org/pub/linux/utils/$(PKG_NAME)/ \
	http://ftp.kernel.org/pub/linux/utils/$(PKG_NAME)/ \
	ftp://ftp.de.kernel.org/pub/linux/utils/$(PKG_NAME)/ \
	http://ftp.de.kernel.org/pub/linux/utils/$(PKG_NAME)/
PKG_MD5SUM:=c261230b27fc0fbcc287c76884caf2d3
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/util-linux/Default
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://www.kernel.org/pub/linux/utils/util-linux/
endef

define Package/fdisk
  $(call Package/util-linux/Default)
  TITLE:=Partition table manipulation utility
  DESCRIPTION:=\
	This package contains an utility for managing disk partition tables.
  URL:=http://www.kernel.org/pub/linux/utils/util-linux/
endef

define Package/losetup
  $(call Package/util-linux/Default)
  TITLE:=Loopback devices setup and control utility
  DESCRIPTION:=\
	This package contains an utility for managing loopback devices.
endef

define Package/swap-utils
  $(call Package/util-linux/Default)
  TITLE:=Swap space management utilities
  DESCRIPTION:=\
	This package contains a collection of tools for managing swap space: \\\
	- mkswap\\\
	- swapon\\\
	- swapoff
endef

define Build/Compile
	$(call Build/Compile/Default, \
		OPT="$(TARGET_CFLAGS)" \
		INSTALLSUID="install -m 4755" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install \
	)
endef

define Package/Template
	@if [ \! -f "$(PKG_INSTALL_DIR)/$(1)" ]; then \
		rm -f $(PKG_BUILD_DIR)/.built; \
		$(MAKE) $(PKG_BUILD_DIR)/.built; \
	fi
	install -d -m0755 $(2)
	$(CP) $(PKG_INSTALL_DIR)/$(1) $(2)/
endef

define Package/fdisk/install
	$(call Package/Template,sbin/fdisk,$(1)/usr/sbin)
endef

define Package/losetup/install
	$(call Package/Template,sbin/losetup,$(1)/usr/sbin)
endef

define Package/swap-utils/install
	$(call Package/Template,sbin/mkswap,$(1)/usr/sbin)
	$(call Package/Template,sbin/swapon,$(1)/usr/sbin)
	$(call Package/Template,sbin/swapoff,$(1)/usr/sbin)
endef

$(eval $(call BuildPackage,fdisk))
$(eval $(call BuildPackage,losetup))
$(eval $(call BuildPackage,swap-utils))
