define Device/mediatek_mt7622-rfb1
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MTK7622 rfb1 AP
  DEVICE_DTS := mt7622-rfb1
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 \
			kmod-ata-core kmod-ata-ahci-mtk
endef
TARGET_DEVICES += mediatek_mt7622-rfb1

define Device/bpi_bananapi-r64
  DEVICE_VENDOR := Bpi
  DEVICE_MODEL := Banana Pi R64
  DEVICE_DTS := mt7622-bananapi-bpi-r64
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := bananapi,bpi-r64
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb2 kmod-usb3 \
			kmod-ata-core kmod-ata-ahci-mtk
endef
TARGET_DEVICES += bpi_bananapi-r64
