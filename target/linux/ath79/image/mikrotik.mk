define Device/mikrotik
  PROFILES := Default
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport rbextract
  KERNEL_INITRAMFS := kernel-bin | append-dtb|lzma|loader-kernel
  LOADER_TYPE := elf
  KERNEL := kernel-bin | append-dtb|lzma|loader-kernel|kernel2minor -s 2048 -e -c 
  FILESYSTEMS := squashfs
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar
endef

define Device/rb912uag-2hpnd
  $(Device/mikrotik)
  ATH_SOC := ar9342
  DEVICE_TITLE := MikroTik RouterBoard RB912UAG-2HPnD
  SUPPORTED_DEVICES := mikrotik,rb912uag-2hpnd
endef
TARGET_DEVICES += rb912uag-2hpnd
