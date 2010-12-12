#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# ralph / blogic

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lqdsl
PKG_BASE_NAME:=drv_dsl_cpe_api_danube
PKG_VERSION:=3.24.4.4
PKG_RELEASE:=2
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/drv_dsl_cpe_api-$(PKG_VERSION)
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources/
PKG_MD5SUM:=c45bc531c1ed2ac80f68fb986b63bb87

ifeq ($(DUMP),)
  STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell grep '^CONFIG_LANTIQ_DSL_' $(TOPDIR)/.config | md5s)
endif

FW_BASE_NAME:=dsl_danube_firmware_adsl
FW_A_VER:=02.04.04.00.00.01
FW_B_VER:=02.04.01.07.00.02
FW_A_FILE_VER:=244001
FW_B_FILE_VER:=241702
FW_A_MD5:=f717db3067a0049a26e233ab11238710
FW_B_MD5:=349de7cd20368f4ac9b7e8322114a512

PATCH_DIR ?= ./patches$(if $(wildcard ./patches-$(KERNEL_PATCHVER)),-$(KERNEL_PATCHVER))

include $(INCLUDE_DIR)/package.mk

define KernelPackage/lqdsl
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Lantiq dsl driver
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq 
  DEPENDS:=@TARGET_lantiq_xway +kmod-atm
  FILES:=$(PKG_BUILD_DIR)/src/mei/lantiq_mei.ko \
	$(PKG_BUILD_DIR)/src/drv_dsl_cpe_api.ko \
	$(PKG_BUILD_DIR)/src/mei/lantiq_atm.ko
  AUTOLOAD:=$(call AutoLoad,50,lantiq_mei drv_dsl_cpe_api lantiq_atm)
endef

define KernelPackage/lqdsl/description
	Infineon DSL CPE API for Amazon SE, Danube and Vinax.

	This package contains the DSL CPE API driver for Amazon SE & Danube.
	
	Supported Devices:
		- Amazon SE
		- Danube 
endef

define KernelPackage/lqdsl/config
	source "$(SOURCE)/Config.in"
endef

define Download/annex-a
	FILE:=$(FW_BASE_NAME)_a-$(FW_A_VER).tar.gz
	URL:=http://mirror2.openwrt.org/sources/
	MD5SUM:=$(FW_A_MD5)
endef
$(eval $(call Download,annex-a))

define Download/annex-b
	FILE:=$(FW_BASE_NAME)_b-$(FW_B_VER).tar.gz
	URL:=http://mirror2.openwrt.org/sources/
	MD5SUM:=$(FW_B_MD5)
endef
$(eval $(call Download,annex-b))

IFX_DSL_MAX_DEVICE=1
IFX_DSL_LINES_PER_DEVICE=1
IFX_DSL_CHANNELS_PER_LINE=1

CONFIGURE_ARGS += --enable-kernel-include="$(LINUX_DIR)/include" \
	--with-max-device="$(IFX_DSL_MAX_DEVICE)" \
	--with-lines-per-device="$(IFX_DSL_LINES_PER_DEVICE)" \
	--with-channels-per-line="$(IFX_DSL_CHANNELS_PER_LINE)" \
	--enable-danube \
	--enable-add-drv-cflags="-DMODULE" \
	--disable-dsl-delt-static \
	--disable-adsl-led \
	--enable-dsl-ceoc \
	--enable-dsl-pm \
	--enable-dsl-pm-total \
	--enable-dsl-pm-history \
	--enable-dsl-pm-showtime \
	--enable-dsl-pm-channel-counters \
	--enable-dsl-pm-datapath-counters \
	--enable-dsl-pm-line-counters \
	--enable-dsl-pm-channel-thresholds \
	--enable-dsl-pm-datapath-thresholds \
	--enable-dsl-pm-line-thresholds \
	--enable-dsl-pm-optional-parameters \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	ARCH=$(LINUX_KARCH)

EXTRA_CFLAGS = -fno-pic -mno-abicalls -mlong-calls -G 0

ifeq ($(CONFIG_LANTIQ_DSL_DEBUG),y)
CONFIGURE_ARGS += \
	--enable-debug=yes \
	--enable-debug-prints=yes
EXTRA_CFLAGS += -DDEBUG
endif

define Build/Prepare
	$(PKG_UNPACK)
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/src/mei/ 
	$(CP) ./src/* $(PKG_BUILD_DIR)/src/mei/
	$(Build/Patch)
ifeq ($(CONFIG_LANTIQ_DSL_FIRMWARE),)
	$(TAR) -C $(PKG_BUILD_DIR) -xzf $(DL_DIR)/$(FW_BASE_NAME)_a-$(FW_A_VER).tar.gz
	$(TAR) -C $(PKG_BUILD_DIR) -xzf $(DL_DIR)/$(FW_BASE_NAME)_b-$(FW_B_VER).tar.gz
endif
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); aclocal && autoconf && automake)
	$(call Build/Configure/Default)
endef

define Build/Compile
	cd $(LINUX_DIR); \
		ARCH=mips CROSS_COMPILE="$(KERNEL_CROSS)" \
		$(MAKE) M=$(PKG_BUILD_DIR)/src/mei/ V=1 modules
	$(call Build/Compile/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_api.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_api_ioctl.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_api_adslmib.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_api_adslmib_ioctl.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_api_g997.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_api_types.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_api_pm.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_api_error.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_danube_ctx.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_cmv_danube.h $(1)/usr/include
endef

define KernelPackage/lqdsl/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(CP) $(PKG_BUILD_DIR)/$(FW_BASE_NAME)_$(if $(CONFIG_LANTIQ_ANNEX_A),a_$(FW_A_FILE_VER),b_$(FW_B_FILE_VER)).bin $(1)/lib/firmware/ModemHWE.bin
endef

$(eval $(call KernelPackage,lqdsl))
