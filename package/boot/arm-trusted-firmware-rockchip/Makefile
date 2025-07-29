# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2021-2023 ImmortalWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=rkbin
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/rockchip-linux/rkbin.git
PKG_SOURCE_DATE:=2025-01-24
PKG_SOURCE_VERSION:=f43a462e7a1429a9d407ae52b4745033034a6cf9
PKG_MIRROR_HASH:=97f899f37f7996e62e8f9dd72e732ea74dd1d553796a863b1533a0d3d6f0c62b

PKG_MAINTAINER:=Tianling Shen <cnsztl@immortalwrt.org>

include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk
include ./atf-version.mk

define Trusted-Firmware-A/Default
  NAME:=Rockchip $(1) SoCs
  BUILD_TARGET:=rockchip
endef

define Trusted-Firmware-A/rk3308
  BUILD_SUBTARGET:=armv8
  ATF:=rk33/$(RK3308_ATF)
  TPL:=rk33/$(RK3308_TPL)
endef

define Trusted-Firmware-A/rk3308-rock-pi-s
  NAME:=Radxa ROCK Pi S board
  BUILD_SUBTARGET:=armv8
  ATF:=rk33/$(RK3308_ATF)
  TPL:=rk33/$(RK3308_TPL_ROCK_PI_S)
endef

define Trusted-Firmware-A/rk3328
  BUILD_SUBTARGET:=armv8
  ATF:=rk33/$(RK3328_ATF)
  TPL:=rk33/$(RK3328_TPL)
  LOADER:=rk33/$(RK3328_LOADER)
endef

define Trusted-Firmware-A/rk3399
  BUILD_SUBTARGET:=armv8
  ATF:=rk33/$(RK3399_ATF)
  TPL:=rk33/$(RK3399_TPL)
  LOADER:=rk33/$(RK3399_LOADER)
endef

define Trusted-Firmware-A/rk3566
  BUILD_SUBTARGET:=armv8
  ATF:=rk35/$(RK3566_ATF)
  TPL:=rk35/$(RK3566_TPL)
endef

define Trusted-Firmware-A/rk3568
  BUILD_SUBTARGET:=armv8
  ATF:=rk35/$(RK3568_ATF)
  TPL:=rk35/$(RK3568_TPL)
endef

define Trusted-Firmware-A/rk3568-e25
  NAME:=Radxa E25 board
  BUILD_SUBTARGET:=armv8
  ATF:=rk35/$(RK3568_ATF)
  TPL:=rk35/$(RK3568_TPL_E25)
endef

define Trusted-Firmware-A/rk3588
  BUILD_SUBTARGET:=armv8
  ATF:=rk35/$(RK3588_ATF)
  TPL:=rk35/$(RK3588_TPL)
endef

TFA_TARGETS:= \
	rk3308 \
	rk3308-rock-pi-s \
	rk3328 \
	rk3399 \
	rk3566 \
	rk3568 \
	rk3568-e25 \
	rk3588

ifeq ($(BUILD_VARIANT),rk3308-rock-pi-s)
  define Download/rk3308-tpl-rock-pi-s
    FILE:=$(RK3308_TPL_ROCK_PI_S)
    URL:=https://github.com/radxa/rkbin/raw/5696fab20dcac57c1458f72dc7604ba60e553adf/bin/rk33/
    HASH:=8a1a42df23cccb86a2dabc14a5c0e9227d64a51b9b83e9968ef5af3b30787f7d
  endef

  define Build/Prepare
	$(eval $(call Download,rk3308-tpl-rock-pi-s))
	$(call Build/Prepare/Default)

	$(CP) $(DL_DIR)/$(RK3308_TPL_ROCK_PI_S) $(PKG_BUILD_DIR)/bin/rk33/
  endef
endif

ifeq ($(BUILD_VARIANT),rk3568-e25)
  define Download/rk3568-tpl-e25
    FILE:=$(RK3568_TPL_E25)
    URL:=https://github.com/radxa/rkbin/raw/5696fab20dcac57c1458f72dc7604ba60e553adf/bin/rk35/
    HASH:=1815f9649dc5661a3ef184b052da39286e51453a66f6ff53cc3e345d65dfabd4
  endef

  define Build/Prepare
	$(eval $(call Download,rk3568-tpl-e25))
	$(call Build/Prepare/Default)

	$(CP) $(DL_DIR)/$(RK3568_TPL_E25) $(PKG_BUILD_DIR)/bin/rk35/
  endef
endif

define Build/Compile
	# This comment is the workaround for "extraneous 'endif'" error
ifneq ($(LOADER),)
	( \
		pushd $(PKG_BUILD_DIR) ; \
		$(SED) 's,$$$$(PKG_BUILD_DIR),$(PKG_BUILD_DIR),g' trust.ini ; \
		$(SED) 's,$$$$(VARIANT),$(BUILD_VARIANT),g' trust.ini ; \
		./tools/mkimage -n $(BUILD_VARIANT) -T rksd -d bin/$(TPL) \
			$(BUILD_VARIANT)-idbloader.bin ; \
		cat bin/$(LOADER) >> $(BUILD_VARIANT)-idbloader.bin ; \
		./tools/trust_merger --replace bl31.elf bin/$(ATF) trust.ini ; \
		popd ; \
	)
endif
endef

define Package/trusted-firmware-a/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)

	$(CP) $(PKG_BUILD_DIR)/bin/$(ATF) $(STAGING_DIR_IMAGE)/
ifneq ($(LOADER),)
	$(CP) $(PKG_BUILD_DIR)/tools/loaderimage $(STAGING_DIR_IMAGE)/
	$(CP) $(PKG_BUILD_DIR)/$(BUILD_VARIANT)-idbloader.bin $(STAGING_DIR_IMAGE)/
	$(CP) $(PKG_BUILD_DIR)/$(BUILD_VARIANT)-trust.bin $(STAGING_DIR_IMAGE)/
else
	$(CP) $(PKG_BUILD_DIR)/bin/$(TPL) $(STAGING_DIR_IMAGE)/
endif
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
