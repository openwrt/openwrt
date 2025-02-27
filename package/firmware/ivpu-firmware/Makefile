#
# Copyright (C) 2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ivpu-firmware
PKG_VERSION:=1.13.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/intel/linux-npu-driver.git
PKG_SOURCE_VERSION:=v$(PKG_VERSION)
PKG_MIRROR_HASH:=45adbbb8a6977953af9abcd9a05c4719aa2ee0440659c9a66fbc0050dd4eb685

include $(INCLUDE_DIR)/package.mk

define Package/ivpu-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Intel VPU firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Build/Compile
	true
endef

define Package/ivpu-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/intel/vpu
	$(INSTALL_DATA) \
	        $(PKG_BUILD_DIR)/firmware/bin/*.bin \
	        $(1)/lib/firmware/intel/vpu
endef

$(eval $(call BuildPackage,ivpu-firmware))
