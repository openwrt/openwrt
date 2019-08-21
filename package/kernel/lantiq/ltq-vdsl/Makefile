# Copyright (C) 2012 OpenWrt.org
# Copyright (C) 2015-2016 Lantiq Beteiligungs GmbH & Co KG.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_BASE_NAME:=drv_dsl_cpe_api

ifeq ($(CONFIG_TARGET_lantiq_xrx200),y)
DEVICE=vr9
PKG_VERSION:=4.17.18.6
PKG_RELEASE:=3
PKG_SOURCE:=$(PKG_BASE_NAME)_vrx-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_HASH:=b4966a60653acc49254b168c6cc9c49eb36c54548e763617788aa4f252a29f21
PATCH_DIR:=patches
endif

ifeq ($(CONFIG_TARGET_ipq40xx),y)
DEVICE=vr11
PKG_VERSION:=4.21.3
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.prpl.dev/intel/dsl_cpe_api.git
PKG_SOURCE_DATE:=2019-06-09
PKG_SOURCE_VERSION:=bbe7b9f847d26a941b63636e1d096082100dde23
PKG_SOURCE_SUBDIR:=$(PKG_BASE_NAME)-4.21.1
PKG_SOURCE:=$(PKG_BASE_NAME)_vrx-4.21.1.tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_UNPACK=$(HOST_TAR) -C $(PKG_BUILD_DIR) -xzf $(DL_DIR)/$(PKG_SOURCE) --strip 1
PKG_HASH:=68eeb63308ec2bca230a62c9541f6a9a6d34905fd690725c0f2ca65a7351c89c
PATCH_DIR:=patches-$(PKG_VERSION)
endif

PKG_NAME:=ltq-vdsl-$(DEVICE)

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_LICENSE:=GPL-2.0 BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_FIXUP:=autoreconf

PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-vdsl-vr9
  TITLE:=vdsl driver
  SECTION:=sys
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_lantiq_xrx200 +kmod-ltq-vdsl-vr9-mei
  FILES:=$(PKG_BUILD_DIR)/src/drv_dsl_cpe_api.ko
  AUTOLOAD:=$(call AutoLoad,51,drv_dsl_cpe_api)
endef

define Package/ltq-vdsl-vr9/description
	This package contains the Lantiq DSL CPE API driver.

	Supported Devices:
		- VRX200 Family
endef

define KernelPackage/ltq-vdsl-vr11
  TITLE:=vdsl driver
  SECTION:=sys
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq40xx +kmod-ltq-vdsl-vr11-mei
  FILES:=$(PKG_BUILD_DIR)/src/drv_dsl_cpe_api.ko
  AUTOLOAD:=$(call AutoLoad,51,drv_dsl_cpe_api)
endef

define Package/ltq-vdsl-vr11/description
	This package contains the Lantiq DSL CPE API driver.

	Supported Devices:
		- VRX500 Family
endef

MAKE_FLAGS += \
	SHELL="$(BASH)"

ifeq ($(PKG_VERSION),4.17.18.6)
CONFIGURE_ARGS += \
	--with-max-device="1" \
	--with-lines-per-device="1" \
	--with-channels-per-line="1" \
	--enable-adsl-led=no \
	--enable-add-drv-cflags="-DMODULE -DINCLUDE_DSL_ATM_PTM_INTERFACE_SUPPORT"
endif

ifeq ($(PKG_VERSION),4.21.3)
CONFIGURE_ARGS += \
	--enable-add-drv-cflags="" \
	--enable-add_ext_drv_cflags="-DDSL_DRV_ATM_PTM_INTERFACE_ENABLE=1" \
	--enable-debug-logger-support=no
endif

CONFIGURE_ARGS += --enable-kernel-include="$(LINUX_DIR)/include" \
	--enable-vrx \
	--enable-vrx-device=$(DEVICE) \
	--enable-ifxos \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos" \
	--enable-driver-include="-I$(STAGING_DIR)/usr/include/vdsl" \
	--enable-adsl-mib=no \
	--enable-dsl-ceoc=no \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	--enable-debug-prints=no \
	ARCH=$(LINUX_KARCH)

CONFIGURE_ARGS += --enable-model=full
#CONFIGURE_ARGS += --enable-model=lite
#CONFIGURE_ARGS += --enable-model=footprint
#CONFIGURE_ARGS += --enable-model=typical
#CONFIGURE_ARGS += --enable-model=debug

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/drv_vdsl_cpe_api
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe*.h $(1)/usr/include/drv_vdsl_cpe_api/
endef

$(eval $(call KernelPackage,ltq-vdsl-vr9))
$(eval $(call KernelPackage,ltq-vdsl-vr11))
