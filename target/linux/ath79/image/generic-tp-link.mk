include ./common-tp-link.mk

define Device/tl-wdr3600
  $(Device/tplink-8mlzma)
  ATH_SOC := ar9344
  DEVICE_TITLE := TP-LINK TL-WDR3600
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  TPLINK_HWID := 0x36000001
  SUPPORTED_DEVICES := tplink,tl-wdr3600 tl-wdr3600
endef
TARGET_DEVICES += tl-wdr3600

define Device/tl-wdr4300
  $(Device/tl-wdr3600)
  DEVICE_TITLE := TP-LINK TL-WDR4300
  TPLINK_HWID := 0x43000001
  SUPPORTED_DEVICES := tplink,tl-wdr4300 tl-wdr4300
endef
TARGET_DEVICES += tl-wdr4300

define Device/tl-wr1043nd-v1
  $(Device/tplink-8m)
  ATH_SOC := ar9132
  DEVICE_TITLE := TP-LINK TL-WR1043N/ND v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  TPLINK_HWID := 0x10430001
  SUPPORTED_DEVICES := tplink,tl-wr1043nd-v1 tl-wr1043nd
endef
TARGET_DEVICES += tl-wr1043nd-v1

define Device/tl-wr1043nd-v2
  $(Device/tplink-8mlzma)
  ATH_SOC := qca9558
  DEVICE_TITLE := TP-LINK TL-WR1043N/ND v2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  TPLINK_HWID := 0x10430002
  SUPPORTED_DEVICES := tplink,tl-wr1043nd-v2 tl-wr1043nd-v2
endef
TARGET_DEVICES += tl-wr1043nd-v2

define Device/tl-wr1043nd-v3
  $(Device/tplink-8mlzma)
  ATH_SOC := qca9558
  DEVICE_TITLE := TP-LINK TL-WR1043N/ND v3
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  TPLINK_HWID := 0x10430003
  SUPPORTED_DEVICES := tplink,tl-wr1043nd-v3 tl-wr1043nd-v3
endef
TARGET_DEVICES += tl-wr1043nd-v3

define Device/tl-wr1043nd-v4
  $(Device/tplink)
  ATH_SOC := qca9563
  IMAGE_SIZE := 15552k
  DEVICE_TITLE := TP-LINK TL-WR1043N/ND v4
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  TPLINK_HWID := 0x10430004
  TPLINK_BOARD_ID := TLWR1043NDV4
  KERNEL := kernel-bin | append-dtb | lzma | tplink-v1-header
  IMAGE/sysupgrade.bin := append-rootfs | tplink-safeloader sysupgrade
  IMAGE/factory.bin := append-rootfs | tplink-safeloader factory
  SUPPORTED_DEVICES := tplink,tl-wr1043nd-v4 tl-wr1043nd-v4
endef
TARGET_DEVICES += tl-wr1043nd-v4
