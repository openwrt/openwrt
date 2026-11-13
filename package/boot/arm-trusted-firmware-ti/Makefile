#
# Copyright (c) 2026 Aerlync Labs Inc.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2.14.0
PKG_RELEASE:=1

PKG_HASH:=b2a3bc360307c929714ffd8e7f1441c4888cd5d80531276e809c2de54db5dc16

PKG_MAINTAINER:=Sayooj K Karun <sayooj@aerlync.com>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk

define Trusted-Firmware-A/Default
  NAME:=TI K3 SoCs
  BUILD_TARGET:=ti_k3
endef

define Trusted-Firmware-A/am62x
  BUILD_SUBTARGET:=am625
  PLAT:=k3
endef

TFA_TARGETS:= \
	am62x

TFA_MAKE_FLAGS += \
	TARGET_BOARD=lite \
	SPD=opteed

define Package/trusted-firmware-a/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/$(PLAT)/lite/release/bl31.bin $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-bl31.bin
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
