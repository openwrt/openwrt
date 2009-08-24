#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=udevtrigger
PKG_VERSION:=106
PKG_RELEASE:=1

PKG_SOURCE:=udev-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/utils/kernel/hotplug/
PKG_MD5SUM:=320ccd2d0f4540d10e021bafa14f8985

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/udev-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/udevtrigger
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Small utility to request kernel devices events for coldplug
	URL:=http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html
	MAINTAINER:=Geoff Levand <geoffrey.levand@am.sony.com>
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		STRIP="/bin/true" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
 		udevtrigger
endef

define Package/udevtrigger/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/udevtrigger $(1)/sbin/
endef

$(eval $(call BuildPackage,udevtrigger))
