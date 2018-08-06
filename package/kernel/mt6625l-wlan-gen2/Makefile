#
# Copyright (C) 2014-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mt6625l-wlan-gen2
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/abbradar/mt6625l-wlan-gen2
PKG_SOURCE_DATE:=2018-07-05
PKG_SOURCE_VERSION:=d7ed406b7d4d4b608f6416269075281d090ecfd7
PKG_MAINTAINER:=Nikolay Amiantov <ab@fmap.me>

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/mt6625l-wlan-gen2
  SUBMENU:=Wireless Drivers
  TITLE:=Mediatek MT6625L wireless chip support
  URL:=http://www.datasheetcafe.com/mt6625l-datasheet-chip-mediatek/
  KCONFIG:= \
    CONFIG_MTK_WAPI_SUPPORT=y \
    CONFIG_MTK_PASSPOINT_R1_SUPPORT=y \
    CONFIG_MTK_PASSPOINT_R2_SUPPORT=y \
    CONFIG_MTK_WIFI_MCC_SUPPORT=y \
    CONFIG_MTK_COMBO_WIFI
  DEPENDS:=@TARGET_mediatek +kmod-mac80211 +@DRIVER_11N_SUPPORT
  FILES:= \
        $(PKG_BUILD_DIR)/wlan_gen2.ko
  AUTOLOAD:=$(call AutoProbe,wlan_gen2)
endef

define KernelPackage/mt6625l-wlan-gen2/description
  Kernel support for Mediatek MT6625L connectivity chip Wi-Fi module
endef

NOSTDINC_FLAGS = \
	-I$(PKG_BUILD_DIR) \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-include backport/backport.h

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

$(eval $(call KernelPackage,mt6625l-wlan-gen2))
