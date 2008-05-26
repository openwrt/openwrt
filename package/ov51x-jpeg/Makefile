#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ov51x-jpeg
PKG_VERSION:=1.5.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.rastageeks.org/downloads/ov51x-jpeg/ \
	http://www.rastageeks.org/downloads/ov51x-jpeg/old-releases/
PKG_MD5SUM:=7de1f426a48bdb55218913e2d713f813

include $(INCLUDE_DIR)/package.mk

define Package/ov51x-jpeg/Default
  URL:=http://www.rastageeks.org/ov51x-jpeg/
endef

define Package/ov51x-jpeg-utils
$(call Package/ov51x-jpeg/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OV51x based USB webcam utilities
  DEPENDS:=+kmod-video-ov51x-jpeg
endef

define Package/ov51x-jpeg-utils/description
 Utility programs for the video-ov51x kernel module
endef

define KernelPackage/video-ov51x-jpeg
$(call Package/ov51x-jpeg/Default)
  SUBMENU:=Video Support
  TITLE:=OV51x based USB webcam support (JPEG)
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-core kmod-video-core
  FILES:=$(PKG_BUILD_DIR)/ov51x-jpeg.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,ov51x-jpeg)
endef

define KernelPackage/video-ov51x-jpeg/description
 Kernel module for OV51x based USB cameras, with in-driver JPEG support.
endef

ifneq ($(CONFIG_PACKAGE_kmod-video-ov51x-jpeg),)
  define Build/Compile/kmod
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
  endef
endif

define Build/Compile
	$(call Build/Compile/kmod)
	$(MAKE) -C $(PKG_BUILD_DIR)/test \
		CFLAGS="$(TARGET_CFLAGS)" \
		CC="$(TARGET_CC)" \
		getjpeg
endef

define Package/ov51x-jpeg-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/test/getjpeg $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ov51x-jpeg-utils))
$(eval $(call KernelPackage,video-ov51x-jpeg))
