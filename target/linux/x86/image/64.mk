define Device/generic
  DEVICE_VENDOR := Generic
  DEVICE_MODEL := x86/64
  DEVICE_PACKAGES += \
	kmod-amazon-ena kmod-amd-xgbe kmod-bnx2 kmod-dwmac-intel kmod-e1000e kmod-e1000 \
	kmod-forcedeth kmod-fs-vfat kmod-igb kmod-igc kmod-ixgbe kmod-r8169 \
	kmod-tg3
  GRUB2_VARIANT := generic
endef

TARGET_DEVICES += generic

define Device/AZB_MT110
  DEVICE_VENDOR := Intel Arizona Beach (NEXSEC MT110)
  DEVICE_PACKAGES += \
	kmod-e1000e kmod-e1000 kmod-fs-vfat kmod-igb kmod-ixgbe kmod-igc kmod-i40e \
	kmod-iwlwifi kmod-usb-net-rndis kmod-usb-net-cdc-mbim kmod-i2c-i801
  GRUB2_VARIANT := generic
endef

TARGET_DEVICES += AZB_MT110
