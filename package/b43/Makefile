#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 7440 2007-06-02 02:22:01Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=b43
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

PKG_FWV4_NAME:=broadcom-wl
PKG_FWV4_VERSION=4.150.10.5
PKG_FWV4_OBJECT:=driver/wl_apsta_mimo.o

PKG_FWV4_SOURCE:=$(PKG_FWV4_NAME)-$(PKG_FWV4_VERSION).tar.bz2
PKG_FWV4_SOURCE_URL:=http://mirror2.openwrt.org/sources/
PKG_FWV4_MD5SUM:=0c6ba9687114c6b598e8019e262d9a60

PKG_FWCUTTER_NAME:=b43-fwcutter
PKG_FWCUTTER_VERSION=011

PKG_FWCUTTER_SOURCE:=$(PKG_FWCUTTER_NAME)-$(PKG_FWCUTTER_VERSION).tar.bz2
PKG_FWCUTTER_SOURCE_URL:=http://bu3sch.de/b43/fwcutter/
PKG_FWCUTTER_MD5SUM:=3db2f4de85a459451f5b391cf67a8d44

define KernelPackage/b43
  SUBMENU:=Wireless Drivers
  TITLE:=Broadcom 43xx wireless support
  DEPENDS:=@TARGET_brcm47xx +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/b43.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,b43)
endef

define KernelPackage/b43/description
Kernel module for Broadcom 43xx wireless support (mac80211)
endef

EXTRA_KCONFIG:= \
	CONFIG_B43=m \
	$(if $(CONFIG_LEDS_TRIGGERS),CONFIG_B43_LEDS=y) \


EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	LINUXINCLUDE="-I$(STAGING_DIR)/usr/include/mac80211 -I$(LINUX_DIR)/include -include linux/autoconf.h" \
	$(EXTRA_KCONFIG)

$(STAMP_PREPARED): $(DL_DIR)/$(PKG_FWV4_SOURCE) $(DL_DIR)/$(PKG_FWCUTTER_SOURCE)

$(DL_DIR)/$(PKG_FWV4_SOURCE):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FWV4_SOURCE)" "$(PKG_FWV4_MD5SUM)" $(PKG_FWV4_SOURCE_URL)

$(DL_DIR)/$(PKG_FWCUTTER_SOURCE):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FWCUTTER_SOURCE)" "$(PKG_FWCUTTER_MD5SUM)" $(PKG_FWCUTTER_SOURCE_URL)


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	tar xjf "$(DL_DIR)/$(PKG_FWV4_SOURCE)" -C "$(PKG_BUILD_DIR)"
	tar xjf "$(DL_DIR)/$(PKG_FWCUTTER_SOURCE)" -C "$(PKG_BUILD_DIR)"
	$(Build/Patch)
	$(if $(QUILT),touch $(PKG_BUILD_DIR)/.quilt_used)
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules

	$(MAKE) -C "$(PKG_BUILD_DIR)/$(PKG_FWCUTTER_NAME)-$(PKG_FWCUTTER_VERSION)" \
		CFLAGS="-I$(STAGING_DIR_HOST)/include -include endian.h"
endef

define KernelPackage/b43/install
	$(INSTALL_DIR) $(1)/lib/firmware/

	$(PKG_BUILD_DIR)/$(PKG_FWCUTTER_NAME)-$(PKG_FWCUTTER_VERSION)/b43-fwcutter \
	-w $(1)/lib/firmware/ \
	$(PKG_BUILD_DIR)/$(PKG_FWV4_NAME)-$(PKG_FWV4_VERSION)/$(PKG_FWV4_OBJECT)
endef

$(eval $(call KernelPackage,b43))
