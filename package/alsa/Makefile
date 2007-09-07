# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=alsa-driver
PKG_VERSION:=1.0.14rc1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.alsa-project.org/pub/driver/
PKG_MD5SUM:=4cff99be4b225e96663fbd61cabe3182

include $(INCLUDE_DIR)/package.mk

define KernelPackage/alsa
  SUBMENU:=Other modules
  DEPENDS:=@USB_SUPPORT @LINUX_2_4
  TITLE:=Advanced Linux Sound Architecture
  FILES:=$(PKG_BUILD_DIR)/modules/*.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD=$(call AutoLoad,70,$(shell cat ./files/alsa.modules))
endef

ifeq ($(KERNEL),2.4)
  ifeq ($(LINUX_KARCH),i386)
    KERNEL_C_OPTS:= -Os -mpreferred-stack-boundary=2 -march=i486 -fno-unit-at-a-time
  endif
  ifeq ($(LINUX_KARCH),mips)
    KERNEL_C_OPTS:= -Os -G 0 -mlong-calls -mno-abicalls -fno-pic -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap
  endif
endif
ifeq ($(LINUX_KARCH),i386)
  KERNEL_C_INCS:= -I$(LINUX_DIR)/include/asm-i386/mach-generic -I$(LINUX_DIR)/include/asm-i386/mach-default
endif
ifeq ($(LINUX_KARCH),mips)
  KERNEL_C_INCS:= -I$(LINUX_DIR)/include/asm-mips/mach-generic
endif

CONFIGURE_VARS:= \
	CFLAGS="$(KERNEL_C_INCS)"

CONFIGURE_ARGS:= \
	--with-build="$(LINUX_DIR)" \
	--with-kernel="$(LINUX_DIR)" \
	--with-cross="$(KERNEL_CROSS)" \
	--with-redhat=no \
	--with-suse=no \
	--with-oss=yes \
	--with-isapnp=no \
	--with-sequencer=no \
	--with-cards=usb-audio

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		c_opts="$(KERNEL_C_OPTS)" \
		CP="$(CP)" \
		all
endef

$(eval $(call KernelPackage,alsa))
