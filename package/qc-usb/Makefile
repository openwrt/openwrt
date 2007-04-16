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

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/qc-usb-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/qce-ga
PKG_MD5SUM:=6f6787e1dda11ca3b936ad434154f426
PKG_CAT:=zcat

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/qc-usb-utils/Default
  TITLE:=Utility programs for the qc-usb kernel module
  DESCRIPTION:=\
  	Utilities to tweak paramters of your QuickCam Express or \\\
  	similar webcam. These programs are completely useless without a \\\
  	qc-usb-modules package.
  URL:=http://qce-ga.sourceforge.net/
endef

define Package/qc-usb-utils
  SECTION:=utils
  CATECORY:=Utilities
  $(call Package/qc-usb-utils/Default)
endef

define KernelPackage/qc-usb
  TITLE:=Support for USB QuickCam Express webcam
  DESCRIPTION:=\
	Kernel support for Logitech's QuickCam Express webcam and \\\
	other webcams with similar chipsets.
  SUBMENU:=USB Support
  DEPENDS:=@LINUX_2_6
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/quickcam.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,quickcam)
endef

ifeq ($(CONFIG_LINUX_2_6),y)
  define Build/Compile/linux26
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
	$(call Build/Compile/linux26)

	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS)" \
		qcset
endef

define Package/qc-usb-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qcset $(1)/usr/bin
endef

$(eval $(call BuildPackage,qc-usb-utils))
$(eval $(call KernelPackage,qc-usb))
