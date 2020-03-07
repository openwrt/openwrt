#
# Copyright (C) 2012-2016 OpenWrt.org
# Copyright (C) 2016 LEDE-project.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Device/cznic_turris-omnia
  DEVICE_VENDOR := CZ.NIC
  DEVICE_MODEL := Turris Omnia
  KERNEL_INSTALL := 1
  KERNEL := kernel-bin
  KERNEL_INITRAMFS := kernel-bin
  DEVICE_PACKAGES :=  \
	mkf2fs e2fsprogs kmod-fs-vfat kmod-nls-cp437 kmod-nls-iso8859-1 \
	wpad-basic kmod-ath9k kmod-ath10k-ct ath10k-firmware-qca988x-ct \
	partx-utils kmod-i2c-mux-pca954x
  IMAGES := $$(IMAGE_PREFIX)-sysupgrade.img.gz omnia-medkit-$$(IMAGE_PREFIX)-initramfs.tar.gz
  IMAGE/$$(IMAGE_PREFIX)-sysupgrade.img.gz := boot-img | sdcard-img | gzip | append-metadata
  IMAGE/omnia-medkit-$$(IMAGE_PREFIX)-initramfs.tar.gz := omnia-medkit-initramfs | gzip
  IMAGE_NAME = $$(2)
  SOC := armada-385
  SUPPORTED_DEVICES += armada-385-turris-omnia
endef
TARGET_DEVICES += cznic_turris-omnia

define Device/linksys
  $(Device/NAND-128K)
  DEVICE_VENDOR := Linksys
  DEVICE_PACKAGES := kmod-mwlwifi wpad-basic
  IMAGES += factory.img
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | \
	append-ubi | pad-to $$$$(PAGESIZE)
  KERNEL_SIZE := 6144k
endef

define Device/linksys_wrt1200ac
  $(call Device/linksys)
  DEVICE_MODEL := WRT1200AC
  DEVICE_ALT0_VENDOR := Linksys
  DEVICE_ALT0_MODEL := Caiman
  DEVICE_DTS := armada-385-linksys-caiman
  DEVICE_PACKAGES += mwlwifi-firmware-88w8864
endef
TARGET_DEVICES += linksys_wrt1200ac

define Device/linksys_wrt1900acs
  $(call Device/linksys)
  DEVICE_MODEL := WRT1900ACS
  DEVICE_VARIANT := v1
  DEVICE_ALT0_VENDOR := Linksys
  DEVICE_ALT0_MODEL := WRT1900ACS
  DEVICE_ALT0_VARIANT := v2
  DEVICE_ALT1_VENDOR := Linksys
  DEVICE_ALT1_MODEL := Shelby
  DEVICE_DTS := armada-385-linksys-shelby
  DEVICE_PACKAGES += mwlwifi-firmware-88w8864
endef
TARGET_DEVICES += linksys_wrt1900acs

define Device/linksys_wrt1900ac-v2
  $(call Device/linksys)
  DEVICE_MODEL := WRT1900AC
  DEVICE_VARIANT := v2
  DEVICE_ALT0_VENDOR := Linksys
  DEVICE_ALT0_MODEL := Cobra
  DEVICE_DTS := armada-385-linksys-cobra
  DEVICE_PACKAGES += mwlwifi-firmware-88w8864
endef
TARGET_DEVICES += linksys_wrt1900ac-v2

define Device/linksys_wrt3200acm
  $(call Device/linksys)
  DEVICE_MODEL := WRT3200ACM
  DEVICE_ALT0_VENDOR := Linksys
  DEVICE_ALT0_MODEL := Rango
  DEVICE_DTS := armada-385-linksys-rango
  DEVICE_PACKAGES += kmod-btmrvl kmod-mwifiex-sdio mwlwifi-firmware-88w8964
endef
TARGET_DEVICES += linksys_wrt3200acm

define Device/linksys_wrt32x
  $(call Device/linksys)
  DEVICE_MODEL := WRT32X
  DEVICE_ALT0_VENDOR := Linksys
  DEVICE_ALT0_MODEL := Venom
  DEVICE_DTS := armada-385-linksys-venom
  DEVICE_PACKAGES += kmod-btmrvl kmod-mwifiex-sdio mwlwifi-firmware-88w8964
  KERNEL_SIZE := 3072k
  KERNEL := kernel-bin | append-dtb
endef
TARGET_DEVICES += linksys_wrt32x

define Device/marvell_a385-db-ap
  $(Device/NAND-256K)
  DEVICE_VENDOR := Marvell
  DEVICE_MODEL := Armada 385 Development Board AP (DB-88F6820-AP)
  DEVICE_DTS := armada-385-db-ap
  IMAGES += factory.img
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | \
	append-ubi | pad-to $$$$(PAGESIZE)
  KERNEL_SIZE := 8192k
  SUPPORTED_DEVICES += armada-385-db-ap
endef
TARGET_DEVICES += marvell_a385-db-ap

define Device/marvell_a388-rd
  DEVICE_VENDOR := Marvell
  DEVICE_MODEL := Armada 388 RD (RD-88F6820-AP)
  DEVICE_DTS := armada-388-rd
  IMAGES := firmware.bin
  IMAGE/firmware.bin := append-kernel | pad-to 256k | append-rootfs | pad-rootfs
  SUPPORTED_DEVICES := armada-388-rd marvell,a385-rd
endef
TARGET_DEVICES += marvell_a388-rd

define Device/solidrun_clearfog-base-a1
  DEVICE_VENDOR := SolidRun
  DEVICE_MODEL := ClearFog Base
  KERNEL_INSTALL := 1
  KERNEL := kernel-bin
  DEVICE_PACKAGES := mkf2fs e2fsprogs partx-utils
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := boot-scr | boot-img-ext4 | sdcard-img-ext4 | gzip | append-metadata
  DEVICE_DTS := armada-388-clearfog-base armada-388-clearfog-pro
  SUPPORTED_DEVICES += armada-388-clearfog-base
  UBOOT := clearfog-u-boot-spl.kwb
  BOOT_SCRIPT := clearfog
endef
TARGET_DEVICES += solidrun_clearfog-base-a1

define Device/solidrun_clearfog-pro-a1
  DEVICE_VENDOR := SolidRun
  DEVICE_MODEL := ClearFog Pro
  KERNEL_INSTALL := 1
  KERNEL := kernel-bin
  DEVICE_PACKAGES := mkf2fs e2fsprogs partx-utils
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := boot-scr | boot-img-ext4 | sdcard-img-ext4 | gzip | append-metadata
  DEVICE_DTS := armada-388-clearfog-pro armada-388-clearfog-base
  UBOOT := clearfog-u-boot-spl.kwb
  BOOT_SCRIPT := clearfog
endef
TARGET_DEVICES += solidrun_clearfog-pro-a1
