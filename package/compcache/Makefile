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
PKG_RELEASE:=3
PKG_SOURCE_URL:=http://compcache.googlecode.com/files/
PKG_MD5SUM:=27aec78dc50e34fb800c74e879057743

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ramzswap
  SUBMENU:=Other modules
  DEPENDS:=@BUSYBOX_CONFIG_SWAPONOFF
  TITLE:=Driver for compressed ram swap device
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)+$(PKG_RELEASE)
  KCONFIG:=CONFIG_RAMZSWAP=m CONFIG_RAMZSWAP_STATS=n
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.33)),1)
  FILES:= \
  	$(LINUX_DIR)/drivers/staging/ramzswap/ramzswap.ko \
	$(LINUX_DIR)/lib/lzo/lzo_compress.ko \
	$(LINUX_DIR)/lib/lzo/lzo_decompress.ko 
else
  FILES:= \
        $(PKG_BUILD_DIR)/ramzswap.ko \
        $(PKG_BUILD_DIR)/sub-projects/compression/lzo-kmod/lzo1x.ko
endif
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

RZSC = sub-projects/rzscontrol

RZSCFLAGS:=-I$(PKG_BUILD_DIR)/$(RZSC)/../include \
	   -I$(PKG_BUILD_DIR)/$(RZSC)/../..

ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.33)),1)
 define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(RZSCFLAGS) $(PKG_BUILD_DIR)/$(RZSC)/rzscontrol.c -o $(PKG_BUILD_DIR)/rzscontrol 
 endef
else 
 define Build/Compile
	$(MAKE) $(KERNEL_MAKEOPTS) EXTRA_CFLAGS="-I$(PKG_BUILD_DIR)/sub-projects/compression/lzo-kmod -Wall" M="$(PKG_BUILD_DIR)" modules
	$(TARGET_CC) $(TARGET_CFLAGS) $(RZSCFLAGS) $(PKG_BUILD_DIR)/$(RZSC)/rzscontrol.c -o $(PKG_BUILD_DIR)/rzscontrol
 endef
endif

define Package/compcache/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/compcache.config $(1)/etc/config/compcache
	$(SED) 's,%ENABLED%,$(if $(CONFIG_COMPCACHE_ENABLE),1,0),g' \
	      -e 's,%RAM_REPORTED%,$(call qstrip,$(CONFIG_COMPCACHE_RAM_REPORTED)),g' \
	      -e 's,%BACKUP_DEV%,$(call qstrip,$(CONFIG_COMPCACHE_BACKUP_DEV)),g' \
	      $(1)/etc/config/compcache
	$(INSTALL_DIR) $(1)/etc/init.d
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.33)),1)
	$(INSTALL_BIN) ./files/compcache.init.new $(1)/etc/init.d/compcache
else
	$(INSTALL_BIN) ./files/compcache.init $(1)/etc/init.d/compcache
endif
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rzscontrol $(1)/sbin/rzscontrol
endef

define Package/compcache/config
	source "$(SOURCE)/Config.in"
endef

$(eval $(call BuildPackage,compcache))
$(eval $(call KernelPackage,ramzswap))
