define Device/generic_x86-64
  DEVICE_TITLE := Generic x86 (64 Bit)
  DEVICE_PACKAGES += kmod-bnx2 kmod-e1000e kmod-e1000 kmod-forcedeth kmod-igb \
	kmod-r8169
  GRUB2_VARIANT := generic
endef
TARGET_DEVICES += generic_x86-64
