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

define Device/dlink_r18
  $(call Device/tclinux-ubi)
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := R18
  DEVICE_ALT0_VENDOR := D-Link
  DEVICE_ALT0_MODEL := EAGLE PRO AI AX1800
  DEVICE_DTS := en7528_dlink_r18
  # "free bootbase" bootloader decompresses to 0x80002000 (stock log)
  TRX_LOADADDR := 0x80002000
  KERNEL := kernel-bin | append-dtb | tclinux-free-bootbase-jump | lzma | \
    kernel-trx
  # The bootloader reads both slots with a size derived from slot A's kernel
  # length, and stock kernels in slot B are larger than 4 MiB.
  KERNEL_SIZE := 8192k
  TRX_PAD := 8388608
  # tclinux.trx is written into slot A (tclinux, 45 MiB)
  FACTORY_SIZE := 45m
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7915-firmware
endef
TARGET_DEVICES += dlink_r18

define Device/jiofiber_en7528
  $(call Device/tclinux-ubi)
  DEVICE_VENDOR := JioFiber
  FACTORY_SIZE := 40m
  TRX_LOADADDR := 0x80002000
  KERNEL := kernel-bin | append-dtb | tclinux-free-bootbase-jump | lzma | \
    kernel-trx
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap
endef

define Device/jiofiber_jcow414
  $(Device/jiofiber_en7528)
  DEVICE_MODEL := JCOW414
  DEVICE_ALT0_VENDOR := JioFiber
  DEVICE_ALT0_MODEL := JCO4032
  DEVICE_DTS := en7528_jiofiber_jcow414
endef
TARGET_DEVICES += jiofiber_jcow414

define Device/jiofiber_jcow407
  $(Device/jiofiber_en7528)
  DEVICE_MODEL := JCOW407
  DEVICE_ALT0_VENDOR := JioFiber
  DEVICE_ALT0_MODEL := JCOW407-IN
  DEVICE_DTS := en7528_jiofiber_jcow407
endef
TARGET_DEVICES += jiofiber_jcow407
