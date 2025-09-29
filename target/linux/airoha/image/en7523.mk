KERNEL_LOADADDR := 0x80208000

define Target/Description
	Build firmware images for Airoha EN7523 ARM based boards.
endef

define Device/airoha_en7523-evb
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := EN7523 Evaluation Board
  DEVICE_DTS := en7523-evb
  DEVICE_DTS_DIR := ../dts
endef
TARGET_DEVICES += airoha_en7523-evb

define Device/tplink_xx230v
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := xx230v
  DEVICE_DTS = en7523-tplink_xx230v
  DEVICE_DTS_DIR := ../dts
  TPLINK_BOARD_ID := XX230-V1
  DEVICE_DTS_CONFIG := config@1
  DEVICE_PACKAGES += kmod-mt7915-firmware kmod-mt7915e uboot-envtools
endef
TARGET_DEVICES += tplink_xx230v