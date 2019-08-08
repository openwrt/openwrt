include ./common-buffalo.mk

define Device/buffalo_whr-g301n
  ATH_SOC := ar7240
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WHR-G301N
  IMAGES += factory.bin tftp.bin
  IMAGE/default := append-kernel | pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size-firmware
  IMAGE/factory.bin := $$(IMAGE/default) | buffalo-enc WHR-G301N 1.99 | buffalo-tag WHR-G301N 3
  IMAGE/tftp.bin := $$(IMAGE/default) | buffalo-tftp-header
  SUPPORTED_DEVICES += whr-g301n
endef
TARGET_DEVICES += buffalo_whr-g301n

define Device/pqi_air-pen
  ATH_SOC := ar9330
  DEVICE_VENDOR := PQI
  DEVICE_MODEL := Air-Pen
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
  SUPPORTED_DEVICES += pqi-air-pen
endef
TARGET_DEVICES += pqi_air-pen
