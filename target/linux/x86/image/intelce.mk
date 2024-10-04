define Device/intelce
  DEVICE_VENDOR := Technicolor/Cisco/ARRIS
  DEVICE_MODEL := x86
  DEVICE_PACKAGES += kmod-e1000 kmod-forcedeth kmod-fs-vfat
  GRUB2_VARIANT := generic
endef
TARGET_DEVICES += intelce
