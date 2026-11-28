#
# Copyright (C) 2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=intel-microcode
PKG_VERSION:=20260925
PKG_RELEASE:=2

PKG_SOURCE:=microcode-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/intel/Intel-Linux-Processor-Microcode-Data-Files/archive/refs/tags/
PKG_HASH:=0ec19023dad1e6dd0c5909174b8dff2555b91cf01671e36271a1ef36655c0cfc
PKG_BUILD_DIR:=$(BUILD_DIR)/Intel-Linux-Processor-Microcode-Data-Files-microcode-$(PKG_VERSION)
PKG_CPE_ID:=cpe:/a:intel:microcode
PKG_LICENSE_FILES:=license

PKG_BUILD_DEPENDS:=iucode-tool/host

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/intel-microcode
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://github.com/intel/Intel-Linux-Processor-Microcode-Data-Files
  DEPENDS:=@TARGET_x86_64||TARGET_x86_generic||TARGET_x86_legacy
  TITLE:=Intel x86 CPU microcode
endef

define Package/intel-microcode/description
  Early load microcode image for Intel CPUs. On the legacy subtarget it
  only carries CPUs without long mode plus signatures shared with 64-bit
  parts, to fit the 16 MiB boot partition.
endef

# CPUs without long mode
CPUID_32BIT_ONLY:= \
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
	0x00000681 \
	0x00000683 \
	0x00000686 \
	0x0000068a \
	0x00000695 \
	0x000006a0 \
	0x000006a1 \
	0x000006b1 \
	0x000006b4 \
	0x000006d6 \
	0x000006e8 \
	0x000006ec \
	0x00000f07 \
	0x00000f0a \
	0x00000f12 \
	0x00000f24 \
	0x00000f25 \
	0x00000f26 \
	0x00000f27 \
	0x00000f29 \
	0x00000f32 \
	0x00000f33 \
	0x00001632 \
	0x00020661

# shared by CPUs with and without long mode
CPUID_MIXED:= \
	0x00000f34 \
	0x00000f41 \
	0x00000f49 \
	0x000106c2

ifdef CONFIG_TARGET_x86_legacy
  IUCODE_SELECT:=$(foreach sig,$(CPUID_32BIT_ONLY) $(CPUID_MIXED),-s $(sig))
else ifdef CONFIG_TARGET_x86_64
  IUCODE_SELECT:=$(foreach sig,$(CPUID_32BIT_ONLY),-s !$(sig))
endif

define Build/Compile
	$(STAGING_DIR)/../host/bin/iucode_tool -q --overwrite \
		$(IUCODE_SELECT) --mini-earlyfw \
		--write-earlyfw=$(PKG_BUILD_DIR)/intel-ucode.img \
		$(PKG_BUILD_DIR)/intel-ucode{,-with-caveats}/
endef

define Package/intel-microcode/install
	$(INSTALL_DIR) $(1)/boot
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/intel-ucode.img $(1)/boot/intel-ucode.img
endef

$(eval $(call BuildPackage,intel-microcode))
