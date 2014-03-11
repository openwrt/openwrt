# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lib_ifxos
PKG_VERSION:=1.5.14
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_RELEASE:=3
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_MD5SUM:=bc107f9d8ff6bed4c2760a2817bbb029
PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

PKG_USE_MIPS16:=0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-ifxos
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Libraries
  TITLE:=Lantiq OS abstraction library
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_lantiq
  FILES:=$(PKG_BUILD_DIR)/src/drv_ifxos.ko
  AUTOLOAD:=$(call AutoLoad,10,drv_ifxos)
endef

CONFIGURE_ARGS += \
	ARCH=$(LINUX_KARCH) \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	--enable-kernelincl="$(LINUX_DIR)/include" \
	--enable-add_drv_cflags="-fno-pic -mno-abicalls -mlong-calls -G 0"

define Build/Configure
	(cd $(PKG_BUILD_DIR); aclocal && autoconf && automake)
	$(call Build/Configure/Default)
endef

ifdef CONFIG_TARGET_lantiq
  define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include/ifxos}
	$(CP) $(PKG_BUILD_DIR)/src/include/* $(1)/usr/include/ifxos
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/libifxos.a $(1)/usr/lib/libifxos.a
  endef
endif

$(eval $(call KernelPackage,ltq-ifxos))
