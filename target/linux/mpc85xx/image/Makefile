#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DEVICE_VARS += TPLINK_HWID TPLINK_HWREV TPLINK_FLASHLAYOUT TPLINK_HEADER_VERSION

define Build/copy-file
	cat "$(1)" > "$@"
endef

define Device/Default
  PROFILES := Default
  DEVICE_DTS := $(lastword $(subst _, ,$(1)))
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL_ENTRY := 0x00000000
  KERNEL_LOADADDR := 0x00000000
  KERNEL := kernel-bin
  SUPPORTED_DEVICES := $(subst _,$(comma),$(1))
endef

ifeq ($(SUBTARGET),generic)

define Device/tplink_tl-wdr4900-v1
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := TL-WDR4900
  DEVICE_VARIANT := v1
  TPLINK_HEADER_VERSION := 1
  TPLINK_HWID := 0x49000001
  TPLINK_HWREV := 1
  TPLINK_FLASHLAYOUT := 16Mppc
  KERNEL_SIZE := 2684k
  KERNEL_NAME := simpleImage.tl-wdr4900-v1
  KERNEL_INITRAMFS :=
  KERNEL := kernel-bin | uImage none
  KERNEL_ENTRY := 0x1000000
  KERNEL_LOADADDR := 0x1000000
  SUPPORTED_DEVICES += tl-wdr4900-v1
  ARTIFACTS := fdt.bin
  ARTIFACT/fdt.bin := append-dtb
  IMAGES := fdt.bin factory.bin sysupgrade.bin
  IMAGE/sysupgrade.bin := tplink-v1-image sysupgrade | append-metadata
  IMAGE/factory.bin := tplink-v1-image factory
endef
TARGET_DEVICES += tplink_tl-wdr4900-v1

define Device/sophos_red-15w-rev1
  DEVICE_VENDOR := Sophos
  DEVICE_MODEL := RED 15w
  DEVICE_VARIANT := Rev.1
  # Original firmware uses a dedicated DTB-partition.
  # The bootloader however supports FIT-images.
  KERNEL = kernel-bin | gzip | fit gzip $(KDIR)/image-$$(DEVICE_DTS).dtb
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += sophos_red-15w-rev1

endif

ifeq ($(SUBTARGET),p1020)

define Device/aerohive_hiveap-330
  DEVICE_VENDOR := Aerohive
  DEVICE_MODEL := HiveAP-330
  DEVICE_PACKAGES := kmod-tpm-i2c-atmel
  BLOCKSIZE := 128k
  KERNEL := kernel-bin | gzip | uImage gzip
  KERNEL_SIZE := 8m
  KERNEL_INITRAMFS := copy-file $(KDIR)/vmlinux-initramfs | uImage none
  IMAGES := fdt.bin sysupgrade.bin
  IMAGE/fdt.bin := append-dtb
  IMAGE/sysupgrade.bin := append-dtb | pad-to 256k | check-size 256k | \
	append-uImage-fakehdr ramdisk | pad-to 256k | check-size 512k | \
	append-rootfs | pad-rootfs $$(BLOCKSIZE) | pad-to 41216k | check-size 41216k | \
	append-kernel | append-metadata
endef
TARGET_DEVICES += aerohive_hiveap-330

define Device/enterasys_ws-ap3710i
  DEVICE_VENDOR := Enterasys
  DEVICE_MODEL := WS-AP3710i
  BLOCKSIZE := 128k
  KERNEL = kernel-bin | lzma | fit lzma $(KDIR)/image-$$(DEVICE_DTS).dtb
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata
endef
TARGET_DEVICES += enterasys_ws-ap3710i

define Device/ocedo_panda
  DEVICE_VENDOR := OCEDO
  DEVICE_MODEL := Panda
  DEVICE_PACKAGES := kmod-rtc-ds1307 uboot-envtools
  KERNEL = kernel-bin | gzip | fit gzip $(KDIR)/image-$$(DEVICE_DTS).dtb
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  BLOCKSIZE := 128k
  IMAGES := fdt.bin sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/fdt.bin := append-dtb
endef
TARGET_DEVICES += ocedo_panda

endif

ifeq ($(SUBTARGET),p2020)

define Device/freescale_p2020rdb
  DEVICE_VENDOR := Freescale
  DEVICE_MODEL := P2020RDB
  DEVICE_DTS_DIR := $(DTS_DIR)/fsl
  DEVICE_PACKAGES := kmod-hwmon-lm90 kmod-rtc-ds1307 \
	kmod-gpio-pca953x kmod-eeprom-at24
  BLOCKSIZE := 128k
  KERNEL := kernel-bin | gzip | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  SUPPORTED_DEVICES := fsl,P2020RDB
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | \
	pad-rootfs $$(BLOCKSIZE) | append-metadata
endef
TARGET_DEVICES += freescale_p2020rdb

endif

$(eval $(call BuildImage))
