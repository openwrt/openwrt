#
# MT7628 Profiles
#

define Device/mt7628
  DTS := MT7628
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := MediaTek MT7628 EVB
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
TARGET_DEVICES += mt7628

define Device/miwifi-nano
  DTS := MIWIFI-NANO
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
  DEVICE_TITLE := Xiaomi MiWiFi Nano
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
TARGET_DEVICES += miwifi-nano

define Device/wrtnode2p
  DTS := WRTNODE2P
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
  DEVICE_TITLE := WRTnode 2P
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
TARGET_DEVICES += wrtnode2p

define Device/duzun-dm06
  DTS := DUZUN-DM06
  IMAGE_SIZE := $(ralink_default_fw_size_8M)
  DEVICE_TITLE := DuZun DM06
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
TARGET_DEVICES += duzun-dm06
