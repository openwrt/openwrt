# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Device/Default
	DEVICE_DTS_DIR := ../dts
	KERNEL_NAME := zImage
	KERNEL := kernel-bin
	KERNEL_INITRAMFS := kernel-bin
	KERNEL_INITRAMFS_SUFFIX := -zImage
	IMAGES :=
endef

define Device/zte_mf283v
	DEVICE_VENDOR := ZTE
	DEVICE_MODEL := MF283V
	DEVICE_DTS := qcom-mdm9607-zte-mf283v
	DEVICE_DTS_CONFIG := config@1
	DEVICE_PACKAGES := mtd ubi-utils mf283v-reboot mf283v-wlan-data \
		kmod-rtl8192es \
		kmod-dsa-qca8k kmod-phy-qca83xx wpad-basic-mbedtls ethtool
endef
TARGET_DEVICES += zte_mf283v

$(eval $(call BuildImage))
