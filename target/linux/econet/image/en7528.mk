TRX_ENDIAN := le

define Device/en7528_generic
  DEVICE_VENDOR := EN7528
  DEVICE_MODEL := Generic
  DEVICE_DTS := en7528_generic
endef
TARGET_DEVICES += en7528_generic

define Device/dasan_h660gm-a
  $(call Device/tclinux-ubi)
  DEVICE_VENDOR := DASAN
  DEVICE_MODEL := H660GM-A
  FACTORY_SIZE := 32m
  TRX_MODEL := Dewberry
  DEVICE_PACKAGES := kmod-usb2 kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap
endef

define Device/dasan_h660gm-a-airtel
  $(Device/dasan_h660gm-a)
  DEVICE_VARIANT := Airtel
  DEVICE_DTS := en7528_dasan_h660gm-a-airtel
endef
TARGET_DEVICES += dasan_h660gm-a-airtel

define Device/dasan_h660gm-a-generic
  $(Device/dasan_h660gm-a)
  DEVICE_VARIANT := Generic
  DEVICE_DTS := en7528_dasan_h660gm-a-generic
endef
TARGET_DEVICES += dasan_h660gm-a-generic

# JioFiber JCOW407 (Reliance Jio GPON ONT): EN7528 + MT7663, 512MB DDR3,
# 256MB SPI-NAND. Same SoC/wifi/image format as the DASAN H660GM-A.
define Device/jio_jcow407
  DEVICE_VENDOR := JioFiber
  DEVICE_MODEL := JCOW407
  DEVICE_DTS := en7528_jio_jcow407
  # luci-ssl + px5g pull in the TLS stack so the LuCI web UI is reachable over
  # https://192.168.1.1 (uhttpd generates a self-signed cert on first boot);
  # without it only http works and HTTPS-first browsers fail to connect.
  DEVICE_PACKAGES := kmod-usb2 kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	luci-ssl px5g-mbedtls
  TRX_MODEL := Dewberry
  IMAGES := tclinux.trx
  IMAGE/tclinux.trx := append-kernel | lzma | tclinux-trx
endef
TARGET_DEVICES += jio_jcow407
