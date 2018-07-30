include ./common-tp-link.mk


define Device/tl-mr10u
  $(Device/tplink-4mlzma)
  ATH_SOC := ar9331
  DEVICE_TITLE := TP-Link TL-MR10U
  DEVICE_PACKAGES := kmod-usb-chipidea2
  TPLINK_HWID := 0x00100101
  SUPPORTED_DEVICES := tplink,tl-mr10u tl-mr10u
endef
TARGET_DEVICES += tl-mr10u

define Device/tl-mr3020-v1
  $(Device/tplink-4mlzma)
  ATH_SOC := ar9331
  DEVICE_TITLE := TP-LINK TL-MR3020 v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-chipidea2 kmod-usb-ledtrig-usbport
  TPLINK_HWID := 0x30200001
  SUPPORTED_DEVICES := tplink,tl-mr3020-v1 tl-mr3020-v1
endef
TARGET_DEVICES += tl-mr3020-v1

define Device/tl-mr3220-v1
  $(Device/tplink-4m)
  ATH_SOC := ar7241
  DEVICE_TITLE := TP-Link TL-MR3220 v1
  TPLINK_HWID := 0x32200001
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES := tplink,tl-mr3220-v1 tl-mr3220-v1
endef
TARGET_DEVICES += tl-mr3220-v1

define Device/tl-mr3420-v1
  $(Device/tplink-4m)
  ATH_SOC := ar7241
  DEVICE_TITLE := TP-Link TL-MR3420 v1
  TPLINK_HWID := 0x34200001
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES := tplink,tl-mr3420-v1 tl-mr3420-v1
endef
TARGET_DEVICES += tl-mr3420-v1

define Device/tl-wr703n
  $(Device/tplink-4mlzma)
  ATH_SOC := ar9331
  DEVICE_TITLE := TP-Link TL-WR703N
  DEVICE_PACKAGES := kmod-usb-chipidea2
  TPLINK_HWID := 0x07030101
  SUPPORTED_DEVICES := tplink,tl-wr703n tl-wr703n
endef
TARGET_DEVICES += tl-wr703n

define Device/tl-wr740n-v2
  $(Device/tplink-4m)
  ATH_SOC := ar7240
  DEVICE_TITLE := TP-Link TL-WR740N/ND v2
  TPLINK_HWID := 0x07400001
  SUPPORTED_DEVICES := tplink,tl-wr740n-v2 tl-wr740n-v2
endef
TARGET_DEVICES += tl-wr740n-v2

define Device/tl-wr740nd-v4
  $(Device/tplink-4mlzma)
  ATH_SOC := ar9331
  DEVICE_TITLE := TP-LINK TL-WR740N/ND v4
  TPLINK_HWID := 0x07400004
  SUPPORTED_DEVICES := tplink,tl-wr740n-v4 tl-wr740n-v4
endef
TARGET_DEVICES += tl-wr740nd-v4

define Device/tl-wr741nd-v4
  $(Device/tplink-4mlzma)
  ATH_SOC := ar9331
  DEVICE_TITLE := TP-LINK TL-WR741N/ND v4
  TPLINK_HWID := 0x07410004
  SUPPORTED_DEVICES := tplink,tl-wr741n-v4 tl-wr741n-v4
endef
TARGET_DEVICES += tl-wr741nd-v4

define Device/tl-wr841-v7
  $(Device/tplink-4m)
  ATH_SOC := ar7241
  DEVICE_TITLE := TP-LINK TL-WR841N/ND v7
  TPLINK_HWID := 0x08410007
  SUPPORTED_DEVICES := tplink,tl-wr841-v7 tl-wr841-v7
endef
TARGET_DEVICES += tl-wr841-v7

define Device/tl-wr841-v9
  $(Device/tplink-4mlzma)
  ATH_SOC := qca9533
  DEVICE_TITLE := TP-LINK TL-WR841N/ND v9
  TPLINK_HWID := 0x08410009
  SUPPORTED_DEVICES := tplink,tl-wr841n-v9 tl-wr841n-v9
endef
TARGET_DEVICES += tl-wr841-v9
