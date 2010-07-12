#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=compcache
PKG_VERSION:=0.6.2
PKG_RELEASE:=2
PKG_SOURCE_URL:=http://compcache.googlecode.com/files/
PKG_MD5SUM:=27aec78dc50e34fb800c74e879057743

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ramzswap
  SUBMENU:=Other modules
  DEPENDS:=@BUSYBOX_CONFIG_SWAPONOFF
  TITLE:=Driver for compressed ram swap device
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)+$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/ramzswap.ko \
	 $(PKG_BUILD_DIR)/sub-projects/compression/lzo-kmod/lzo1x.ko
  URL:=http://code.google.com/p/compcache/
endef

define Package/compcache
  SECTION:=util
  CATEGORY:=Utilities
  DEPENDS:=+kmod-ramzswap
  TITLE:=Compressed ram swap device
  URL:=http://code.google.com/p/compcache/
  MENU:=1
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

LZO = sub-projects/compression/lzo-kmod
RZSC = sub-projects/rzscontrol

BUILDFLAGS:=-DCONFIG_RAMZSWAP_STATS \
	    -I$(PKG_BUILD_DIR)/$(LZO) \
	    -Wall

RZSCFLAGS:=-I$(PKG_BUILD_DIR)/$(RZSC)/../include \
	   -I$(PKG_BUILD_DIR)/$(RZSC)/../..

define Build/Compile
	$(MAKE) $(KERNEL_MAKEOPTS) EXTRA_CFLAGS="$(BUILDFLAGS)" M="$(PKG_BUILD_DIR)" modules
	$(TARGET_CC) $(TARGET_CFLAGS) $(RZSCFLAGS) $(PKG_BUILD_DIR)/$(RZSC)/rzscontrol.c -o $(PKG_BUILD_DIR)/rzscontrol
endef

define Package/compcache/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/compcache.config $(1)/etc/config/compcache
	$(SED) 's,%ENABLED%,$(if $(CONFIG_COMPCACHE_ENABLE),1,0),g' \
	      -e 's,%RAM_REPORTED%,$(call qstrip,$(CONFIG_COMPCACHE_RAM_REPORTED)),g' \
	      -e 's,%BACKUP_DEV%,$(call qstrip,$(CONFIG_COMPCACHE_BACKUP_DEV)),g' \
	      $(1)/etc/config/compcache
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/compcache.init $(1)/etc/init.d/compcache
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rzscontrol $(1)/sbin/rzscontrol
endef

define Package/compcache/config
	source "$(SOURCE)/Config.in"
endef

$(eval $(call BuildPackage,compcache))
$(eval $(call KernelPackage,ramzswap))
