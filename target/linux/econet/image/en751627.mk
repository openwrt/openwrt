TRX_ENDIAN := be

define Device/zyxel-ubi
  $(call Device/tclinux-ubi)
  FACTORY_SIZE := 40960k
  NAND_SIZE := 128m
  IMAGE/tclinux.trx := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | \
    tclinux-trx-fixup | check-size $$$$(FACTORY_SIZE)
endef

define Device/zyxel_ex3301-t0
  $(call Device/zyxel-ubi)
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := EX3301-T0
  DEVICE_COMPAT_VERSION := 2.0
  DEVICE_COMPAT_MESSAGE := NAND layout changed to UBI. Install tclinux.trx through the bootloader; raw-TRX builds cannot flash the new sysupgrade archive.
  DEVICE_DTS := en751627_zyxel_ex3301-t0
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7915e kmod-mt7915-firmware
endef
TARGET_DEVICES += zyxel_ex3301-t0

define Device/zyxel_wx3100-t0
  $(call Device/zyxel-ubi)
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := WX3100-T0
  DEVICE_DTS := en751627_zyxel_wx3100-t0
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7915-firmware
endef
TARGET_DEVICES += zyxel_wx3100-t0
