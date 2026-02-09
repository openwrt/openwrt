TRX_ENDIAN := le

define Device/en7528_generic
  DEVICE_VENDOR := EN7528
  DEVICE_MODEL := Generic
  DEVICE_DTS := en7528_generic
endef
TARGET_DEVICES += en7528_generic

define Device/dasan_h660gm-a
  DEVICE_VENDOR := DASAN
  DEVICE_MODEL := H660GM-A
  DEVICE_DTS := en7528_dasan_h660gm-a
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap
  TRX_MODEL := Dewberry
  IMAGES := tclinux.trx
  IMAGE/tclinux.trx := append-kernel | lzma | tclinux-trx
endef
TARGET_DEVICES += dasan_h660gm-a
