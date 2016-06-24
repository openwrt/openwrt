#
# MT7688 Profiles
#

define Device/LinkIt7688
  DTS := LINKIT7688
  IMAGE_SIZE := $(ralink_default_fw_size_32M)
  DEVICE_TITLE := MediaTek LinkIt Smart 7688
  DEVICE_PACKAGES:= kmod-usb2 kmod-usb-ohci uboot-envtools
endef
TARGET_DEVICES += LinkIt7688

define Device/wrtnode2r
  DTS := WRTNODE2R
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
  DEVICE_TITLE := WRTnode 2R
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += wrtnode2r

define Device/widora-neo
  DTS := WIDORA-NEO
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
  DEVICE_TITLE := Widora-NEO
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += widora-neo
