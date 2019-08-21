# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lib_ifxos
PKG_VERSION:=1.6.9
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.prpl.dev/intel/ifxos.git
PKG_SOURCE_DATE:=2019-10-27
PKG_SOURCE_VERSION:=3bab16149813af37056d2063da7c5c67c96ec470
PKG_HASH:=7d0456360dfe3abbce225428026003c817433691462b073d2eb6fce01b93fc8f
PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=GPL-2.0 BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-ifxos
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Libraries
  TITLE:=Lantiq OS abstraction library
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_ipq40xx||TARGET_lantiq
  FILES:=$(PKG_BUILD_DIR)/src/drv_ifxos.ko
  AUTOLOAD:=$(call AutoLoad,10,drv_ifxos)
endef

CONFIGURE_ARGS += \
	ARCH=$(LINUX_KARCH) \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	--enable-kernelincl="$(LINUX_DIR)/include" \
	--with-kernel-module

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include/ifxos}
	$(CP) $(PKG_BUILD_DIR)/src/include/* $(1)/usr/include/ifxos
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/libifxos.a $(1)/usr/lib/libifxos.a
endef

$(eval $(call KernelPackage,ltq-ifxos))
