# Copyright (C) 2012 OpenWrt.org
# Copyright (C) 2015-2016 Lantiq Beteiligungs GmbH & Co KG.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-vdsl-vr11
PKG_VERSION:=4.23.1
PKG_RELEASE:=3
PKG_BASE_NAME:=dsl_cpe_api

UGW_VERSION=8.5.2.10
UGW_BASENAME=$(PKG_BASE_NAME)-ugw_$(UGW_VERSION)

PKG_SOURCE:=$(UGW_BASENAME).tar.bz2
PKG_SOURCE_URL:=https://gitlab.com/prpl-foundation/intel/$(PKG_BASE_NAME)/-/archive/ugw_$(UGW_VERSION)/
PKG_HASH:=5e8bbab841d67dc16e329d9b3774f6db4189dd1d01f575d0e921ccf2c426dd9f
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(UGW_BASENAME)
PKG_LICENSE:=GPL-2.0 BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_FIXUP:=autoreconf
PKG_BUILD_FLAGS:=no-mold

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-vdsl-vr11
  TITLE:=vdsl driver
  SECTION:=sys
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq40xx +kmod-ltq-vdsl-vr11-mei +dsl_vr11_firmware_xdsl
  FILES:=$(PKG_BUILD_DIR)/src/drv_dsl_cpe_api.ko
  AUTOLOAD:=$(call AutoLoad,51,drv_dsl_cpe_api)
endef

define Package/ltq-vdsl-vr11/description
  This package contains the Lantiq DSL CPE API driver.

  Supported Devices:
  - VRX500 Family
endef

MAKE_FLAGS += \
	$(KERNEL_MAKE_FLAGS) \
	SHELL="$(BASH)"

CONFIGURE_ARGS += \
	--enable-add-drv-cflags="" \
	--enable-add_ext_drv_cflags="-DDSL_DRV_ATM_PTM_INTERFACE_ENABLE=1" \
	--enable-debug-logger-support=no

CONFIGURE_ARGS += --enable-kernel-include="$(LINUX_DIR)/include" \
	--enable-vrx \
	--enable-vrx-device=vr11 \
	--enable-ifxos \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos" \
	--enable-driver-include="-I$(STAGING_DIR)/usr/include/vdsl" \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	--enable-debug-prints=no \
	ARCH=$(LINUX_KARCH)

CONFIGURE_ARGS += \
	--enable-model=full \
	--enable-dsl-ceoc=no
#CONFIGURE_ARGS += --enable-model=lite
#CONFIGURE_ARGS += --enable-model=footprint
#CONFIGURE_ARGS += --enable-model=typical
#CONFIGURE_ARGS += --enable-model=debug

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/drv_vdsl_cpe_api
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe*.h $(1)/usr/include/drv_vdsl_cpe_api/
endef

$(eval $(call KernelPackage,ltq-vdsl-vr11))
