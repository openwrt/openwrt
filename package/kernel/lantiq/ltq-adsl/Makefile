#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-adsl
PKG_VERSION:=3.24.4.4
PKG_RELEASE:=1
PKG_SOURCE:=drv_dsl_cpe_api_danube-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/ltq-dsl-$(BUILD_VARIANT)/drv_dsl_cpe_api-$(PKG_VERSION)
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources/
PKG_MD5SUM:=c45bc531c1ed2ac80f68fb986b63bb87
PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

PKG_USE_MIPS16:=0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-adsl-template
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=adsl driver for $(1)
  URL:=http://www.lantiq.com/
  VARIANT:=$(1)
  DEPENDS:=@TARGET_lantiq_$(2) +kmod-ltq-adsl-$(1)-mei
  FILES:=$(PKG_BUILD_DIR)/src/drv_dsl_cpe_api.ko
  AUTOLOAD:=$(call AutoLoad,51,drv_dsl_cpe_api)
endef

KernelPackage/ltq-adsl-danube=$(call KernelPackage/ltq-adsl-template,danube,xway)
KernelPackage/ltq-adsl-ar9=$(call KernelPackage/ltq-adsl-template,ar9,xway)
KernelPackage/ltq-adsl-ase=$(call KernelPackage/ltq-adsl-template,ase,ase)

define KernelPackage/ltq-dsl/config
	source "$(SOURCE)/Config.in"
endef

IFX_DSL_MAX_DEVICE=1
IFX_DSL_LINES_PER_DEVICE=1
IFX_DSL_CHANNELS_PER_LINE=1

CONFIGURE_ARGS += --enable-kernel-include="$(LINUX_DIR)/include" \
	--with-max-device="$(IFX_DSL_MAX_DEVICE)" \
	--with-lines-per-device="$(IFX_DSL_LINES_PER_DEVICE)" \
	--with-channels-per-line="$(IFX_DSL_CHANNELS_PER_LINE)" \
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

CONFIG_TAG_danube:=DANUBE
CONFIG_TAG_ase:=AMAZON_SE
CONFIG_TAG_ar9:=AR9
CONFIGURE_ARGS += --enable-add-drv-cflags="-DMODULE -DCONFIG_$(CONFIG_TAG_$(BUILD_VARIANT))"

CONFIGURE_ARGS += --enable-danube

ifeq ($(CONFIG_LANTIQ_DSL_DEBUG),y)
CONFIGURE_ARGS += \
	--enable-debug=yes \
	--enable-debug-prints=yes
EXTRA_CFLAGS += -DDEBUG
endif

EXTRA_CFLAGS = -fno-pic -mno-abicalls -mlong-calls -G 0

define Build/Configure
	(cd $(PKG_BUILD_DIR); aclocal && autoconf && automake)
	$(call Build/Configure/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/adsl
	$(CP) $(PKG_BUILD_DIR)/src/include/drv_dsl_cpe_*.h $(1)/usr/include/adsl/
endef

$(eval $(call KernelPackage,ltq-adsl-danube))
$(eval $(call KernelPackage,ltq-adsl-ase))
$(eval $(call KernelPackage,ltq-adsl-ar9))
