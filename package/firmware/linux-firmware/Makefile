#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-firmware
PKG_VERSION:=20261124
PKG_RELEASE:=5

PKG_SOURCE_URL:=@KERNEL/linux/kernel/firmware
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_HASH:=2cabe10253005f55bf4bbc0ebb25c0964d66f7c4121ffa47853ff1e70ea1aa54

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

SCAN_DEPS = *.mk

PKG_CONFIG_DEPENDS:=CONFIG_PACKAGE_amd64-microcode

include $(INCLUDE_DIR)/package.mk

RSTRIP:=:
STRIP:=:

define Package/firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git
  TITLE:=$(1)
  DEPENDS:=$(2)
  LICENSE_FILES:=$(3)
  LICENSE:=$(4)
  $(if $(5),PKGFLAGS:=$(5))
endef

# amd64-microcode early load image. No directory entries, so the initramfs
# unpacker skips the file and it does not stay in RAM after boot. Keep the
# default 512-byte cpio block padding, so an Intel image loaded after it
# would keep its data 16-byte aligned.
define Build/Compile/amd64-microcode
	rm -rf $(PKG_BUILD_DIR)/amd-earlyfw
	mkdir -p $(PKG_BUILD_DIR)/amd-earlyfw/kernel/x86/microcode
	cat $(PKG_BUILD_DIR)/amd-ucode/microcode_amd*.bin \
		> $(PKG_BUILD_DIR)/amd-earlyfw/kernel/x86/microcode/AuthenticAMD.bin
	$(if $(SOURCE_DATE_EPOCH),touch -hcd "@$(SOURCE_DATE_EPOCH)" \
		$(PKG_BUILD_DIR)/amd-earlyfw/kernel/x86/microcode/AuthenticAMD.bin)
	( cd $(PKG_BUILD_DIR)/amd-earlyfw && \
		echo kernel/x86/microcode/AuthenticAMD.bin | \
		$(STAGING_DIR_HOST)/bin/cpio --reproducible -o -H newc -R 0:0 \
		> $(PKG_BUILD_DIR)/amd-ucode.img )
endef

define Build/Compile
	$(if $(CONFIG_PACKAGE_amd64-microcode)$(DEVELOPER),$(Build/Compile/amd64-microcode))
endef

include $(wildcard ./*.mk)
#$(eval $(call BuildPackage,linux-firmware))
