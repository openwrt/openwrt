#
# Copyright 2018 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Device/Default
  PROFILES := Default
  IMAGES := firmware.bin
  FILESYSTEMS := ubifs
  KERNEL := kernel-bin | uImage none
  KERNEL_NAME := zImage
  KERNEL_LOADADDR := 0x80008000
  KERNEL_ENTRY_POINT := 0x80008000
endef

define Device/ls1012ardb
  DEVICE_TITLE := LS1012ARDB
  DEVICE_PACKAGES += \
    layerscape-rcw-ls1012ardb \
    layerscape-ppfe \
    layerscape-ppa-ls1012ardb \
    u-boot-ls1012ardb-image \
    kmod-ppfe
  DEVICE_DTS := ../../../arm64/boot/dts/freescale/fsl-ls1012a-rdb
  UBIFS_OPTS := -m 1 -e 262016 -c 128
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 256KiB
  PAGESIZE := 1
  IMAGE/firmware.bin := \
    ls-clean | \
    ls-append $(1)-rcw.bin | pad-to 1M | \
    ls-append $(1)-uboot.bin | pad-to 3M | \
    ls-append $(1)-uboot-env.bin | pad-to 4M | \
    ls-append $(1)-ppa.itb | pad-to 10M | \
    ls-append pfe.itb | pad-to 15M | \
    ls-append-dtb $$(DEVICE_DTS) | pad-to 16M | \
    append-kernel | pad-to 32M | \
    append-ubi | check-size 67108865
endef
TARGET_DEVICES += ls1012ardb

define Device/ls1012afrdm
  DEVICE_TITLE := LS1012AFRDM
  DEVICE_PACKAGES += \
    layerscape-rcw-ls1012afrdm \
    layerscape-ppfe \
    layerscape-ppa-ls1012afrdm \
    u-boot-ls1012afrdm-image \
    kmod-ppfe
  DEVICE_DTS := ../../../arm64/boot/dts/freescale/fsl-ls1012a-frdm
  UBIFS_OPTS := -m 1 -e 262016 -c 128
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 256KiB
  PAGESIZE := 1
  IMAGE/firmware.bin := \
    ls-clean | \
    ls-append $(1)-rcw.bin | pad-to 1M | \
    ls-append $(1)-uboot.bin | pad-to 3M | \
    ls-append $(1)-uboot-env.bin | pad-to 4M | \
    ls-append $(1)-ppa.itb | pad-to 10M | \
    ls-append pfe.itb | pad-to 15M | \
    ls-append-dtb $$(DEVICE_DTS) | pad-to 16M | \
    append-kernel | pad-to 32M | \
    append-ubi | check-size 67108865
endef
TARGET_DEVICES += ls1012afrdm

define Device/ls1043ardb
  DEVICE_TITLE := LS1043ARDB
  DEVICE_PACKAGES += \
    layerscape-rcw-ls1043ardb \
    layerscape-fman-ls1043ardb \
    layerscape-ppa-ls1043ardb \
    u-boot-ls1043ardb-image
  DEVICE_DTS := ../../../arm64/boot/dts/freescale/fsl-ls1043a-rdb-sdk
  FILESYSTEMS := squashfs
  IMAGE/firmware.bin := \
    ls-clean | \
    ls-append $(1)-rcw.bin | pad-to 1M | \
    ls-append $(1)-uboot.bin | pad-to 3M | \
    ls-append $(1)-uboot-env.bin | pad-to 4M | \
    ls-append $(1)-ppa.itb | pad-to 9M | \
    ls-append $(1)-fman.bin | pad-to 15M | \
    ls-append-dtb $$(DEVICE_DTS) | pad-to 16M | \
    append-kernel | pad-to 32M | \
    append-rootfs | pad-rootfs | check-size 67108865
endef
TARGET_DEVICES += ls1043ardb

define Device/ls1046ardb
  DEVICE_TITLE := LS1046ARDB
  DEVICE_PACKAGES += \
    layerscape-rcw-ls1046ardb \
    layerscape-fman-ls1046ardb \
    layerscape-ppa-ls1046ardb \
    u-boot-ls1046ardb-image
  DEVICE_DTS := ../../../arm64/boot/dts/freescale/fsl-ls1046a-rdb-sdk
  UBIFS_OPTS := -m 1 -e 262016 -c 128
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 256KiB
  PAGESIZE := 1
  IMAGE/firmware.bin := \
    ls-clean | \
    ls-append $(1)-rcw.bin | pad-to 1M | \
    ls-append $(1)-uboot.bin | pad-to 3M | \
    ls-append $(1)-uboot-env.bin | pad-to 4M | \
    ls-append $(1)-ppa.itb | pad-to 9M | \
    ls-append $(1)-fman.bin | pad-to 15M | \
    ls-append-dtb $$(DEVICE_DTS) | pad-to 16M | \
    append-kernel | pad-to 32M | \
    append-ubi | check-size 67108865
endef
TARGET_DEVICES += ls1046ardb
