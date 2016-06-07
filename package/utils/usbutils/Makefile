#
# Copyright (C) 2007-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=usbutils
PKG_VERSION:=007
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/utils/usb/usbutils
PKG_MD5SUM:=a6bd63d5c44cebb717a960eae22a3ca9
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING

USB_IDS_VERSION:=2016-03-03
USB_IDS_MD5SUM:=273596b6ee101b1df454827cfa87dff8
USB_IDS_FILE:=usb.ids

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

PKG_MAINTAINER := Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk

define Package/usbutils
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb-1.0 +librt +libpthread
  TITLE:=USB devices listing utilities
  URL:=http://www.linux-usb.org/
endef

define Download/usb_ids
  FILE:=$(USB_IDS_FILE)
  URL:=@GITHUB/gentoo/hwids/d9e840aa3d5cedf5637d59ef0dc555c380a0e822
  MD5SUM:=$(USB_IDS_MD5SUM)
endef
$(eval $(call Download,usb_ids))

define Build/Prepare
	$(Build/Prepare/Default)
	$(CP) $(DL_DIR)/$(USB_IDS_FILE) $(PKG_BUILD_DIR)
endef

CONFIGURE_ARGS += \
	--disable-zlib

define Package/usbutils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lsusb $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/share/usb.ids $(1)/usr/share/
endef

$(eval $(call BuildPackage,usbutils))
