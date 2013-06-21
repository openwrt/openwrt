#
# Copyright (C) 2007-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=usbutils
PKG_VERSION:=006
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/utils/usb/usbutils
PKG_MD5SUM:=9d13954981f4adbe3fd02aae6dbfafa9

USB_IDS_VERSION:=2013-01-16
USB_IDS_MD5SUM:=2a2344907b6344f0935c86efaf9de620
USB_IDS_FILE:=usb.ids.$(USB_IDS_VERSION).gz

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

PKG_MAINTAINER := Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/usbutils
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb-1.0 +zlib +librt +libpthread
  TITLE:=USB devices listing utilities
  URL:=http://www.linux-usb.org/
endef

define Download/usb_ids
  FILE:=$(USB_IDS_FILE)
  URL:=http://mirror2.openwrt.org/sources
  MD5SUM:=$(USB_IDS_MD5SUM)
endef
$(eval $(call Download,usb_ids))

define Build/Prepare
	$(Build/Prepare/Default)
	echo '#!/bin/sh' > $(PKG_BUILD_DIR)/update-usbids.sh.in
	echo 'cp $(DL_DIR)/$(USB_IDS_FILE) usb.ids.gz' >> $(PKG_BUILD_DIR)/update-usbids.sh.in
endef

define Package/usbutils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lsusb $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/share/usb.ids.gz $(1)/usr/share/
endef

$(eval $(call BuildPackage,usbutils))
