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
PKG_VERSION:=1.0.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.alsa-project.org/pub/driver/
PKG_MD5SUM:=57534e4297cd683371402220e3753885
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/kmod-alsa
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  DEPENDS:=@USB_SUPPORT
  TITLE:=Advanced Linux Sound Architecture
  URL:=http://alsa-project.org/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

ifeq ($(KERNEL),2.4)
  ifeq ($(LINUX_KARCH),i386)
    KERNEL_C_OPTS:= -Os -mpreferred-stack-boundary=2 -march=i486 -fno-unit-at-a-time
  endif
  ifeq ($(LINUX_KARCH),mips)
    KERNEL_C_OPTS:= -Os -G 0 -mno-abicalls -fno-pic -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap
  endif
endif
ifeq ($(LINUX_KARCH),i386)
  KERNEL_C_INCS:= -I$(LINUX_DIR)/include/asm-i386/mach-generic -I$(LINUX_DIR)/include/asm-i386/mach-default
endif
ifeq ($(LINUX_KARCH),mips)
  KERNEL_C_INCS:= -I$(LINUX_DIR)/include/asm-mips/mach-generic
endif

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		CFLAGS="$(KERNEL_C_INCS)" \
		./configure \
			--with-build="$(LINUX_DIR)" \
			--with-kernel="$(LINUX_DIR)" \
			--with-cross="$(KERNEL_CROSS)" \
			--with-redhat=no \
			--with-suse=no \
			--with-oss=yes \
			--with-isapnp=no \
			--with-sequencer=no \
			--with-cards=usb-audio \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		c_opts="$(KERNEL_C_OPTS)" \
		all
endef

define Package/kmod-alsa/install
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	install -m0644 $(PKG_BUILD_DIR)/modules/*.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
	install -d -m0755 $(1)/etc/modules.d
	install -m0644 ./files/alsa.modules $(1)/etc/modules.d/70-alsa
endef

$(eval $(call BuildPackage,kmod-alsa))
