#
# Copyright (C) 2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=intel-microcode
PKG_VERSION:=20251111
PKG_RELEASE:=1

PKG_SOURCE:=intel-microcode_3.$(PKG_VERSION).1.tar.xz
PKG_SOURCE_URL:=@DEBIAN/pool/non-free-firmware/i/intel-microcode/
PKG_HASH:=9b6d11cf851dbe483d3afe73e93049263f06b2f93c78a5f7b0068fb2f7ea3411
PKG_BUILD_DIR:=$(BUILD_DIR)/intel-microcode-3.$(PKG_VERSION).1
PKG_CPE_ID:=cpe:/a:intel:microcode

PKG_BUILD_DEPENDS:=iucode-tool/host

PKG_FLAGS:=nonshared
include $(INCLUDE_DIR)/package.mk

define Package/intel-microcode-32bit
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=@TARGET_x86_generic||TARGET_x86_legacy
  TITLE:=Intel x86 CPU microcode (32-bit only)
endef

define Package/intel-microcode-64bit
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=@TARGET_x86_64||TARGET_x86_generic
  TITLE:=Intel x86 CPU microcode (64-bit)
endef

CPUID_32BIT_ONLY:= \
	0x00000611 \
	0x00000612 \
	0x00000616 \
	0x00000617 \
	0x00000619 \
	0x00000633 \
	0x00000634 \
	0x00000650 \
	0x00000651 \
	0x00000652 \
	0x00000653 \
	0x00000660 \
	0x00000665 \
	0x0000066a \
	0x0000066d \
	0x00000671 \
	0x00000672 \
	0x00000673 \
	0x00000680 \
	0x00000681 \
	0x00000683 \
	0x00000686 \
	0x0000068a \
	0x00000695 \
	0x00000696 \
	0x000006a0 \
	0x000006a1 \
	0x000006a4 \
	0x000006b1 \
	0x000006b4 \
	0x000006d6 \
	0x000006e8 \
	0x000006ec \
	0x00000f07 \
	0x00000f0a \
	0x00000f11 \
	0x00000f12 \
	0x00000f13 \
	0x00000f22 \
	0x00000f24 \
	0x00000f25 \
	0x00000f26 \
	0x00000f27 \
	0x00000f29 \
	0x00000f32 \
	0x00000f33 \
	0x00001632 \
	0x000106c1 \
	0x00020661

CPUID_MIXED:= \
	0x00000f34 \
	0x000106c2

define Build/Compile
	IUCODE_TOOL=$(STAGING_DIR)/../host/bin/iucode_tool $(MAKE) all -C $(PKG_BUILD_DIR)
	$(STAGING_DIR)/../host/bin/iucode_tool -q \
		$(foreach sig,$(CPUID_32BIT_ONLY),-s $(sig)) \
		--write-earlyfw=$(PKG_BUILD_DIR)/intel-ucode-32bit.img $(PKG_BUILD_DIR)/intel-ucode/
	$(STAGING_DIR)/../host/bin/iucode_tool -q \
		$(foreach sig,$(filter-out $(CPUID_MIXED), $(CPUID_32BIT_ONLY)),-s !$(sig)) \
		--write-earlyfw=$(PKG_BUILD_DIR)/intel-ucode-64bit.img $(PKG_BUILD_DIR)/intel-ucode{,-with-caveats}/
endef

define Package/intel-microcode-32bit/install
	$(INSTALL_DIR) $(1)/boot
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/intel-ucode-32bit.img $(1)/boot/intel-ucode.img
endef

define Package/intel-microcode-64bit/install
	$(INSTALL_DIR) $(1)/boot
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/intel-ucode-64bit.img $(1)/boot/intel-ucode.img
endef

$(eval $(call BuildPackage,intel-microcode-32bit))
$(eval $(call BuildPackage,intel-microcode-64bit))
