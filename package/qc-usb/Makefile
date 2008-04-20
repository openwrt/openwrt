# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qc-usb
PKG_VERSION:=0.6.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/qce-ga
PKG_MD5SUM:=9eab8fb3a75326d1565d59b0c7256075

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

define KernelPackage/video-quickcam
$(call Package/qc-usb/Default)
  SUBMENU:=Video Support
  TITLE:=QuickCam Express USB webcam support
  DEPENDS:=@USB_SUPPORT +kmod-usb-core kmod-video-core
  FILES:=$(PKG_BUILD_DIR)/quickcam.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,quickcam)
endef

define KernelPackage/video-quickcam/description
 Kernel support for Logitech's QuickCam Express webcam and other webcams 
 with similar chipsets.
endef

ifeq ($(KERNEL),2.4)
  ifeq ($(LINUX_KARCH),i386)
    KERNEL_C_OPTS:= -Os -mpreferred-stack-boundary=2 -march=i486 -fno-unit-at-a-time
  endif
  ifeq ($(LINUX_KARCH),mips)
    KERNEL_C_OPTS:= -Os -G 0 -mlong-calls -mno-abicalls -fno-pic -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap
  endif
endif

ifneq ($(CONFIG_PACKAGE_kmod-video-quickcam),)
  define Build/Compile/kmod
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LINUX_DIR="$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CC) -nodefaultlibs -nostartfiles" \
		USER_OPT="$(KERNEL_C_OPTS)" \
		quickcam.$(LINUX_KMOD_SUFFIX)
  endef
endif

define Build/Compile/user
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS)" \
		qcset
endef

define Build/Compile
	$(call Build/Compile/kmod)
	$(call Build/Compile/user)
endef

define Package/qc-usb-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qcset $(1)/usr/bin/
endef

$(eval $(call BuildPackage,qc-usb-utils))
$(eval $(call KernelPackage,video-quickcam))
