#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-dsl
PKG_BASE_NAME:=drv_dsl_cpe_api_danube
PKG_VERSION:=3.24.4.4
PKG_RELEASE:=3
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/drv_dsl_cpe_api-$(PKG_VERSION)
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources/
PKG_MD5SUM:=c45bc531c1ed2ac80f68fb986b63bb87

FW_NAME:=ltq-dsl-fw-0.1
FW_MD5:=4700a36b66b955b4c5544227267356f4

include $(INCLUDE_DIR)/package.mk

define Download/ltq-dsl-fw
	FILE:=$(FW_NAME).tar.bz2
	MD5SUM:=$(FW_MD5)
	URL:=http://mirror2.openwrt.org/sources/
endef
$(eval $(call Download,ltq-dsl-fw))

define KernelPackage/ltq-dsl-template
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Lantiq dsl driver for $(1)
  URL:=http://www.lantiq.com/
  VARIANT:=$(1)
  DEPENDS:=@TARGET_lantiq_$(1) +kmod-atm
  FILES:=$(PKG_BUILD_DIR)/src/mei/lantiq_mei.ko \
	$(PKG_BUILD_DIR)/src/drv_dsl_cpe_api.ko \
	$(PKG_BUILD_DIR)/src/mei/lantiq_atm.ko
  AUTOLOAD:=$(call AutoLoad,50,lantiq_mei drv_dsl_cpe_api lantiq_atm)
  MAINTAINER:=John Crispin <blogic@openwrt.org>
endef

KernelPackage/ltq-dsl-danube=$(call KernelPackage/ltq-dsl-template,danube)
KernelPackage/ltq-dsl-ase=$(call KernelPackage/ltq-dsl-template,ase)
KernelPackage/ltq-dsl-ar9=$(call KernelPackage/ltq-dsl-template,ar9)
KernelPackage/ltq-dsl-vr9=$(call KernelPackage/ltq-dsl-template,vr9)

define Package/kmod-ltq-dsl-firmware-template
  TITLE+=Firmware Annex-$(1) $(2)
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  VARIANT:= $(1)-$(2)
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_lantiq_$(2) kmod-ltq-dsl-$(2)
  MAINTAINER:=John Crispin <blogic@openwrt.org>
endef

Package/kmod-ltq-dsl-firmware-a-danube=$(call Package/kmod-ltq-dsl-firmware-template,a,danube)
Package/kmod-ltq-dsl-firmware-b-danube=$(call Package/kmod-ltq-dsl-firmware-template,b,danube)
Package/kmod-ltq-dsl-firmware-a-ase=$(call Package/kmod-ltq-dsl-firmware-template,a,ase)
Package/kmod-ltq-dsl-firmware-b-ase=$(call Package/kmod-ltq-dsl-firmware-template,b,ase)
Package/kmod-ltq-dsl-firmware-a-ar9=$(call Package/kmod-ltq-dsl-firmware-template,a,ar9)
Package/kmod-ltq-dsl-firmware-b-ar9=$(call Package/kmod-ltq-dsl-firmware-template,b,ar9)
Package/kmod-ltq-dsl-firmware-a-vr9=$(call Package/kmod-ltq-dsl-firmware-template,a,vr9)
Package/kmod-ltq-dsl-firmware-b-vr9=$(call Package/kmod-ltq-dsl-firmware-template,b,vr9)

define KernelPackage/ltq-dsl/description
	Lantiq DSL driver for AR9, Amazon SE, Danube and VR9
endef

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
CONFIG_TAG_vr9:=VR9
CONFIGURE_ARGS += --enable-add-drv-cflags="-DMODULE -DCONFIG_$(CONFIG_TAG_$(BUILD_VARIANT))"

ifeq ($(BUILD_VARIANT),vr9)
CONFIGURE_ARGS += --enable-vinax
else
CONFIGURE_ARGS += --enable-danube
endif

ifeq ($(CONFIG_LANTIQ_DSL_DEBUG),y)
CONFIGURE_ARGS += \
	--enable-debug=yes \
	--enable-debug-prints=yes
EXTRA_CFLAGS += -DDEBUG
endif

EXTRA_CFLAGS = -fno-pic -mno-abicalls -mlong-calls -G 0

define Build/Prepare
	$(PKG_UNPACK)
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/src/mei/ 
	$(CP) ./src/* $(PKG_BUILD_DIR)/src/mei/
	$(Build/Patch)
	$(TAR) -C $(PKG_BUILD_DIR) -xjf $(DL_DIR)/$(FW_NAME).tar.bz2
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); aclocal && autoconf && automake)
	$(call Build/Configure/Default)
endef

define Build/Compile
	cd $(LINUX_DIR); \
		ARCH=mips CROSS_COMPILE="$(KERNEL_CROSS)" \
		BUILD_VARIANT=$(BUILD_VARIANT) $(MAKE) M=$(PKG_BUILD_DIR)/src/mei/ V=1 modules
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

define Package/kmod-ltq-dsl-firmware-$(BUILD_VARIANT)/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(CP) $(PKG_BUILD_DIR)/$(FW_NAME)/ltq-dsl-fw-$(BUILD_VARIANT).bin $(1)/lib/firmware/ModemHWE.bin
endef

$(eval $(call KernelPackage,ltq-dsl-danube))
$(eval $(call KernelPackage,ltq-dsl-ase))
$(eval $(call KernelPackage,ltq-dsl-ar9))
#$(eval $(call KernelPackage,ltq-dsl-vr9))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-a-danube))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-b-danube))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-a-ase))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-b-ase))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-a-ar9))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-b-ar9))
#$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-a-vr9))
#$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-b-vr9))
