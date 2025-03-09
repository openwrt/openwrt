#
# Copyright (C) 2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=intel-microcode
PKG_VERSION:=20250512
PKG_RELEASE:=1

PKG_SOURCE:=intel-microcode_3.$(PKG_VERSION).1.tar.xz
PKG_SOURCE_URL:=@DEBIAN/pool/non-free-firmware/i/intel-microcode/
PKG_HASH:=5773cf59867d90f4f5479bae973ac85f1cce2f7ae407645ec29c4ec1ba60f8e2
PKG_BUILD_DIR:=$(BUILD_DIR)/intel-microcode-3.$(PKG_VERSION).1
PKG_CPE_ID:=cpe:/a:intel:microcode

PKG_BUILD_DEPENDS:=iucode-tool/host

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/intel-microcode
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=@TARGET_x86
  TITLE:=Intel x86 CPU microcode
endef

define Build/Compile
	IUCODE_TOOL=$(STAGING_DIR)/../host/bin/iucode_tool $(MAKE) all -C $(PKG_BUILD_DIR)
	$(STAGING_DIR)/../host/bin/iucode_tool -q \
		--write-earlyfw=$(PKG_BUILD_DIR)/intel-ucode.img $(PKG_BUILD_DIR)/intel-ucode{,-with-caveats}/
endef

define Package/intel-microcode/install
	$(INSTALL_DIR) $(1)/boot
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/intel-ucode.img $(1)/boot/intel-ucode.img
endef

$(eval $(call BuildPackage,intel-microcode))
