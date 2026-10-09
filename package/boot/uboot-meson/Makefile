# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2026.07
PKG_RELEASE:=1

PKG_HASH:=78e8bfc382fe388f9b55aa1daf8c563522a037779b5d4c349d1415e381f1243e

PKG_MAINTAINER:=OpenWrt Developers <openwrt-devel@lists.openwrt.org>
PKG_BUILD_DEPENDS:=amlogic-boot-fip/host

UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=meson
  HIDDEN:=1
  UBOOT_IMAGE:=u-boot.bin
endef

define U-Boot/odroid-c2
  BUILD_SUBTARGET:=gxbb
  NAME:=ODROID-C2
  BUILD_DEVICES:=hardkernel_odroid-c2
  UBOOT_CONFIG:=odroid-c2
endef

UBOOT_TARGETS := \
	odroid-c2

UBOOT_CUSTOMIZE_CONFIG := \
	--disable TOOLS_MKEFICAPSULE \
	--disable TOOLS_LIBCRYPTO \
	--disable EFI_LOADER \
	--disable EFI_VARIABLES \
	--disable EFI_BOOT_MANAGER \
	--disable EFI_CAPSULE \
	--disable EFI_DEVICE_PATH_TO_TEXT \
	--disable EFI_DEVICE_PATH_UTILITIES \
	--disable EFI_UNICODE_COLLATION \
	--disable VIDEO \
	--disable SPLASH_SCREEN \
	--disable BMP_16BPP \
	--disable BMP_24BPP \
	--disable BMP_32BPP \
	--disable VIDEO_BMP_RLE8 \
	--disable VIDEO_MESON \
	--disable VIDEO_DT_SIMPLEFB \
	--disable USB_KEYBOARD \
	--disable CMD_USB \
	--disable USB \
	--disable USB_DWC2 \
	--disable USB_HOST \
	--disable USB_STORAGE \
	--disable USB_COMMON \
	--disable NET \
	--disable CMD_NET \
	--disable CMD_TFTPBOOT \
	--disable CMD_PING \
	--disable CMD_NFS \
	--disable SYSINFO_SMBIOS \
	--disable CMD_ADC \
	--disable CMD_REGULATOR \
	--disable CMD_GPIO \
	--disable CMD_I2C \
	--disable I2C \
	--disable DM_I2C \
	--disable SYS_I2C_MESON \
	--disable SARADC_MESON \
	--disable PHY \
	--disable PHY_REALTEK \
	--disable ETH_DESIGNWARE_MESON8B \
	--disable MESON_GXBB_USB_PHY \
	--disable USB_PHY \
	--enable CMD_FAT \
	--enable FS_FAT \
	--disable FAT_WRITE \
	--disable CMD_GPT \
	--enable CMD_PART \
	--disable OF_BOARD_SETUP \
	--disable OF_LIBFDT_OVERLAY \
	--disable DISTRO_DEFAULTS \
	--disable BOOTSTD \
	--disable BOOTMETH_EXTLINUX \
	--disable BOOTMETH_PXE \
	--disable BOOTMETH_VBE \
	--disable VBE \
	--disable VBE_SIMPLE \
	--disable VBE_REQUEST \
	--disable CMD_BOOTFLOW \
	--disable CMD_PXE \
	--disable PXE_UTILS \
	--disable BOOT_DEFAULTS_CMDS \
	--enable USE_BOOTCOMMAND \
	--enable CMD_SOURCE \
	--set-str BOOTCOMMAND 'mmc dev 0; fatload mmc 0:1 $${scriptaddr} boot.scr; source $${scriptaddr}' \
	--disable FIT \
	--disable HASH \
	--disable SHA1 \
	--disable SHA256 \
	--disable MD5 \
	--disable LIB_RSA \
	--disable ZLIB \
	--disable GZIP \
	--disable LZMA \
	--disable LZ4 \
	--enable CC_OPTIMIZE_FOR_SIZE \
	--enable LTO

define Build/Compile
	$(call Build/Compile/U-Boot)
	mkdir -p $(PKG_BUILD_DIR)/fip-out
	cd $(STAGING_DIR_HOST)/share/amlogic-boot-fip && \
		./build-fip.sh \
			odroid-c2 \
			$(PKG_BUILD_DIR)/u-boot.bin \
			$(PKG_BUILD_DIR)/fip-out
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/fip-out/u-boot.bin.sd.bin \
		$(STAGING_DIR_IMAGE)/$(BUILD_DEVICES)-u-boot-meson.bin
	mkimage -A arm64 -O linux -T script -C none -a 0 -e 0 \
		-d ./boot.scr.txt \
		$(STAGING_DIR_IMAGE)/$(BUILD_DEVICES)-boot.scr
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
