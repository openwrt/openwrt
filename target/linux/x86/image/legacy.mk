define Device/generic_x86-legacy
  DEVICE_TITLE := Generic x86 (legacy)
  DEVICE_PACKAGES += kmod-3c59x kmod-8139too kmod-e100 kmod-e1000 \
	kmod-natsemi kmod-ne2k-pci kmod-pcnet32 kmod-r8169 kmod-sis900 \
	kmod-tg3 kmod-via-rhine kmod-via-velocity kmod-forcedeth
  GRUB2_VARIANT := legacy
endef
TARGET_DEVICES += generic_x86-legacy
