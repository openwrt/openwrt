# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-vdsl-vr9-mei
PKG_VERSION:=1.2.0
PKG_RELEASE:=1

PKG_BASE_NAME:=drv_mei_cpe
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=cf2fccc1bc72390b2aec46650abf2f20
PKG_FIXUP:=autoreconf
PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>
PKG_USE_MIPS16:=0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-vdsl-vr9-mei
  TITLE:=mei driver for vdsl
  SECTION:=sys
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_lantiq_xrx200 +kmod-ltq-ifxos
  FILES:=$(PKG_BUILD_DIR)/src/drv_mei_cpe.ko
  AUTOLOAD:=$(call AutoLoad,50,drv_mei_cpe)
endef

define KernelPackage/ltq-vdsl-vr9-mei/description
	Lantiq MEI CPE Kernel Module Driver
endef

#DEBUG=-DDEBUG_PRINT=1

MAKE_FLAGS += \
	SHELL="$(BASH)"

CONFIGURE_ARGS += \
	--enable-kernelincl="$(LINUX_DIR)/include" \
	--enable-device=vr9 \
	--with-max-device=1 \
	--with-lines-per-device=1 \
	--enable-debug \
	--enable-error_print \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos/" \
	--enable-ifxos-library="-L$(STAGING_DIR)/usr/lib" \
	--enable-add_drv_cflags="$(DEBUG) -DMEI_DRV_ATM_PTM_INTERFACE_ENABLE=1 -fno-pic -mno-abicalls -mlong-calls -O2 -g0" \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	ARCH=$(LINUX_KARCH)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/vdsl
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_api_intern.h $(1)/usr/include/vdsl/
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_api_atm_ptm_intern.h $(1)/usr/include/vdsl/
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_interface.h $(1)/usr/include/vdsl
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_config.h $(1)/usr/include/vdsl/
	$(CP) $(PKG_BUILD_DIR)/src/cmv_message_format.h $(1)/usr/include/vdsl/
endef

$(eval $(call KernelPackage,ltq-vdsl-vr9-mei))
