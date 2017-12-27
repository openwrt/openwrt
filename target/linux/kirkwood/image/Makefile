#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR:=0x8000

define Device/Default
  PROFILES := Default
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL := kernel-bin | append-dtb | uImage none
  KERNEL_NAME := zImage
  KERNEL_SUFFIX  := -uImage
  KERNEL_IN_UBI := 1

  PAGESIZE := 2048
  SUBPAGESIZE := 512
  BLOCKSIZE := 128k
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar
  IMAGE/factory.bin := append-ubi
endef

define Device/dockstar
  DEVICE_DTS := kirkwood-dockstar
  DEVICE_TITLE := Seagate FreeAgent Dockstar
endef
TARGET_DEVICES += dockstar

define Device/goflexnet
  DEVICE_TITLE := Seagate GoFlexNet
  DEVICE_DTS := kirkwood-goflexnet
endef
TARGET_DEVICES += goflexnet

define Device/goflexhome
  DEVICE_TITLE := Seagate GoFlexHome
  DEVICE_DTS := kirkwood-goflexhome
endef
TARGET_DEVICES += goflexhome

define Device/linksys-audi
  DEVICE_TITLE := Linksys EA3500 (Audi)
  DEVICE_PACKAGES := kmod-mwl8k swconfig wpad-mini
  DEVICE_DTS := kirkwood-linksys-audi
  KERNEL_SIZE := 2624k
  KERNEL_IN_UBI := 0
  UBINIZE_OPTS := -E 5
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef
TARGET_DEVICES += linksys-audi

define Device/linksys-viper
  DEVICE_TITLE := Linksys E4200v2 / EA4500 (Viper)
  DEVICE_PACKAGES := kmod-mwl8k swconfig wpad-mini
  DEVICE_DTS := kirkwood-linksys-viper
  KERNEL_SIZE := 2688k
  KERNEL_IN_UBI := 0
  UBINIZE_OPTS := -E 5
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef
TARGET_DEVICES += linksys-viper

define Device/iconnect
  DEVICE_TITLE := Iomega Iconnect
  DEVICE_DTS := kirkwood-iconnect
endef
TARGET_DEVICES += iconnect

define Device/nsa310b
  DEVICE_TITLE := ZyXEL NSA310b
  DEVICE_DTS := kirkwood-nsa310b
  DEVICE_PACKAGES := kmod-r8169 kmod-gpio-button-hotplug kmod-hwmon-lm85
endef
TARGET_DEVICES += nsa310b

define Device/nsa325
  DEVICE_TITLE := ZyXEL NSA325 (v1 and v2)
  DEVICE_DTS := kirkwood-nsa325
  DEVICE_PACKAGES := kmod-gpio-button-hotplug kmod-rtc-pcf8563 kmod-usb3
endef
TARGET_DEVICES += nsa325

define Device/on100
  DEVICE_TITLE := Cisco Systems ON100
  DEVICE_DTS := kirkwood-on100
  DEVICE_PACKAGES := kmod-i2c-mv64xxx
  KERNEL_SIZE := 5376k
  KERNEL_IN_UBI := 0
  UBINIZE_OPTS := -E 5
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef
TARGET_DEVICES += on100

define Device/pogo_e02
  DEVICE_TITLE := Cloud Engines Pogoplug E02
  DEVICE_DTS := kirkwood-pogo_e02
endef
TARGET_DEVICES += pogo_e02

define Device/ib62x0
  DEVICE_TITLE := RaidSonic ICY BOX IB-NAS62x0
  DEVICE_DTS := kirkwood-ib62x0
endef
TARGET_DEVICES += ib62x0

$(eval $(call BuildImage))
