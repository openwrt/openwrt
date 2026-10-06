#
# Copyright (C) 2026 Martin Schiller <ms@dev.tdt.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2.14.0
PKG_RELEASE:=1

PKG_HASH:=b2a3bc360307c929714ffd8e7f1441c4888cd5d80531276e809c2de54db5dc16

PKG_MAINTAINER:=Martin Schiller <ms@dev.tdt.de>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk

define Trusted-Firmware-A/Default
  BUILD_TARGET:=imx
  BUILD_SUBTARGET:=cortexa53
endef

define Trusted-Firmware-A/imx8mp
  NAME:=NXP i.MX8MP
  PLAT:=imx8mp
endef

TFA_TARGETS:= \
	imx8mp

TFA_MAKE_FLAGS += \
	IMX_BOOT_UART_BASE="auto"

define Package/trusted-firmware-a/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/$(PLAT)/release/bl31.bin $(STAGING_DIR_IMAGE)/bl31_$(BUILD_VARIANT).bin
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
