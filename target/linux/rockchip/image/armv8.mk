# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Sarah Maedel

# FIT will be loaded at 0x02080000. Leave 16M for that, align it to 2M and load the kernel after it.
KERNEL_LOADADDR := 0x03200000

define Device/armsom_sige7
  DEVICE_VENDOR := ArmSoM
  DEVICE_MODEL := Sige7
  DEVICE_ALT0_VENDOR := Bananapi
  DEVICE_ALT0_MODEL := BPi-M7
  SOC := rk3588
  DEVICE_DTS := rockchip/rk3588-armsom-sige7
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += armsom_sige7

define Device/firefly_roc-rk3328-cc
  DEVICE_VENDOR := Firefly
  DEVICE_MODEL := ROC-RK3328-CC
  SOC := rk3328
  DEVICE_DTS := rockchip/rk3328-roc-cc
  UBOOT_DEVICE_NAME := roc-cc-rk3328
endef
TARGET_DEVICES += firefly_roc-rk3328-cc

define Device/friendlyarm_nanopc-t4
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPC T4
  SOC := rk3399
  DEVICE_PACKAGES := kmod-brcmfmac brcmfmac-nvram-4356-sdio cypress-firmware-4356-sdio
endef
TARGET_DEVICES += friendlyarm_nanopc-t4

define Device/friendlyarm_nanopc-t6
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPC T6
  SOC := rk3588
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopc-t6

define Device/friendlyarm_nanopi-r2c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2C
  SOC := rk3328
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2c

define Device/friendlyarm_nanopi-r2c-plus
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2C Plus
  SOC := rk3328
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2c-plus

define Device/friendlyarm_nanopi-r2s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2S
  SOC := rk3328
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2s

define Device/friendlyarm_nanopi-neo3
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi NEO3
  SOC := rk3328
  UBOOT_DEVICE_NAME := nanopi-neo3-rk3328
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-neo3 | pine64-bin | gzip | append-metadata
  DEVICE_PACKAGES := kmod-usb-net kmod-usb-net-rtl8152
  SUPPORTED_DEVICES := friendlyarm,nanopi-neo3
endef
TARGET_DEVICES += friendlyarm_nanopi-neo3

define Device/friendlyarm_nanopi-r3s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R3S
  SOC := rk3566
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r3s

define Device/friendlyarm_nanopi-r4s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4S
  DEVICE_VARIANT := 4GB LPDDR4
  SOC := rk3399
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r4s

define Device/friendlyarm_nanopi-r4s-enterprise
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4S Enterprise Edition
  DEVICE_VARIANT := 4GB LPDDR4
  SOC := rk3399
  UBOOT_DEVICE_NAME := nanopi-r4s-rk3399
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r4s-enterprise

define Device/friendlyarm_nanopi-r5c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R5C
  SOC := rk3568
  DEVICE_PACKAGES := kmod-r8169 kmod-rtw88-8822ce rtl8822ce-firmware wpad-basic-mbedtls
endef
TARGET_DEVICES += friendlyarm_nanopi-r5c

define Device/friendlyarm_nanopi-r5s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R5S
  SOC := rk3568
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r5s

define Device/friendlyarm_nanopi-r6c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R6C
  SOC := rk3588s
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r6c

define Device/friendlyarm_nanopi-r6s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R6S
  SOC := rk3588s
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r6s

define Device/pine64_rock64
  DEVICE_VENDOR := Pine64
  DEVICE_MODEL := Rock64
  SOC := rk3328
endef
TARGET_DEVICES += pine64_rock64

define Device/pine64_rockpro64
  DEVICE_VENDOR := Pine64
  DEVICE_MODEL := RockPro64
  SOC := rk3399
  SUPPORTED_DEVICES += pine64,rockpro64-v2.1
endef
TARGET_DEVICES += pine64_rockpro64

define Device/radxa_cm3-io
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := CM3 IO
  SOC := rk3566
  DEVICE_DTS := rockchip/rk3566-radxa-cm3-io
  UBOOT_DEVICE_NAME := radxa-cm3-io-rk3566
endef
TARGET_DEVICES += radxa_cm3-io

define Device/radxa_e25
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := E25
  SOC := rk3568
  DEVICE_DTS := rockchip/rk3568-radxa-e25
  BOOT_SCRIPT := radxa-e25
  UBOOT_DEVICE_NAME := radxa-e25-rk3568
  DEVICE_PACKAGES := kmod-r8169 kmod-ata-ahci-dwc
endef
TARGET_DEVICES += radxa_e25

define Device/radxa_rock-3a
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 3A
  SOC := rk3568
  SUPPORTED_DEVICES := radxa,rock3a
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_rock-3a

define Device/radxa_rock-3b
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 3B
  SOC := rk3568
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_rock-3b

define Device/radxa_rock-3c
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 3C
  SOC := rk3566
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_rock-3c

define Device/radxa_rock-4c-plus
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 4C+
  SOC := rk3399
endef
TARGET_DEVICES += radxa_rock-4c-plus

define Device/radxa_rock-4se
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 4SE
  SOC := rk3399
endef
TARGET_DEVICES += radxa_rock-4se

define Device/radxa_rock-5a
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 5A
  SOC := rk3588s
  UBOOT_DEVICE_NAME := rock5a-rk3588s
  DEVICE_PACKAGES := kmod-hwmon-pwmfan
endef
TARGET_DEVICES += radxa_rock-5a

define Device/radxa_rock-5b
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 5B
  SOC := rk3588
  UBOOT_DEVICE_NAME := rock5b-rk3588
  DEVICE_PACKAGES := kmod-r8169 kmod-hwmon-pwmfan
endef
TARGET_DEVICES += radxa_rock-5b

define Device/radxa_rock-4a
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK Pi 4A
  SOC := rk3399
  SUPPORTED_DEVICES := radxa,rockpi4a radxa,rockpi4
  UBOOT_DEVICE_NAME := rock-pi-4-rk3399
endef
TARGET_DEVICES += radxa_rock-4a

define Device/radxa_rock-pi-e
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK Pi E
  SOC := rk3328
endef
TARGET_DEVICES += radxa_rock-pi-e

define Device/radxa_rock-pi-e-v3
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK Pi E v3.0
  SOC := rk3328
  DEVICE_DTS := rockchip/rk3328-rock-pi-e
  DEVICE_PACKAGES := kmod-rtw88-8723du kmod-usb-net-cdc-ncm kmod-usb-net-rndis wpad-basic-mbedtls
endef
TARGET_DEVICES += radxa_rock-pi-e-v3

define Device/radxa_rock-pi-s
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK Pi S
  SOC := rk3308
  SUPPORTED_DEVICES := radxa,rockpis
  BOOT_SCRIPT := rock-pi-s
  DEVICE_PACKAGES := kmod-rtw88-8723ds kmod-usb-net-cdc-ncm kmod-usb-net-rndis wpad-basic-mbedtls
endef
TARGET_DEVICES += radxa_rock-pi-s

define Device/radxa_zero-3e
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ZERO 3E
  SOC := rk3566
  DEVICE_DTS := rockchip/rk3566-radxa-zero-3e
  UBOOT_DEVICE_NAME := radxa-zero-3-rk3566
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_zero-3e

define Device/radxa_zero-3w
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ZERO 3W
  SOC := rk3566
  DEVICE_DTS := rockchip/rk3566-radxa-zero-3w
  UBOOT_DEVICE_NAME := radxa-zero-3-rk3566
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_zero-3w

define Device/sinovoip_bpi-r2-pro
  DEVICE_VENDOR := Sinovoip
  DEVICE_MODEL := Bananapi-R2 Pro
  SOC := rk3568
  SUPPORTED_DEVICES := sinovoip,rk3568-bpi-r2pro
  DEVICE_PACKAGES := kmod-ata-ahci-dwc
endef
TARGET_DEVICES += sinovoip_bpi-r2-pro

define Device/xunlong_orangepi-r1-plus
  DEVICE_VENDOR := Xunlong
  DEVICE_MODEL := Orange Pi R1 Plus
  SOC := rk3328
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += xunlong_orangepi-r1-plus

define Device/xunlong_orangepi-r1-plus-lts
  DEVICE_VENDOR := Xunlong
  DEVICE_MODEL := Orange Pi R1 Plus LTS
  SOC := rk3328
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += xunlong_orangepi-r1-plus-lts