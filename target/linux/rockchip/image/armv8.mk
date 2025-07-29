# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Sarah Maedel

# FIT will be loaded at 0x02080000. Leave 16M for that, align it to 2M and load the kernel after it.
KERNEL_LOADADDR := 0x03200000

define Device/IfnameMigration
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Network interface names have been changed
endef

define Device/ariaboard_photonicat
  DEVICE_VENDOR := Ariaboard
  DEVICE_MODEL := Photonicat
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ath10k-sdio ath10k-firmware-qca9377-sdio wpad-openssl \
	kmod-usb-net-cdc-mbim kmod-usb-net-qmi-wwan kmod-usb-serial-option uqmi
endef
TARGET_DEVICES += ariaboard_photonicat

define Device/armsom_sige3
  DEVICE_VENDOR := ArmSoM
  DEVICE_MODEL := Sige3
  SOC := rk3568
  DEVICE_DTS := rockchip/rk3568-armsom-sige3
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-brcmfmac kmod-r8125 wpad-openssl \
	brcmfmac-firmware-43752-sdio brcmfmac-nvram-43752-sdio
endef
TARGET_DEVICES += armsom_sige3

define Device/armsom_sige7
  DEVICE_VENDOR := ArmSoM
  DEVICE_MODEL := Sige7
  DEVICE_ALT0_VENDOR := Bananapi
  DEVICE_ALT0_MODEL := BPi-M7
  SOC := rk3588
  DEVICE_DTS := rockchip/rk3588-armsom-sige7
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-brcmfmac kmod-r8125 wpad-openssl \
	brcmfmac-firmware-43752-pcie brcmfmac-nvram-43752-pcie
endef
TARGET_DEVICES += armsom_sige7

define Device/cyber_cyber3588-aib
  DEVICE_VENDOR := Cyber
  DEVICE_MODEL := 3588 AIB
  SOC := rk3588
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ata-ahci-dwc kmod-r8125 kmod-mt7921e wpad-openssl \
	kmod-hwmon-pwmfan kmod-usb-net-cdc-mbim kmod-usb-net-qmi-wwan \
	kmod-usb-serial-option uqmi
endef
TARGET_DEVICES += cyber_cyber3588-aib

define Device/ezpro_mrkaio-m68s
  DEVICE_VENDOR := EZPRO
  DEVICE_MODEL := Mrkaio M68S
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ata-ahci-dwc
endef
TARGET_DEVICES += ezpro_mrkaio-m68s

define Device/firefly_roc-rk3328-cc
  DEVICE_VENDOR := Firefly
  DEVICE_MODEL := ROC-RK3328-CC
  SOC := rk3328
  DEVICE_DTS := rockchip/rk3328-roc-cc
  UBOOT_DEVICE_NAME := roc-cc-rk3328
  BOOT_FLOW := pine64-bin
endef
TARGET_DEVICES += firefly_roc-rk3328-cc

define Device/firefly_roc-rk3568-pc
  DEVICE_VENDOR := Firefly
  DEVICE_MODEL := Station P2
  DEVICE_ALT0_VENDOR := Firefly
  DEVICE_ALT0_MODEL := ROC-RK3568-PC
  SOC := rk3568
  DEVICE_DTS := rockchip/rk3568-roc-pc
  SUPPORTED_DEVICES := firefly,rk3568-roc-pc
  UBOOT_DEVICE_NAME := roc-pc-rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ata-ahci-dwc kmod-brcmfmac wpad-openssl \
	brcmfmac-firmware-43752-sdio brcmfmac-nvram-43752-sdio
endef
TARGET_DEVICES += firefly_roc-rk3568-pc

define Device/friendlyarm_nanopc-t4
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPC T4
  SOC := rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-brcmfmac wpad-openssl \
	brcmfmac-firmware-4356-sdio brcmfmac-nvram-4356-sdio
endef
TARGET_DEVICES += friendlyarm_nanopc-t4

define Device/friendlyarm_nanopc-t6
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPC T6
  SOC := rk3588
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += friendlyarm_nanopc-t6

define Device/friendlyarm_nanopi-r2c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2C
  SOC := rk3328
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2c

define Device/friendlyarm_nanopi-r2c-plus
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2C Plus
  SOC := rk3328
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2c-plus

define Device/friendlyarm_nanopi-r2s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2S
  SOC := rk3328
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2s

define Device/friendlyarm_nanopi-r3s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R3S
  SOC := rk3566
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r3s

define Device/friendlyarm_nanopi-r4s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4S
  SOC := rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r4s

define Device/friendlyarm_nanopi-r4se
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4SE
  SOC := rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r4se

define Device/friendlyarm_nanopi-r4s-enterprise
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4S Enterprise Edition
  SOC := rk3399
  UBOOT_DEVICE_NAME := nanopi-r4s-rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += friendlyarm_nanopi-r4s-enterprise

define Device/friendlyarm_nanopi-r5c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R5C
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125 kmod-rtw88-8822ce rtl8822ce-firmware wpad-openssl
endef
TARGET_DEVICES += friendlyarm_nanopi-r5c

define Device/friendlyarm_nanopi-r5s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R5S
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += friendlyarm_nanopi-r5s

define Device/friendlyarm_nanopi-r6c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R6C
  SOC := rk3588s
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += friendlyarm_nanopi-r6c

define Device/friendlyarm_nanopi-r6s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R6S
  SOC := rk3588s
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += friendlyarm_nanopi-r6s

define Device/huake_guangmiao-g4c
  DEVICE_VENDOR := Huake-Cloud
  DEVICE_MODEL := GuangMiao G4C
  SOC := rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-r8169
endef
TARGET_DEVICES += huake_guangmiao-g4c

define Device/lunzn_fastrhino-r66s
  DEVICE_VENDOR := Lunzn
  DEVICE_MODEL := FastRhino R66S
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += lunzn_fastrhino-r66s

define Device/lunzn_fastrhino-r68s
  $(Device/IfnameMigration)
  DEVICE_VENDOR := Lunzn
  DEVICE_MODEL := FastRhino R68S
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += lunzn_fastrhino-r68s

define Device/lyt_t68m
  DEVICE_VENDOR := LYT
  DEVICE_MODEL := T68M
  SOC := rk3568
  DEVICE_DTS := rockchip/rk3568-lyt-t68m
  UBOOT_DEVICE_NAME := lyt-t68m-rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125 kmod-mt7921e wpad-openssl
endef
TARGET_DEVICES += lyt_t68m

define Device/mmbox_anas3035
  DEVICE_VENDOR := MMBOX
  DEVICE_MODEL := ANAS3035
  SOC := rk3568
  DEVICE_DTS := rockchip/rk3568-mmbox-anas3035
  UBOOT_DEVICE_NAME := mmbox-anas3035-rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125 kmod-ata-ahci-dwc kmod-hwmon-pwmfan kmod-hwmon-drivetemp
endef
TARGET_DEVICES += mmbox_anas3035

define Device/pine64_rock64
  DEVICE_VENDOR := Pine64
  DEVICE_MODEL := Rock64
  SOC := rk3328
  BOOT_FLOW := pine64-bin
endef
TARGET_DEVICES += pine64_rock64

define Device/pine64_rockpro64
  DEVICE_VENDOR := Pine64
  DEVICE_MODEL := RockPro64
  SOC := rk3399
  BOOT_FLOW := pine64-bin
  SUPPORTED_DEVICES += pine64,rockpro64-v2.1
endef
TARGET_DEVICES += pine64_rockpro64

define Device/radxa_cm3-io
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := CM3 IO
  SOC := rk3566
  DEVICE_DTS := rockchip/rk3566-radxa-cm3-io
  UBOOT_DEVICE_NAME := radxa-cm3-io-rk3566
  BOOT_FLOW := pine64-img
endef
TARGET_DEVICES += radxa_cm3-io

define Device/radxa_e25
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := E25
  SOC := rk3568
  DEVICE_DTS := rockchip/rk3568-radxa-e25
  UBOOT_DEVICE_NAME := radxa-e25-rk3568
  BOOT_FLOW := pine64-img
  BOOT_SCRIPT := radxa-e25
  DEVICE_PACKAGES := kmod-r8125 kmod-ata-ahci-dwc
endef
TARGET_DEVICES += radxa_e25

define Device/radxa_rock-3a
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 3A
  SOC := rk3568
  SUPPORTED_DEVICES := radxa,rock3a
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_rock-3a

define Device/radxa_rock-3b
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 3B
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_rock-3b

define Device/radxa_rock-3c
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 3C
  SOC := rk3566
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_rock-3c

define Device/radxa_rock-4c-plus
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 4C+
  SOC := rk3399
  BOOT_FLOW := pine64-bin
endef
TARGET_DEVICES += radxa_rock-4c-plus

define Device/radxa_rock-4se
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 4SE
  SOC := rk3399
  BOOT_FLOW := pine64-bin
endef
TARGET_DEVICES += radxa_rock-4se

define Device/radxa_rock-5a
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 5A
  SOC := rk3588s
  UBOOT_DEVICE_NAME := rock5a-rk3588s
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125 kmod-hwmon-pwmfan
endef
TARGET_DEVICES += radxa_rock-5a

define Device/radxa_rock-5b
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK 5B
  SOC := rk3588
  UBOOT_DEVICE_NAME := rock5b-rk3588
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125 kmod-hwmon-pwmfan
endef
TARGET_DEVICES += radxa_rock-5b

define Device/radxa_rock-pi-4a
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK Pi 4A
  SOC := rk3399
  SUPPORTED_DEVICES := radxa,rockpi4a radxa,rockpi4
  UBOOT_DEVICE_NAME := rock-pi-4-rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-brcmfmac wpad-openssl \
	brcmfmac-firmware-43456-sdio brcmfmac-nvram-43456-sdio
endef
TARGET_DEVICES += radxa_rock-pi-4a

define Device/radxa_rock-pi-e
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK Pi E
  DEVICE_ALT0_VENDOR := Radxa
  DEVICE_ALT0_MODEL := ROCK Pi E v3.0
  SOC := rk3328
  SUPPORTED_DEVICES := radxa,rockpi-e
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-rtw88-8723du kmod-usb-net-cdc-ncm kmod-usb-net-rndis wpad-openssl
endef
TARGET_DEVICES += radxa_rock-pi-e

define Device/radxa_rock-pi-s
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ROCK Pi S
  SOC := rk3308
  SUPPORTED_DEVICES := radxa,rockpis
  BOOT_FLOW := pine64-img
  BOOT_SCRIPT := rock-pi-s
  DEVICE_PACKAGES := kmod-rtw88-8723ds kmod-usb-net-cdc-ncm kmod-usb-net-rndis wpad-openssl
endef
TARGET_DEVICES += radxa_rock-pi-s

define Device/radxa_zero-3e
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ZERO 3E
  SOC := rk3566
  DEVICE_DTS := rockchip/rk3566-radxa-zero-3e
  UBOOT_DEVICE_NAME := radxa-zero-3-rk3566
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_zero-3e

define Device/radxa_zero-3w
  DEVICE_VENDOR := Radxa
  DEVICE_MODEL := ZERO 3W
  SOC := rk3566
  DEVICE_DTS := rockchip/rk3566-radxa-zero-3w
  UBOOT_DEVICE_NAME := radxa-zero-3-rk3566
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-usb-net-cdc-ncm kmod-usb-net-rndis
endef
TARGET_DEVICES += radxa_zero-3w

define Device/sinovoip_bpi-r2-pro
  DEVICE_VENDOR := Bananapi
  DEVICE_MODEL := BPi-R2 Pro
  SOC := rk3568
  SUPPORTED_DEVICES := sinovoip,rk3568-bpi-r2pro
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ata-ahci-dwc
endef
TARGET_DEVICES += sinovoip_bpi-r2-pro

define Device/xunlong_orangepi-5
  DEVICE_VENDOR := Xunlong
  DEVICE_MODEL := Orange Pi 5
  SOC := rk3588s
  BOOT_FLOW := pine64-img
endef
TARGET_DEVICES += xunlong_orangepi-5

define Device/xunlong_orangepi-5-plus
  DEVICE_VENDOR := Xunlong
  DEVICE_MODEL := Orange Pi 5 Plus
  SOC := rk3588
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += xunlong_orangepi-5-plus

define Device/xunlong_orangepi-r1-plus
  DEVICE_VENDOR := Xunlong
  DEVICE_MODEL := Orange Pi R1 Plus
  SOC := rk3328
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += xunlong_orangepi-r1-plus

define Device/xunlong_orangepi-r1-plus-lts
  DEVICE_VENDOR := Xunlong
  DEVICE_MODEL := Orange Pi R1 Plus LTS
  SOC := rk3328
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += xunlong_orangepi-r1-plus-lts
