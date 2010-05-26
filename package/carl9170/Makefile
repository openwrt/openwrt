#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=carl9170
PKG_VERSION:=1.0.5.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/kernel/people/chr/carl9170/$(PKG_VERSION) \
	@KERNEL/linux/kernel/people/chr/carl9170/old/$(PKG_VERSION)
PKG_MD5SUM:=91d714d1e5c5c62da1191ac5afd8bcb0

PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_CARL9170_DEBUGFS \

include $(INCLUDE_DIR)/package.mk

DRV_PATH:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath/carl9170

define KernelPackage/carl9170
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Atheros AR9170 USB sticks
  DEPENDS:=@USB_SUPPORT +kmod-mac80211 +kmod-ath +kmod-usb-core
  URL:=http://www.kernel.org/pub/linux/kernel/people/chr/carl9170
  FILES:=$(DRV_PATH)/carl9170usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,carl9170usb)
  MENU:=1
endef

define KernelPackage/carl9170/config
	menu "Configuration"
		depends PACKAGE_kmod-carl9170

	config PACKAGE_CARL9170_DEBUGFS
		bool "Enable Debugging (DebugFS) Support"
		depends PACKAGE_MAC80211_DEBUGFS
		help
		  Say Y if you need access to carl9170usb's statistics for QoS queue 
		  status, rate control, etc...

	endmenu
endef

define KernelPackage/carl9170/description
 This package contains a driver for Atheros AR9170 USB sticks using the free firmware
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(HOST_TAR) -C $(PKG_BUILD_DIR) -xjf $(DL_DIR)/$(PKG_SOURCE)
	$(HOST_TAR) -C $(PKG_BUILD_DIR) -xf $(PKG_BUILD_DIR)/$(PKG_VERSION)/driver/carl9170-driver-$(PKG_VERSION).tar
	ln -s $(STAGING_DIR)/usr/include/mac80211/ath/*.h $(PKG_BUILD_DIR)/drivers/net/wireless/ath/
	$(Build/Patch)
endef

BUILDFLAGS = \
	CONFIG_CARL9170_USB=m \
	CONFIG_CARL9170_LEDS=$(if $(CONFIG_LEDS_TRIGGERS),y) \
	CONFIG_CARL9170_DEBUGFS=$(if $(CONFIG_PACKAGE_CARL9170_DEBUGFS),y) \
	CONFIG_MAC80211_DEBUGFS=$(if $(CONFIG_PACKAGE_MAC80211_DEBUGFS),y) \

EXTRA_CFLAGS = \
	$(if $(CONFIG_ATH_USER_REGD),-DATH_USER_REGD) \
	$(if $(CONFIG_LEDS_TRIGGERS), -DCONFIG_MAC80211_LEDS -DCONFIG_LEDS_TRIGGERS -DCONFIG_CARL9170_LEDS) \
	$(if $(CONFIG_PACKAGE_CARL9170_DEBUGFS),-DCONFIG_CARL9170_DEBUGFS) \
	$(if $(CONFIG_PACKAGE_MAC80211_DEBUGFS),-DCONFIG_MAC80211_DEBUGFS) \

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		SUBDIRS="$(DRV_PATH)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(KERNEL_CROSS)" \
		CC="$(TARGET_CC) -I$(STAGING_DIR)/usr/include/mac80211 $(EXTRA_CFLAGS)" \
		$(BUILDFLAGS)
endef

define KernelPackage/carl9170/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_VERSION)/firmware_build/carl9170-1.fw $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,carl9170))
