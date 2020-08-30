#
# Copyright (C) 2019 Sartura Ltd.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arm-trusted-firmware-mvebu
PKG_VERSION:=2.3
PKG_RELEASE:=1

PKG_SOURCE:=trusted-firmware-a-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/snapshot
PKG_HASH:=37f917922bcef181164908c470a2f941006791c0113d738c498d39d95d543b21
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/trusted-firmware-a-$(PKG_VERSION)

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=docs/license.rst

PKG_MAINTAINER:=Vladimir Vid <vladimir.vid@sartura.hr>

include $(INCLUDE_DIR)/package.mk

define Package/arm-trusted-firmware-mvebu
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=ARM Trusted Firmware for mvebu devices
endef

define Package/arm-trusted-firmware-mvebu/uDPU
$(call Package/arm-trusted-firmware-mvebu)
  BOARDNAME:=uDPU
  TITLE:=ARM Trusted Firmware for Methode uDPU
  DEPENDS:=+u-boot-uDPU
  UBOOT:=uDPU
  CLOCKSPRESET:=CPU_1000_DDR_800
  PLAT:=a3700
endef

A3700_UTILS_NAME:=a3700-utils
A3700_UTILS_RELEASE:=09679790
A3700_UTILS_SOURCE=$(A3700_UTILS_NAME)-$(A3700_UTILS_RELEASE).tar.bz2

define Download/a3700-utils
  FILE:=$(A3700_UTILS_SOURCE)
  PROTO:=git
  URL:=https://github.com/MarvellEmbeddedProcessors/A3700-utils-marvell.git
  VERSION:=096797908ddd69a679fd55595c41fc02809829a9
  MIRROR_HASH:=9276d63d41b2e221c2fd5cceee988ca26eff5005747787f084868b3940173b7a
  SUBDIR:=$(A3700_UTILS_NAME)
endef

MV_DDR_NAME:=mv-ddr-marvell
MV_DDR_RELEASE:=mainline
MV_DDR_SOURCE:=$(MV_DDR_NAME)-$(MV_DDR_RELEASE).tar.bz2

define Download/mv-ddr-marvell
  FILE:=$(MV_DDR_SOURCE)
  PROTO:=git
  URL:=https://github.com/MarvellEmbeddedProcessors/mv-ddr-marvell.git
  VERSION:=a881467ef0f0185e6570dd0483023fde93cbb5f5
  MIRROR_HASH:=19f34817616c583b2b5f2612705857cf2120f6c2132355217899b1403c3fd336
  SUBDIR:=$(MV_DDR_NAME)
endef

LINARO_NAME:=gcc-linaro
LINARO_RELEASE:=6
LINARO_VERSION:=5.0-2018.12-$(HOST_ARCH)_arm-linux-gnueabi
LINARO_SOURCE=$(LINARO_NAME)-$(LINARO_RELEASE).$(LINARO_VERSION).tar.xz

define Download/gcc-linaro
  FILE:=$(LINARO_SOURCE)
  URL:=https://releases.linaro.org/components/toolchain/binaries/latest-$(LINARO_RELEASE)/arm-linux-gnueabi/
  HASH:=2d4a92d6c8b384ae404b2e02c1c412e3ec18f9b714135acf046b2b1b510e9ace
endef

define Build/Prepare
	# Download sources
	$(eval $(call Download,a3700-utils))
	$(eval $(call Download,mv-ddr-marvell))
	$(eval $(call Download,gcc-linaro))

	$(call Build/Prepare/Default,)

	mkdir -p $(STAGING_DIR_IMAGE)
	$(TAR) -C $(STAGING_DIR_IMAGE) -xf $(DL_DIR)/$(A3700_UTILS_SOURCE)
	$(TAR) -C $(STAGING_DIR_IMAGE) -xf $(DL_DIR)/$(MV_DDR_SOURCE)
	$(TAR) -C $(STAGING_DIR_IMAGE) -xf $(DL_DIR)/$(LINARO_SOURCE)
endef

export GCC_HONOUR_COPTS=s

TARGET_CFLAGS = ""

MAKE_VARS= \
	CROSS_COMPILE="$(TARGET_CROSS)"

MAKE_FLAGS = \
	OPENSSL_DIR=$(STAGING_DIR_HOST) \
	CROSS_CM3=$(STAGING_DIR_IMAGE)/$(LINARO_NAME)-$(LINARO_RELEASE).$(LINARO_VERSION)/bin/arm-linux-gnueabi- \
	BL33=$(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot.bin \
	MV_DDR_PATH=$(STAGING_DIR_IMAGE)/$(MV_DDR_NAME) \
	WTP=$(STAGING_DIR_IMAGE)/$(A3700_UTILS_NAME) \
	CLOCKSPRESET=$(CLOCKSPRESET) \
	PLAT=$(PLAT) \
	all \
	fip

define Build/InstallDev
	$(INSTALL_DIR) $(BIN_DIR)/flash-image-$(BOARDNAME)
	$(CP) $(PKG_BUILD_DIR)/build/$(PLAT)/release/flash-image.bin $(BIN_DIR)/flash-image-$(BOARDNAME)/
	$(CP) $(PKG_BUILD_DIR)/build/$(PLAT)/release/uart-images.tgz $(BIN_DIR)/flash-image-$(BOARDNAME)/
endef

$(eval $(call BuildPackage,arm-trusted-firmware-mvebu/uDPU))
