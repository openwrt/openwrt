# Copyright (C) 2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=dsl_vr11_firmware_xdsl
PKG_VERSION:=08.0C.01.07.01.07_08.0C.01.01.01.01
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.prpl.dev/intel/dsl-vr11-firmware-xdsl.git
PKG_SOURCE_DATE:=2019-10-27
PKG_SOURCE_VERSION:=4d8f55f4e13f8a795e68716153ef68d42487b06f
PKG_HASH:=b56669b6ce15dc538342ac2a620d16d8a89e5013ad6573cb4a06bf6b0c9b7730

include $(INCLUDE_DIR)/package.mk

ANNEX_A_VER:=8C1717_8C1111

define Package/dsl-vr11-firmware-xdsl-a
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=VRX518 / VR11 CPE xDSL Annex A firmware
  URL:=http://www.intel.com
  DEPENDS:=@TARGET_ipq40xx
endef

define Package/dsl-vr11-firmware-xdsl-a/description
	VRX518 / VR11 CPE VDSL and ADSL Annex A firmware
endef

define Build/Compile
endef

define Package/dsl-vr11-firmware-xdsl-a/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/LICENSE $(1)/lib/firmware/xcpe_$(ANNEX_A_VER).LICENSE
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/xcpe_$(ANNEX_A_VER).bin $(1)/lib/firmware/
	ln -s xcpe_$(ANNEX_A_VER).bin $(1)/lib/firmware/vdsl.bin

endef

$(eval $(call BuildPackage,dsl-vr11-firmware-xdsl-a))
