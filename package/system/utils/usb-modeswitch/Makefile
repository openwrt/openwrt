#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=usb-modeswitch
PKG_VERSION:=1.2.5
PKG_DATA_VERSION:=20121109
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.draisberghof.de/usb_modeswitch
PKG_MD5SUM:=c393603908eceab95444c5bde790f6f0

PKG_DATA_PATH:=usb-modeswitch-data-$(PKG_DATA_VERSION)
PKG_DATA_FILENAME:=$(PKG_DATA_PATH).tar.bz2

PKG_MAINTAINER := Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/usb-modeswitch
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb-compat
  TITLE:=USB ModeSwitch
  URL:=http://www.draisberghof.de/usb_modeswitch/
endef

define Package/usb-modeswitch/description
  A mode switching tool for controlling
  "flip flop" (multiple device) USB gear.
endef

define Package/usb-modeswitch-data
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=usb-modeswitch
  VERSION:=$(PKG_DATA_VERSION)
  TITLE:=USB ModeSwitch - Dongle configuration database
  URL:=http://www.draisberghof.de/usb_modeswitch/
endef

define Package/usb-modeswitch-data/description
  Configuration database for usb-modeswitch.
  Contains definitions for many USB devices required to
  autoswitch "flip flop" gear.
endef

define Download/data
  FILE:=$(PKG_DATA_FILENAME)
  URL:=$(PKG_SOURCE_URL)
  MD5SUM:=a7d23a03157871013a0d708ab2b1b6df
endef
$(eval $(call Download,data))

define Build/Compile
	tar xvfj $(DL_DIR)/$(PKG_DATA_FILENAME) -C $(PKG_BUILD_DIR)
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) \
		-lusb \
		-o $(PKG_BUILD_DIR)/usb_modeswitch-OpenWrt \
		$(PKG_BUILD_DIR)/usb_modeswitch.c
endef

define Package/usb-modeswitch/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/hotplug.d/usb
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usb_modeswitch-OpenWrt $(1)/usr/bin/usb_modeswitch
	$(INSTALL_DATA) ./files/modeswitch.hotplug $(1)/etc/hotplug.d/usb/20-modeswitch
endef

define Package/usb-modeswitch-data/install
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_BUILD_DIR)/$(PKG_DATA_PATH)/usb_modeswitch.d $(1)/etc/
endef

$(eval $(call BuildPackage,usb-modeswitch))
$(eval $(call BuildPackage,usb-modeswitch-data))
