define Device/generic
  DEVICE_TITLE := Generic EFI Boot
  DEVICE_PACKAGES :=
  GRUB2_VARIANT := generic
  FILESYSTEMS := ext4 squashfs
  DEVICE_PACKAGES += kmod-amazon-ena kmod-e1000e kmod-vmxnet3 kmod-rtc-rx8025 \
	kmod-i2c-mux-pca954x kmod-gpio-pca953x
endef
TARGET_DEVICES += generic
