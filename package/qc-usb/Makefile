# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qc-usb
PKG_VERSION:=0.6.5
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/qce-ga
PKG_MD5SUM:=6f6787e1dda11ca3b936ad434154f426

include $(INCLUDE_DIR)/package.mk

define Package/qc-usb/Default
  URL:=http://qce-ga.sourceforge.net/
endef

define Package/qc-usb-utils
$(call Package/qc-usb/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Utility programs for the qc-usb kernel module
endef

define Package/qc-usb-utils/description
 Utilities to tweak parameters of your QuickCam Express or similar webcam. 
 These programs are completely useless without a qc-usb-modules package.
endef

define KernelPackage/usb-qc
$(call Package/qc-usb/Default)
  TITLE:=Support for USB QuickCam Express webcam
  SUBMENU:=USB Support
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT kmod-usb-core +kmod-videodev
  FILES:=$(PKG_BUILD_DIR)/quickcam.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,quickcam)
endef

define KernelPackage/usb-qc/description
 Kernel support for Logitech's QuickCam Express webcam and other webcams 
 with similar chipsets.
endef

ifneq ($(CONFIG_PACKAGE_kmod-usb-qc),)
  define Build/Compile/kmod
	$(MAKE) -C $(LINUX_DIR) \
		PATH="$(TARGET_PATH)" \
                KVER="$(LINUX_VERSION)" \
		KERNELRELEASE="$(LINUX_VERSION)" \
                KSRC="$(LINUX_DIR)" \
                SUBDIRS="$(PKG_BUILD_DIR)" \
                KERNEL_DIR="$(KERNEL_DIR)" \
                ARCH="$(LINUX_KARCH)" \
                CROSS_COMPILE="$(TARGET_CROSS)" \
		modules
  endef
endif

define Build/Compile
	$(call Build/Compile/kmod)

	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS)" \
		qcset
endef

define Package/qc-usb-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qcset $(1)/usr/bin/
endef

$(eval $(call BuildPackage,qc-usb-utils))
$(eval $(call KernelPackage,usb-qc))
