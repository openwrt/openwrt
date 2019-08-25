#
# Copyright (C) 2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=intel-microcode
PKG_VERSION:=20190618
PKG_RELEASE:=1

PKG_SOURCE:=intel-microcode_3.$(PKG_VERSION).$(PKG_RELEASE).tar.xz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/non-free/i/intel-microcode/
PKG_HASH:=0026ade98745205841fbab5d298c5efe6c571438f55619aeb1f2686f95a0f4b8
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-3.$(PKG_VERSION).$(PKG_RELEASE)

PKG_BUILD_DEPENDS:=iucode-tool/host

ifdef CONFIG_TARGET_x86_64
	MICROCODE:="intel-microcode-64"
else
	MICROCODE:="intel-microcode"
endif

include $(INCLUDE_DIR)/package.mk

define Package/intel-microcode
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=@TARGET_x86
  TITLE:=Intel x86 CPU microcode
endef

define Build/Compile
	IUCODE_TOOL=$(STAGING_DIR)/../host/bin/iucode_tool $(MAKE) -C $(PKG_BUILD_DIR)
	mkdir $(PKG_BUILD_DIR)/intel-ucode-ipkg
	$(STAGING_DIR)/../host/bin/iucode_tool -q \
		--write-firmware=$(PKG_BUILD_DIR)/intel-ucode-ipkg $(PKG_BUILD_DIR)/$(MICROCODE).bin
endef

define Package/intel-microcode/install
	$(INSTALL_DIR) $(1)/lib/firmware/intel-ucode
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/intel-ucode-ipkg/* $(1)/lib/firmware/intel-ucode
endef

$(eval $(call BuildPackage,intel-microcode))
