DTS_DIR := $(DTS_DIR)/siflower
KERNEL_LOADADDR := 0x20200000

define Device/siflower_sf21a6826p-evb
  BLOCKSIZE := 64k
  DEVICE_VENDOR := SIFLOWER
  DEVICE_MODEL := SF21A6826P evaluation board
  DEVICE_DTS := sf21a6826p-evb
  DEVICE_DTS_DIR := ../dts
endef
TARGET_DEVICES += siflower_sf21a6826p-evb
