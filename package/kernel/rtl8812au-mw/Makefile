# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8812au-mw
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=LICENSE

PKG_SOURCE_URL:=https://github.com/morrownr/8812au
PKG_MIRROR_HASH:=fa3a0eb4f97565efb2a12f83711cb0c25343c7bc83fc4dbc0247fc47d8d6c79b
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-05-31
PKG_SOURCE_VERSION:=2ad8892541d0321f1b859f52c438a74822b01bfa

PKG_MAINTAINER:=Dirk Neukirchen <plntyk.lede@plntyk.name>
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl8812au-mw
  SUBMENU:=Wireless Drivers
  TITLE:=Realtek 8812AU driver based on 5.9.3.2 mod by morrownr
  DEPENDS:=+kmod-cfg80211 +kmod-usb-core +@DRIVER_11N_SUPPORT +@DRIVER_11AC_SUPPORT
  FILES:=\
	$(PKG_BUILD_DIR)/rtl8812au.ko
  AUTOLOAD:=$(call AutoProbe,rtl8812au)
  PROVIDES:=kmod-rtl8812au
endef

NOSTDINC_FLAGS := \
	$(KERNEL_NOSTDINC_FLAGS) \
	-I$(PKG_BUILD_DIR) \
	-I$(PKG_BUILD_DIR)/include \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-include backport/backport.h

ifdef CONFIG_PACKAGE_kmod-rtl8812au-mw
   PKG_MAKE_FLAGS += CONFIG_RTL8812AU=m
endif

NOSTDINC_FLAGS+=-DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -DBUILD_OPENWRT


define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		$(PKG_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

$(eval $(call KernelPackage,rtl8812au-mw))
