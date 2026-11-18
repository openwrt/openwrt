#
# Copyright (C) 2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=intel-microcode
PKG_VERSION:=20260925
PKG_RELEASE:=1

PKG_SOURCE:=microcode-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/intel/Intel-Linux-Processor-Microcode-Data-Files/archive/refs/tags/
PKG_HASH:=0ec19023dad1e6dd0c5909174b8dff2555b91cf01671e36271a1ef36655c0cfc
PKG_BUILD_DIR:=$(BUILD_DIR)/Intel-Linux-Processor-Microcode-Data-Files-microcode-$(PKG_VERSION)
PKG_CPE_ID:=cpe:/a:intel:microcode

PKG_BUILD_DEPENDS:=iucode-tool/host

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/intel-microcode
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://github.com/intel/Intel-Linux-Processor-Microcode-Data-Files
  DEPENDS:=@TARGET_x86
  TITLE:=Intel x86 CPU microcode
endef

define Build/Compile
	rm -rf $(PKG_BUILD_DIR)/intel-ucode-ipkg
	mkdir -p $(PKG_BUILD_DIR)/intel-ucode-ipkg
	$(STAGING_DIR)/../host/bin/iucode_tool -q \
		--write-firmware=$(PKG_BUILD_DIR)/intel-ucode-ipkg $(PKG_BUILD_DIR)/intel-ucode{,-with-caveats}/
endef

define Package/intel-microcode/install
	$(INSTALL_DIR) $(1)/lib/firmware/intel-ucode
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/intel-ucode-ipkg/* $(1)/lib/firmware/intel-ucode
endef

$(eval $(call BuildPackage,intel-microcode))
