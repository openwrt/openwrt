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

CPUID_64BIT:= \
	0x000006f2 \
	0x000006f6 \
	0x000006f7 \
	0x000006fa \
	0x000006fb \
	0x000006fd \
	0x00000f34 \
	0x00000f41 \
	0x00000f43 \
	0x00000f44 \
	0x00000f47 \
	0x00000f48 \
	0x00000f49 \
	0x00000f4a \
	0x00000f62 \
	0x00000f64 \
	0x00000f65 \
	0x00000f68 \
	0x00010661 \
	0x00010676 \
	0x00010677 \
	0x0001067a \
	0x000106a4 \
	0x000106a5 \
	0x000106c2 \
	0x000106ca \
	0x000106d1 \
	0x000106e5 \
	0x00020652 \
	0x00020655 \
	0x000206a7 \
	0x000206c2 \
	0x000206d6 \
	0x000206d7 \
	0x000206e6 \
	0x000206f2 \
	0x00030678 \
	0x00030679 \
	0x000306a9 \
	0x000306c3 \
	0x000306d4 \
	0x000306e4 \
	0x000306e6 \
	0x000306e7 \
	0x000306f2 \
	0x000306f4 \
	0x00040651 \
	0x00040661 \
	0x00040671 \
	0x000406c3 \
	0x000406c4 \
	0x000406d8 \
	0x000406e3 \
	0x000406f1 \
	0x00050653 \
	0x00050654 \
	0x00050655 \
	0x00050656 \
	0x00050657 \
	0x0005065b \
	0x00050662 \
	0x00050663 \
	0x00050664 \
	0x00050665 \
	0x000506c2 \
	0x000506c9 \
	0x000506ca \
	0x000506e3 \
	0x000506f1 \
	0x00060663 \
	0x000606a5 \
	0x000606a6 \
	0x000606c1 \
	0x000706a1 \
	0x000706a8 \
	0x000706e5 \
	0x000806a1 \
	0x000806c1 \
	0x000806c2 \
	0x000806d1 \
	0x000806e9 \
	0x000806ea \
	0x000806eb \
	0x000806ec \
	0x000806f4 \
	0x000806f5 \
	0x000806f6 \
	0x000806f7 \
	0x000806f8 \
	0x00090661 \
	0x00090672 \
	0x00090675 \
	0x000906a3 \
	0x000906a4 \
	0x000906c0 \
	0x000906e9 \
	0x000906ea \
	0x000906eb \
	0x000906ec \
	0x000906ed \
	0x000a0652 \
	0x000a0653 \
	0x000a0655 \
	0x000a0660 \
	0x000a0661 \
	0x000a0671 \
	0x000a06a4 \
	0x000a06f3 \
	0x000b0671 \
	0x000b0674 \
	0x000b06a2 \
	0x000b06a3 \
	0x000b06a8 \
	0x000b06e0 \
	0x000b06f2 \
	0x000b06f5 \
	0x000b06f6 \
	0x000b06f7 \
	0x000c06f1 \
	0x000c06f2

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
