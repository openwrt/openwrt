#
# Copyright (c) 2026 Aerlync Labs Inc.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ti-k3-firmware
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/TexasInstruments/ti-linux-firmware.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=d084a9f75833dc9184d2682bdfd668bf6808d1a3
PKG_SOURCE_DATE:=2026-02-06
PKG_MIRROR_HASH:=ed64c3f27edabeec0b3268ad32e01c764ddc7b65c3802092173ec63710d3f706

PKG_MAINTAINER:=Sayooj K Karun <sayooj@aerlync.com>
PKG_LICENSE:=LicenseRef-TI-Software-License-Agreement
PKG_LICENSE_FILES:=LICENSE.ti

include $(INCLUDE_DIR)/package.mk

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)/ti-k3-firmware/ti-sysfw
	$(CP) $(PKG_BUILD_DIR)/ti-sysfw/ti-fs-firmware-am62x-gp.bin \
		$(STAGING_DIR_IMAGE)/ti-k3-firmware/ti-sysfw/ti-fs-firmware-am62x-gp.bin
	$(CP) $(PKG_BUILD_DIR)/ti-sysfw/ti-fs-stub-firmware-am62x-gp.bin \
		$(STAGING_DIR_IMAGE)/ti-k3-firmware/ti-sysfw/ti-fs-stub-firmware-am62x-gp.bin
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)/ti-k3-firmware/ti-dm/am62xx
	$(CP) $(PKG_BUILD_DIR)/ti-dm/am62xx/ipc_echo_testb_mcu1_0_release_strip.xer5f \
		$(STAGING_DIR_IMAGE)/ti-k3-firmware/ti-dm/am62xx/ipc_echo_testb_mcu1_0_release_strip.xer5f
endef

define Package/ti-k3-firmware
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=TI K3 SYSFW firmware
  DEPENDS:=@TARGET_ti_k3
endef

define Package/ti-k3-firmware/description
  Signed TI System Firmware (SYSFW/TIFS), a TIFS stub firmware, and
  Device Management (DM) firmware for AM62x, required by the R5 SPL
  stage of the K3 boot chain. Proprietary, binary-only.
endef

define Package/ti-k3-firmware/install
endef

$(eval $(call BuildPackage,ti-k3-firmware))
